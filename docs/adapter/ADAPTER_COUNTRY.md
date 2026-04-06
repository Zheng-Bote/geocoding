# Country Adapter

The `CountryAdapter` provides access to country information stored in a local JSON file (`data/countries.json`). Unlike other adapters, it does not perform network requests but serves as a local lookup service for country details based on ISO 3166-1 alpha-2 codes.

---

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Details](#details)
- [Functionality](#functionality)
  - [`get_country(std::string_view country_code)`](#get_countrystdstring_view-country_code)
- [Example](#example)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

---

## Details

- **Header:** `adapter_country.hpp`
- **Class:** `regeocode::CountryAdapter`
- **Data Source:** `data/countries.json`

## Functionality

### `get_country(std::string_view country_code)`

Retrieves details for a given country code (e.g., "de", "cn"). The lookup is case-insensitive.

It returns a JSON object containing:

- `name.common`: Common name of the country.
- `name.official`: Official name of the country.
- `capital`: The capital city.
- `region`: The world region (e.g., Asia, Europe).
- `flag`: The country's flag emoji.
- `flag_url`: Link to the 4x3 SVG flag icon on GitHub.

## Example

**C++ Usage**

```cpp
#include "regeocode/adapter_country.hpp"
#include <iostream>

int main() {
    regeocode::CountryAdapter adapter("data/countries.json");
    auto cn = adapter.get_country("cn");
    std::cout << cn.dump(4) << std::endl;
    return 0;
}
```

**Output**

```json
{
  "name.common": "China",
  "name.official": "People's Republic of China",
  "capital": "Beijing",
  "region": "Asia",
  "flag": "🇨🇳",
  "flag_url": "https://github.com/lipis/flag-icons/blob/main/flags/4x3/cn.svg"
}
```
