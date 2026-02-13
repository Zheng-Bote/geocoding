/**
 * SPDX-FileComment: API Adapter base class and address result structure
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file api_adapter.hpp
 * @brief Definition of the ApiAdapter base class and AddressResult structure.
 * @version 0.1.0
 * @date 2026-02-13
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT License
 */

#pragma once

#include <memory>
#include <string>

/**
 * @brief Namespace for the re-geocode library.
 */
namespace regeocode {

/**
 * @brief Structure representing the result of a reverse geocoding request.
 */
struct AddressResult {
  /**
   * @brief Formatted address in English.
   */
  std::string address_english;

  /**
   * @brief Formatted address in the local language of the location.
   */
  std::string address_local;

  /**
   * @brief The raw JSON response from the geocoding service.
   */
  std::string raw_json;

  /**
   * @brief The country code (ISO 3166-1 alpha-2) of the location.
   */
  std::string country_code;
};

/**
 * @brief Abstract base class for API adapters.
 *
 * Each geocoding service (e.g., Nominatim, Google) requires a specific adapter
 * to parse its response format.
 */
class ApiAdapter {
public:
  /**
   * @brief Virtual destructor.
   */
  virtual ~ApiAdapter() = default;

  /**
   * @brief Returns the name of the adapter.
   *
   * @return std::string The name of the adapter (e.g., "nominatim").
   */
  [[nodiscard]] virtual std::string name() const = 0;

  /**
   * @brief Parses the raw HTTP response body into an AddressResult.
   *
   * @param response_body The raw response body from the API.
   * @return AddressResult The parsed address information.
   */
  [[nodiscard]] virtual AddressResult
  parse_response(const std::string &response_body) const = 0;
};

/**
 * @brief Type alias for a unique pointer to an ApiAdapter.
 */
using ApiAdapterPtr = std::unique_ptr<ApiAdapter>;

} // namespace regeocode
