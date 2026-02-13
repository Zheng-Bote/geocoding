#include "regeocode/http_client.hpp"
#include <curl/curl.h>
#include <mutex>
#include <stdexcept>

namespace regeocode {

namespace {
// Global Init für Thread-Safety
std::once_flag curl_init_flag;

void init_curl_once() { curl_global_init(CURL_GLOBAL_ALL); }

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
  auto *out = static_cast<std::string *>(userdata);
  out->append(ptr, size * nmemb);
  return size * nmemb;
}
} // namespace

HttpClient::HttpClient() {
  // Sicherstellen, dass CURL global initialisiert ist
  std::call_once(curl_init_flag, init_curl_once);
}

HttpClient::~HttpClient() {
  // curl_global_cleanup rufen wir bewusst nicht auf, da es in
  // Multi-Threaded Umgebungen zu Race Conditions beim Beenden führen kann.
  // Das OS räumt auf.
}

HttpResponse
HttpClient::get(const std::string &url,
                const std::map<std::string, std::string> &headers) const {
  CURL *curl = curl_easy_init();
  if (!curl)
    throw std::runtime_error("Failed to init CURL");

  std::string response_body;
  // ... (Rest bleibt wie in Ihrem Code, ist thread-safe da curl handles lokal
  // sind)

  // Config...
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "re-geocode/1.0");
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

  // Header Handling (wie gehabt)
  struct curl_slist *header_list = nullptr;
  for (const auto &[key, value] : headers) {
    std::string h = key + ": " + value;
    header_list = curl_slist_append(header_list, h.c_str());
  }
  if (header_list)
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

  CURLcode res = curl_easy_perform(curl);

  long status_code = 0;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);

  if (header_list)
    curl_slist_free_all(header_list);
  curl_easy_cleanup(curl);

  if (res != CURLE_OK)
    throw std::runtime_error(curl_easy_strerror(res));

  return {status_code, response_body};
}

} // namespace regeocode