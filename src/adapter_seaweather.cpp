/**
 * SPDX-FileComment: Implementation of the SeaWeather API adapter.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file adapter_seaweather.cpp
 * @brief Implementation of the SeaWeather API adapter.
 * @version 0.1.0
 * @date 2026-02-14
 *
 * @author ZHENG Robert
 * @license MIT License
 */

#include "regeocode/adapter_seaweather.hpp"
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

namespace regeocode {

AddressResult
SeaWeatherAdapter::parse_response(const std::string &response_body) const {
  AddressResult result;

  // Defaults
  result.address_english = "Sea Weather Conditions";
  result.address_local = "No data available";
  result.country_code = "";

  try {
    auto root = nlohmann::json::parse(response_body);

    // 1. Process Metadata
    if (root.contains("meta")) {
      const auto &meta = root["meta"];
      if (meta.contains("lat"))
        result.attributes["meta_lat"] =
            std::to_string(meta["lat"].get<double>());
      if (meta.contains("lng"))
        result.attributes["meta_lng"] =
            std::to_string(meta["lng"].get<double>());
      if (meta.contains("dailyQuota"))
        result.attributes["meta_dailyQuota"] =
            std::to_string(meta["dailyQuota"].get<long>());
      if (meta.contains("requestCount"))
        result.attributes["meta_requestCount"] =
            std::to_string(meta["requestCount"].get<long>());
    }

    // 2. Weather data (Hours)
    // We focus on the first entry (current hour) to avoid exploding the
    // attributes.
    if (root.contains("hours") && root["hours"].is_array() &&
        !root["hours"].empty()) {
      const auto &current = root["hours"][0];

      std::stringstream summary_ss;
      bool first_summary_item = true;

      // Timestamp
      if (current.contains("time")) {
        std::string t = current["time"].get<std::string>();
        result.attributes["time"] = t;
        summary_ss << "Time: " << t << " | ";
      }

      // Iterate dynamically through all parameters (airTemperature, waveHeight,
      // etc.)
      for (auto &[key, val_obj] : current.items()) {
        if (key == "time")
          continue; // Already handled

        if (val_obj.is_object()) {
          // Iterate through providers (smhi, noaa, sg, etc.)
          for (auto &[source, value] : val_obj.items()) {
            std::string attr_key = key + "_" + source;
            std::string attr_val;

            if (value.is_number()) {
              attr_val = std::to_string(value.get<double>());
            } else if (value.is_string()) {
              attr_val = value.get<std::string>();
            }

            result.attributes[attr_key] = attr_val;

            // For the summary we only take the first found provider per
            // category to keep the string short (e.g. only sg or noaa)
            if (first_summary_item) {
              summary_ss << key << ": " << attr_val;
              first_summary_item = false;
            }
          }
          if (!first_summary_item) {
            // Reset flag for next category (but we want simple summary,
            // so we don't add anything complicated here, maybe only for the
            // very first key) Better Summary Logic:
          }
        }
      }

      // Attempt at a clean summary: prioritize temperature and wave height
      std::string temp = "";
      std::string wave = "";

      // Helper lambda to get any value from the object
      auto get_any_value = [&](const std::string &key) -> std::string {
        if (current.contains(key) && current[key].is_object() &&
            !current[key].empty()) {
          // Take the first provider (e.g. begin()->value())
          auto it = current[key].begin();
          if (it.value().is_number())
            return std::to_string(it.value().get<double>());
          if (it.value().is_string())
            return it.value().get<std::string>();
        }
        return "";
      };

      temp = get_any_value("airTemperature");
      wave = get_any_value("waveHeight");

      std::stringstream loc;
      if (!temp.empty())
        loc << "Air Temp: " << temp << "°C ";
      if (!wave.empty())
        loc << "Wave: " << wave << "m";

      if (!loc.str().empty()) {
        result.address_local = loc.str();
      } else {
        result.address_local = summary_ss.str(); // Fallback to generic string
      }
    }

  } catch (const std::exception &e) {
    result.attributes["error"] = e.what();
  }

  return result;
}

} // namespace regeocode