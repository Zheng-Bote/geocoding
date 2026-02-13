find_package(CURL REQUIRED)

add_library(qt_regeocode SHARED
    re_geocode_core.cpp
    http_client.cpp
    adapter_nominatim.cpp
    adapter_google.cpp
    adapter_opencage.cpp
    re_geocode_c_api.cpp
)

target_include_directories(qt_regeocode
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(qt_regeocode
    PUBLIC
        CURL::libcurl
    PRIVATE
        nlohmann_json::nlohmann_json
        inicpp
)

set_target_properties(qt_regeocode PROPERTIES
    EXPORT_NAME qt_regeocode
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

include(GNUInstallDirs)

# Installiere Library + Export
install(TARGETS qt_regeocode
    EXPORT qt_regeocodeTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Installiere Header
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include/qt_regeocode
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Exportiere CMake-Targets
install(EXPORT qt_regeocodeTargets
    FILE qt_regeocodeTargets.cmake
    NAMESPACE qt_regeocode::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/qt_regeocode
)
