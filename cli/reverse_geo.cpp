/**
 * SPDX-FileComment: CLI entry point for the Reverse Geocoding application.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file reverse_geo.cpp
 * @brief Main entry point for the command-line interface.
 * @version 0.2.0
 * @date 2026-03-30
 *
 * Erweiterungen:
 *  - --folder <path> und --rekursive: Ordnerverarbeitung
 *  - --copyright <text>: schreibt Copyright in EXIF/IPTC/XMP
 */

#include <CLI/CLI.hpp>
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include <exiv2/exiv2.hpp>
#include <nlohmann/json.hpp>

#include "regeocode/http_client.hpp"
#include "regeocode/re_geocode_core.hpp"

// Adapter Headers
#include "regeocode/adapter_bing.hpp"
#include "regeocode/adapter_geonames_timezone.hpp"
#include "regeocode/adapter_geonames_wikipedia.hpp"
#include "regeocode/adapter_google.hpp"
#include "regeocode/adapter_marea_tides.hpp"
#include "regeocode/adapter_nominatim.hpp"
#include "regeocode/adapter_opencage.hpp"
#include "regeocode/adapter_openweather.hpp"
#include "regeocode/adapter_pollution.hpp"
#include "regeocode/adapter_seaweather.hpp"
#include "regeocode/adapter_tides.hpp"

namespace fs = std::filesystem;

/**
 * @brief Helper function to parse a comma-separated string into a vector.
 */
std::vector<std::string> parse_list(const std::string &input) {
  std::vector<std::string> list;
  std::stringstream ss(input);
  std::string segment;
  while (std::getline(ss, segment, ',')) {
    size_t first = segment.find_first_not_of(' ');
    if (std::string::npos != first) {
      size_t last = segment.find_last_not_of(' ');
      list.push_back(segment.substr(first, (last - first + 1)));
    }
  }
  return list;
}

/**
 * @brief Convert a single rational token like "123/100" or "123" to double.
 */
static double rational_token_to_double(const std::string &token) {
  auto slash = token.find('/');
  if (slash == std::string::npos) {
    return std::stod(token);
  } else {
    double num = std::stod(token.substr(0, slash));
    double den = std::stod(token.substr(slash + 1));
    if (den == 0.0)
      return 0.0;
    return num / den;
  }
}

/**
 * @brief Parse EXIF GPS value string into decimal degrees.
 *
 * Expects typical Exiv2 string formats like:
 *  "48/1 7/1 1234/100" or "48 7 12.34"
 */
static std::optional<double>
parse_exif_gps_to_decimal(const std::string &value_str,
                          const std::optional<std::string> &ref) {
  if (value_str.empty())
    return std::nullopt;
  std::stringstream ss(value_str);
  std::vector<std::string> parts;
  std::string token;
  while (ss >> token)
    parts.push_back(token);
  if (parts.empty())
    return std::nullopt;

  // degrees, minutes, seconds
  double deg = 0.0, min = 0.0, sec = 0.0;
  if (parts.size() >= 1)
    deg = rational_token_to_double(parts[0]);
  if (parts.size() >= 2)
    min = rational_token_to_double(parts[1]);
  if (parts.size() >= 3)
    sec = rational_token_to_double(parts[2]);

  double decimal = deg + (min / 60.0) + (sec / 3600.0);
  if (ref) {
    std::string r = *ref;
    if (!r.empty()) {
      char c = std::toupper(r[0]);
      if (c == 'S' || c == 'W')
        decimal = -decimal;
    }
  }
  return decimal;
}

/**
 * @brief Determine if a file extension looks like an image we want to process.
 */
static bool is_image_file(const fs::path &p) {
  if (!p.has_extension())
    return false;
  std::string ext = p.extension().string();
  for (auto &c : ext)
    c = std::tolower(c);
  return (ext == ".jpg" || ext == ".jpeg" || ext == ".tif" || ext == ".tiff" ||
          ext == ".png");
}

/**
 * @brief Process a single image file: read GPS, reverse-geocode, write
 * metadata.
 */
static void
process_image_file(const fs::path &file_path,
                   regeocode::ReverseGeocoder &geocoder,
                   const std::vector<std::string> &priority_list,
                   const std::string &lang_override,
                   const std::optional<std::string> &copyright_opt) {
  try {
    std::cout << "Processing: " << file_path << std::endl;
    auto image = Exiv2::ImageFactory::open(file_path.string());
    if (!image.get()) {
      std::cerr << "  Unable to open image: " << file_path << std::endl;
      return;
    }
    image->readMetadata();

    Exiv2::ExifData &exifData = image->exifData();
    Exiv2::IptcData &iptcData = image->iptcData();
    Exiv2::XmpData &xmpData = image->xmpData();

    // Try to find GPS latitude/longitude and refs
    std::optional<std::string> lat_str, lon_str, lat_ref, lon_ref;
    auto itLat = exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLatitude"));
    auto itLon = exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLongitude"));
    auto itLatRef =
        exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLatitudeRef"));
    auto itLonRef =
        exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLongitudeRef"));

    if (itLat != exifData.end())
      lat_str = itLat->toString();
    if (itLon != exifData.end())
      lon_str = itLon->toString();
    if (itLatRef != exifData.end())
      lat_ref = itLatRef->toString();
    if (itLonRef != exifData.end())
      lon_ref = itLonRef->toString();

    if (!lat_str || !lon_str) {
      std::cout << "  No GPS EXIF found, skipping reverse-geocode.\n";
    } else {
      auto lat_opt = parse_exif_gps_to_decimal(*lat_str, lat_ref);
      auto lon_opt = parse_exif_gps_to_decimal(*lon_str, lon_ref);
      if (!lat_opt || !lon_opt) {
        std::cerr << "  Failed to parse GPS coordinates for " << file_path
                  << std::endl;
      } else {
        regeocode::Coordinates coords{*lat_opt, *lon_opt};
        nlohmann::json result = geocoder.reverse_geocode_fallback(
            coords, priority_list, lang_override);

        // Extract fields from result (best-effort)
        std::string address_english = "";
        std::string address_local = "";
        std::string country_code = "";

        // std::cout << "result: " << result.dump() << std::endl;

        if (result["result"].contains("address_english") &&
            result["result"]["address_english"].is_string())
          address_english =
              result["result"]["address_english"].get<std::string>();
        if (result["result"].contains("address_local") &&
            result["result"]["address_local"].is_string())
          address_local = result["result"]["address_local"].get<std::string>();
        if (result["result"].contains("country_code") &&
            result["result"]["country_code"].is_string())
          country_code = result["result"]["country_code"].get<std::string>();

        // Write XMP fields (overwrite)
        if (!address_english.empty())
          xmpData["Xmp.dc.address_english"] = address_english;
        if (!address_local.empty())
          xmpData["Xmp.dc.address_local"] = address_local;
        if (!country_code.empty())
          xmpData["Xmp.dc.country_code"] = country_code;

        if (result["result"]["attributes"].contains("country")) {
          xmpData["Xmp.photoshop.Country"] =
              result["result"]["attributes"]["country"].get<std::string>();
        }
        if (result["result"]["attributes"].contains("state")) {
          xmpData["Xmp.photoshop.State"] =
              result["result"]["attributes"]["state"].get<std::string>();
        }
        if (result["result"]["attributes"].contains("city")) {
          xmpData["Xmp.photoshop.City"] =
              result["result"]["attributes"]["city"].get<std::string>();
        }
        std::cout << "  Reverse-geocode result: " << address_local << " / "
                  << country_code << std::endl;
      }
    }

    // If copyright provided, write to EXIF/IPTC/XMP
    if (copyright_opt) {
      const std::string &cp = *copyright_opt;
      exifData["Exif.Image.Copyright"] = cp;
      iptcData["Iptc.Application2.Copyright"] = cp;
      xmpData["Xmp.dc.rights"] = cp;
      xmpData["Xmp.plus.CopyrightOwner"] = cp;
      std::cout << "  Wrote copyright metadata.\n";
    }

    // Commit metadata back to file
    image->setExifData(exifData);
    image->setIptcData(iptcData);
    image->setXmpData(xmpData);
    image->writeMetadata();

  } catch (const Exiv2::Error &e) {
    std::cerr << "  Exiv2 error for file " << file_path << ": " << e.what()
              << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "  Error processing file " << file_path << ": " << e.what()
              << std::endl;
  }
}

/**
 * @brief Walk a folder and process image files.
 */
static void process_folder(const fs::path &folder, bool recursive,
                           regeocode::ReverseGeocoder &geocoder,
                           const std::vector<std::string> &priority_list,
                           const std::string &lang_override,
                           const std::optional<std::string> &copyright_opt) {
  if (!fs::exists(folder) || !fs::is_directory(folder)) {
    std::cerr << "Folder does not exist or is not a directory: " << folder
              << std::endl;
    return;
  }

  if (recursive) {
    for (auto const &entry : fs::recursive_directory_iterator(folder)) {
      if (!entry.is_regular_file())
        continue;
      if (is_image_file(entry.path())) {
        process_image_file(entry.path(), geocoder, priority_list, lang_override,
                           copyright_opt);
      }
    }
  } else {
    for (auto const &entry : fs::directory_iterator(folder)) {
      if (!entry.is_regular_file())
        continue;
      if (is_image_file(entry.path())) {
        process_image_file(entry.path(), geocoder, priority_list, lang_override,
                           copyright_opt);
      }
    }
  }
}

/**
 * @brief Main execution function for the CLI.
 */
int main(int argc, char **argv) {
  CLI::App app{"Reverse Geocoding CLI"};

  double lat = 0.0;
  double lon = 0.0;
  std::string config_path = "re-geocode.ini";

  std::string strategy_raw =
      "opencage, nominatim, google, bing"; //"nominatim"; // Default Strategy
  std::string api_name = ""; // Default empty, to detect if user has set it

  std::string lang_override = "";
  bool batch_mode = false;

  std::string folder_path = "";
  bool recursive = false;
  std::string copyright_text = "";

  app.add_option("--lat", lat, "Latitude");
  app.add_option("--lon", lon, "Longitude");
  app.add_option("--config", config_path, "Config file");

  // --strategy allows "nominatim,google"
  app.add_option("--strategy", strategy_raw,
                 "Comma-separated list of APIs (Priority List)");

  // --api is the "shortcut" for a single API
  app.add_option("--api", api_name, "API Name (Overrides strategy)");

  app.add_option("--lang", lang_override, "Language");
  app.add_flag("--batch", batch_mode, "Run a demo batch process");

  // New options
  app.add_option("--folder", folder_path, "Folder to scan for images");
  app.add_flag("--rekursive", recursive, "Scan folder recursively");
  app.add_option("--copyright", copyright_text,
                 "Copyright text to write into metadata");

  CLI11_PARSE(app, argc, argv);

  try {
    // 1. Load config
    regeocode::ConfigLoader loader(config_path);
    auto config_result = loader.load();

    // 2. Instantiate adapters
    std::vector<regeocode::ApiAdapterPtr> adapters;
    adapters.push_back(std::make_unique<regeocode::NominatimAdapter>());
    adapters.push_back(std::make_unique<regeocode::GoogleAdapter>());
    adapters.push_back(std::make_unique<regeocode::OpenCageAdapter>());
    adapters.push_back(std::make_unique<regeocode::BingAdapter>());
    adapters.push_back(std::make_unique<regeocode::GeoNamesTimezoneAdapter>());
    adapters.push_back(std::make_unique<regeocode::GeoNamesWikipediaAdapter>());
    adapters.push_back(std::make_unique<regeocode::OpenWeatherAdapter>());
    adapters.push_back(std::make_unique<regeocode::PollutionAdapter>());
    adapters.push_back(std::make_unique<regeocode::MareaTidesAdapter>());
    adapters.push_back(std::make_unique<regeocode::TidesAdapter>());
    adapters.push_back(std::make_unique<regeocode::SeaWeatherAdapter>());

    // 3. Instantiate geocoder
    auto client = std::make_unique<regeocode::HttpClient>();

    regeocode::ReverseGeocoder geocoder(
        std::move(config_result.apis), std::move(adapters), std::move(client),
        config_result.quota_file_path // <--- New parameter
    );

    // --- LOGIC FIX: --api vs --strategy ---
    std::vector<std::string> priority_list;

    if (!api_name.empty()) {
      // User explicitly set --api -> This wins
      priority_list.push_back(api_name);
    } else {
      // Otherwise we take the strategy (Default: nominatim)
      priority_list = parse_list(strategy_raw);
    }
    // --------------------------------------

    // If folder mode is requested, process folder (overrides single/batch
    // modes)
    if (!folder_path.empty()) {
      std::optional<std::string> cp_opt =
          copyright_text.empty() ? std::nullopt
                                 : std::optional<std::string>(copyright_text);
      process_folder(fs::path(folder_path), recursive, geocoder, priority_list,
                     lang_override, cp_opt);
      return 0;
    }

    if (batch_mode) {
      std::cout << "Starting DEMO Batch Processing...\n";
      // Demo coordinates
      std::vector<regeocode::Coordinates> batch_inputs = {
          {48.1351, 11.5820}, // Munich
          {52.5200, 13.4050}, // Berlin
          {48.8566, 2.3522},  // Paris
          {48.2082, 16.3738}, // Vienna
          {40.7128, -74.0060} // New York
      };

      auto results = geocoder.batch_reverse_geocode(batch_inputs, priority_list,
                                                    lang_override);

      nlohmann::json batch_output = results;
      std::cout << batch_output.dump(4) << std::endl;

    } else {
      // Single Mode
      if (lat == 0.0 && lon == 0.0) {
        std::cerr << "Error: --lat and --lon required for single mode\n";
        return 1;
      }

      nlohmann::json result = geocoder.reverse_geocode_fallback(
          {lat, lon}, priority_list, lang_override);
      std::cout << result.dump(4) << std::endl;
    }

  } catch (const std::exception &e) {
    std::cerr << "Fatal Error: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
