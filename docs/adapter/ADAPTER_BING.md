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

## Example

**call**

```bash
./regeocode-cli --config data/re-geocode.ini --lat 48.137222 --lon 11.575278 --api bing
```

**returns**

```json
{
  "meta": {
    "api": "bing",
    "latitude": 48.139722,
    "longitude": 11.574444,
    "type": "geocoding"
  },
  "result": {
    "address_english": "Maffeistraße 14, 80333 Munich",
    "address_local": "Maffeistraße 14, 80333 München",
    "country_code": "DE",
    "details": {}
  }
}
```
