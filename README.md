# re-geocode

A modern C++23 Reverse Geocoding Library with support for multiple providers (Nominatim, Google Maps, OpenCage). It provides a clean C++ API, a C-compatible interface, and a command-line tool.

## Features

- **Multiple Adapters**: Support for Nominatim (OpenStreetMap), Google Maps, and OpenCage.
- **C++23 Core**: Utilizes modern C++ features for performance and safety.
- **Dual Language Support**: Automatically fetches addresses in English and attempts to detect/provide the local language.
- **C-API**: Easy integration with other languages (C, Python, Rust, etc.).
- **Template-based URIs**: Uses Inja templates for flexible API request formatting.

## Prerequisites & Dependencies

### System Requirements
- **Compiler**: GCC 13+, Clang 16+, or MSVC 19.36+ (C++23 support required).
- **Build System**: CMake 3.25+.
- **Library**: `libcurl` (with development headers).

### Bundled/Fetched Dependencies
The following are automatically handled via CMake `FetchContent`:
- [nlohmann/json](https://github.com/nlohmann/json)
- [Rookfighter/inifile-cpp](https://github.com/Rookfighter/inifile-cpp)
- [CLIUtils/CLI11](https://github.com/CLIUtils/CLI11)
- [inja](https://github.com/pantor/inja) (included in source)

## Build and Installation

### Building from Source

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Installation

```bash
sudo make install
```
This installs the shared library, headers, and the `regeocode-cli` tool.

## Integration via CMake FetchContent

You can include `re-geocode` directly in your project without manual installation:

```cmake
include(FetchContent)

FetchContent_Declare(
    regeocode
    GIT_REPOSITORY https://github.com/Zheng-Bote/re-geocode.git
    GIT_TAG main  # Or a specific commit/version
)
FetchContent_MakeAvailable(regeocode)

# Link to your target
target_link_libraries(your_app PRIVATE regeocode::lib)
```

## C++ Usage Example

```cpp
#include <regeocode/re_geocode_core.hpp>
#include <regeocode/adapter_nominatim.hpp>
#include <iostream>

int main() {
    using namespace regeocode;

    // 1. Setup configurations (usually loaded from INI)
    std::unordered_map<std::string, ApiConfig> configs;
    configs["nominatim"] = {
        .name = "nominatim",
        .uri_template = "https://nominatim.openstreetmap.org/reverse?lat={{latitude}}&lon={{longitude}}&format=json",
        .api_key = "",
        .adapter = "nominatim"
    };

    // 2. Register adapters
    std::vector<ApiAdapterPtr> adapters;
    adapters.push_back(std::make_unique<NominatimAdapter>());

    // 3. Initialize Geocoder
    ReverseGeocoder geocoder(std::move(configs), std::move(adapters));

    // 4. Perform Lookup
    Coordinates coords{48.137154, 11.576124}; // Munich
    auto result = geocoder.reverse_geocode_dual_language(coords, "nominatim", "de");

    std::cout << "English: " << result.address_english << "
";
    std::cout << "Local:   " << result.address_local << "
";

    return 0;
}
```

## Python Integration (via C-API)

Since the library provides a C-API, you can use `ctypes` to call it from Python:

```python
import ctypes
from ctypes import c_char_p, c_double, c_int, Structure, POINTER

# Define the C-Result structure
class GeocodeResult(Structure):
    _fields_ = [
        ("address_english", c_char_p),
        ("address_local", c_char_p),
        ("country_code", c_char_p),
        ("success", c_int)
    ]

# Load the library
lib = ctypes.CDLL("libregeocode.so")

# Configure function signatures
lib.geocoder_new.argtypes = [c_char_p]
lib.geocoder_new.restype = ctypes.c_void_p

lib.geocoder_lookup.argtypes = [ctypes.c_void_p, c_double, c_double, c_char_p, c_char_p]
lib.geocoder_lookup.restype = GeocodeResult

lib.geocoder_result_free.argtypes = [POINTER(GeocodeResult)]

# Usage
geocoder = lib.geocoder_new(b"re-geocode.ini")
if geocoder:
    res = lib.geocoder_lookup(geocoder, 48.137, 11.576, b"nominatim", None)
    
    if res.success:
        print(f"Address (EN): {res.address_english.decode('utf-8')}")
        print(f"Country:      {res.country_code.decode('utf-8')}")
    
    # Clean up
    lib.geocoder_result_free(ctypes.byref(res))
    lib.geocoder_free(geocoder)
```

## Configuration (INI File)

The library uses an INI file to define API endpoints and keys:

```ini
[nominatim]
URI=https://nominatim.openstreetmap.org/reverse?lat={{ latitude }}&lon={{ longitude }}&format=json
API-Key=none
Adapter=nominatim

[google]
URI=https://maps.googleapis.com/maps/api/geocode/json?latlng={{ latitude }},{{ longitude }}&key={{ apikey }}&language={{ lang }}
API-Key=YOUR_GOOGLE_KEY
Adapter=google
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
Copyright (c) 2026 ZHENG Robert.
