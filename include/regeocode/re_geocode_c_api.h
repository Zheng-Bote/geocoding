#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct geocoder_t geocoder_t;

// Result Struct für C
typedef struct {
  char *address_english;
  char *address_local;
  char *country_code;
  int success; // 1 = ok, 0 = error
} geocode_result_t;

// Konstruktor / Destruktor
geocoder_t *geocoder_new(const char *ini_path);
void geocoder_free(geocoder_t *handle);

// Funktion
geocode_result_t geocoder_lookup(geocoder_t *handle, double lat, double lon,
                                 const char *api_name,
                                 const char *local_lang_override);

// Ergebnis freigeben
void geocoder_result_free(geocode_result_t *res);

#ifdef __cplusplus
}
#endif