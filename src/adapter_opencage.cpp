#include "regeocode/adapter_opencage.hpp"

#include <nlohmann/json.hpp>

namespace regeocode {

AddressResult
OpenCageAdapter::parse_response(const std::string &response_body) const {
  nlohmann::json j = nlohmann::json::parse(response_body);

  AddressResult res;
  res.raw_json = response_body;

  if (j.contains("results") && !j["results"].empty()) {
    const auto &first = j["results"][0];

    if (first.contains("formatted")) {
      res.address_english = first["formatted"].get<std::string>();
      res.address_local = res.address_english;
    }

    if (first.contains("components") &&
        first["components"].contains("country_code")) {
      res.country_code = first["components"]["country_code"].get<std::string>();
    }
  }

  return res;
}

} // namespace regeocode
