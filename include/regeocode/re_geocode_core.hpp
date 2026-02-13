#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "api_adapter.hpp"
#include "http_client.hpp"

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
};

class ConfigLoader {
public:
  explicit ConfigLoader(std::string ini_path);
  std::unordered_map<std::string, ApiConfig> load() const;

private:
  std::string ini_path_;
};

class ReverseGeocoder {
public:
  ReverseGeocoder(
      std::unordered_map<std::string, ApiConfig> configs,
      std::vector<ApiAdapterPtr> adapters,
      std::unique_ptr<HttpClient> http_client = std::make_unique<HttpClient>());

  AddressResult reverse_geocode(const Coordinates &coords,
                                const std::string &api_name,
                                const std::string &language_code = "en") const;

  AddressResult
  reverse_geocode_dual_language(const Coordinates &coords,
                                const std::string &api_name,
                                const std::string &user_lang) const;

private:
  std::unordered_map<std::string, ApiConfig> configs_;
  std::unordered_map<std::string, ApiAdapterPtr> adapters_;
  std::unique_ptr<HttpClient> http_client_;
};

} // namespace regeocode
