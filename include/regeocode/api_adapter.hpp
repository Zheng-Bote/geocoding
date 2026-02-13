#pragma once

#include <memory>
#include <string>

namespace regeocode {

struct AddressResult {
  std::string address_english;
  std::string address_local;
  std::string raw_json;
  std::string country_code;
};

class ApiAdapter {
public:
  virtual ~ApiAdapter() = default;
  virtual std::string name() const = 0;
  virtual AddressResult
  parse_response(const std::string &response_body) const = 0;
};

using ApiAdapterPtr = std::unique_ptr<ApiAdapter>;

} // namespace regeocode
