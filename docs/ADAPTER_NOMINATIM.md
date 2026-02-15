# Nominatim Adapter

The `NominatimAdapter` provides support for the Nominatim Reverse Geocoding API (OpenStreetMap).

## Details

- **Adapter Name:** `nominatim`
- **Header:** `adapter_nominatim.hpp`
- **Class:** `regeocode::NominatimAdapter`

## Functionality

### `name()`

Returns the string `"nominatim"`. This name is used to identify the adapter in configuration files.

### `parse_response(const std::string &response_body)`

Parses the raw JSON response from Nominatim. It extracts:
- Display name (English and Local).
- ISO Country Code.
- Raw JSON response.
