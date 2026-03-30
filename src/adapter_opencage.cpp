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

static inline std::string default_if_empty(const std::string &s,
                                           const std::string &def) {
  return s.empty() ? def : s;
}

AddressResult
OpenCageAdapter::parse_response(const std::string &response_body) const {
  nlohmann::json j = nlohmann::json::parse(response_body);

  AddressResult res;
  res.raw_json = response_body;

  if (j.contains("results") && !j["results"].empty()) {
    const auto &first = j["results"][0];

    if (first.contains("formatted")) {
      res.address_english = first["formatted"].get<std::string>();
      res.address_local = res.address_english;
    }

    if (first.contains("components") &&
        first["components"].contains("country_code")) {
      res.country_code = first["components"]["country_code"].get<std::string>();

      res.attributes["continent"] = default_if_empty(
          first["components"]["continent"].get<std::string>(), "");
      res.attributes["country"] = default_if_empty(
          first["components"]["country"].get<std::string>(), "");

      if (first["components"].contains("state")) {
        res.attributes["state"] = default_if_empty(
            first["components"]["state"].get<std::string>(), "");
      } else {
        res.attributes["state"] = "";
      }
      if (first["components"].contains("city")) {
        res.attributes["city"] = default_if_empty(
            first["components"]["city"].get<std::string>(), "");
      } else {
        res.attributes["city"] = "";
      }
    }
  }

  return res;
}

} // namespace regeocode
