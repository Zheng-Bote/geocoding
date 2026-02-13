cmake_minimum_required(VERSION 3.25)

project(regeocode
    VERSION 0.1.0
    DESCRIPTION "C++ Reverse Geocoding Library"
    HOMEPAGE_URL "https://github.com/Zheng-Bote/re-geocode"
    LANGUAGES CXX C
)

# C++23 Standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)

# --- 1. JSON ---
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# --- 2. inifile-cpp ---
FetchContent_Declare(
  inifile-cpp
  GIT_REPOSITORY https://github.com/Rookfighter/inifile-cpp.git
  GIT_TAG master
)
FetchContent_MakeAvailable(inifile-cpp)

# --- 3. CLI11 ---
FetchContent_Declare(
    cli11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.4.1
)
FetchContent_MakeAvailable(cli11)

find_package(CURL REQUIRED)

# --- Library (Core) ---

add_library(regeocode SHARED
    src/re_geocode_core.cpp
    src/http_client.cpp
    src/adapter_nominatim.cpp
    src/adapter_google.cpp
    src/adapter_opencage.cpp
    src/re_geocode_c_api.cpp
    src/adapter_geonames_timezone.cpp
    src/adapter_geonames_wikipedia.cpp
    src/adapter_openweather.cpp
    src/adapter_pollution.cpp
)

add_library(regeocode::lib ALIAS regeocode)

target_include_directories(regeocode
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${inifile-cpp_SOURCE_DIR}/include
)

target_link_libraries(regeocode
    PUBLIC
        CURL::libcurl
    PRIVATE
        nlohmann_json::nlohmann_json
        inicpp
)

set_target_properties(regeocode PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    EXPORT_NAME lib
)

# --- CLI Executable ---

add_executable(regeocode-cli cli/main_cli.cpp)
target_link_libraries(regeocode-cli PRIVATE regeocode::lib CLI11::CLI11)

# --- Installation ---

include(GNUInstallDirs)

# 1. Targets (Lib + CLI) installieren
install(TARGETS regeocode regeocode-cli
    EXPORT regeocodeTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 2. Header installieren
install(DIRECTORY include/regeocode
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# 3. Optional Inja Header (falls benötigt)
install(DIRECTORY include/inja
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    OPTIONAL
)

# 4. CMake Config exportieren
install(EXPORT regeocodeTargets
    FILE regeocodeTargets.cmake
    NAMESPACE regeocode::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/regeocode
)

# --- PKG-CONFIG INTEGRATION (NEU) ---

# .pc Datei aus dem Template generieren
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/regeocode.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/regeocode.pc"
    @ONLY
)

# .pc Datei an den Standardort installieren (meist /usr/local/lib/pkgconfig)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/regeocode.pc"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
)

# --- Tests ---
enable_testing()
# Nur bauen, wenn Test-Datei existiert, um Fehler zu vermeiden, falls Ordner fehlt
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_reverse_geocode.cpp")
    add_executable(test_reverse_geocode tests/test_reverse_geocode.cpp)
    target_link_libraries(test_reverse_geocode PRIVATE regeocode::lib)
    add_test(NAME reverse_geocode_test COMMAND test_reverse_geocode)
endif()