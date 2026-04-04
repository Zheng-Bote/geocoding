/**
 * SPDX-FileComment: Implementation of the Nominatim API adapter.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file adapter_nominatim.cpp
 * @brief Implementation of the Nominatim Reverse Geocoding API adapter.
 * @version 0.3.0
 * @date 2026-03-31
 *
 * @author ZHENG Robert
 * @license MIT License
 */

#include "regeocode/adapter_nominatim.hpp"

#include <nlohmann/json.hpp>

namespace regeocode {

namespace {
/**
 * @brief Safely extracts a string from a JSON object.
 *
 * @param j The JSON object.
 * @param key The key to look for.
 * @return std::string The value if it's a string, otherwise an empty string.
 */
std::string get_string_safe(const nlohmann::json &j, const std::string &key) {
  if (j.contains(key) && j[key].is_string()) {
    return j[key].get<std::string>();
  }
  return "";
}
} // namespace

AddressResult
NominatimAdapter::parse_response(const std::string &response_body) const {
  nlohmann::json j = nlohmann::json::parse(response_body);

  AddressResult res;
  res.raw_json = response_body;

  res.address_english = get_string_safe(j, "display_name");
  res.address_local = res.address_english;

  if (j.contains("address") && j["address"].is_object()) {
    const auto &addr = j["address"];
    res.country_code = get_string_safe(addr, "country_code");
    res.attributes["country"] = get_string_safe(addr, "country");
    res.attributes["state"] = get_string_safe(addr, "state");

    if (addr.contains("city")) {
      res.attributes["city"] = get_string_safe(addr, "city");
    } else if (addr.contains("town")) {
      res.attributes["city"] = get_string_safe(addr, "town");
    } else if (addr.contains("village")) {
      res.attributes["city"] = get_string_safe(addr, "village");
    } else {
      res.attributes["city"] = "";
    }
  }

  return res;
}

} // namespace regeocode
