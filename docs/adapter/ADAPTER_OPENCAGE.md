<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [OpenCage Data Adapter](#opencage-data-adapter)
  - [Details](#details)
  - [Functionality](#functionality)
    - [`name()`](#name)
    - [`parse_response(const std::string &response_body)`](#parse_responseconst-stdstring-response_body)
  - [Example](#example)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

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

## Example

**call**

```bash
./regeocode-cli --config data/re-geocode.ini --lat 48.137222 --lon 11.575278 --api opencage
```

**returns**

```json
{
  "meta": {
    "api": "opencage",
    "latitude": 48.139722,
    "longitude": 11.574444,
    "type": "geocoding"
  },
  "result": {
    "address_english": "Maffeistraße 14, 80333 Munich, Germany",
    "address_local": "Maffeistraße 14, 80333 München, Deutschland",
    "country_code": "de",
    "details": {}
  }
}
```
