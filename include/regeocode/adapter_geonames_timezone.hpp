#pragma once

#include "api_adapter.hpp"

namespace regeocode {

class GeoNamesTimezoneAdapter : public ApiAdapter {
public:
  // Der Name muss mit dem Eintrag "Adapter = timezone" in der INI
  // übereinstimmen
  std::string name() const override { return "timezone"; }

  AddressResult parse_response(const std::string &response_body) const override;
};

} // namespace regeocode