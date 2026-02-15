# Tides API Adapter

The `TidesAdapter` provides support for retrieving tidal information.

## Details

- **Adapter Name:** `tides`
- **Header:** `adapter_tides.hpp`
- **Class:** `regeocode::TidesAdapter`

## Functionality

### `name()`

Returns the string `"tides"`.

### `parse_response(const std::string &response_body)`

Parses the raw JSON response containing tide levels and times. Data is stored in the `attributes` map of the `AddressResult`.
