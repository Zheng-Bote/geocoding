/**
 * SPDX-FileComment: Core reverse geocoding logic and configuration
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file re_geocode_core.hpp
 * @brief Core classes for loading configuration and performing reverse geocoding.
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
#include <unordered_map>
#include <vector>

#include "api_adapter.hpp"
#include "http_client.hpp"

namespace regeocode {

/**
 * @brief Structure representing geographic coordinates.
 */
struct Coordinates {
  /**
   * @brief Latitude in decimal degrees.
   */
  double latitude;

  /**
   * @brief Longitude in decimal degrees.
   */
  double longitude;
};

/**
 * @brief Configuration for a geocoding API.
 */
struct ApiConfig {
  /**
   * @brief The name of the API (e.g., "google").
   */
  std::string name;

  /**
   * @brief The URI template with placeholders for inja rendering.
   */
  std::string uri_template;

  /**
   * @brief The API key required for the service.
   */
  std::string api_key;

  /**
   * @brief The name of the adapter to use for this API.
   */
  std::string adapter;
};

/**
 * @brief Class responsible for loading API configurations from an INI file.
 */
class ConfigLoader {
public:
  /**
   * @brief Constructs a ConfigLoader with the path to the INI file.
   *
   * @param ini_path Path to the configuration file.
   */
  explicit ConfigLoader(std::string ini_path);

  /**
   * @brief Loads the configurations from the INI file.
   *
   * @return std::unordered_map<std::string, ApiConfig> A map of API names to their configurations.
   * @throws std::runtime_error if the INI file is not found or missing required fields.
   */
  [[nodiscard]] std::unordered_map<std::string, ApiConfig> load() const;

private:
  std::string ini_path_;
};

/**
 * @brief The main class for performing reverse geocoding operations.
 */
class ReverseGeocoder {
public:
  /**
   * @brief Constructs a ReverseGeocoder with configurations, adapters, and an optional HTTP client.
   *
   * @param configs A map of available API configurations.
   * @param adapters A vector of available API adapters.
   * @param http_client A unique pointer to an HttpClient instance.
   */
  ReverseGeocoder(
      std::unordered_map<std::string, ApiConfig> configs,
      std::vector<ApiAdapterPtr> adapters,
      std::unique_ptr<HttpClient> http_client = std::make_unique<HttpClient>());

  /**
   * @brief Performs a reverse geocode lookup.
   *
   * @param coords The coordinates to lookup.
   * @param api_name The name of the API to use.
   * @param language_code The language code for the response (default is "en").
   * @return AddressResult The resulting address information.
   * @throws std::runtime_error if the API is unknown or an HTTP error occurs.
   */
  [[nodiscard]] AddressResult reverse_geocode(const Coordinates &coords,
                                              const std::string &api_name,
                                              const std::string &language_code = "en") const;

  /**
   * @brief Performs a dual-language reverse geocode lookup.
   *
   * This method attempts to get the address in English and a secondary language.
   * If user_lang is empty, it attempts to auto-detect the local language based on the country code.
   *
   * @param coords The coordinates to lookup.
   * @param api_name The name of the API to use.
   * @param user_lang The desired secondary language code.
   * @return AddressResult The resulting address information with both English and local addresses.
   */
  [[nodiscard]] AddressResult
  reverse_geocode_dual_language(const Coordinates &coords,
                                const std::string &api_name,
                                const std::string &user_lang) const;

private:
  std::unordered_map<std::string, ApiConfig> configs_;
  std::unordered_map<std::string, ApiAdapterPtr> adapters_;
  std::unique_ptr<HttpClient> http_client_;
};

} // namespace regeocode
