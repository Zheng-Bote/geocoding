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

## Example

**call**

```bash
./regeocode-cli --config data/re-geocode.ini --lat 52.370197 --lon 4.890444 --api tides
```

**returns**

```json
{
  "meta": {
    "api": "tides",
    "latitude": 52.370197,
    "longitude": 4.890444,
    "type": "info"
  },
  "result": {
    "country_code": "",
    "data": {
      "current_height": "-0.504535",
      "current_state": "FALLING",
      "disclaimer": "NOT SUITABLE FOR NAVIGATIONAL PURPOSES. API Hood does not warrant that the provided data will be free from errors or omissions. Provided data are NOT suitable for usage where someone could be harmed or suffer any damage.",
      "event_0_height": "-0.625164",
      "event_0_state": "LOW TIDE",
      "event_0_time": "2026-02-15T09:50:15+00:00",
      "event_1_height": "0.709432",
      "event_1_state": "HIGH TIDE",
      "event_1_time": "2026-02-15T14:54:15+00:00",
      "event_2_height": "-0.648449",
      "event_2_state": "LOW TIDE",
      "event_2_time": "2026-02-15T22:21:26+00:00",
      "event_3_height": "0.581024",
      "event_3_state": "HIGH TIDE",
      "event_3_time": "2026-02-16T02:28:12+00:00",
      "station_distance": "26.570000 km",
      "unit": "m"
    },
    "summary": "LOW TIDE (-0.63m) at 2026-02-15T09:50:15+00:00",
    "title": "Tide Information"
  }
}
```
