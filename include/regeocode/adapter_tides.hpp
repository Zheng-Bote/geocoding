#pragma once

#include "api_adapter.hpp"
#include <string>

namespace regeocode {

class TidesAdapter : public ApiAdapter {
public:
  std::string name() const override { return "tides"; }

  AddressResult parse_response(const std::string &response_body) const override;
};

} // namespace regeocode