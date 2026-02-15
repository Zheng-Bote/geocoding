# re-geocode pkg-config example

## Usage via pkg-config (for Makefiles or Terminal)

pkg-config is the standard on Linux systems for querying compiler and linker flags for installed libraries. This assumes that you have run `sudo make install` and (ideally) your CMake configuration has installed a .pc file (or you simply use the paths if they are standard).

**Prerequisite**: The library is located under /usr/local/lib and headers under /usr/local/include.

### A. Directly in the Terminal (Quick & Dirty)

If you want to test a small main.cpp quickly without setting up CMake:

```bash
g++ -std=c++23 main.cpp -o my_app \
    $(pkg-config --cflags --libs libcurl) \
    -lregeocode
```

### B. Integration into a classic Makefile

If you have a project that still uses Make instead of CMake:

```makefile
CXX      = g++
CXXFLAGS = -std=c++23 -Wall -Wextra -O2
# Includes for regeocode (if under /usr/local/include)
INCLUDES = -I/usr/local/include

# Link libraries: regeocode + curl
# We use pkg-config here for curl to be safe
LIBS     = -L/usr/local/lib -lregeocode $(shell pkg-config --libs libcurl)

TARGET   = my_app
SRC      = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(SRC) $(LIBS)

clean:
	rm -f $(TARGET)
```

```bash
g++ main.cpp $(pkg-config --cflags --libs regeocode)
```
