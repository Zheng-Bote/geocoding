# Google Maps Adapter

The `GoogleAdapter` provides support for the Google Maps Geocoding API.

---

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->

**Table of Contents**

- [Google Maps Adapter](#google-maps-adapter)
  - [Details](#details)
  - [Functionality](#functionality)
    - [`name()`](#name)
    - [`parse_response(const std::string &response_body)`](#parse_responseconst-stdstring-response_body)
  - [Example](#example)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

---

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
