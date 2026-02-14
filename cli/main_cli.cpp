/**
 * SPDX-FileComment: CLI entry point for the Reverse Geocoding application.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file main_cli.cpp
 * @brief Main entry point for the command-line interface.
 * @version 0.1.0
 * @date 2026-02-14
 *
 * @author ZHENG Robert
 * @license MIT License
 */

#include <CLI/CLI.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

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

/**
 * @brief Helper function to parse a comma-separated string into a vector.
 *
 * @param input The input string containing comma-separated values.
 * @return std::vector<std::string> A list of parsed strings.
 */
std::vector<std::string> parse_list(const std::string &input) {
  std::vector<std::string> list;
  std::stringstream ss(input);
  std::string segment;
  while (std::getline(ss, segment, ',')) {
    // Optional: Trim whitespace
    size_t first = segment.find_first_not_of(' ');
    if (std::string::npos != first) {
      size_t last = segment.find_last_not_of(' ');
      list.push_back(segment.substr(first, (last - first + 1)));
    }
  }
  return list;
}

/**
 * @brief Main execution function for the CLI.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return int Exit code.
 */
int main(int argc, char **argv) {
  CLI::App app{"Reverse Geocoding CLI"};

  double lat = 0.0;
  double lon = 0.0;
  std::string config_path = "re-geocode.ini";

  std::string strategy_raw = "nominatim"; // Default Strategy
  std::string api_name = ""; // Default empty, to detect if user has set it

  std::string lang_override = "";
  bool batch_mode = false;

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