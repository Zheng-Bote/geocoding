/**
 * SPDX-FileComment: Header file for the Tides API adapter.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file adapter_tides.hpp
 * @brief Adapter for the Tides API.
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
 * @brief Adapter implementation for the Tides API.
 */
class TidesAdapter : public ApiAdapter {
public:
  /**
   * @brief Gets the name of the API adapter.
   * @return std::string "tides".
   */
  std::string name() const override { return "tides"; }

  /**
   * @brief Parses the JSON response from the Tides API.
   * @param response_body The raw JSON response string.
   * @return AddressResult The parsed address result.
   */
  AddressResult parse_response(const std::string &response_body) const override;
};

} // namespace regeocode