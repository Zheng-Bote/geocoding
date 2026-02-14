#pragma once

#include <string>

namespace regeocode {

struct HttpResponse {
  long status_code{};
  std::string body;
};

class HttpClient {
public:
  HttpClient();
  ~HttpClient();

  virtual HttpResponse get(const std::string &url, long timeout = 10) const;
};

} // namespace regeocode
