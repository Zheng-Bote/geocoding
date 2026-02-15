# OpenWeather Adapter

The `OpenWeatherAdapter` provides support for the OpenWeather Geocoding API.

## Details

- **Adapter Name:** `openweather`
- **Header:** `adapter_openweather.hpp`
- **Class:** `regeocode::OpenWeatherAdapter`

## Functionality

### `name()`

Returns the string `"openweather"`.

### `parse_response(const std::string &response_body)`

Parses the raw JSON response from OpenWeather. It extracts location information and stores it in the `AddressResult` structure.
