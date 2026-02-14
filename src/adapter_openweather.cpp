/**
 * SPDX-FileComment: Implementation of the OpenWeather API adapter.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file adapter_openweather.cpp
 * @brief Implementation of the OpenWeather Geocoding API adapter.
 * @version 0.1.0
 * @date 2026-02-14
 *
 * @author ZHENG Robert
 * @license MIT License
 */

#include "regeocode/adapter_openweather.hpp"
#include <iomanip>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

namespace regeocode {

AddressResult
OpenWeatherAdapter::parse_response(const std::string &response_body) const {
  AddressResult res;
  res.raw_json = response_body;

  try {
    auto j = nlohmann::json::parse(response_body);

    // Title & Country
    if (j.contains("name"))
      res.address_english = j["name"].get<std::string>();
    if (j.contains("sys") && j["sys"].contains("country")) {
      res.country_code = j["sys"]["country"].get<std::string>();
    }

    // Fill attributes
    if (j.contains("weather") && !j["weather"].empty()) {
      res.address_local = j["weather"][0]["description"].get<std::string>();
      res.attributes["condition"] = res.address_local;
    }

    if (j.contains("main")) {
      auto &m = j["main"];
      res.attributes["temp"] = std::to_string(m.value("temp", 0.0));
      res.attributes["feels_like"] = std::to_string(m.value("feels_like", 0.0));
      res.attributes["pressure"] = std::to_string(m.value("pressure", 0));
      res.attributes["humidity"] = std::to_string(m.value("humidity", 0));
    }

    if (j.contains("wind")) {
      res.attributes["wind_speed"] =
          std::to_string(j["wind"].value("speed", 0.0));
      res.attributes["wind_deg"] = std::to_string(j["wind"].value("deg", 0));
    }

  } catch (...) {
  }
  return res;
}
} // namespace regeocode