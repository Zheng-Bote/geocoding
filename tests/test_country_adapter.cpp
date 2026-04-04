/**
 * SPDX-FileComment: Unit test for the Country adapter.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file test_country_adapter.cpp
 * @brief Test case for local country data lookup.
 * @version 0.1.0
 * @date 2026-04-04
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT License
 */

#include "regeocode/adapter_country.hpp"
#include <cassert>
#include <iostream>
#include <print>

/**
 * @brief Main function for the country adapter test.
 *
 * @return int Exit code (0 for success).
 */
int main() {
  using namespace regeocode;

  try {
    CountryAdapter adapter("data/countries.json");

    // Test China
    auto cn = adapter.get_country("cn");
    assert(cn["name.common"] == "China");
    assert(cn["name.official"] == "People's Republic of China");
    assert(cn["capital"] == "Beijing");
    assert(cn["region"] == "Asia");
    assert(cn["flag"] == "🇨🇳");
    assert(cn["flag_url"] ==
           "https://github.com/lipis/flag-icons/blob/main/flags/4x3/cn.svg");
    std::println("Test CN: OK");

    // Test Germany
    auto de = adapter.get_country("DE");
    assert(de["name.common"] == "Germany");
    assert(de["capital"] == "Berlin");
    assert(de["flag_url"] ==
           "https://github.com/lipis/flag-icons/blob/main/flags/4x3/de.svg");
    std::println("Test DE: OK");

    // Test non-existent
    auto xx = adapter.get_country("XX");
    assert(xx.empty());
    std::println("Test XX: OK (Empty)");

    std::println("All CountryAdapter tests passed!");
  } catch (const std::exception &e) {
    std::cerr << "Test failed with exception: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
