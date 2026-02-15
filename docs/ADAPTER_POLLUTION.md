# Pollution API Adapter

The `PollutionAdapter` provides support for environmental pollution data APIs.

## Details

- **Adapter Name:** `pollution`
- **Header:** `adapter_pollution.hpp`
- **Class:** `regeocode::PollutionAdapter`

## Functionality

### `name()`

Returns the string `"pollution"`. This name matches the entry `Adapter = pollution` in the configuration.

### `parse_response(const std::string &response_body)`

Parses the raw JSON response containing pollution metrics. Results are typically stored in the `attributes` map of the `AddressResult`.
