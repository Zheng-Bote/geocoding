#include "regeocode/re_geocode_core.hpp"

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

// Stellen Sie sicher, dass dieser Pfad stimmt (abhängig davon wo inja liegt)
// Falls es direkt in include/ liegt, wäre #include <inja.hpp> korrekter.
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

  // Rookfighter: ini::IniFile
  ini::IniFile ini;
  ini.load(ini_path_);

  std::unordered_map<std::string, ApiConfig> result;

  // KORREKTUR: "const" entfernt, damit operator[] genutzt werden kann
  for (auto &sectionPair : ini) {
    const std::string &sectionName = sectionPair.first;
    ini::IniSection &section =
        sectionPair.second; // Muss non-const sein für [] Zugriff

    ApiConfig cfg;
    cfg.name = sectionName;

    // Rookfighter (wie std::map) nutzt count()
    if (section.count("URI") == 0)
      throw std::runtime_error("Missing URI in section: " + sectionName);

    if (section.count("API-Key") == 0)
      throw std::runtime_error("Missing API-Key in section: " + sectionName);

    // Zugriff via [], Rückgabe ist IniField, Konvertierung via .as<T>()
    cfg.uri_template = section["URI"].as<std::string>();
    cfg.api_key = section["API-Key"].as<std::string>();

    if (section.count("Adapter") != 0) {
      cfg.adapter = section["Adapter"].as<std::string>();

      // Anführungszeichen bereinigen
      if (!cfg.adapter.empty() && cfg.adapter.front() == '"' &&
          cfg.adapter.back() == '"' && cfg.adapter.size() >= 2) {
        cfg.adapter = cfg.adapter.substr(1, cfg.adapter.size() - 2);
      }
    } else {
      cfg.adapter = sectionName;
    }

    // Type of adapter
    if (section.count("type") != 0) {
      cfg.type = section["type"].as<std::string>();
    } else {
      // Fallback for old configs: Standard is Geocoding
      cfg.type = "geocoding";
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

  // Inja Rendering
  inja::Environment env;
  std::string url = env.render(cfg.uri_template, params);

  auto resp = http_client_->get(url);

  if (resp.status_code < 200 || resp.status_code >= 300) {
    throw std::runtime_error("HTTP error: " + std::to_string(resp.status_code));
  }

  return adapter->parse_response(resp.body);
}

AddressResult ReverseGeocoder::reverse_geocode_dual_language(
    const Coordinates &coords, const std::string &api_name,
    const std::string &user_lang) const {

  // --- check config first ---
  auto it = configs_.find(api_name);
  if (it == configs_.end()) {
    throw std::runtime_error("Unknown API: " + api_name);
  }
  const auto &cfg = it->second;

  // if type == "information": only one request, return directly.
  if (cfg.type == "information") {
    // if user_lang is empty, use "en" as default for information
    std::string lang = user_lang.empty() ? "en" : user_lang;
    return reverse_geocode(coords, api_name, lang);
  }
  // ----------------------------------

  AddressResult result;

  // 1. English request
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

  // 3. Auto-Detect Local via Country Code
  if (!en.country_code.empty()) {
    std::string local_lang = language_from_country(en.country_code);
    auto local = reverse_geocode(coords, api_name, local_lang);
    result.address_local = local.address_english;
  } else {
    result.address_local.clear();
  }

  return result;
}

} // namespace regeocode