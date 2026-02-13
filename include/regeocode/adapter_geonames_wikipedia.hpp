#pragma once

#include "api_adapter.hpp"

namespace regeocode {

class GeoNamesWikipediaAdapter : public ApiAdapter {
public:
  // Name muss mit "Adapter = nearbyWikipedia" in der INI übereinstimmen
  std::string name() const override { return "nearbyWikipedia"; }

  AddressResult parse_response(const std::string &response_body) const override;
};

} // namespace regeocode