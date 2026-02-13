#include "regeocode/adapter_geonames_timezone.hpp"
#include <nlohmann/json.hpp>

namespace regeocode {

AddressResult GeoNamesTimezoneAdapter::parse_response(
    const std::string &response_body) const {
  AddressResult res;
  res.raw_json = response_body;

  try {
    auto j = nlohmann::json::parse(response_body);

    // Fehlerbehandlung: GeoNames gibt oft ein "status"-Objekt bei Fehlern
    // zurück
    if (j.contains("status")) {
      // Wenn wir Zeit/Lust haben, könnten wir hier eine Exception werfen
      // oder das in raw_json lassen. Für jetzt parsen wir einfach weiter,
      // falls Daten da sind.
    }

    // Mapping der Felder
    if (j.contains("countryCode")) {
      res.country_code = j["countryCode"].get<std::string>();
    }

    // Wir nutzen address_english für die Zeitzone-ID
    if (j.contains("timezoneId")) {
      res.address_english = j["timezoneId"].get<std::string>();
    }

    // Wir nutzen address_local für die lokale Zeit
    if (j.contains("time")) {
      res.address_local = j["time"].get<std::string>();
    }

    // Optional: Falls countryName existiert und wir noch keine "Adresse" haben,
    // könnten wir das ergänzen. Hier hängen wir es an die Zeitzone an,
    // damit der User mehr Infos hat.
    if (j.contains("countryName")) {
      std::string cname = j["countryName"].get<std::string>();
      if (!res.address_english.empty()) {
        res.address_english += " (" + cname + ")";
      } else {
        res.address_english = cname;
      }
    }

  } catch (const nlohmann::json::exception &) {
    // JSON Fehler ignorieren, leeres Result zurückgeben (oder raw_json bleibt
    // gesetzt)
  }

  return res;
}

} // namespace regeocode