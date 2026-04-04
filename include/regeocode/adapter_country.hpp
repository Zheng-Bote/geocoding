/**
 * SPDX-FileComment: Header file for the Country adapter.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file adapter_country.hpp
 * @brief Adapter for fetching country information from a local JSON file.
 * @version 0.1.0
 * @date 2026-04-04
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT License
 */

#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <unordered_map>

namespace regeocode {

/**
 * @class CountryAdapter
 * @brief Provides access to country information stored in a local JSON file.
 *
 * This adapter allows querying country details such as common name, official name,
 * capital, region, and flag using an ISO 3166-1 alpha-2 country code.
 */
class CountryAdapter {
public:
  /**
   * @brief Initializes the adapter with the path to the countries JSON file.
   * @param json_path The filesystem path to data/countries.json.
   * @throws std::runtime_error if the file cannot be opened or parsed.
   */
  explicit CountryAdapter(const std::string &json_path);

  /**
   * @brief Retrieves country details for a given country code.
   *
   * @param country_code ISO 3166-1 alpha-2 country code (e.g., "de", "cn").
   * @return nlohmann::json A JSON object containing the requested country details:
   *         - name.common
   *         - name.official
   *         - capital
   *         - region
   *         - flag
   *         Returns an empty JSON object if the country code is not found.
   */
  [[nodiscard]] nlohmann::json
  get_country(std::string_view country_code) const;

private:
  std::unordered_map<std::string, nlohmann::json> indexed_data_;
};

} // namespace regeocode
