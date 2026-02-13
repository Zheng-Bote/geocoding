#include "regeocode/adapter_openweather.hpp"
#include <nlohmann/json.hpp>
#include <string>

namespace regeocode {

AddressResult
OpenWeatherAdapter::parse_response(const std::string &response_body) const {
  AddressResult res;
  res.raw_json = response_body;

  try {
    auto j = nlohmann::json::parse(response_body);

    // 1. Ortsname -> address_english
    if (j.contains("name")) {
      res.address_english = j["name"].get<std::string>();
    }

    // 2. Land -> country_code
    if (j.contains("sys") && j["sys"].contains("country")) {
      res.country_code = j["sys"]["country"].get<std::string>();
    }

    // 3. Wetterdaten -> address_local
    std::string description;
    double temp = -999.0;
    bool has_temp = false;

    // Beschreibung holen (z.B. "clear sky")
    if (j.contains("weather") && j["weather"].is_array() &&
        !j["weather"].empty()) {
      const auto &w = j["weather"][0];
      if (w.contains("description")) {
        description = w["description"].get<std::string>();
      }
    }

    // Temperatur holen
    if (j.contains("main") && j["main"].contains("temp")) {
      temp = j["main"]["temp"].get<double>();
      has_temp = true;
    }

    // String zusammenbauen
    if (!description.empty()) {
      res.address_local = description;
      if (has_temp) {
        res.address_local += ", " + std::to_string(temp) + "°C";
      }
    } else if (has_temp) {
      res.address_local = std::to_string(temp) + "°C";
    }

  } catch (const nlohmann::json::exception &) {
    // Parsing-Fehler ignorieren, leeres Result
  }

  return res;
}

} // namespace regeocode