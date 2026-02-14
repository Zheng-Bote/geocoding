#include "regeocode/adapter_pollution.hpp"
#include <nlohmann/json.hpp>

namespace regeocode {

AddressResult
PollutionAdapter::parse_response(const std::string &response_body) const {
  AddressResult res;
  res.raw_json = response_body;

  try {
    auto j = nlohmann::json::parse(response_body);
    if (j.contains("list") && !j["list"].empty()) {
      const auto &first = j["list"][0];

      // AQI
      if (first.contains("main") && first["main"].contains("aqi")) {
        int aqi = first["main"]["aqi"].get<int>();
        res.attributes["aqi"] = std::to_string(aqi);

        static const char *ratings[] = {"",         "Good", "Fair",
                                        "Moderate", "Poor", "Very Poor"};
        if (aqi >= 1 && aqi <= 5)
          res.address_english = std::string("Air Quality: ") + ratings[aqi];
        else
          res.address_english = "Air Quality: Unknown";
      }

      // Komponenten in die Map schieben
      if (first.contains("components")) {
        for (auto &el : first["components"].items()) {
          // Speichert z.B. "co" -> "331.41"
          res.attributes[el.key()] = std::to_string(el.value().get<double>());
        }
      }
    }
  } catch (...) {
  }
  return res;
}
} // namespace regeocode