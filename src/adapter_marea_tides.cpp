/**
 * SPDX-FileComment: Implementation of the Marea Tides API adapter.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file adapter_marea_tides.cpp
 * @brief Implementation of the Marea Tides API adapter.
 * @version 0.1.0
 * @date 2026-02-14
 *
 * @author ZHENG Robert
 * @license MIT License
 */

#include "regeocode/adapter_marea_tides.hpp"
#include <iomanip>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <vector>

namespace regeocode {

AddressResult
MareaTidesAdapter::parse_response(const std::string &response_body) const {
  AddressResult result;

  // Defaults
  result.address_english = "Tide Information";
  result.address_local = "No data available";
  result.country_code = "";

  try {
    auto root = nlohmann::json::parse(response_body);

    // 1. Metadata
    std::string unit = "m";
    if (root.contains("unit"))
      unit = root["unit"].get<std::string>();
    result.attributes["unit"] = unit;

    if (root.contains("copyright"))
      result.attributes["copyright"] = root["copyright"].get<std::string>();
    if (root.contains("source"))
      result.attributes["source"] = root["source"].get<std::string>();

    // 2. Origin / Station Distance
    if (root.contains("origin")) {
      auto origin = root["origin"];
      if (origin.contains("distance") && origin.contains("unit")) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2)
           << origin["distance"].get<double>() << " "
           << origin["unit"].get<std::string>();
        result.attributes["station_distance"] = ss.str();
      }
    }

    // 3. Datums (Reference heights like LAT, HAT, MSL)
    if (root.contains("datums")) {
      auto datums = root["datums"];
      for (auto &[key, val] : datums.items()) {
        if (val.is_number()) {
          result.attributes["datum_" + key] = std::to_string(val.get<double>());
        }
      }
    }

    // 4. Extreme values (High/Low Tides)
    if (root.contains("extremes") && root["extremes"].is_array() &&
        !root["extremes"].empty()) {
      int i = 0;
      std::string summary_text = "";

      for (const auto &item : root["extremes"]) {
        std::string prefix = "event_" + std::to_string(i);

        std::string state = item["state"];
        std::string time = item["datetime"];
        double height = item["height"].get<double>();

        result.attributes[prefix + "_state"] = state;
        result.attributes[prefix + "_time"] = time;
        result.attributes[prefix + "_height"] = std::to_string(height);

        // Create summary for the very first event
        if (i == 0) {
          std::stringstream ss;
          ss << state << " (" << std::fixed << std::setprecision(2) << height
             << unit << ") at " << time;
          summary_text = ss.str();
        }

        i++;
        if (i >= 5)
          break;
      }
      result.address_local = summary_text;
    }

    // 5. Current trend (from "heights" array, first entry)
    if (root.contains("heights") && root["heights"].is_array() &&
        !root["heights"].empty()) {
      const auto &current = root["heights"][0];
      if (current.contains("height"))
        result.attributes["current_height"] =
            std::to_string(current["height"].get<double>());
      if (current.contains("state"))
        result.attributes["current_state"] =
            current["state"].get<std::string>();
    }

  } catch (const std::exception &e) {
    result.attributes["error"] = e.what();
  }

  return result;
}

} // namespace regeocode