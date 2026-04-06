# GeoNames Wikipedia Adapter

The `GeoNamesWikipediaAdapter` provides support for the GeoNames Wikipedia API, which finds Wikipedia entries near a specific location.

---

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->

**Table of Contents**

- [GeoNames Wikipedia Adapter](#geonames-wikipedia-adapter)
  - [Details](#details)
  - [Functionality](#functionality)
    - [`name()`](#name)
    - [`parse_response(const std::string &response_body)`](#parse_responseconst-stdstring-response_body)
  - [Example](#example)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

---

## Details

- **Adapter Name:** `nearbyWikipedia`
- **Header:** `adapter_geonames_wikipedia.hpp`
- **Class:** `regeocode::GeoNamesWikipediaAdapter`

## Functionality

### `name()`

Returns the string `"nearbyWikipedia"`. This matches the entry `Adapter = nearbyWikipedia` in the configuration.

### `parse_response(const std::string &response_body)`

Parses the raw JSON response from GeoNames. It extracts information about nearby Wikipedia articles and stores it in the `AddressResult`.

## Example

**call**

```bash
./regeocode-cli --config data/re-geocode.ini --lat 48.137222 --lon 11.575278 --api nearbyWikipedia
```

**returns**

```json
{
  "meta": {
    "api": "nearbyWikipedia",
    "latitude": 48.139722,
    "longitude": 11.574444,
    "type": "info"
  },
  "result": {
    "country_code": "DE",
    "data": {},
    "summary": "Munich is the capital and most populous city of the second most populous German federal state of Bavaria, and, with a population of around 1.5 million, it is the third-largest city of Germany after Berlin and Hamburg, as well as the 12th-largest city in the European Union (...) (en.wikipedia.org/wiki/Munich)",
    "title": "Munich"
  }
}
```
