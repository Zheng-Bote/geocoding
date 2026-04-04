/**
 * SPDX-FileComment: Implementation of the OpenCage Data API adapter.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file adapter_opencage.cpp
 * @brief Implementation of the OpenCage Data Geocoding API adapter.
 * @version 0.3.0
 * @date 2026-03-31
 *
 * @author ZHENG Robert
 * @license MIT License
 */

#include "regeocode/adapter_opencage.hpp"

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
OpenCageAdapter::parse_response(const std::string &response_body) const {
  nlohmann::json j = nlohmann::json::parse(response_body);

  AddressResult res;
  res.raw_json = response_body;

  if (j.contains("results") && j["results"].is_array() &&
      !j["results"].empty()) {
    const auto &first = j["results"][0];

    res.address_english = get_string_safe(first, "formatted");
    res.address_local = res.address_english;

    if (first.contains("components") && first["components"].is_object()) {
      const auto &comp = first["components"];
      res.country_code = get_string_safe(comp, "country_code");
      res.attributes["continent"] = get_string_safe(comp, "continent");
      res.attributes["country"] = get_string_safe(comp, "country");
      res.attributes["state"] = get_string_safe(comp, "state");

      if (comp.contains("city")) {
        res.attributes["city"] = get_string_safe(comp, "city");
      } else if (comp.contains("town")) {
        res.attributes["city"] = get_string_safe(comp, "town");
      } else if (comp.contains("village")) {
        res.attributes["city"] = get_string_safe(comp, "village");
      } else {
        res.attributes["city"] = "";
      }
    }
  }

  return res;
}

} // namespace regeocode
