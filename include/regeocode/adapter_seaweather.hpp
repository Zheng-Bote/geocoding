/**
 * SPDX-FileComment: Header file for the SeaWeather API adapter.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file adapter_seaweather.hpp
 * @brief Adapter for the SeaWeather API.
 * @version 0.1.0
 * @date 2026-02-14
 *
 * @author ZHENG Robert
 * @license MIT License
 */

#pragma once

#include "api_adapter.hpp"
#include <string>

namespace regeocode {

/**
 * @brief Adapter implementation for the SeaWeather API.
 */
class SeaWeatherAdapter : public ApiAdapter {
public:
  /**
   * @brief Gets the name of the API adapter.
   * @return std::string "seaweather".
   */
  std::string name() const override { return "seaweather"; }

  /**
   * @brief Parses the JSON response from the SeaWeather API.
   * @param response_body The raw JSON response string.
   * @return AddressResult The parsed address result.
   */
  AddressResult parse_response(const std::string &response_body) const override;
};

} // namespace regeocode