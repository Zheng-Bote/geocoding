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

## Example

**call**

```bash
./regeocode-cli --config data/re-geocode.ini --lat 52.370197 --lon 4.890444 --api seaweather
```

**returns**

```json
{
  "meta": {
    "api": "seaweather",
    "latitude": 52.370197,
    "longitude": 4.890444,
    "type": "info"
  },
  "result": {
    "country_code": "",
    "data": {
      "airTemperature_ecmwf": "-2.570000",
      "airTemperature_ecmwf:aifs": "-3.410000",
      "airTemperature_metno": "-3.630000",
      "airTemperature_noaa": "-1.350000",
      "airTemperature_sg": "-3.630000",
      "meta_dailyQuota": "10",
      "meta_lat": "52.370197",
      "meta_lng": "4.890444",
      "meta_requestCount": "1",
      "time": "2026-02-15T00:00:00+00:00",
      "waveHeight_dwd": "0.200000",
      "waveHeight_metno": "0.420000",
      "waveHeight_noaa": "0.430000",
      "waveHeight_sg": "0.420000"
    },
    "summary": "Air Temp: -2.570000°C Wave: 0.200000m",
    "title": "Sea Weather Conditions"
  }
}
```
