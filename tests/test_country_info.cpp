/**
 * SPDX-FileComment: Test program for the Country Info API adapter.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file test_country_info.cpp
 * @brief Test program for fetching and parsing country information.
 * @version 0.1.0
 * @date 2026-04-06
 *
 * @author ZHENG Robert
 * @license MIT License
 */

#include "regeocode/adapter_country_info.hpp"
#include "regeocode/http_client.hpp"
#include "regeocode/re_geocode_core.hpp"

#include <iostream>
#include <memory>
#include <print>
#include <string>
#include <vector>

/**
 * @brief Main function for the country info test.
 *
 * @param argc Argument count.
 * @param argv Argument vector (argv[1] is the country code).
 * @return int Exit code.
 */
int main(int argc, char **argv) {
  using namespace regeocode;

  if (argc < 2) {
    std::println("Usage: {} <country_code>", argv[0]);
    std::println("Example: {} cn", argv[0]);
    return 1;
  }

  std::string country_code = argv[1];

  try {
    // 1. Setup API config
    ApiConfig cfg;
    cfg.name = "country_info";
    cfg.uri_template = "https://restcountries.com/v3.1/alpha/{{ country_code }}";
    cfg.api_key = country_code; // We use API-Key to store the country_code for the template
    cfg.adapter = "country_info";
    cfg.type = "info";

    std::unordered_map<std::string, ApiConfig> configs;
    configs["country_info"] = cfg;

    // 2. Setup adapter
    std::vector<ApiAdapterPtr> adapters;
    adapters.push_back(std::make_unique<CountryInfoAdapter>());

    // 3. Setup HttpClient
    auto client = std::make_unique<HttpClient>();

    // 4. Setup Geocoder
    ReverseGeocoder geocoder(std::move(configs), std::move(adapters), std::move(client));

    // 5. Fetch (coordinates are dummy here as the adapter doesn't use them)
    Coordinates dummy_coords{0.0, 0.0};
    auto result = geocoder.reverse_geocode(dummy_coords, "country_info");

    // 6. Print result
    std::println("--- Country Info Result for: {} ---", country_code);
    std::println("Official Name: {}", result.address_english);
    std::println("Common Name:   {}", result.address_local);
    std::println("Country Code:  {}", result.country_code);
    
    std::println("\nAttributes:");
    for (const auto &[key, val] : result.attributes) {
      std::println("  {:<15}: {}", key, val);
    }

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
