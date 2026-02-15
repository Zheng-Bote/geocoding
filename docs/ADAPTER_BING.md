# Bing Maps Adapter

The `BingAdapter` provides support for the Bing Maps Reverse Geocoding API.

## Details

- **Adapter Name:** `bing`
- **Header:** `adapter_bing.hpp`
- **Class:** `regeocode::BingAdapter`

## Functionality

### `name()`

Returns the string `"bing"`. This name is used to identify the adapter in configuration files.

### `parse_response(const std::string &response_body)`

Parses the raw JSON response from Bing Maps. It extracts:
- Formatted address (English and Local).
- ISO Country Code.
- Raw JSON response.
