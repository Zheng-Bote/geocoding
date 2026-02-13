/**
 * SPDX-FileComment: Nominatim (OpenStreetMap) API adapter
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file adapter_nominatim.hpp
 * @brief Implementation of the ApiAdapter for Nominatim (OpenStreetMap) API.
 * @version 0.1.0
 * @date 2026-02-13
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT License
 */

#pragma once

#include "api_adapter.hpp"

namespace regeocode {

/**
 * @brief Adapter for the Nominatim (OpenStreetMap) API.
 */
class NominatimAdapter : public ApiAdapter {
public:
  /**
   * @brief Returns the name of the adapter.
   *
   * @return std::string "nominatim".
   */
  [[nodiscard]] std::string name() const override { return "nominatim"; }

  /**
   * @brief Parses the Nominatim API JSON response.
   *
   * @param response_body The raw JSON response from Nominatim.
   * @return AddressResult The parsed address information.
   */
  [[nodiscard]] AddressResult parse_response(const std::string &response_body) const override;
};

} // namespace regeocode
