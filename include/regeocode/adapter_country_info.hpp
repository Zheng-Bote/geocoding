/**
 * SPDX-FileComment: Header file for the Country Info API adapter.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file adapter_country_info.hpp
 * @brief Adapter for the RestCountries API (Country Info).
 * @version 0.1.0
 * @date 2026-04-06
 *
 * @author ZHENG Robert
 * @license MIT License
 */

#pragma once

#include "api_adapter.hpp"

namespace regeocode {

/**
 * @brief Adapter implementation for the RestCountries API.
 */
class CountryInfoAdapter : public ApiAdapter {
public:
  /**
   * @brief Gets the name of the API adapter.
   * @return std::string "country_info".
   */
  std::string name() const override { return "country_info"; }

  /**
   * @brief Parses the JSON response from the RestCountries API.
   * @param response_body The raw JSON response string.
   * @return AddressResult The parsed address result.
   */
  AddressResult parse_response(const std::string &response_body) const override;
};

} // namespace regeocode
