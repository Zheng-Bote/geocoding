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

## Example

**call**

```bash
./regeocode-cli --config data/re-geocode.ini --lat 48.137222 --lon 11.575278 --api pollution
```

**returns**

```json
{
  "meta": {
    "api": "pollution",
    "latitude": 48.139722,
    "longitude": 11.574444,
    "type": "info"
  },
  "result": {
    "country_code": "",
    "data": {
      "aqi": "1",
      "co": "215.340000",
      "nh3": "0.970000",
      "no": "1.910000",
      "no2": "4.820000",
      "o3": "29.880000",
      "pm10": "2.250000",
      "pm2_5": "2.100000",
      "so2": "0.240000"
    },
    "summary": "",
    "title": "Air Quality: Good"
  }
}
```
