#include "regeocode/adapter_bing.hpp"
#include <iostream>

namespace regeocode {

AddressResult
BingAdapter::parse_response(const std::string &response_body) const {
  AddressResult result;

  // Default values
  result.address_english = "";
  result.address_local = "";
  result.country_code = "";

  try {
    auto root = nlohmann::json::parse(response_body);

    // Azure Maps / Bing Struktur: { "addresses": [ { "address": { ... } } ] }
    if (root.contains("addresses") && root["addresses"].is_array() &&
        !root["addresses"].empty()) {

      const auto &first_hit = root["addresses"][0];

      if (first_hit.contains("address")) {
        const auto &addr = first_hit["address"];

        // 1. Hauptadresse (freeformAddress ist meist am besten lesbar)
        if (addr.contains("freeformAddress")) {
          result.address_english = addr["freeformAddress"].get<std::string>();
          // Bei Azure Maps wird die Sprache über den API-Parameter gesteuert,
          // daher ist das Resultat hier meist schon lokalisiert.
          result.address_local = result.address_english;
        }

        // 2. Ländercode
        if (addr.contains("countryCode")) {
          result.country_code = addr["countryCode"].get<std::string>();
        }

        // 3. Attribute befüllen (Alle Felder aus "address" übernehmen)
        for (auto &[key, val] : addr.items()) {
          if (val.is_string()) {
            result.attributes[key] = val.get<std::string>();
          } else if (val.is_number()) {
            // Zahlen in Strings wandeln
            if (val.is_number_integer()) {
              result.attributes[key] = std::to_string(val.get<long>());
            } else {
              result.attributes[key] = std::to_string(val.get<double>());
            }
          } else if (val.is_boolean()) {
            result.attributes[key] = val.get<bool>() ? "true" : "false";
          }
        }
      }

      // Optional: Position aus dem Root-Objekt des Hits holen, falls nötig
      if (first_hit.contains("position")) {
        result.attributes["position_raw"] =
            first_hit["position"].get<std::string>();
      }
    }

  } catch (const std::exception &e) {
    // Parsing error: return empty result or log
    // result.attributes["error"] = e.what();
  }

  return result;
}

} // namespace regeocode