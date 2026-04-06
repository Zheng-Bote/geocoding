# Country Info Adapter

The `CountryInfoAdapter` provides access to detailed country information from the RestCountries API.

---

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->

**Table of Contents**

- [Country Info Adapter](#country-info-adapter)
  - [Details](#details)
  - [Configuration](#configuration)
  - [Functionality](#functionality)
  - [Example Result](#example-result)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

---

## Details

- **Header:** `adapter_country_info.hpp`
- **Class:** `regeocode::CountryInfoAdapter`
- **INI Section:** `[country_info]`
- **Data Source:** [RestCountries API](https://restcountries.com/)

## Configuration

In `re-geocode.ini`:

```ini
[country_info]
URI = https://restcountries.com/v3.1/alpha/{{ country_code }}
Adapter = country_info
API-Key = CN
daily-limit = 1000
timeout = 10
type = info
```

_Note: The `API-Key` field in the INI section is used to populate the `{{ country_code }}` template variable if it's not provided elsewhere._

## Functionality

The adapter parses the following fields from the RestCountries JSON response:

| Field           | Source JSON Path          | Description                      |
| --------------- | ------------------------- | -------------------------------- |
| `cca2`          | `[0].cca2`                | ISO 3166-1 alpha-2 country code  |
| `cca3`          | `[0].cca3`                | ISO 3166-1 alpha-3 country code  |
| `capital`       | `[0].capital`             | Capital city (stringified array) |
| `region`        | `[0].region`              | World region                     |
| `subregion`     | `[0].subregion`           | World subregion                  |
| `maps_osm`      | `[0].maps.openStreetMaps` | Link to OpenStreetMap            |
| `continents`    | `[0].continents`          | Continents (stringified array)   |
| `name_official` | `[0].name.official`       | Official country name            |
| `name_common`   | `[0].name.common`         | Common country name              |
| `currencies`    | `[0].currencies`          | Currencies (stringified object)  |
| `flag_png`      | `[0].flags.png`           | URL to flag PNG                  |
| `flag_svg`      | `[0].flags.svg`           | URL to flag SVG                  |
| `flag_alt`      | `[0].flags.alt`           | Alt text for the flag            |

## Example Result

When used in a geocoding chain, the `AddressResult` attributes will contain the above fields.

```json
{
  "cca2": "CN",
  "cca3": "CHN",
  "capital": "[\"Beijing\"]",
  "region": "Asia",
  "subregion": "Eastern Asia",
  "maps_osm": "https://www.openstreetmap.org/relation/270056",
  "continents": "[\"Asia\"]",
  "name_official": "People's Republic of China",
  "name_common": "China",
  "currencies": "{\"CNY\":{\"name\":\"Chinese yuan\",\"symbol\":\"¥\"}}",
  "flag_png": "https://flagcdn.com/w320/cn.png",
  "flag_svg": "https://flagcdn.com/cn.svg",
  "flag_alt": "The flag of China has a red field..."
}
```
