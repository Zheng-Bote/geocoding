#include <CLI/CLI.hpp>
#include <iostream>
#include <memory>
#include <vector>

#include "regeocode/http_client.hpp"
#include "regeocode/re_geocode_core.hpp"

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
  std::string lang_override = "";

  app.add_option("--lat", lat, "Latitude")->required();
  app.add_option("--lon", lon, "Longitude")->required();
  app.add_option("--config", config_path, "Config file");
  app.add_option("--api", api_name, "API Name");
  app.add_option("--lang", lang_override, "Language");

  CLI11_PARSE(app, argc, argv);

  try {
    regeocode::ConfigLoader loader(config_path);
    auto configs = loader.load();

    std::vector<regeocode::ApiAdapterPtr> adapters;
    adapters.push_back(std::make_unique<regeocode::NominatimAdapter>());
    adapters.push_back(std::make_unique<regeocode::GoogleAdapter>());
    adapters.push_back(std::make_unique<regeocode::OpenCageAdapter>());
    adapters.push_back(std::make_unique<regeocode::GeoNamesTimezoneAdapter>());
    adapters.push_back(std::make_unique<regeocode::GeoNamesWikipediaAdapter>());
    adapters.push_back(std::make_unique<regeocode::OpenWeatherAdapter>());
    adapters.push_back(std::make_unique<regeocode::PollutionAdapter>());

    auto client = std::make_unique<regeocode::HttpClient>();
    regeocode::ReverseGeocoder geocoder(std::move(configs), std::move(adapters),
                                        std::move(client));

    // NEU: Einfach JSON abrufen und ausgeben
    nlohmann::json result =
        geocoder.reverse_geocode_json({lat, lon}, api_name, lang_override);

    // Pretty Print mit 4 Leerzeichen Indentation
    std::cout << result.dump(4) << std::endl;

  } catch (const std::exception &e) {
    std::cerr << "{\"error\": \"" << e.what() << "\"}" << std::endl;
    return 1;
  }

  return 0;
}