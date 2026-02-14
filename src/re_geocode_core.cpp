#include "regeocode/re_geocode_core.hpp"

#include <algorithm>
#include <fstream>
#include <future> // Wichtig für batch/async
#include <iostream>
#include <sstream> // Wichtig für split
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "regeocode/inja.hpp"
#include <inicpp.h>
#include <nlohmann/json.hpp>

namespace regeocode {

namespace {

bool is_valid_language(const std::string &lang) {
  static const std::unordered_set<std::string> valid = {
      "en", "de",    "fr",    "es", "it", "ar", "ru", "pt", "nl", "pl",
      "zh", "zh-CN", "zh-TW", "ja", "ko", "tr", "sv", "no", "fi"};
  return valid.contains(lang);
}

std::string language_from_country(const std::string &cc_raw) {
  std::string cc = cc_raw;
  for (auto &c : cc)
    c = static_cast<char>(::tolower(c));

  static const std::unordered_map<std::string, std::string> map = {
      {"de", "de"}, {"at", "de"}, {"ch", "de"},    {"ae", "ar"},
      {"sa", "ar"}, {"eg", "ar"}, {"cn", "zh-CN"}, {"tw", "zh-TW"},
      {"jp", "ja"}, {"kr", "ko"}, {"fr", "fr"},    {"es", "es"},
      {"it", "it"}, {"ru", "ru"}, {"pt", "pt"},    {"nl", "nl"}};

  auto it = map.find(cc);
  if (it != map.end())
    return it->second;
  return "en";
}

} // namespace

// -------------------------
// ConfigLoader
// -------------------------

ConfigLoader::ConfigLoader(std::string ini_path)
    : ini_path_(std::move(ini_path)) {}

std::unordered_map<std::string, ApiConfig> ConfigLoader::load() const {
  std::ifstream test(ini_path_);
  if (!test.good()) {
    throw std::runtime_error("INI file not found: " + ini_path_);
  }

  ini::IniFile ini;
  ini.load(ini_path_);

  std::unordered_map<std::string, ApiConfig> result;

  for (auto &sectionPair : ini) {
    const std::string &sectionName = sectionPair.first;
    ini::IniSection &section = sectionPair.second;

    // --- SCHRITT 1: ZUERST DEN TYP LESEN ---
    std::string type = "geocoding"; // Default für Rückwärtskompatibilität
    if (section.count("type") != 0) {
      type = section["type"].as<std::string>();
    }

    // --- SCHRITT 2: WEICHE ---
    // Wenn es eine reine Konfigurations-Sektion ist, überspringen wir
    // das Laden als API-Provider.
    // (Hier könnten wir später Code einfügen, der die Strategien tatsächlich
    // parst und in eine separate Config-Struktur speichert)
    if (type == "strategies" || type == "config" || type == "general") {
      continue;
    }

    ApiConfig cfg;
    cfg.name = sectionName;
    cfg.type = type;

    // --- SCHRITT 3: API VALIDIERUNG ---
    // Jetzt wissen wir, dass es ein Provider sein MUSS.
    // Fehlt hier die URI, ist es ein echter Fehler.

    if (section.count("URI") == 0)
      throw std::runtime_error("Missing URI in API section: " + sectionName);

    if (section.count("API-Key") == 0)
      throw std::runtime_error("Missing API-Key in API section: " +
                               sectionName);

    cfg.uri_template = section["URI"].as<std::string>();
    cfg.api_key = section["API-Key"].as<std::string>();

    if (section.count("Adapter") != 0) {
      cfg.adapter = section["Adapter"].as<std::string>();
      // Anführungszeichen entfernen falls vorhanden
      if (!cfg.adapter.empty() && cfg.adapter.front() == '"' &&
          cfg.adapter.back() == '"' && cfg.adapter.size() >= 2) {
        cfg.adapter = cfg.adapter.substr(1, cfg.adapter.size() - 2);
      }
    } else {
      cfg.adapter = sectionName;
    }

    // Timeout lesen (Default 10s, falls nicht angegeben)
    if (section.count("timeout") != 0) {
      cfg.timeout = section["timeout"].as<long>();
    } else {
      cfg.timeout = 10;
    }

    result.emplace(sectionName, std::move(cfg));
  }

  return result;
}

// -------------------------
// ReverseGeocoder
// -------------------------

ReverseGeocoder::ReverseGeocoder(
    std::unordered_map<std::string, ApiConfig> configs,
    std::vector<ApiAdapterPtr> adapters,
    std::unique_ptr<HttpClient> http_client)
    : configs_(std::move(configs)), http_client_(std::move(http_client)) {

  for (auto &a : adapters) {
    adapters_.emplace(a->name(), std::move(a));
  }
}

AddressResult
ReverseGeocoder::reverse_geocode(const Coordinates &coords,
                                 const std::string &api_name,
                                 const std::string &language_code) const {

  auto it = configs_.find(api_name);
  if (it == configs_.end()) {
    throw std::runtime_error("Unknown API: " + api_name);
  }

  const auto &cfg = it->second;

  auto adapter_it = adapters_.find(cfg.adapter);
  if (adapter_it == adapters_.end()) {
    throw std::runtime_error("No adapter registered for: " + cfg.adapter);
  }

  const auto &adapter = adapter_it->second;

  nlohmann::json params;
  params["latitude"] = coords.latitude;
  params["longitude"] = coords.longitude;
  params["apikey"] = cfg.api_key;
  params["lang"] = language_code;

  inja::Environment env;
  std::string url = env.render(cfg.uri_template, params);

  auto resp = http_client_->get(url, cfg.timeout);

  if (resp.status_code < 200 || resp.status_code >= 300) {
    throw std::runtime_error("HTTP error: " + std::to_string(resp.status_code));
  }

  return adapter->parse_response(resp.body);
}

AddressResult ReverseGeocoder::reverse_geocode_dual_language(
    const Coordinates &coords, const std::string &api_name,
    const std::string &user_lang) const {

  auto it = configs_.find(api_name);
  if (it == configs_.end()) {
    throw std::runtime_error("Unknown API: " + api_name);
  }
  const auto &cfg = it->second;

  if (cfg.type == "info" || cfg.type == "information") {
    std::string lang = user_lang.empty() ? "en" : user_lang;
    return reverse_geocode(coords, api_name, lang);
  }

  AddressResult result;

  // 1. English
  auto en = reverse_geocode(coords, api_name, "en");
  result.address_english = en.address_english;
  result.country_code = en.country_code;

  // 2. User language or Local
  if (!user_lang.empty()) {
    if (is_valid_language(user_lang)) {
      auto local = reverse_geocode(coords, api_name, user_lang);
      result.address_local = local.address_english;
    } else {
      result.address_local.clear();
    }
    return result;
  }

  // 3. Auto-Detect Local
  if (!en.country_code.empty()) {
    std::string local_lang = language_from_country(en.country_code);
    auto local = reverse_geocode(coords, api_name, local_lang);
    result.address_local = local.address_english;
  } else {
    result.address_local.clear();
  }

  return result;
}

nlohmann::json
ReverseGeocoder::reverse_geocode_json(const Coordinates &coords,
                                      const std::string &api_name,
                                      const std::string &lang_override) const {

  auto it = configs_.find(api_name);
  if (it == configs_.end()) {
    throw std::runtime_error("Unknown API: " + api_name);
  }
  std::string type = it->second.type;

  AddressResult res =
      reverse_geocode_dual_language(coords, api_name, lang_override);

  nlohmann::json root;
  root["meta"] = {{"api", api_name},
                  {"type", type},
                  {"latitude", coords.latitude},
                  {"longitude", coords.longitude}};

  if (type == "geocoding") {
    root["result"] = {{"address_english", res.address_english},
                      {"address_local", res.address_local},
                      {"country_code", res.country_code}};
    root["result"]["details"] = res.attributes;

  } else {
    root["result"] = {{"title", res.address_english},
                      {"summary", res.address_local},
                      {"country_code", res.country_code},
                      {"data", res.attributes}};
  }

  return root;
}

// ---------------------------------------------------------
// FALLBACK LOGIC (Circuit Breaker Light) - HIER FEHLTE DER CODE
// ---------------------------------------------------------
nlohmann::json ReverseGeocoder::reverse_geocode_fallback(
    const Coordinates &coords, const std::vector<std::string> &priority_list,
    const std::string &lang_override) const {

  nlohmann::json last_error;

  for (const auto &api_name : priority_list) {
    std::string clean_name = api_name;
    // Trim whitespace
    clean_name.erase(0, clean_name.find_first_not_of(' '));
    clean_name.erase(clean_name.find_last_not_of(' ') + 1);

    if (clean_name.empty())
      continue;

    try {
      return reverse_geocode_json(coords, clean_name, lang_override);
    } catch (const std::exception &e) {
      std::cerr << "[Warning] API '" << clean_name << "' failed: " << e.what()
                << ". Trying next provider...\n";

      last_error = {{"error", "Provider failed"},
                    {"provider", clean_name},
                    {"details", e.what()}};
      continue;
    }
  }

  nlohmann::json error_json;
  error_json["error"] = "All providers failed";
  error_json["last_attempt"] = last_error;
  return error_json;
}

// ---------------------------------------------------------
// BATCH PROCESSING (Async) - HIER FEHLTE DER CODE
// ---------------------------------------------------------
std::vector<nlohmann::json> ReverseGeocoder::batch_reverse_geocode(
    const std::vector<Coordinates> &coords_list,
    const std::vector<std::string> &priority_list,
    const std::string &lang_override) const {

  std::vector<std::future<nlohmann::json>> futures;
  futures.reserve(coords_list.size());

  // 1. Tasks starten
  for (const auto &coord : coords_list) {
    futures.push_back(std::async(std::launch::async,
                                 [this, coord, priority_list, lang_override]() {
                                   return this->reverse_geocode_fallback(
                                       coord, priority_list, lang_override);
                                 }));
  }

  // 2. Ergebnisse einsammeln
  std::vector<nlohmann::json> results;
  results.reserve(coords_list.size());

  for (auto &f : futures) {
    results.push_back(f.get());
  }

  return results;
}

} // namespace regeocode