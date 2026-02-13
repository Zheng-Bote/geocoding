/**
 * SPDX-FileComment: Core reverse geocoding implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file re_geocode_core.cpp
 * @brief Implementation of configuration loading and geocoding logic.
 * @version 0.1.0
 * @date 2026-02-13
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT License
 */

#include "regeocode/re_geocode_core.hpp"

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "regeocode/inja.hpp"

#include <inicpp.h>
#include <nlohmann/json.hpp>

namespace regeocode {

namespace {

/**
 * @brief Checks if a language code is supported.
 *
 * @param lang The language code (e.g., "en").
 * @return true if supported, false otherwise.
 */
bool is_valid_language(const std::string &lang) {
  static const std::unordered_set<std::string> valid = {
      "en", "de",    "fr",    "es", "it", "ar", "ru", "pt", "nl", "pl",
      "zh", "zh-CN", "zh-TW", "ja", "ko", "tr", "sv", "no", "fi"};
  return valid.contains(lang);
}

/**
 * @brief Maps a country code to a likely local language.
 *
 * @param cc_raw The raw country code (e.g., "DE").
 * @return std::string The mapped language code.
 */
std::string language_from_country(const std::string &cc_raw) {
  std::string cc = cc_raw;
  for (auto &c : cc)
    c = static_cast<char>(::tolower(c));

  static const std::unordered_map<std::string, std::string> map = {
      {"de", "de"}, {"at", "de"}, {"ch", "de"},    {"ae", "ar"},
      {"sa", "ar"}, {"eg", "ar"}, {"cn", "zh-CN"}, {"tw", "zh-TW"},
      {"jp", "ja"}, {"kr", "ko"}, {"fr", "fr"},    {"es", "es"},
      {"it", "it"}, {"ru", "ru"}, {"pt", "pt"},    {"nl", "nl"}};

  auto it = map.find(cc);
  if (it != map.end())
    return it->second;
  return "en";
}

} // namespace

// -------------------------
// ConfigLoader
// -------------------------

ConfigLoader::ConfigLoader(std::string ini_path)
    : ini_path_(std::move(ini_path)) {}

std::unordered_map<std::string, ApiConfig> ConfigLoader::load() const {
  std::ifstream test(ini_path_);
  if (!test.good()) {
    throw std::runtime_error("INI file not found: " + ini_path_);
  }

  ini::IniFile ini;
  ini.load(ini_path_);

  std::unordered_map<std::string, ApiConfig> result;

  for (auto &sectionPair : ini) {
    const std::string &sectionName = sectionPair.first;
    ini::IniSection &section = sectionPair.second; // Must be non-const for [] access

    ApiConfig cfg;
    cfg.name = sectionName;

    if (section.count("URI") == 0)
      throw std::runtime_error("Missing URI in section: " + sectionName);

    if (section.count("API-Key") == 0)
      throw std::runtime_error("Missing API-Key in section: " + sectionName);

    cfg.uri_template = section["URI"].as<std::string>();
    cfg.api_key = section["API-Key"].as<std::string>();

    if (section.count("Adapter") != 0) {
      cfg.adapter = section["Adapter"].as<std::string>();

      // Clean up quotes
      if (!cfg.adapter.empty() && cfg.adapter.front() == '"' &&
          cfg.adapter.back() == '"' && cfg.adapter.size() >= 2) {
        cfg.adapter = cfg.adapter.substr(1, cfg.adapter.size() - 2);
      }
    } else {
      cfg.adapter = sectionName;
    }

    result.emplace(sectionName, std::move(cfg));
  }

  return result;
}

// -------------------------
// ReverseGeocoder
// -------------------------

ReverseGeocoder::ReverseGeocoder(
    std::unordered_map<std::string, ApiConfig> configs,
    std::vector<ApiAdapterPtr> adapters,
    std::unique_ptr<HttpClient> http_client)
    : configs_(std::move(configs)), http_client_(std::move(http_client)) {

  for (auto &a : adapters) {
    adapters_.emplace(a->name(), std::move(a));
  }
}

AddressResult
ReverseGeocoder::reverse_geocode(const Coordinates &coords,
                                 const std::string &api_name,
                                 const std::string &language_code) const {

  auto it = configs_.find(api_name);
  if (it == configs_.end()) {
    throw std::runtime_error("Unknown API: " + api_name);
  }

  const auto &cfg = it->second;

  auto adapter_it = adapters_.find(cfg.adapter);
  if (adapter_it == adapters_.end()) {
    throw std::runtime_error("No adapter registered for: " + cfg.adapter);
  }

  const auto &adapter = adapter_it->second;

  nlohmann::json params;
  params["latitude"] = coords.latitude;
  params["longitude"] = coords.longitude;
  params["apikey"] = cfg.api_key;
  params["lang"] = language_code;

  // Inja Rendering
  inja::Environment env;
  std::string url = env.render(cfg.uri_template, params);

  auto resp = http_client_->get(url);

  if (resp.status_code < 200 || resp.status_code >= 300) {
    throw std::runtime_error("HTTP error: " + std::to_string(resp.status_code));
  }

  return adapter->parse_response(resp.body);
}

AddressResult ReverseGeocoder::reverse_geocode_dual_language(
    const Coordinates &coords, const std::string &api_name,
    const std::string &user_lang) const {

  AddressResult result;

  // 1. Query English
  auto en = reverse_geocode(coords, api_name, "en");
  result.address_english = en.address_english;
  result.country_code = en.country_code;

  // 2. User language or local
  if (!user_lang.empty()) {
    if (is_valid_language(user_lang)) {
      auto local = reverse_geocode(coords, api_name, user_lang);
      result.address_local = local.address_english;
    } else {
      result.address_local.clear();
    }
    return result;
  }

  // 3. Auto-detect local via country code
  if (!en.country_code.empty()) {
    std::string local_lang = language_from_country(en.country_code);
    auto local = reverse_geocode(coords, api_name, local_lang);
    result.address_local = local.address_english;
  } else {
    result.address_local.clear();
  }

  return result;
}

} // namespace regeocode
