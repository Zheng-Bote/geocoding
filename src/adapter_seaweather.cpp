#include "regeocode/adapter_seaweather.hpp"
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

namespace regeocode {

AddressResult
SeaWeatherAdapter::parse_response(const std::string &response_body) const {
  AddressResult result;

  // Defaults
  result.address_english = "Sea Weather Conditions";
  result.address_local = "No data available";
  result.country_code = "";

  try {
    auto root = nlohmann::json::parse(response_body);

    // 1. Meta-Daten verarbeiten
    if (root.contains("meta")) {
      const auto &meta = root["meta"];
      if (meta.contains("lat"))
        result.attributes["meta_lat"] =
            std::to_string(meta["lat"].get<double>());
      if (meta.contains("lng"))
        result.attributes["meta_lng"] =
            std::to_string(meta["lng"].get<double>());
      if (meta.contains("dailyQuota"))
        result.attributes["meta_dailyQuota"] =
            std::to_string(meta["dailyQuota"].get<long>());
      if (meta.contains("requestCount"))
        result.attributes["meta_requestCount"] =
            std::to_string(meta["requestCount"].get<long>());
    }

    // 2. Wetterdaten (Hours)
    // Wir konzentrieren uns auf den ersten Eintrag (aktuellste Stunde),
    // um die Attribute nicht zu sprengen.
    if (root.contains("hours") && root["hours"].is_array() &&
        !root["hours"].empty()) {
      const auto &current = root["hours"][0];

      std::stringstream summary_ss;
      bool first_summary_item = true;

      // Zeitstempel
      if (current.contains("time")) {
        std::string t = current["time"].get<std::string>();
        result.attributes["time"] = t;
        summary_ss << "Time: " << t << " | ";
      }

      // Dynamisch durch alle Parameter iterieren (airTemperature, waveHeight,
      // etc.)
      for (auto &[key, val_obj] : current.items()) {
        if (key == "time")
          continue; // Schon behandelt

        if (val_obj.is_object()) {
          // Iteriere durch die Provider (smhi, noaa, sg, etc.)
          for (auto &[source, value] : val_obj.items()) {
            std::string attr_key = key + "_" + source;
            std::string attr_val;

            if (value.is_number()) {
              attr_val = std::to_string(value.get<double>());
            } else if (value.is_string()) {
              attr_val = value.get<std::string>();
            }

            result.attributes[attr_key] = attr_val;

            // Für die Summary nehmen wir nur den ersten gefundenen Provider pro
            // Kategorie um den String kurz zu halten (z.B. nur sg oder noaa)
            if (first_summary_item) {
              summary_ss << key << ": " << attr_val;
              first_summary_item = false;
            }
          }
          if (!first_summary_item) {
            // Reset flag for next category (aber wir wollen simple summary,
            // also fügen wir hier nichts kompliziertes hinzu, evtl. nur beim
            // allerersten Key) Bessere Summary Logik:
          }
        }
      }

      // Versuch einer sauberen Summary: Temperatur und Wellenhöhe priorisieren
      std::string temp = "";
      std::string wave = "";

      // Helper Lambda um irgendeinen Wert aus dem Objekt zu holen
      auto get_any_value = [&](const std::string &key) -> std::string {
        if (current.contains(key) && current[key].is_object() &&
            !current[key].empty()) {
          // Nimm den ersten Provider (z.B. begin()->value())
          auto it = current[key].begin();
          if (it.value().is_number())
            return std::to_string(it.value().get<double>());
          if (it.value().is_string())
            return it.value().get<std::string>();
        }
        return "";
      };

      temp = get_any_value("airTemperature");
      wave = get_any_value("waveHeight");

      std::stringstream loc;
      if (!temp.empty())
        loc << "Air Temp: " << temp << "°C ";
      if (!wave.empty())
        loc << "Wave: " << wave << "m";

      if (!loc.str().empty()) {
        result.address_local = loc.str();
      } else {
        result.address_local =
            summary_ss.str(); // Fallback auf generischen String
      }
    }

  } catch (const std::exception &e) {
    result.attributes["error"] = e.what();
  }

  return result;
}

} // namespace regeocode