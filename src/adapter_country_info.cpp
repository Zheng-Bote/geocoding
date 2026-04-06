/**
 * SPDX-FileComment: Implementation of the Country Info API adapter.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file adapter_country_info.cpp
 * @brief Implementation of the RestCountries API adapter.
 * @version 0.1.0
 * @date 2026-04-06
 *
 * @author ZHENG Robert
 * @license MIT License
 */

#include "regeocode/adapter_country_info.hpp"

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

/**
 * @brief Safely extracts and stringifies a field (array or object).
 *
 * @param j The JSON object.
 * @param key The key to look for.
 * @return std::string The stringified value or empty string.
 */
std::string get_stringified_safe(const nlohmann::json &j, const std::string &key) {
  if (j.contains(key)) {
    if (j[key].is_string()) {
      return j[key].get<std::string>();
    }
    return j[key].dump();
  }
  return "";
}
} // namespace

AddressResult
CountryInfoAdapter::parse_response(const std::string &response_body) const {
  nlohmann::json full_json = nlohmann::json::parse(response_body);
  AddressResult res;
  res.raw_json = response_body;

  if (full_json.is_array() && !full_json.empty()) {
    const auto &j = full_json[0];

    res.country_code = get_string_safe(j, "cca2");
    res.address_english = get_stringified_safe(j.at("name"), "official");
    res.address_local = get_stringified_safe(j.at("name"), "common");

    res.attributes["cca2"] = res.country_code;
    res.attributes["cca3"] = get_string_safe(j, "cca3");
    res.attributes["capital"] = get_stringified_safe(j, "capital");
    res.attributes["region"] = get_string_safe(j, "region");
    res.attributes["subregion"] = get_string_safe(j, "subregion");
    
    if (j.contains("maps") && j["maps"].is_object()) {
      res.attributes["maps_osm"] = get_string_safe(j["maps"], "openStreetMaps");
    }
    
    res.attributes["continents"] = get_stringified_safe(j, "continents");
    
    if (j.contains("name") && j["name"].is_object()) {
      res.attributes["name_official"] = get_string_safe(j["name"], "official");
      res.attributes["name_common"] = get_string_safe(j["name"], "common");
    }
    
    res.attributes["currencies"] = get_stringified_safe(j, "currencies");
    
    if (j.contains("flags") && j["flags"].is_object()) {
      res.attributes["flag_png"] = get_string_safe(j["flags"], "png");
      res.attributes["flag_svg"] = get_string_safe(j["flags"], "svg");
      res.attributes["flag_alt"] = get_string_safe(j["flags"], "alt");
    }
  }

  return res;
}

} // namespace regeocode
