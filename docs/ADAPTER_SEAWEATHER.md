# SeaWeather Adapter

The `SeaWeatherAdapter` provides support for specialized maritime weather APIs.

## Details

- **Adapter Name:** `seaweather`
- **Header:** `adapter_seaweather.hpp`
- **Class:** `regeocode::SeaWeatherAdapter`

## Functionality

### `name()`

Returns the string `"seaweather"`.

### `parse_response(const std::string &response_body)`

Parses the raw JSON response from a sea weather provider. Maritime-specific weather data is extracted and stored in `AddressResult`.
