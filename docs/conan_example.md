# Building and Distributing re-geocode via Conan

The recommended way to integrate `re-geocode` into other C++ projects is by using the Conan 2 package manager. 

## 1. Creating the Package Locally

If you want to build and test the package on your local machine, you can export it to your local Conan cache:

```bash
# Clone the repository
git clone https://github.com/Zheng-Bote/geocoding.git
cd geocoding

# Build and create the Conan package
conan create . -s build_type=Release
```

> **Note on Missing Binaries**: If you encounter a `Missing prebuilt package` error for dependencies like `libcurl` or `openssl` (because no precompiled binaries exist for your specific compiler setup on ConanCenter), append `--build=missing` to the command:
> ```bash
> conan create . -s build_type=Release --build=missing
> ```

## 2. Using the Package in your Project

Once the package is in your local cache (or available via a remote Artifactory), you can easily add it to any C++ project.

### `conanfile.txt` / `conanfile.py`
Add the dependency:

```ini
[requires]
regeocode/1.0.0
```

### `CMakeLists.txt`
Link it to your target:

```cmake
find_package(regeocode REQUIRED)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE regeocode::lib)
```

## 3. Uploading to a Remote Server

To share the package with your team or use it in CI pipelines, you can upload it to a Conan Remote (e.g., JFrog Artifactory).

### Add Remote & Authenticate
```bash
conan remote add myremote https://your-artifactory-url/api/conan/conan-local
conan remote login myremote your_username
```

### Upload Package and Binaries
Use the `-c` flag to upload all locally built binaries alongside the recipe:
```bash
conan upload regeocode/1.0.0 -r myremote -c
```
