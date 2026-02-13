#include "regeocode/adapter_nominatim.hpp"

#include <nlohmann/json.hpp>

namespace regeocode {

AddressResult
NominatimAdapter::parse_response(const std::string &response_body) const {
  nlohmann::json j = nlohmann::json::parse(response_body);

  AddressResult res;
  res.raw_json = response_body;

  if (j.contains("display_name")) {
    res.address_english = j["display_name"].get<std::string>();
    res.address_local = res.address_english;
  }

  if (j.contains("address") && j["address"].contains("country_code")) {
    res.country_code = j["address"]["country_code"].get<std::string>();
  }

  return res;
}

} // namespace regeocode
