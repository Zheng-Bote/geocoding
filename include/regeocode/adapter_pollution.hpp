#pragma once

#include "api_adapter.hpp"

namespace regeocode {

class PollutionAdapter : public ApiAdapter {
public:
  // Name muss mit "Adapter = pollution" in der INI übereinstimmen
  std::string name() const override { return "pollution"; }

  AddressResult parse_response(const std::string &response_body) const override;
};

} // namespace regeocode