/**
 * SPDX-FileComment: Nominatim API adapter implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file adapter_nominatim.cpp
 * @brief Implementation of the Nominatim (OpenStreetMap) API adapter.
 * @version 0.1.0
 * @date 2026-02-13
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT License
 */

#include "regeocode/adapter_nominatim.hpp"

#include <nlohmann/json.hpp>

namespace regeocode {

AddressResult
NominatimAdapter::parse_response(const std::string &response_body) const {
  nlohmann::json j = nlohmann::json::parse(response_body);

  AddressResult res;
  res.raw_json = response_body;

  if (j.contains("display_name")) {
    res.address_english = j["display_name"].get<std::string>();
    res.address_local = res.address_english;
  }

  if (j.contains("address") && j["address"].contains("country_code")) {
    res.country_code = j["address"]["country_code"].get<std::string>();
  }

  return res;
}

} // namespace regeocode
