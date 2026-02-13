/**
 * SPDX-FileComment: Command line interface for the re-geocode library
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file main_cli.cpp
 * @brief CLI application for performing reverse geocoding lookups.
 * @version 0.1.0
 * @date 2026-02-13
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT License
 */

#include <CLI/CLI.hpp>
#include <iostream>
#include <memory>
#include <vector>

#include "regeocode/adapter_google.hpp"
#include "regeocode/adapter_nominatim.hpp"
#include "regeocode/adapter_opencage.hpp"
#include "regeocode/http_client.hpp"
#include "regeocode/re_geocode_core.hpp"

/**
 * @brief Entry point for the regeocode-cli application.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return int Exit code (0 for success, 1 for error).
 */
int main(int argc, char **argv) {
  CLI::App app{"Reverse Geocoding CLI (C++23)"};

  double lat = 0.0;
  double lon = 0.0;
  std::string config_path = "re-geocode.ini";
  std::string api_name = "nominatim";
  std::string lang_override = ""; // Empty = Auto-Detect Local

  // Define arguments
  app.add_option("--lat", lat, "Latitude")->required();
  app.add_option("--lon", lon, "Longitude")->required();
  app.add_option("--config", config_path, "Path to INI config file");
  app.add_option("--api", api_name,
                 "API section name in INI (e.g. google, nominatim)");
  app.add_option(
      "--lang", lang_override,
      "Override local language (e.g. 'de', 'ja'). English is always fetched.");

  CLI11_PARSE(app, argc, argv);

  try {
    // 1. Setup: Load configuration
    regeocode::ConfigLoader loader(config_path);
    auto configs = loader.load();

    // 2. Register adapters
    std::vector<regeocode::ApiAdapterPtr> adapters;
    adapters.push_back(std::make_unique<regeocode::NominatimAdapter>());
    adapters.push_back(std::make_unique<regeocode::GoogleAdapter>());
    adapters.push_back(std::make_unique<regeocode::OpenCageAdapter>());

    // 3. Instantiate HTTP Client
    auto client = std::make_unique<regeocode::HttpClient>();

    // 4. Create Geocoder
    regeocode::ReverseGeocoder geocoder(std::move(configs), std::move(adapters),
                                        std::move(client));

    // 5. Execute query
    std::cout << "Querying API '" << api_name << "' for Coordinates: " << lat
              << ", " << lon << "...\n";

    // Uses dual-language logic (English + Local/Auto)
    auto result = geocoder.reverse_geocode_dual_language({lat, lon}, api_name,
                                                         lang_override);

    // 6. Output results
    std::cout << "==================================================\n";
    std::cout << "English Address: " << result.address_english << "\n";

    if (!result.address_local.empty()) {
      std::cout << "Local Address  : " << result.address_local << "\n";
      // Info output if language was auto-detected
      if (lang_override.empty() && !result.country_code.empty()) {
        std::cout << "                 [Auto-detected via Country Code: "
                  << result.country_code << "]\n";
      }
    } else {
      std::cout << "Local Address  : <Not available or identical to English>\n";
    }
    std::cout << "==================================================\n";

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
