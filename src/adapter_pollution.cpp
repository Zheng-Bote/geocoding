#include "regeocode/adapter_pollution.hpp"
#include <map>
#include <nlohmann/json.hpp>
#include <sstream>

namespace regeocode {

AddressResult
PollutionAdapter::parse_response(const std::string &response_body) const {
  AddressResult res;
  res.raw_json = response_body;

  try {
    auto j = nlohmann::json::parse(response_body);

    if (j.contains("list") && j["list"].is_array() && !j["list"].empty()) {
      const auto &first = j["list"][0];

      // 1. AQI (Air Quality Index) -> address_english
      if (first.contains("main") && first["main"].contains("aqi")) {
        int aqi = first["main"]["aqi"].get<int>();

        // Mapping OpenWeatherMap AQI 1-5
        std::string quality;
        switch (aqi) {
        case 1:
          quality = "Good";
          break;
        case 2:
          quality = "Fair";
          break;
        case 3:
          quality = "Moderate";
          break;
        case 4:
          quality = "Poor";
          break;
        case 5:
          quality = "Very Poor";
          break;
        default:
          quality = "Unknown";
          break;
        }
        res.address_english =
            "AQI: " + std::to_string(aqi) + " (" + quality + ")";
      }

      // 2. Komponenten -> address_local (Komma-separierte Liste)
      if (first.contains("components")) {
        std::ostringstream oss;
        const auto &comps = first["components"];
        bool is_first = true;

        // Wir gehen sicherheitshalber spezifische Keys durch, um die
        // Reihenfolge zu haben, oder wir iterieren einfach über das JSON
        // Objekt. Hier iterieren wir über das Objekt für Flexibilität:
        for (auto &el : comps.items()) {
          if (!is_first) {
            oss << ", ";
          }
          oss << el.key() << ": " << el.value();
          is_first = false;
        }
        res.address_local = oss.str();
      }
    }

    // Country Code gibt diese API nicht zurück, bleibt leer.

  } catch (const nlohmann::json::exception &) {
    // Parsing Fehler ignorieren
  }

  return res;
}

} // namespace regeocode