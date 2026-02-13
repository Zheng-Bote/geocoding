#pragma once

#include "api_adapter.hpp"

namespace regeocode {

class NominatimAdapter : public ApiAdapter {
public:
  std::string name() const override { return "nominatim"; }
  AddressResult parse_response(const std::string &response_body) const override;
};

} // namespace regeocode
