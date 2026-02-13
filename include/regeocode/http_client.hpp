#pragma once

#include <map>
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

  HttpResponse
  get(const std::string &url,
      const std::map<std::string, std::string> &headers = {}) const;
};

} // namespace regeocode
