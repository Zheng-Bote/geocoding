# GeoNames Wikipedia Adapter

The `GeoNamesWikipediaAdapter` provides support for the GeoNames Wikipedia API, which finds Wikipedia entries near a specific location.

## Details

- **Adapter Name:** `nearbyWikipedia`
- **Header:** `adapter_geonames_wikipedia.hpp`
- **Class:** `regeocode::GeoNamesWikipediaAdapter`

## Functionality

### `name()`

Returns the string `"nearbyWikipedia"`. This matches the entry `Adapter = nearbyWikipedia` in the configuration.

### `parse_response(const std::string &response_body)`

Parses the raw JSON response from GeoNames. It extracts information about nearby Wikipedia articles and stores it in the `AddressResult`.
