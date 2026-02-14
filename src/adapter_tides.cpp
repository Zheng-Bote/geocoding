#include "regeocode/adapter_tides.hpp"
#include <iomanip>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <vector>

namespace regeocode {

AddressResult
TidesAdapter::parse_response(const std::string &response_body) const {
  AddressResult result;

  // Defaults
  result.address_english = "Tide Information";
  result.address_local = "No data available";
  result.country_code = ""; // Tides haben oft keinen Country Code im Root

  try {
    auto root = nlohmann::json::parse(response_body);

    // 1. Basis-Informationen
    std::string unit = "m";
    if (root.contains("unit"))
      unit = root["unit"].get<std::string>();
    result.attributes["unit"] = unit;

    if (root.contains("disclaimer")) {
      result.attributes["disclaimer"] = root["disclaimer"].get<std::string>();
    }

    // 2. Origin / Station Distance
    if (root.contains("origin")) {
      auto origin = root["origin"];
      if (origin.contains("distance") && origin.contains("unit")) {
        std::string dist = std::to_string(origin["distance"].get<double>()) +
                           " " + origin["unit"].get<std::string>();
        result.attributes["station_distance"] = dist;
      }
    }

    // 3. Extremwerte (High/Low Tides) verarbeiten
    // Das ist meist das Wichtigste für den User
    if (root.contains("extremes") && root["extremes"].is_array() &&
        !root["extremes"].empty()) {
      int i = 0;
      std::string summary_text = "";

      for (const auto &item : root["extremes"]) {
        std::string prefix =
            "event_" + std::to_string(i); // event_0, event_1 ...

        std::string state = item["state"];
        std::string time = item["datetime"];
        double height = item["height"].get<double>();

        // Ins Attribute Map schreiben
        result.attributes[prefix + "_state"] = state;
        result.attributes[prefix + "_time"] = time;
        result.attributes[prefix + "_height"] = std::to_string(height);

        // Den allerersten Eintrag als "Summary" nutzen
        if (i == 0) {
          std::stringstream ss;
          ss << state << " (" << std::fixed << std::setprecision(2) << height
             << unit << ") at " << time;
          summary_text = ss.str();
        }

        i++;
        if (i >= 5)
          break; // Max 5 Einträge speichern
      }

      result.address_local = summary_text;
    }

    // 4. Aktuelle/Nächste stündliche Vorhersage (optional)
    if (root.contains("heights") && root["heights"].is_array() &&
        !root["heights"].empty()) {
      const auto &current = root["heights"][0];
      result.attributes["current_height"] =
          std::to_string(current["height"].get<double>());
      result.attributes["current_state"] = current["state"];
    }

  } catch (const std::exception &e) {
    // Fehlerbehandlung: Leeres Result oder Error Attribute
    result.attributes["error"] = e.what();
  }

  return result;
}

} // namespace regeocode