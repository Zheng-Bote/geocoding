#include "regeocode/re_geocode_c_api.h"
// ... alle anderen includes ...
#include "regeocode/re_geocode_core.hpp"
#include <cstring>
#include <nlohmann/json.hpp>

using namespace regeocode;

struct geocoder_t {
  std::unique_ptr<ReverseGeocoder> impl;
};

// Helper bleibt gleich
char *str_dup(const std::string &s) {
  if (s.empty())
    return nullptr;
  char *res = new char[s.size() + 1];
  std::strcpy(res, s.c_str());
  return res;
}

// ... geocoder_new und geocoder_free bleiben gleich ...

geocode_result_t geocoder_lookup(geocoder_t *handle, double lat, double lon,
                                 const char *api_name,
                                 const char *local_lang_override) {
  // Init mit null
  geocode_result_t c_res = {nullptr, nullptr, nullptr, nullptr, 0};

  if (!handle || !handle->impl)
    return c_res;

  try {
    std::string lang = local_lang_override ? local_lang_override : "";

    // 1. Wir holen uns das standardisierte JSON Objekt vom Core
    nlohmann::json j_root =
        handle->impl->reverse_geocode_json({lat, lon}, api_name, lang);

    // 2. JSON String für C erzeugen (dump)
    std::string json_str = j_root.dump(); // oder .dump(4) für pretty print
    c_res.json_full = str_dup(json_str);

    // 3. Die klassischen Felder füllen wir zur Bequemlichkeit trotzdem
    // Wir holen sie aus dem JSON "result" Teil, da reverse_geocode_json diese
    // schon sauber aufbereitet hat.
    if (j_root.contains("result")) {
      const auto &res = j_root["result"];

      // Je nach Typ (info/geocoding) heißen die Felder leicht anders,
      // aber unser JSON-Schema im Core hat das vereinheitlicht:
      // "title" oder "address_english" -> wir prüfen beides oder nutzen den
      // Core Output

      // Da wir im Core nun unterscheiden, ist es am sichersten,
      // wenn wir kurz schauen, was da ist.
      // Im Core Mapping:
      // Geocoding: address_english, address_local
      // Info: title, summary

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
    // Optional: Error message in json_full schreiben
    // c_res.json_full = str_dup(std::string("{\"error\": \"") + e.what() +
    // "\"}");
  }
  return c_res;
}

void geocoder_result_free(geocode_result_t *res) {
  if (!res)
    return;
  delete[] res->address_english;
  delete[] res->address_local;
  delete[] res->country_code;
  delete[] res->json_full; // <--- WICHTIG: Auch den neuen String freigeben

  res->address_english = nullptr;
  res->address_local = nullptr;
  res->country_code = nullptr;
  res->json_full = nullptr;
}