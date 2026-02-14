/**
 * SPDX-FileComment: Header file for the GeoNames Wikipedia API adapter.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file adapter_geonames_wikipedia.hpp
 * @brief Adapter for the GeoNames Wikipedia API (find nearby Wikipedia
 * entries).
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
 * @brief Adapter implementation for the GeoNames Wikipedia API.
 */
class GeoNamesWikipediaAdapter : public ApiAdapter {
public:
  /**
   * @brief Gets the name of the API adapter.
   *
   * Name must match "Adapter = nearbyWikipedia" in the INI file.
   *
   * @return std::string "nearbyWikipedia".
   */
  std::string name() const override { return "nearbyWikipedia"; }

  /**
   * @brief Parses the JSON response from the GeoNames Wikipedia API.
   * @param response_body The raw JSON response string.
   * @return AddressResult The parsed address result (containing Wikipedia
   * info).
   */
  AddressResult parse_response(const std::string &response_body) const override;
};

} // namespace regeocode