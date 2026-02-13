/**
 * SPDX-FileComment: HTTP client for geocoding API requests
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file http_client.hpp
 * @brief Definition of the HttpClient class and HttpResponse structure.
 * @version 0.1.0
 * @date 2026-02-13
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT License
 */

#pragma once

#include <map>
#include <string>

namespace regeocode {

/**
 * @brief Structure representing an HTTP response.
 */
struct HttpResponse {
  /**
   * @brief The HTTP status code (e.g., 200, 404).
   */
  long status_code{};

  /**
   * @brief The body of the HTTP response as a string.
   */
  std::string body;
};

/**
 * @brief A simple HTTP client wrapper for making GET requests.
 *
 * This client is primarily used to interact with reverse geocoding APIs.
 */
class HttpClient {
public:
  /**
   * @brief Constructs a new HttpClient.
   */
  HttpClient();

  /**
   * @brief Destroys the HttpClient and cleans up resources.
   */
  ~HttpClient();

  /**
   * @brief Performs an HTTP GET request.
   *
   * @param url The URL to request.
   * @param headers An optional map of HTTP headers to include in the request.
   * @return HttpResponse The result of the HTTP request.
   */
  [[nodiscard]] HttpResponse
  get(const std::string &url,
      const std::map<std::string, std::string> &headers = {}) const;
};

} // namespace regeocode
