## C++ Example

Here is the complete C++ example for using the installed libregeocode shared library in your own application.

This example demonstrates initialization, the fallback strategy, and accessing specialized data (like Tides) via the JSON interface.

### 1. The Code: example_app.cpp

```cpp
#include <iostream>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>

// Core Header
#include <regeocode/re_geocode_core.hpp>
#include <regeocode/http_client.hpp>

// Adapter Headers (include only what you need)
#include <regeocode/adapter_nominatim.hpp>
#include <regeocode/adapter_google.hpp>
#include <regeocode/adapter_tides.hpp>

using namespace regeocode;

int main() {
    try {
        // 1. Load Configuration (INI File)
        // The loader reads APIs, Keys, Timeouts, and the path to the quota file
        ConfigLoader loader("re-geocode.ini");
        auto config_data = loader.load();

        // 2. Register Adapters
        // Decide which services your app should support
        std::vector<ApiAdapterPtr> adapters;
        adapters.push_back(std::make_unique<NominatimAdapter>());
        adapters.push_back(std::make_unique<GoogleAdapter>());
        adapters.push_back(std::make_unique<TidesAdapter>());

        // 3. Instantiate Geocoder
        // Pass the configs, adapters, an HTTP client, and the quota file path
        ReverseGeocoder geocoder(
            std::move(config_data.apis),
            std::move(adapters),
            std::make_unique<HttpClient>(),
            config_data.quota_file_path
        );

        std::cout << "--- Example 1: Robust Geocoding (Fallback) ---\n";

        // Strategy: Try Google first (precision), if quota full or error -> Nominatim (free)
        std::vector<std::string> city_strategy = {"google", "nominatim"};
        Coordinates munich = {48.137154, 11.576124};

        auto result_city = geocoder.reverse_geocode_fallback(munich, city_strategy, "de");

        // Access standard fields in JSON
        if (result_city.contains("result")) {
            std::cout << "Address: " << result_city["result"]["address_english"] << "\n";
            std::cout << "Country: " << result_city["result"]["country_code"] << "\n";
            std::cout << "Source:  " << result_city["meta"]["api"] << "\n"; // Which provider answered?
        }


        std::cout << "\n--- Example 2: Specialized Data (Tides) ---\n";

        // Strategy: Only Tides
        std::vector<std::string> tide_strategy = {"tides"};
        Coordinates coast_france = {44.43, -2.12}; // Coast off France

        auto result_tides = geocoder.reverse_geocode_fallback(coast_france, tide_strategy);

        // Access specialized attributes in the "data" block
        if (result_tides.contains("result") && result_tides["result"].contains("data")) {
            auto& data = result_tides["result"]["data"];

            std::cout << "Summary: " << result_tides["result"]["summary"] << "\n";

            if (data.contains("current_height")) {
                std::cout << "Current Water Level: " << data["current_height"] << "m\n";
            }
            if (data.contains("event_0_state")) {
                std::cout << "Next Event: " << data["event_0_state"]
                          << " at " << data["event_0_time"] << "\n";
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
```

### 2. Compile and Link

Since you installed the library (sudo make install), the headers are located in /usr/local/include and the lib in /usr/local/lib.

#### Option A: Quick test via command line (g++)

```bash
g++ -std=c++23 example_app.cpp -o example_app \
    -lregeocode -lcurl
```

If you get errors about missing headers:

```bash
g++ -std=c++23 example_app.cpp -o example_app \
    -I/usr/local/include -L/usr/local/lib \
    -lregeocode -lcurl
```

#### Option B: Using CMake (Professional)

Create a CMakeLists.txt next to example_app.cpp:

```cmake
cmake_minimum_required(VERSION 3.25)
project(MyGeocodingApp)

set(CMAKE_CXX_STANDARD 23)

# Finds the installed library (or via pkg-config)
find_library(RE_GEOCODE_LIB regeocode REQUIRED)
find_package(CURL REQUIRED)

add_executable(example_app example_app.cpp)

# Header paths (if not automatically in system path)
target_include_directories(example_app PRIVATE /usr/local/include)

# Linking
target_link_libraries(example_app PRIVATE
    ${RE_GEOCODE_LIB}
    CURL::libcurl
)
```

### 3. Execution

Ensure that re-geocode.ini is in the same directory as the executable, as we call ConfigLoader("re-geocode.ini") in the code.

```bash
./example_app
```

Expected Output:

```text
--- Example 1: Robust Geocoding (Fallback) ---
Address: Marienplatz, Munich, Germany
Country: DE
Source:  nominatim

--- Example 2: Specialized Data (Tides) ---
Summary: RISING (0.82m) at 2026-02-14T13:07:01+00:00
Current Water Level: 0.821142m
Next Event: HIGH TIDE at 2026-02-14T14:11:44+00:00
```
