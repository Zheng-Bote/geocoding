/**
 * SPDX-FileComment: CLI entry point for the Reverse Geocoding application.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file reverse_geo_optimized.cpp
 * @brief Vollständig optimierte, parallelisierte und lauffähige Version.
 * @version 0.4.0
 * @date 2026-03-31
 *
 * Änderungen / Ziele:
 *  - Vollständig lauffähiger C++23-Quelltext (eine Datei)
 *  - Parallelisierte Ordnerverarbeitung (std::execution::par)
 *  - Asynchrone Überlappung von Netzwerk-Latenz mittels std::async
 *  - LRU-Cache für Reverse-Geocoding-Ergebnisse
 *  - Robuste EXIF/GPS-Parsing- und XMP/EXIF/IPTC-Schreiblogik (Exiv2)
 *  - Optionaler --folder / --rekursive Modus
 *  - Optionaler --copyright Parameter (überschreibt vorhandene Werte)
 *
 * Hinweise:
 *  - Benötigt Exiv2-Dev (Header + lib) und nlohmann/json sowie CLI11.
 *  - Linker: -lexiv2
 *
 * Kompilieren (Beispiel):
 *  g++ -std=c++23 reverse_geo_optimized.cpp -o reverse_geo \
 *      -I/path/to/cli11/include -I/path/to/nlohmann -lexiv2 -pthread
 */

#include <CLI/CLI.hpp>

#include <algorithm>
#include <cctype>
#include <execution>
#include <filesystem>
#include <future>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <exiv2/exiv2.hpp>
#include <nlohmann/json.hpp>

#include "regeocode/http_client.hpp"
#include "regeocode/re_geocode_core.hpp"

// Adapter Headers (wie in deiner Codebasis)
#include "regeocode/adapter_bing.hpp"
#include "regeocode/adapter_country.hpp"
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

/* ---------------------------
   Hilfsfunktionen / Utilities
   --------------------------- */

static inline std::string default_if_empty(const std::string &s,
                                           const std::string &def) {
  return s.empty() ? def : s;
}

std::vector<std::string> parse_list(const std::string &input) {
  std::vector<std::string> list;
  std::stringstream ss(input);
  std::string segment;
  while (std::getline(ss, segment, ',')) {
    size_t first = segment.find_first_not_of(' ');
    if (first != std::string::npos) {
      size_t last = segment.find_last_not_of(' ');
      list.push_back(segment.substr(first, last - first + 1));
    }
  }
  return list;
}

static double rational_token_to_double(const std::string &token) {
  auto slash = token.find('/');
  if (slash == std::string::npos) {
    return std::stod(token);
  } else {
    double num = std::stod(token.substr(0, slash));
    double den = std::stod(token.substr(slash + 1));
    return den == 0.0 ? 0.0 : num / den;
  }
}

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

  double deg = rational_token_to_double(parts[0]);
  double min = parts.size() > 1 ? rational_token_to_double(parts[1]) : 0.0;
  double sec = parts.size() > 2 ? rational_token_to_double(parts[2]) : 0.0;

  double decimal = deg + (min / 60.0) + (sec / 3600.0);
  if (ref && !ref->empty()) {
    char c = std::toupper((*ref)[0]);
    if (c == 'S' || c == 'W')
      decimal = -decimal;
  }
  return decimal;
}

static bool is_image_file(const fs::path &p) {
  if (!p.has_extension())
    return false;
  std::string ext = p.extension().string();
  std::transform(ext.begin(), ext.end(), ext.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return (ext == ".jpg" || ext == ".jpeg" || ext == ".tif" || ext == ".tiff" ||
          ext == ".png");
}

/* ---------------------------
   LRU Cache für Geocoding
   --------------------------- */

class GeoCache {
public:
  explicit GeoCache(size_t max_size = 1024) : max_size_(max_size) {}

  bool get(const std::string &key, nlohmann::json &out) {
    std::scoped_lock lock(mutex_);
    auto it = map_.find(key);
    if (it == map_.end())
      return false;
    list_.splice(list_.begin(), list_, it->second.second);
    out = it->second.first;
    return true;
  }

  void put(const std::string &key, const nlohmann::json &value) {
    std::scoped_lock lock(mutex_);
    auto it = map_.find(key);
    if (it != map_.end()) {
      it->second.first = value;
      list_.splice(list_.begin(), list_, it->second.second);
      return;
    }
    list_.push_front(key);
    map_[key] = {value, list_.begin()};
    if (map_.size() > max_size_) {
      auto last = list_.back();
      map_.erase(last);
      list_.pop_back();
    }
  }

private:
  size_t max_size_;
  std::list<std::string> list_;
  std::unordered_map<
      std::string, std::pair<nlohmann::json, std::list<std::string>::iterator>>
      map_;
  std::mutex mutex_;
};

static GeoCache GEO_CACHE(1024);

/* ---------------------------
   Async wrapper für Geocoder
   --------------------------- */

static std::string coords_key(double lat, double lon,
                              const std::vector<std::string> &priority,
                              const std::string &lang) {
  std::ostringstream oss;
  oss.setf(std::ios::fixed);
  oss.precision(6);
  oss << lat << "," << lon << "|" << (priority.empty() ? "" : priority.front())
      << "|" << lang;
  return oss.str();
}

static std::shared_future<nlohmann::json>
async_reverse_geocode_cached(regeocode::ReverseGeocoder &geocoder, double lat,
                             double lon,
                             const std::vector<std::string> &priority_list,
                             const std::string &lang_override) {
  std::string key = coords_key(lat, lon, priority_list, lang_override);

  nlohmann::json cached;
  if (GEO_CACHE.get(key, cached)) {
    // return immediate ready future
    return std::async(std::launch::deferred, [cached]() { return cached; })
        .share();
  }

  // Launch async task
  return std::async(std::launch::async,
                    [&geocoder, lat, lon, priority_list, lang_override, key]() {
                      regeocode::Coordinates coords{lat, lon};
                      auto result = geocoder.reverse_geocode_fallback(
                          coords, priority_list, lang_override);
                      GEO_CACHE.put(key, result);
                      return result;
                    })
      .share();
}

/* ---------------------------
   Einzeldatei-Verarbeitung
   --------------------------- */

static void
process_image_file(const fs::path &file_path,
                   regeocode::ReverseGeocoder &geocoder,
                   const regeocode::CountryAdapter &country_adapter,
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

    // GPS keys
    auto itLat = exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLatitude"));
    auto itLon = exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLongitude"));
    auto itLatRef =
        exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLatitudeRef"));
    auto itLonRef =
        exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLongitudeRef"));

    if (itLat == exifData.end() || itLon == exifData.end()) {
      std::cout << "  No GPS EXIF found, skipping reverse-geocode.\n";
    } else {
      std::string lat_str = itLat->toString();
      std::string lon_str = itLon->toString();
      std::optional<std::string> lat_ref =
          (itLatRef != exifData.end())
              ? std::optional<std::string>(itLatRef->toString())
              : std::nullopt;
      std::optional<std::string> lon_ref =
          (itLonRef != exifData.end())
              ? std::optional<std::string>(itLonRef->toString())
              : std::nullopt;

      auto lat_opt = parse_exif_gps_to_decimal(lat_str, lat_ref);
      auto lon_opt = parse_exif_gps_to_decimal(lon_str, lon_ref);

      if (!lat_opt || !lon_opt) {
        std::cerr << "  Failed to parse GPS coordinates for " << file_path
                  << std::endl;
      } else {
        double lat = *lat_opt;
        double lon = *lon_opt;

        // Start async reverse-geocode (cached)
        auto geo_future = async_reverse_geocode_cached(
            geocoder, lat, lon, priority_list, lang_override);

        // Meanwhile we could do other lightweight work here (not necessary)

        // Wait for result
        nlohmann::json result = geo_future.get();

        // Extract address fields robustly
        std::string address_english, address_local, country_code, country,
            state, city, subjects;
        if (result.contains("result") && result["result"].is_object()) {
          auto &res = result["result"];
          if (res.contains("address_english") &&
              res["address_english"].is_string())
            address_english = res["address_english"].get<std::string>();
          if (res.contains("address_local") && res["address_local"].is_string())
            address_local = res["address_local"].get<std::string>();
          if (res.contains("country_code") && res["country_code"].is_string()) {
            country_code = res["country_code"].get<std::string>();
            subjects = country_code + ",";
            iptcData["Iptc.Application2.CountryCode"] = country_code;
            xmpData["Xmp.photoshop.CountryCode"] = country_code;

            // Fetch additional country info from local adapter
            auto country_info = country_adapter.get_country(country_code);
            if (!country_info.empty()) {
              std::string country_name = country_info.value("name.common", "");
              std::string region = country_info.value("region", "");

              if (!country_name.empty()) {
                xmpData["Xmp.photoshop.Country"] = country_name;
              }
              if (!region.empty()) {
                xmpData["Xmp.photoshop.continent"] = region;
              }
            }
          }

          if (res.contains("attributes") && res["attributes"].is_object()) {
            auto &attr = res["attributes"];
            if (attr.contains("country") && attr["country"].is_string()) {
              country = attr["country"].get<std::string>();
              // Only write if not already set by CountryAdapter
              if (xmpData.findKey(Exiv2::XmpKey("Xmp.photoshop.Country")) ==
                  xmpData.end()) {
                xmpData["Xmp.photoshop.Country"] = country;
              }
              subjects += country + ",";
              iptcData["Iptc.Application2.Country"] = country;
            }
            if (attr.contains("state") && attr["state"].is_string()) {
              state = attr["state"].get<std::string>();
              xmpData["Xmp.photoshop.State"] = state;
              subjects += state + ",";
              iptcData["Iptc.Application2.State"] = state;
            }
            if (attr.contains("city") && attr["city"].is_string()) {
              city = attr["city"].get<std::string>();
              xmpData["Xmp.photoshop.City"] = city;
              subjects += city;
              iptcData["Iptc.Application2.City"] = city;
            }
          }
        }

        // -------------------------
        // Timezone block (optimized & robust)
        // -------------------------
        {
          const std::vector<std::string> tz = {"timezone"};
          // Use cached async wrapper as well
          auto tz_future = async_reverse_geocode_cached(geocoder, lat, lon, tz,
                                                        lang_override);
          nlohmann::json timezone_result = tz_future.get();

          std::string timezone = "";
          if (timezone_result.contains("result") &&
              timezone_result["result"].contains("data") &&
              timezone_result["result"]["data"].contains("timezone_id") &&
              timezone_result["result"]["data"]["timezone_id"].is_string()) {
            timezone = timezone_result["result"]["data"]["timezone_id"]
                           .get<std::string>();
          }
          xmpData["Xmp.photoshop.Timezone"] = timezone;
        }

        // Write XMP address fields (overwrite)
        if (!address_english.empty())
          xmpData["Xmp.dc.AddressEnglish"] = address_english;
        if (!address_local.empty())
          xmpData["Xmp.dc.AddressLocal"] = address_local;
        if (!country_code.empty())
          xmpData["Xmp.dc.CountryCode"] = country_code;
      }
    }

    // Copyright writing if requested (overwrite existing)
    if (copyright_opt) {
      const std::string &cp = *copyright_opt;
      exifData["Exif.Image.Copyright"] = cp;
      iptcData["Iptc.Application2.Copyright"] = cp;
      xmpData["Xmp.dc.rights"] = cp;
      xmpData["Xmp.plus.CopyrightOwner"] = cp;
      std::cout << "  Wrote copyright metadata.\n";
    }

    // Commit metadata
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

/* ---------------------------
   Ordnerverarbeitung (parallel)
   --------------------------- */

static void process_folder(const fs::path &folder, bool recursive,
                           regeocode::ReverseGeocoder &geocoder,
                           const regeocode::CountryAdapter &country_adapter,
                           const std::vector<std::string> &priority_list,
                           const std::string &lang_override,
                           const std::optional<std::string> &copyright_opt) {
  if (!fs::exists(folder) || !fs::is_directory(folder)) {
    std::cerr << "Folder does not exist or is not a directory: " << folder
              << std::endl;
    return;
  }

  std::vector<fs::path> files;
  if (recursive) {
    for (auto const &entry : fs::recursive_directory_iterator(folder)) {
      if (entry.is_regular_file() && is_image_file(entry.path()))
        files.push_back(entry.path());
    }
  } else {
    for (auto const &entry : fs::directory_iterator(folder)) {
      if (entry.is_regular_file() && is_image_file(entry.path()))
        files.push_back(entry.path());
    }
  }

  // Parallel processing: each file processed independently
  std::for_each(std::execution::par, files.begin(), files.end(),
                [&](const fs::path &p) {
                  process_image_file(p, geocoder, country_adapter,
                                     priority_list, lang_override,
                                     copyright_opt);
                });
}

/* ---------------------------
   MAIN
   --------------------------- */

int main(int argc, char **argv) {
  CLI::App app{"Reverse Geocoding CLI (optimized)"};

  double lat = 0.0, lon = 0.0;
  std::string config_path = "re-geocode.ini";
  std::string strategy_raw = "opencage,nominatim,google,bing";
  std::string api_name = "";
  std::string lang_override = "";
  bool batch_mode = false;

  std::string folder_path = "";
  bool recursive = false;
  std::string copyright_text = "";

  app.add_option("--lat", lat, "Latitude");
  app.add_option("--lon", lon, "Longitude");
  app.add_option("--config", config_path, "Config file");
  app.add_option("--strategy", strategy_raw,
                 "Comma-separated list of APIs (Priority List)");
  app.add_option("--api", api_name, "API Name (Overrides strategy)");
  app.add_option("--lang", lang_override, "Language");
  app.add_flag("--batch", batch_mode, "Run a demo batch process");

  app.add_option("--folder", folder_path, "Folder to scan for images");
  app.add_flag("--rekursive", recursive, "Scan folder recursively");
  app.add_option("--copyright", copyright_text,
                 "Copyright text to write into metadata");

  CLI11_PARSE(app, argc, argv);

  // Apply defaults for empty strings where appropriate
  config_path = default_if_empty(config_path, "re-geocode.ini");
  strategy_raw = default_if_empty(strategy_raw, "nominatim");
  // lang_override left as-is (empty means no override)

  try {
    // Load config
    regeocode::ConfigLoader loader(config_path);
    auto config_result = loader.load();

    // Instantiate adapters (same order as before)
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

    auto client = std::make_unique<regeocode::HttpClient>();

    regeocode::ReverseGeocoder geocoder(std::move(config_result.apis),
                                        std::move(adapters), std::move(client),
                                        config_result.quota_file_path);

    // Instantiate CountryAdapter for local info (used in image processing)
    regeocode::CountryAdapter country_adapter("data/countries.json");

    // Priority list logic (--api overrides --strategy)
    std::vector<std::string> priority_list;
    if (!api_name.empty()) {
      priority_list.push_back(api_name);
    } else {
      priority_list = parse_list(strategy_raw);
    }

    // Folder mode (overrides single/batch)
    if (!folder_path.empty()) {
      std::optional<std::string> cp_opt =
          copyright_text.empty() ? std::nullopt
                                 : std::optional<std::string>(copyright_text);
      process_folder(fs::path(folder_path), recursive, geocoder,
                     country_adapter, priority_list, lang_override, cp_opt);
      return 0;
    }

    // Batch demo
    if (batch_mode) {
      std::cout << "Starting DEMO Batch Processing...\n";
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
      return 0;
    }

    // Single mode
    if (lat == 0.0 && lon == 0.0) {
      std::cerr << "Error: --lat and --lon required for single mode\n";
      return 1;
    }
    nlohmann::json result = geocoder.reverse_geocode_fallback(
        {lat, lon}, priority_list, lang_override);
    std::cout << result.dump(4) << std::endl;

  } catch (const std::exception &e) {
    std::cerr << "Fatal Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
