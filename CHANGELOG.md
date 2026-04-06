<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Changelog](#changelog)
  - [[1.2.0] - 2026-04-06](#120---2026-04-06)
    - [Added](#added)
    - [Changed](#changed)
  - [[1.1.0] - 2026-04-04](#110---2026-04-04)
    - [Added](#added-1)
    - [Fixed](#fixed)
    - [Changed](#changed-1)
  - [[1.0.0] - 2026-03-14](#100---2026-03-14)
    - [Added](#added-2)
    - [Changed](#changed-2)
  - [[0.1.0] - 2026-02-14](#010---2026-02-14)
    - [Added](#added-3)
    - [Changed](#changed-3)
    - [Fixed](#fixed-1)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.2.0] - 2026-04-06

### Added

- **CountryInfoAdapter**: New API-based adapter for [RestCountries](https://restcountries.com/) providing rich country metadata.
- **Dynamic Template Support**: Added `{{ country_code }}` support to URI templates in `ReverseGeocoder`.
- **Enhanced Image Processing (reverse_geo_batch)**:
  - **Auto-Renaming**: Optional `--rename` flag to rename images based on GPS/Exif/File timestamp (`YYYY-MM-DD_hhmmss.ext`).
  - **Title Extraction**: Automatically extracts filename (replacing underscores with spaces) and writes it to `Xmp.dc.title`.
  - **RestCountries Integration**: Fetches and embeds detailed country info (Continent, Cca2/3, Capital) into XMP metadata.
- **Testing**: Added `tests/test_country_info.cpp` to verify RestCountries API integration.

### Changed

- Updated `cli/reverse_geo.cpp` to use `CountryInfoAdapter` instead of local `CountryAdapter` for richer metadata.
- Improved `Coordinates` structure to optionally carry a `country_code`.

## [1.1.0] - 2026-04-04

### Added

- **CountryAdapter**: Introduced a local adapter to fetch country details (name, capital, region, flag) from `data/countries.json` using ISO codes.
- **Image Metadata Enrichment**: Extended `cli/reverse_geo.cpp` to write `Xmp.photoshop.continent` (region) and `Xmp.photoshop.Country` (official/common name) using the new CountryAdapter.
- **Flag URL Support**: Added `flag_url` field to country data, linking to high-quality SVG flag icons.
- **Testing**: Added `tests/test_country_adapter.cpp` to verify local country data lookup.

### Fixed

- **JSON Robustness**: Improved `NominatimAdapter`, `GoogleAdapter`, and `OpenCageAdapter` to safely handle null or missing fields in API responses, preventing potential crashes.

### Changed

- Updated documentation in `README.md` and `docs/adapter/` to include the new Country adapter.

## [1.0.0] - 2026-03-14

### Added

- **Conan Package**: Added `conanfile.py` to support building and installing the library as a Conan package.
- **CMake Improvements**: Updated `CMakeLists.txt` to dynamically find Conan dependencies via `find_package` when available, and added toggles for `BUILD_SHARED_LIBS` and `BUILD_CLI`.

### Changed

- Bumped project version to `1.0.0`.
- Updated homepage URL to `https://github.com/Zheng-Bote/geocoding`.

## [0.1.0] - 2026-02-14

### Added

- **C-API**: Implemented C-compatible API wrapper (`re_geocode_c_api.cpp` and `re_geocode_c_api.h`) for integration with other languages like Python or Rust.
- **Quota Management**:
  - Added `quota_file` parameter to `re-geocode.ini` `[config]` section to allow custom quota file paths.
  - `QuotaManager` is now thread-safe for batch processing.
  - `ReverseGeocoder` constructor now accepts a quota file path.
- **Documentation**:
  - Added Doxygen comments to all header (`.hpp`) and source (`.cpp`) files.
  - Added SPDX license headers to all files.
- **CMake**: `FetchContent` support is now documented.

### Changed

- **Translations**: Translated all German code comments and documentation to English.
- **Refactoring**:
  - Fixed `HttpClient` implementation (added missing constructor/destructor to resolve linker errors).
  - `ConfigLoader` now returns a `Configuration` struct containing both API configs and global settings.
- **Tests**: Standardized header information in `tests/test_reverse_geocode.cpp`.
- **Docs**: Translated `cmake_example.md` and `pkg-config_example.md` to English.

### Fixed

- Linker errors due to missing `HttpClient` implementation.
