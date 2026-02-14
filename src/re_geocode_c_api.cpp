/**
 * SPDX-FileComment: Implementation of the C API.
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file re_geocode_c_api.cpp
 * @brief Wrapper for the C++ Core API to C.
 * @version 0.1.0
 * @date 2026-02-14
 *
 * @author ZHENG Robert
 * @license MIT License
 */

#include "regeocode/re_geocode_c_api.h"
#include "regeocode/re_geocode_core.hpp"

// Adapter Includes
#include "regeocode/adapter_geonames_timezone.hpp"
#include "regeocode/adapter_geonames_wikipedia.hpp"
#include "regeocode/adapter_google.hpp"
#include "regeocode/adapter_marea_tides.hpp"
#include "regeocode/adapter_nominatim.hpp"
#include "regeocode/adapter_opencage.hpp"
#include "regeocode/adapter_openweather.hpp"
#include "regeocode/adapter_pollution.hpp"
#include "regeocode/adapter_seaweather.hpp"
#include "regeocode/adapter_tides.hpp"

#include "regeocode/http_client.hpp"

#include <cstring>
#include <iostream>
#include <nlohmann/json.hpp>

using namespace regeocode;

struct geocoder_t {
  std::unique_ptr<ReverseGeocoder> impl;
};

// Helper to copy strings for C
static char *str_dup(const std::string &s) {
  if (s.empty())
    return nullptr;
  char *res = new char[s.size() + 1];
  std::strcpy(res, s.c_str());
  return res;
}

// --- HERE IS THE IMPORTANT CHANGE ---
geocoder_t *geocoder_new(const char *ini_path) {
  if (!ini_path)
    return nullptr;

  try {
    ConfigLoader loader(ini_path);
    // 1. Now loads the Configuration Struct (APIs + Quota Path)
    auto config_result = loader.load();

    std::vector<ApiAdapterPtr> adapters;
    adapters.push_back(std::make_unique<NominatimAdapter>());
    adapters.push_back(std::make_unique<GoogleAdapter>());
    adapters.push_back(std::make_unique<OpenCageAdapter>());
    adapters.push_back(std::make_unique<GeoNamesTimezoneAdapter>());
    adapters.push_back(std::make_unique<GeoNamesWikipediaAdapter>());
    adapters.push_back(std::make_unique<OpenWeatherAdapter>());
    adapters.push_back(std::make_unique<PollutionAdapter>());
    adapters.push_back(std::make_unique<MareaTidesAdapter>());
    adapters.push_back(std::make_unique<TidesAdapter>());
    adapters.push_back(std::make_unique<SeaWeatherAdapter>());

    auto client = std::make_unique<HttpClient>();

    auto *ptr = new geocoder_t();

    // 2. Call constructor with quota path
    ptr->impl = std::make_unique<ReverseGeocoder>(
        std::move(config_result.apis), std::move(adapters), std::move(client),
        config_result.quota_file_path // <--- New parameter from Config Struct
    );
    return ptr;
  } catch (const std::exception &e) {
    std::cerr << "C-API Init Error: " << e.what() << std::endl;
    return nullptr;
  }
}

void geocoder_free(geocoder_t *handle) { delete handle; }

geocode_result_t geocoder_lookup(geocoder_t *handle, double lat, double lon,
                                 const char *api_name,
                                 const char *local_lang_override) {
  geocode_result_t c_res = {nullptr, nullptr, nullptr, nullptr, 0};

  if (!handle || !handle->impl)
    return c_res;

  try {
    std::string lang = local_lang_override ? local_lang_override : "";

    // 1. We get the standardized JSON object from Core
    nlohmann::json j_root =
        handle->impl->reverse_geocode_json({lat, lon}, api_name, lang);

    // 2. Create JSON string for C (dump)
    std::string json_str = j_root.dump();
    c_res.json_full = str_dup(json_str);

    // 3. Fallback mapping for old C-Struct fields
    if (j_root.contains("result")) {
      const auto &res = j_root["result"];
      std::string s_eng, s_loc, s_cc;

      if (res.contains("address_english"))
        s_eng = res["address_english"];
      else if (res.contains("title"))
        s_eng = res["title"];

      if (res.contains("address_local"))
        s_loc = res["address_local"];
      else if (res.contains("summary"))
        s_loc = res["summary"];

      if (res.contains("country_code"))
        s_cc = res["country_code"];

      c_res.address_english = str_dup(s_eng);
      c_res.address_local = str_dup(s_loc);
      c_res.country_code = str_dup(s_cc);
    }

    c_res.success = 1;

  } catch (const std::exception &e) {
    c_res.success = 0;
    // Optional: One could write the error into the JSON if changing the API
    // design.
  }
  return c_res;
}

void geocoder_result_free(geocode_result_t *res) {
  if (!res)
    return;
  delete[] res->address_english;
  delete[] res->address_local;
  delete[] res->country_code;
  delete[] res->json_full;

  res->address_english = nullptr;
  res->address_local = nullptr;
  res->country_code = nullptr;
  res->json_full = nullptr;
}