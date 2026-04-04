/**
 * SPDX-FileComment: Implementation of the Country adapter.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file adapter_country.cpp
 * @brief Implementation of the Country info lookup from a local JSON file.
 * @version 0.1.0
 * @date 2026-04-04
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT License
 */

#include "regeocode/adapter_country.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <stdexcept>
#include <string>

namespace regeocode {

CountryAdapter::CountryAdapter(const std::string &json_path) {
  std::ifstream f(json_path);
  if (!f.is_open()) {
    throw std::runtime_error("Could not open country data file: " + json_path);
  }

  nlohmann::json countries_data;
  f >> countries_data;

  if (!countries_data.is_array()) {
    throw std::runtime_error("Invalid country data format: expected an array.");
  }

  for (const auto &item : countries_data) {
    if (item.contains("cca2") && item["cca2"].is_string()) {
      std::string code = item["cca2"].get<std::string>();
      std::ranges::transform(code, code.begin(),
                             [](unsigned char c) {
                               return static_cast<char>(std::toupper(c));
                             });

      nlohmann::json entry;
      if (item.contains("name")) {
        entry["name.common"] = item["name"].value("common", "");
        entry["name.official"] = item["name"].value("official", "");
      } else {
        entry["name.common"] = "";
        entry["name.official"] = "";
      }

      if (item.contains("capital") && item["capital"].is_array() &&
          !item["capital"].empty()) {
        entry["capital"] = item["capital"][0];
      } else {
        entry["capital"] = "";
      }

      entry["region"] = item.value("region", "");
      entry["flag"] = item.value("flag", "");

      std::string code_lower = code;
      std::ranges::transform(code_lower, code_lower.begin(),
                             [](unsigned char c) {
                               return static_cast<char>(std::tolower(c));
                             });
      entry["flag_url"] =
          "https://github.com/lipis/flag-icons/blob/main/flags/4x3/" +
          code_lower + ".svg";

      indexed_data_[code] = std::move(entry);
    }
  }
}

nlohmann::json CountryAdapter::get_country(std::string_view country_code) const {
  std::string target_code{country_code};
  std::ranges::transform(target_code, target_code.begin(),
                         [](unsigned char c) {
                           return static_cast<char>(std::toupper(c));
                         });

  if (auto it = indexed_data_.find(target_code); it != indexed_data_.end()) {
    return it->second;
  }

  return nlohmann::json::object();
}

} // namespace regeocode
