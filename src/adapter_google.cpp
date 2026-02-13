#include "regeocode/adapter_google.hpp"

#include <algorithm>
#include <nlohmann/json.hpp>

namespace regeocode {

AddressResult
GoogleAdapter::parse_response(const std::string &response_body) const {
  nlohmann::json j = nlohmann::json::parse(response_body);

  AddressResult res;
  res.raw_json = response_body;

  if (j.contains("results") && !j["results"].empty()) {
    const auto &first = j["results"][0];

    if (first.contains("formatted_address")) {
      res.address_english = first["formatted_address"].get<std::string>();
      res.address_local = res.address_english;
    }

    if (first.contains("address_components")) {
      for (const auto &comp : first["address_components"]) {
        if (!comp.contains("types") || !comp.contains("short_name"))
          continue;

        const auto &types = comp["types"];
        if (std::find(types.begin(), types.end(), "country") != types.end()) {
          res.country_code = comp["short_name"].get<std::string>();
          break;
        }
      }
    }
  }

  return res;
}

} // namespace regeocode
