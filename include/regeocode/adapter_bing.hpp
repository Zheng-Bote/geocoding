#pragma once

#include "api_adapter.hpp"
#include <nlohmann/json.hpp>
#include <string>

namespace regeocode {

class BingAdapter : public ApiAdapter {
public:
  std::string name() const override { return "bing"; }

  AddressResult parse_response(const std::string &response_body) const override;
};

} // namespace regeocode