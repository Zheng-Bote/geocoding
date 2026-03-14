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

## Example

**call**

```bash
./regeocode-cli --config data/re-geocode.ini --lat 48.137222 --lon 11.575278 --api openweather
```

**returns**

```json
{
  "meta": {
    "api": "openweather",
    "latitude": 48.139722,
    "longitude": 11.574444,
    "type": "info"
  },
  "result": {
    "country_code": "DE",
    "data": {
      "condition": "overcast clouds",
      "feels_like": "-1.340000",
      "humidity": "94",
      "pressure": "1018",
      "temp": "-1.340000",
      "wind_deg": "98",
      "wind_speed": "0.450000"
    },
    "summary": "overcast clouds",
    "title": "Munich"
  }
}
```
