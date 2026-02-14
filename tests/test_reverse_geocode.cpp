/**
 * SPDX-FileComment: Basic unit test for reverse geocoding.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file test_reverse_geocode.cpp
 * @brief Simple test case for Nominatim reverse geocoding.
 * @version 0.1.0
 * @date 2026-02-14
 *
 * @author ZHENG Robert
 * @license MIT License
 */

#include "regeocode/adapter_nominatim.hpp"
#include "regeocode/re_geocode_core.hpp"

#include <cassert>
#include <iostream>

/**
 * @brief Main function for the simple test.
 *
 * @return int Exit code (0 for success).
 */
int main() {
  using namespace regeocode;

  ApiConfig cfg;
  cfg.name = "nominatim";
  cfg.uri_template = "https://nominatim.openstreetmap.org/reverse?lat={{ "
                     "latitude }}&lon={{ longitude }}&format=json";
  cfg.api_key = "";
  cfg.adapter = "nominatim";

  std::unordered_map<std::string, ApiConfig> configs;
  configs["nominatim"] = cfg;

  std::vector<ApiAdapterPtr> adapters;
  adapters.emplace_back(std::make_unique<NominatimAdapter>());

  ReverseGeocoder geocoder{std::move(configs), std::move(adapters)};

  Coordinates coords{48.137154, 11.576124}; // Munich Marienplatz

  auto result = geocoder.reverse_geocode(coords, "nominatim");

  assert(!result.address_english.empty());

  std::cout << "Test passed\n";
  return 0;
}
