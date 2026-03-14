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

## Example

**call**

```bash
./regeocode-cli --config data/re-geocode.ini --lat 48.137222 --lon 11.575278 --api google
```

**returns**

```json
{
  "meta": {
    "api": "google",
    "latitude": 48.137222,
    "longitude": 11.575278,
    "type": "geocoding"
  },
  "result": {
    "address_english": "Marienplatz 25, 80331 Munich, Germany",
    "address_local": "Marienplatz 25, 80331 München, Deutschland",
    "country_code": "DE",
    "details": {}
  }
}
```
