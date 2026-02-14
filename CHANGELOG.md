<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Changelog](#changelog)
  - [[0.1.0] - 2026-02-14](#010---2026-02-14)
    - [Added](#added)
    - [Changed](#changed)
    - [Fixed](#fixed)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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
