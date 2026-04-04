/**
 * SPDX-FileComment: Implementation of the Google Maps API adapter.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file adapter_google.cpp
 * @brief Implementation of the Google Maps Geocoding API adapter.
 * @version 0.3.0
 * @date 2026-03-31
 *
 * @author ZHENG Robert
 * @license MIT License
 */

#include "regeocode/adapter_google.hpp"

#include <algorithm>
#include <nlohmann/json.hpp>

namespace regeocode {

namespace {
std::string get_string_safe(const nlohmann::json &j, const std::string &key) {
  if (j.contains(key) && j[key].is_string()) {
    return j[key].get<std::string>();
  }
  return "";
}
} // namespace

AddressResult
GoogleAdapter::parse_response(const std::string &response_body) const {
  nlohmann::json j = nlohmann::json::parse(response_body);

  AddressResult res;
  res.raw_json = response_body;

  if (j.contains("results") && j["results"].is_array() &&
      !j["results"].empty()) {
    const auto &first = j["results"][0];

    res.address_english = get_string_safe(first, "formatted_address");
    res.address_local = res.address_english;

    if (first.contains("address_components") &&
        first["address_components"].is_array()) {
      for (const auto &comp : first["address_components"]) {
        if (!comp.is_object() || !comp.contains("types"))
          continue;

        const auto &types = comp["types"];
        if (!types.is_array())
          continue;

        if (std::find(types.begin(), types.end(), "country") != types.end()) {
          res.country_code = get_string_safe(comp, "short_name");
          res.attributes["country"] = get_string_safe(comp, "long_name");
        }
        if (std::find(types.begin(), types.end(),
                      "administrative_area_level_1") != types.end()) {
          res.attributes["state"] = get_string_safe(comp, "long_name");
        }
        if (std::find(types.begin(), types.end(), "locality") != types.end()) {
          res.attributes["city"] = get_string_safe(comp, "long_name");
        }
      }
    }
  }

  return res;
}

} // namespace regeocode
