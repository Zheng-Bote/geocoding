#include "regeocode/adapter_openweather.hpp"
#include <iomanip> // für setprecision
#include <nlohmann/json.hpp>
#include <sstream> // für stringstream
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

    // Wir nutzen double/int statt string für die Zahlenwerte
    double temp = 0.0;
    double feels_like = 0.0;
    double temp_min = 0.0;
    double temp_max = 0.0;
    int pressure = 0;
    int humidity = 0;

    double wind_speed = 0.0;
    int wind_deg = 0;

    bool has_temp = false;
    bool has_wind = false;

    // Beschreibung holen (z.B. "clear sky")
    if (j.contains("weather") && j["weather"].is_array() &&
        !j["weather"].empty()) {
      const auto &w = j["weather"][0];
      if (w.contains("description")) {
        description = w["description"].get<std::string>();
      }
    }

    // Temperatur & Main Daten holen (Zahlen, keine Strings!)
    if (j.contains("main")) {
      auto &m = j["main"];
      if (m.contains("temp")) {
        temp = m["temp"].get<double>();
        has_temp = true;
      }
      if (m.contains("feels_like"))
        feels_like = m["feels_like"].get<double>();
      if (m.contains("temp_min"))
        temp_min = m["temp_min"].get<double>();
      if (m.contains("temp_max"))
        temp_max = m["temp_max"].get<double>();
      if (m.contains("pressure"))
        pressure = m["pressure"].get<int>();
      if (m.contains("humidity"))
        humidity = m["humidity"].get<int>();
    }

    // Wind holen
    if (j.contains("wind")) {
      auto &w = j["wind"];
      if (w.contains("speed")) {
        wind_speed = w["speed"].get<double>();
        has_wind = true;
      }
      if (w.contains("deg")) {
        wind_deg = w["deg"].get<int>();
      }
    }

    // String zusammenbauen (nur EIN Block)
    std::ostringstream oss;
    if (!description.empty()) {
      oss << description;
    }

    if (has_temp) {
      if (!description.empty())
        oss << ", ";
      // fixed und setprecision(1) sorgt für "5.8" statt "5.750000"
      oss << std::fixed << std::setprecision(1) << temp << "°C"
          << " (Feels: " << feels_like << "°C, "
          << "Min: " << temp_min << "°C, "
          << "Max: " << temp_max << "°C)";

      oss << ", " << pressure << "hPa";
      oss << ", " << humidity << "% hum.";
    }

    if (has_wind) {
      if (has_temp || !description.empty())
        oss << ", ";
      oss << "Wind: " << std::fixed << std::setprecision(1) << wind_speed
          << "m/s (" << wind_deg << "°)";
    }

    res.address_local = oss.str();

  } catch (const nlohmann::json::exception &) {
    // Parsing-Fehler ignorieren, leeres Result oder Raw zurückgeben
  }

  return res;
}

} // namespace regeocode