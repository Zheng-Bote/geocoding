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

## Example

**call**

```bash
./regeocode-cli --config data/re-geocode.ini --lat 48.137222 --lon 11.575278 --api nominatim
```

**returns**

```json
{
  "meta": {
    "api": "nominatim",
    "latitude": 48.139722,
    "longitude": 11.574444,
    "type": "geocoding"
  },
  "result": {
    "address_english": "14, Maffeistraße, Kreuzviertel, Old Town, Munich, Bavaria, 80333, Germany",
    "address_local": "14, Maffeistraße, Kreuzviertel, Altstadt-Lehel, München, Bayern, 80333, Deutschland",
    "country_code": "de",
    "details": {}
  }
}
```
