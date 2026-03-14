# Marea Tides Adapter

The `MareaTidesAdapter` provides support for the Marea Tides API.

## Details

- **Adapter Name:** `marea_tides`
- **Header:** `adapter_marea_tides.hpp`
- **Class:** `regeocode::MareaTidesAdapter`

## Functionality

### `name()`

Returns the string `"marea_tides"`.

### `parse_response(const std::string &response_body)`

Parses the raw JSON response from the Marea Tides service.

## Example

**call**

```bash
./regeocode-cli --config data/re-geocode.ini --lat 52.370197 --lon 4.890444 --api tides
```

**returns**

```json
{
  "meta": {
    "api": "marea_tides",
    "latitude": 52.370197,
    "longitude": 4.890444,
    "type": "info"
  },
  "result": {
    "country_code": "",
    "data": {
      "copyright": "©2025 Marea | Generated using AVISO+ Products. FES2014 was produced by Noveltis, Legos and CLS and distributed by Aviso+, with support from Cnes (https://www.aviso.altimetry.fr/)",
      "current_height": "-0.505993",
      "current_state": "FALLING",
      "datum_DHQ": "0.163000",
      "datum_DLQ": "0.106000",
      "datum_DTL": "0.046000",
      "datum_GT": "1.738000",
      "datum_HAT": "1.446000",
      "datum_LAT": "-1.339000",
      "datum_MHHW": "0.915000",
      "datum_MHW": "0.752000",
      "datum_MLLW": "-0.823000",
      "datum_MLW": "-0.717000",
      "datum_MN": "1.469000",
      "datum_MSL": "0.000000",
      "datum_MTL": "0.018000",
      "event_0_height": "-0.625212",
      "event_0_state": "LOW TIDE",
      "event_0_time": "2026-02-15T09:50:13+00:00",
      "event_1_height": "0.709519",
      "event_1_state": "HIGH TIDE",
      "event_1_time": "2026-02-15T14:54:22+00:00",
      "event_2_height": "-0.648456",
      "event_2_state": "LOW TIDE",
      "event_2_time": "2026-02-15T22:21:27+00:00",
      "event_3_height": "0.581175",
      "event_3_state": "HIGH TIDE",
      "event_3_time": "2026-02-16T02:28:18+00:00",
      "source": "FES2014",
      "station_distance": "26.57 km",
      "unit": "m"
    },
    "summary": "LOW TIDE (-0.63m) at 2026-02-15T09:50:13+00:00",
    "title": "Tide Information"
  }
}
```
