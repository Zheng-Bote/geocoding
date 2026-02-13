#include "regeocode/adapter_geonames_wikipedia.hpp"
#include <nlohmann/json.hpp>

namespace regeocode {

AddressResult GeoNamesWikipediaAdapter::parse_response(
    const std::string &response_body) const {
  AddressResult res;
  res.raw_json = response_body;

  try {
    auto j = nlohmann::json::parse(response_body);

    // GeoNames liefert oft ein "geonames" Array
    if (j.contains("geonames") && j["geonames"].is_array() &&
        !j["geonames"].empty()) {

      // Wir nehmen das erste Element (das ist normalerweise das nächste)
      const auto &first = j["geonames"][0];

      // Titel -> Englisch (z.B. "Glärnisch")
      if (first.contains("title")) {
        res.address_english = first["title"].get<std::string>();
      }

      // Summary -> Local
      if (first.contains("summary")) {
        res.address_local = first["summary"].get<std::string>();
      }

      // Wikipedia URL -> An Local anhängen
      if (first.contains("wikipediaUrl")) {
        std::string wikiUrl = first["wikipediaUrl"].get<std::string>();

        // GeoNames liefert oft URLs ohne Protokoll (z.B.
        // "en.wikipedia.org/...") Wir lassen es "raw", packen es aber in
        // Klammern dahinter.
        if (!res.address_local.empty()) {
          res.address_local += " (" + wikiUrl + ")";
        } else {
          // Falls keine Summary da ist, ist die URL der einzige Inhalt
          res.address_local = wikiUrl;
        }
      }

      if (first.contains("countryCode")) {
        res.country_code = first["countryCode"].get<std::string>();
      }
    }

  } catch (const nlohmann::json::exception &) {
    // Fehler beim Parsen ignorieren, leeres Result zurückgeben
  }

  return res;
}

} // namespace regeocode