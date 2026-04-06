# GeoNames Timezone Adapter

The `GeoNamesTimezoneAdapter` provides support for the GeoNames Timezone API.

---

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->

**Table of Contents**

- [GeoNames Timezone Adapter](#geonames-timezone-adapter)
  - [Details](#details)
  - [Functionality](#functionality)
    - [`name()`](#name)
    - [`parse_response(const std::string &response_body)`](#parse_responseconst-stdstring-response_body)
  - [Example](#example)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

---

## Details

- **Adapter Name:** `timezone`
- **Header:** `adapter_geonames_timezone.hpp`
- **Class:** `regeocode::GeoNamesTimezoneAdapter`

## Functionality

### `name()`

Returns the string `"timezone"`. This matches the entry `Adapter = timezone` in the configuration.

### `parse_response(const std::string &response_body)`

Parses the raw JSON response from GeoNames. It extracts timezone information such as GMT offset, DST, and timezone ID, storing them in the `attributes` map.

## Example

**call**

```bash
./regeocode-cli --config data/re-geocode.ini --lat 48.137222 --lon 11.575278 --api timezone
```

**returns**

```json
{
  "meta": {
    "api": "timezone",
    "latitude": 48.139722,
    "longitude": 11.574444,
    "type": "info"
  },
  "result": {
    "country_code": "DE",
    "data": {
      "gmt_offset": "1.000000",
      "local_time": "2026-02-15 09:28",
      "timezone_id": "Europe/Berlin"
    },
    "summary": "2026-02-15 09:28",
    "title": "Europe/Berlin (Germany)"
  }
}
```
