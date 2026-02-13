#pragma once

#include "api_adapter.hpp"

namespace regeocode {

class OpenCageAdapter : public ApiAdapter {
public:
  std::string name() const override { return "opencage"; }
  AddressResult parse_response(const std::string &response_body) const override;
};

} // namespace regeocode
