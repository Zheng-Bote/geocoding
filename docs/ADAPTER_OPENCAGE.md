# OpenCage Data Adapter

The `OpenCageAdapter` provides support for the OpenCage Data Geocoding API.

## Details

- **Adapter Name:** `opencage`
- **Header:** `adapter_opencage.hpp`
- **Class:** `regeocode::OpenCageAdapter`

## Functionality

### `name()`

Returns the string `"opencage"`. This name is used to identify the adapter in configuration files.

### `parse_response(const std::string &response_body)`

Parses the raw JSON response from OpenCage Data. It extracts:
- Formatted address (English and Local).
- ISO Country Code.
- Raw JSON response.
