# GeoNames Timezone Adapter

The `GeoNamesTimezoneAdapter` provides support for the GeoNames Timezone API.

## Details

- **Adapter Name:** `timezone`
- **Header:** `adapter_geonames_timezone.hpp`
- **Class:** `regeocode::GeoNamesTimezoneAdapter`

## Functionality

### `name()`

Returns the string `"timezone"`. This matches the entry `Adapter = timezone` in the configuration.

### `parse_response(const std::string &response_body)`

Parses the raw JSON response from GeoNames. It extracts timezone information such as GMT offset, DST, and timezone ID, storing them in the `attributes` map.
