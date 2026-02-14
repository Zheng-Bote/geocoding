/**
 * SPDX-FileComment: Header file for the Pollution API adapter.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file adapter_pollution.hpp
 * @brief Adapter for the Pollution API.
 * @version 0.1.0
 * @date 2026-02-14
 *
 * @author ZHENG Robert
 * @license MIT License
 */

#pragma once

#include "api_adapter.hpp"

namespace regeocode {

/**
 * @brief Adapter implementation for the Pollution API.
 */
class PollutionAdapter : public ApiAdapter {
public:
  /**
   * @brief Gets the name of the API adapter.
   *
   * Name must match "Adapter = pollution" in the INI file.
   *
   * @return std::string "pollution".
   */
  std::string name() const override { return "pollution"; }

  /**
   * @brief Parses the JSON response from the Pollution API.
   * @param response_body The raw JSON response string.
   * @return AddressResult The parsed address result.
   */
  AddressResult parse_response(const std::string &response_body) const override;
};

} // namespace regeocode