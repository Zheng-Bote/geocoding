#include <CLI/CLI.hpp> // Via FetchContent verfügbar
#include <iostream>
#include <memory>
#include <vector>

// Da wir gegen qt_regeocode::lib linken und die Include-Directories
// dort als PUBLIC definiert sind, finden wir diese Header problemlos.
#include "regeocode/adapter_geonames_timezone.hpp"
#include "regeocode/adapter_geonames_wikipedia.hpp"
#include "regeocode/adapter_google.hpp"
#include "regeocode/adapter_nominatim.hpp"
#include "regeocode/adapter_opencage.hpp"
#include "regeocode/adapter_openweather.hpp"
#include "regeocode/adapter_pollution.hpp"
#include "regeocode/http_client.hpp"
#include "regeocode/re_geocode_core.hpp"

int main(int argc, char **argv) {
  CLI::App app{"Reverse Geocoding CLI (C++23)"};

  double lat = 0.0;
  double lon = 0.0;
  std::string config_path = "re-geocode.ini";
  std::string api_name = "nominatim";
  std::string lang_override = ""; // Leer = Auto-Detect Local

  // Argumente definieren
  app.add_option("--lat", lat, "Latitude")->required();
  app.add_option("--lon", lon, "Longitude")->required();
  app.add_option("--config", config_path, "Path to INI config file");
  app.add_option("--api", api_name,
                 "API section name in INI (e.g. google, nominatim)");
  app.add_option(
      "--lang", lang_override,
      "Override local language (e.g. 'de', 'ja'). English is always fetched.");

  CLI11_PARSE(app, argc, argv);

  try {
    // 1. Setup: Config laden
    regeocode::ConfigLoader loader(config_path);
    auto configs = loader.load();

    // 2. Adapter registrieren
    std::vector<regeocode::ApiAdapterPtr> adapters;
    adapters.push_back(std::make_unique<regeocode::NominatimAdapter>());
    adapters.push_back(std::make_unique<regeocode::GoogleAdapter>());
    adapters.push_back(std::make_unique<regeocode::OpenCageAdapter>());
    adapters.push_back(std::make_unique<regeocode::GeoNamesTimezoneAdapter>());
    adapters.push_back(std::make_unique<regeocode::GeoNamesWikipediaAdapter>());
    adapters.push_back(std::make_unique<regeocode::OpenWeatherAdapter>());
    adapters.push_back(std::make_unique<regeocode::PollutionAdapter>());

    // 3. HTTP Client instanziieren (Thread-safe init via call_once im
    // Konstruktor)
    auto client = std::make_unique<regeocode::HttpClient>();

    // 4. Geocoder erstellen
    regeocode::ReverseGeocoder geocoder(std::move(configs), std::move(adapters),
                                        std::move(client));

    // 5. Abfrage ausführen
    std::cout << "Querying API '" << api_name << "' for Coordinates: " << lat
              << ", " << lon << "...\n";

    // Nutzt die Dual-Language Logik (Englisch + Local/Auto)
    auto result = geocoder.reverse_geocode_dual_language({lat, lon}, api_name,
                                                         lang_override);

    // 6. Ausgabe
    std::cout << "==================================================\n";
    std::cout << "English Address: " << result.address_english << "\n";

    if (!result.address_local.empty()) {
      std::cout << "Local Address  : " << result.address_local << "\n";
      // Info-Ausgabe, falls Sprache automatisch erkannt wurde
      if (lang_override.empty() && !result.country_code.empty()) {
        std::cout << "                 [Auto-detected via Country Code: "
                  << result.country_code << "]\n";
      }
    } else {
      std::cout << "Local Address  : <Not available or identical to English>\n";
    }
    std::cout << "==================================================\n";

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}