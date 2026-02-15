# re-geocode cmake example

## Integration via CMake FetchContent

This is the modern method to build dependencies directly from source code without needing the library to be installed system-wide beforehand. This is ideal for CI/CD pipelines or developers who want to handle everything in a single build step.

**Scenario**: You have a project MyApp and want to automatically load and compile re-geocode from GitHub (or a local path).

```cmake
cmake_minimum_required(VERSION 3.25)
project(MyApp CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Find system dependency (libcurl is usually installed system-wide)
find_package(CURL REQUIRED)

# 2. Include FetchContent
include(FetchContent)

# 3. Declare re-geocode
FetchContent_Declare(
    regeocode
    # Option A: From GitHub (once pushed)
    # GIT_REPOSITORY https://github.com/Zheng-Bote/geocoding.git
    # GIT_TAG main

    # Option B: Local (for testing during development)
    SOURCE_DIR "/home/zb_bamboo/DEV/__NEW__/CPP/re-geocode"
)

# 4. Download and make available (add_subdirectory happens automatically here)
FetchContent_MakeAvailable(regeocode)

# 5. Build your own app
add_executable(my_app main.cpp)

# 6. Link
# 'regeocode' is the target made available by FetchContent.
# Since regeocode defines its includes as PUBLIC internally,
# 'my_app' automatically has access to <regeocode/re_geocode_core.hpp>
target_link_libraries(my_app PRIVATE regeocode CURL::libcurl)
```
