/**
 * SPDX-FileComment: Implementation of the Tides API adapter.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file adapter_tides.cpp
 * @brief Implementation of the Tides API adapter.
 * @version 0.1.0
 * @date 2026-02-14
 *
 * @author ZHENG Robert
 * @license MIT License
 */

#include "regeocode/adapter_tides.hpp"
#include <iomanip>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <vector>

namespace regeocode {

AddressResult
TidesAdapter::parse_response(const std::string &response_body) const {
  AddressResult result;

  // Defaults
  result.address_english = "Tide Information";
  result.address_local = "No data available";
  result.country_code = ""; // Tides often don't have a country code in the root

  try {
    auto root = nlohmann::json::parse(response_body);

    // 1. Basic information
    std::string unit = "m";
    if (root.contains("unit"))
      unit = root["unit"].get<std::string>();
    result.attributes["unit"] = unit;

    if (root.contains("disclaimer")) {
      result.attributes["disclaimer"] = root["disclaimer"].get<std::string>();
    }

    // 2. Origin / Station Distance
    if (root.contains("origin")) {
      auto origin = root["origin"];
      if (origin.contains("distance") && origin.contains("unit")) {
        std::string dist = std::to_string(origin["distance"].get<double>()) +
                           " " + origin["unit"].get<std::string>();
        result.attributes["station_distance"] = dist;
      }
    }

    // 3. Process extreme values (High/Low Tides)
    // This is usually the most important thing for the user
    if (root.contains("extremes") && root["extremes"].is_array() &&
        !root["extremes"].empty()) {
      int i = 0;
      std::string summary_text = "";

      for (const auto &item : root["extremes"]) {
        std::string prefix =
            "event_" + std::to_string(i); // event_0, event_1 ...

        std::string state = item["state"];
        std::string time = item["datetime"];
        double height = item["height"].get<double>();

        // Write to attribute map
        result.attributes[prefix + "_state"] = state;
        result.attributes[prefix + "_time"] = time;
        result.attributes[prefix + "_height"] = std::to_string(height);

        // Use the very first entry as "Summary"
        if (i == 0) {
          std::stringstream ss;
          ss << state << " (" << std::fixed << std::setprecision(2) << height
             << unit << ") at " << time;
          summary_text = ss.str();
        }

        i++;
        if (i >= 5)
          break; // Save max 5 entries
      }

      result.address_local = summary_text;
    }

    // 4. Current/Next hourly forecast (optional)
    if (root.contains("heights") && root["heights"].is_array() &&
        !root["heights"].empty()) {
      const auto &current = root["heights"][0];
      result.attributes["current_height"] =
          std::to_string(current["height"].get<double>());
      result.attributes["current_state"] = current["state"];
    }

  } catch (const std::exception &e) {
    // Error handling: Empty result or error attribute
    result.attributes["error"] = e.what();
  }

  return result;
}

} // namespace regeocode