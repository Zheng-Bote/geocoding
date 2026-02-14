#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <nlohmann/json.hpp>

#include "regeocode/api_adapter.hpp"
#include "regeocode/http_client.hpp"
#include "regeocode/quota_manager.hpp"

namespace regeocode {

struct Coordinates {
  double latitude;
  double longitude;
};

struct ApiConfig {
  std::string name;
  std::string uri_template;
  std::string api_key;
  std::string adapter;
  std::string type;
  long timeout = 10;
  long daily_limit = 0;
};

// NEU: Container für das gesamte Config-Ergebnis
struct Configuration {
  std::unordered_map<std::string, ApiConfig> apis;
  std::string quota_file_path = "quota_status.json"; // Default
};

class ConfigLoader {
public:
  explicit ConfigLoader(std::string ini_path);
  // ÄNDERUNG: Gibt jetzt Configuration Struct zurück statt nur die Map
  Configuration load() const;

private:
  std::string ini_path_;
};

class ReverseGeocoder {
public:
  // ÄNDERUNG: Konstruktor nimmt jetzt quota_file_path entgegen
  ReverseGeocoder(
      std::unordered_map<std::string, ApiConfig> configs,
      std::vector<ApiAdapterPtr> adapters,
      std::unique_ptr<HttpClient> http_client = std::make_unique<HttpClient>(),
      const std::string &quota_file_path = "quota_status.json");

  // ... (Rest der Methoden bleibt gleich) ...
  AddressResult reverse_geocode(const Coordinates &coords,
                                const std::string &api_name,
                                const std::string &language_code = "en") const;

  AddressResult
  reverse_geocode_dual_language(const Coordinates &coords,
                                const std::string &api_name,
                                const std::string &user_lang) const;

  nlohmann::json
  reverse_geocode_json(const Coordinates &coords, const std::string &api_name,
                       const std::string &lang_override = "") const;

  nlohmann::json
  reverse_geocode_fallback(const Coordinates &coords,
                           const std::vector<std::string> &priority_list,
                           const std::string &lang_override = "") const;

  std::vector<nlohmann::json>
  batch_reverse_geocode(const std::vector<Coordinates> &coords_list,
                        const std::vector<std::string> &priority_list,
                        const std::string &lang_override = "") const;

private:
  std::unordered_map<std::string, ApiConfig> configs_;
  std::unordered_map<std::string, ApiAdapterPtr> adapters_;
  std::unique_ptr<HttpClient> http_client_;

  mutable QuotaManager quota_manager_;
};

} // namespace regeocode