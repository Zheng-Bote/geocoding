# API Adapters

This document provides an overview of the available API adapters in the `regeocode` library. All adapters inherit from the `ApiAdapter` base class and provide a consistent interface for parsing responses from various geocoding and environmental data services.

## Available Adapters

- [Bing Maps](ADAPTER_BING.md)
- [Google Maps](ADAPTER_GOOGLE.md)
- [Nominatim (OpenStreetMap)](ADAPTER_NOMINATIM.md)
- [OpenCage Data](ADAPTER_OPENCAGE.md)
- [OpenWeather](ADAPTER_OPENWEATHER.md)
- [Pollution API](ADAPTER_POLLUTION.md)
- [SeaWeather](ADAPTER_SEAWEATHER.md)
- [Tides API](ADAPTER_TIDES.md)
- [Marea Tides](ADAPTER_MAREA_TIDES.md)
- [GeoNames Timezone](ADAPTER_GEONAMES_TIMEZONE.md)
- [GeoNames Wikipedia](ADAPTER_GEONAMES_WIKIPEDIA.md)

## Common Interface

All adapters implement the following methods:

- `name()`: Returns the unique identifier for the adapter.
- `parse_response(const std::string &response_body)`: Parses the raw JSON response into an `AddressResult` structure.
