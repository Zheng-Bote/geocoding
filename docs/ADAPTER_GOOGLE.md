# Google Maps Adapter

The `GoogleAdapter` provides support for the Google Maps Geocoding API.

## Details

- **Adapter Name:** `google`
- **Header:** `adapter_google.hpp`
- **Class:** `regeocode::GoogleAdapter`

## Functionality

### `name()`

Returns the string `"google"`. This name is used to identify the adapter in configuration files.

### `parse_response(const std::string &response_body)`

Parses the raw JSON response from Google Maps. It extracts:
- Formatted address (English and Local).
- ISO Country Code.
- Raw JSON response.
