#include "regeocode/quota_manager.hpp"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace regeocode {

QuotaManager::QuotaManager(const std::string &state_file_path)
    : file_path_(state_file_path) {
  load();
}

std::string QuotaManager::get_current_date() const {
  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);

  std::stringstream ss;
  ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d");
  return ss.str();
}

void QuotaManager::load() {
  std::ifstream f(file_path_);
  if (f.good()) {
    try {
      f >> state_;
    } catch (...) {
      state_ = nlohmann::json::object();
    }
  } else {
    state_ = nlohmann::json::object();
  }
}

void QuotaManager::save() {
  std::ofstream f(file_path_);
  f << state_.dump(4);
}

bool QuotaManager::try_consume(const std::string &api_name, long limit) {
  if (limit <= 0)
    return true; // 0 = unbegrenzt

  std::lock_guard<std::mutex> lock(mutex_); // Thread-Safe machen

  std::string today = get_current_date();

  // Prüfen, ob Eintrag existiert und ob Datum noch stimmt
  if (!state_.contains(api_name) || state_[api_name]["date"] != today) {
    // Neuer Tag oder neuer Eintrag -> Reset
    state_[api_name] = {{"date", today}, {"count", 0}};
  }

  long current_count = state_[api_name]["count"].get<long>();

  if (current_count >= limit) {
    return false; // Limit erreicht
  }

  // Hochzählen und sofort speichern
  state_[api_name]["count"] = current_count + 1;
  save(); // Performance-Tipp: Bei sehr hoher Last evtl. nicht *jedes* mal
          // speichern, aber für CLI ok.

  return true;
}

} // namespace regeocode