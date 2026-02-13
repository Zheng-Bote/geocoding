#pragma once

#include "api_adapter.hpp"

namespace regeocode {

class OpenWeatherAdapter : public ApiAdapter {
public:
  std::string name() const override { return "openweather"; }
  AddressResult parse_response(const std::string &response_body) const override;
};

} // namespace regeocode