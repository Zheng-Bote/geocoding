/**
 * SPDX-FileComment: Implementation of the GeoNames Wikipedia API adapter.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file adapter_geonames_wikipedia.cpp
 * @brief Implementation of the GeoNames Wikipedia API adapter.
 * @version 0.1.0
 * @date 2026-02-14
 *
 * @author ZHENG Robert
 * @license MIT License
 */

#include "regeocode/adapter_geonames_wikipedia.hpp"
#include <nlohmann/json.hpp>

namespace regeocode {

AddressResult GeoNamesWikipediaAdapter::parse_response(
    const std::string &response_body) const {
  AddressResult res;
  res.raw_json = response_body;

  try {
    auto j = nlohmann::json::parse(response_body);

    // GeoNames often delivers a "geonames" array
    if (j.contains("geonames") && j["geonames"].is_array() &&
        !j["geonames"].empty()) {

      // We take the first element (usually the nearest)
      const auto &first = j["geonames"][0];

      // Title -> English (e.g. "Glärnisch")
      if (first.contains("title")) {
        res.address_english = first["title"].get<std::string>();
      }

      // Summary -> Local
      if (first.contains("summary")) {
        res.address_local = first["summary"].get<std::string>();
      }

      // Wikipedia URL -> Append to Local
      if (first.contains("wikipediaUrl")) {
        std::string wikiUrl = first["wikipediaUrl"].get<std::string>();

        // GeoNames often returns URLs without protocol (e.g.
        // "en.wikipedia.org/..."). We leave it "raw" but put it in
        // parentheses behind.
        if (!res.address_local.empty()) {
          res.address_local += " (" + wikiUrl + ")";
        } else {
          // If no summary is there, the URL is the only content
          res.address_local = wikiUrl;
        }
      }

      if (first.contains("countryCode")) {
        res.country_code = first["countryCode"].get<std::string>();
      }
    }

  } catch (const nlohmann::json::exception &) {
    // Ignore parsing errors, return empty result
  }

  return res;
}

} // namespace regeocode