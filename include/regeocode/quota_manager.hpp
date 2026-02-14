#pragma once
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

namespace regeocode {

class QuotaManager {
public:
  explicit QuotaManager(
      const std::string &state_file_path = "quota_status.json");

  // Versucht, 1 Request-Slot zu reservieren.
  // Gibt false zurück, wenn das Limit erreicht ist.
  bool try_consume(const std::string &api_name, long limit);

private:
  void load();
  void save();
  std::string get_current_date() const;

  std::string file_path_;
  std::mutex mutex_; // Wichtig für Batch-Processing!

  // Struktur: { "google": { "date": "2023-10-27", "count": 150 } }
  nlohmann::json state_;
};

} // namespace regeocode