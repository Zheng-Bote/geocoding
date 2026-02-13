/**
 * SPDX-FileComment: HTTP client implementation using libcurl
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file http_client.cpp
 * @brief Implementation of the HttpClient class using libcurl.
 * @version 0.1.0
 * @date 2026-02-13
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT License
 */

#include "regeocode/http_client.hpp"
#include <curl/curl.h>
#include <mutex>
#include <stdexcept>

namespace regeocode {

namespace {
/**
 * @brief Global initialization flag for libcurl thread-safety.
 */
std::once_flag curl_init_flag;

/**
 * @brief Initializes libcurl globally once.
 */
void init_curl_once() { curl_global_init(CURL_GLOBAL_ALL); }

/**
 * @brief Callback function for libcurl to write received data into a string.
 *
 * @param ptr Pointer to the received data.
 * @param size Size of each data element.
 * @param nmemb Number of elements.
 * @param userdata Pointer to the std::string where data should be appended.
 * @return size_t Total number of bytes processed.
 */
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
  auto *out = static_cast<std::string *>(userdata);
  out->append(ptr, size * nmemb);
  return size * nmemb;
}
} // namespace

HttpClient::HttpClient() {
  // Ensure CURL is globally initialized
  std::call_once(curl_init_flag, init_curl_once);
}

HttpClient::~HttpClient() {
  // We don't call curl_global_cleanup here as it can cause race conditions
  // in multi-threaded environments during shutdown. The OS will clean up.
}

HttpResponse
HttpClient::get(const std::string &url,
                const std::map<std::string, std::string> &headers) const {
  CURL *curl = curl_easy_init();
  if (!curl)
    throw std::runtime_error("Failed to init CURL");

  std::string response_body;

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "re-geocode/1.0");
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

  // Header Handling
  struct curl_slist *header_list = nullptr;
  for (const auto &[key, value] : headers) {
    std::string h = key + ": " + value;
    header_list = curl_slist_append(header_list, h.c_str());
  }
  if (header_list)
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

  CURLcode res = curl_easy_perform(curl);

  long status_code = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);

  if (header_list)
    curl_slist_free_all(header_list);
  curl_easy_cleanup(curl);

  if (res != CURLE_OK)
    throw std::runtime_error(curl_easy_strerror(res));

  return {status_code, response_body};
}

} // namespace regeocode
