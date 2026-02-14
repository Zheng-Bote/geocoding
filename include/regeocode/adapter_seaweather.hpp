#pragma once

#include "api_adapter.hpp"
#include <string>

namespace regeocode {

class SeaWeatherAdapter : public ApiAdapter {
public:
  std::string name() const override { return "seaweather"; }

  AddressResult parse_response(const std::string &response_body) const override;
};

} // namespace regeocode