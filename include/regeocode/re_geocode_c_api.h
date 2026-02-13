/**
 * SPDX-FileComment: C API for the re-geocode library
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file re_geocode_c_api.h
 * @brief C-compatible interface for the re-geocode library.
 * @version 0.1.0
 * @date 2026-02-13
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT License
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque handle to a ReverseGeocoder instance.
 */
typedef struct geocoder_t geocoder_t;

/**
 * @brief Structure representing the result of a reverse geocoding request in C.
 */
typedef struct {
  /**
   * @brief Formatted address in English (dynamically allocated).
   */
  char *address_english;

  /**
   * @brief Formatted address in the local language (dynamically allocated).
   */
  char *address_local;

  /**
   * @brief The country code (dynamically allocated).
   */
  char *country_code;

  /**
   * @brief Success flag (1 = ok, 0 = error).
   */
  int success;
} geocode_result_t;

/**
 * @brief Creates a new geocoder instance.
 *
 * @param ini_path Path to the configuration INI file.
 * @return geocoder_t* Pointer to the new geocoder instance, or NULL on error.
 */
geocoder_t *geocoder_new(const char *ini_path);

/**
 * @brief Frees a geocoder instance.
 *
 * @param handle The geocoder handle to free.
 */
void geocoder_free(geocoder_t *handle);

/**
 * @brief Performs a reverse geocode lookup.
 *
 * @param handle The geocoder handle.
 * @param lat Latitude.
 * @param lon Longitude.
 * @param api_name The name of the API to use.
 * @param local_lang_override Optional language code override (can be NULL).
 * @return geocode_result_t The result of the lookup. Memory must be freed with geocoder_result_free.
 */
geocode_result_t geocoder_lookup(geocoder_t *handle, double lat, double lon,
                                 const char *api_name,
                                 const char *local_lang_override);

/**
 * @brief Frees the memory associated with a geocode result.
 *
 * @param res Pointer to the result structure to free.
 */
void geocoder_result_free(geocode_result_t *res);

#ifdef __cplusplus
}
#endif
