/**
 * SPDX-FileComment: C API implementation for the re-geocode library
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file re_geocode_c_api.cpp
 * @brief Implementation of the C-compatible interface.
 * @version 0.1.0
 * @date 2026-02-13
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT License
 */

#include "regeocode/re_geocode_c_api.h"
#include "regeocode/adapter_geonames_timezone.hpp"
#include "regeocode/adapter_geonames_wikipedia.hpp"
#include "regeocode/adapter_google.hpp"
#include "regeocode/adapter_nominatim.hpp"
#include "regeocode/adapter_opencage.hpp"
#include "regeocode/adapter_openweather.hpp"
#include "regeocode/adapter_pollution.hpp"
#include "regeocode/http_client.hpp"
#include "regeocode/re_geocode_core.hpp"

#include <cstring>
#include <iostream>

using namespace regeocode;

struct geocoder_t {
  std::unique_ptr<ReverseGeocoder> impl;
};

// Helper um Strings für C zu kopieren
char *str_dup(const std::string &s) {
  if (s.empty())
    return nullptr;
  char *res = new char[s.size() + 1];
  std::strcpy(res, s.c_str());
  return res;
}

geocoder_t *geocoder_new(const char *ini_path) {
  try {
    ConfigLoader loader(ini_path);
    auto configs = loader.load();

    std::vector<ApiAdapterPtr> adapters;
    adapters.push_back(std::make_unique<NominatimAdapter>());
    adapters.push_back(std::make_unique<GoogleAdapter>());
    adapters.push_back(std::make_unique<OpenCageAdapter>());
    adapters.push_back(std::make_unique<GeoNamesTimezoneAdapter>());
    adapters.push_back(std::make_unique<GeoNamesWikipediaAdapter>());
    adapters.push_back(std::make_unique<OpenWeatherAdapter>());
    adapters.push_back(std::make_unique<PollutionAdapter>());

    auto client = std::make_unique<HttpClient>();

    auto *ptr = new geocoder_t();
    ptr->impl = std::make_unique<ReverseGeocoder>(
        std::move(configs), std::move(adapters), std::move(client));
    return ptr;
  } catch (...) {
    return nullptr;
  }
}

void geocoder_free(geocoder_t *handle) { delete handle; }

geocode_result_t geocoder_lookup(geocoder_t *handle, double lat, double lon,
                                 const char *api_name,
                                 const char *local_lang_override) {
  geocode_result_t c_res = {nullptr, nullptr, nullptr, 0};
  if (!handle || !handle->impl)
    return c_res;

  try {
    std::string lang = local_lang_override ? local_lang_override : "";
    auto cpp_res =
        handle->impl->reverse_geocode_dual_language({lat, lon}, api_name, lang);

    c_res.address_english = str_dup(cpp_res.address_english);
    c_res.address_local = str_dup(cpp_res.address_local);
    c_res.country_code = str_dup(cpp_res.country_code);
    c_res.success = 1;

  } catch (const std::exception &e) {
    // Fehlerbehandlung könnte hier erweitert werden (z.B. Error String im
    // Struct)
    c_res.success = 0;
  }
  return c_res;
}

void geocode_result_free(geocode_result_t *res) {
  if (!res)
    return;
  delete[] res->address_english;
  delete[] res->address_local;
  delete[] res->country_code;
  res->address_english = nullptr;
  res->address_local = nullptr;
  res->country_code = nullptr;
}