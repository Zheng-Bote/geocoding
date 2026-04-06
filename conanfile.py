import os
from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.files import copy

class RegeocodeConan(ConanFile):
    name = "regeocode"
    version = "1.2.0"
    description = "C++ Reverse Geocoding Library"
    homepage = "https://github.com/Zheng-Bote/geocoding"
    license = "MIT"
    topics = ("geocoding", "reverse-geocoding", "cpp23")

    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "build_cli": [True, False],
        "build_tests": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "build_cli": True,
        "build_tests": False,
    }

    exports_sources = (
        "CMakeLists.txt", 
        "src/*", 
        "include/*", 
        "cli/*", 
        "tests/*", 
        "regeocode.pc.in", 
        "LICENSE"
    )

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        self.requires("nlohmann_json/3.11.3")
        self.requires("libcurl/[>=7.80.0]")
        if self.options.build_cli:
            self.requires("cli11/2.4.1")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["BUILD_CLI"] = self.options.build_cli
        tc.variables["BUILD_TESTING"] = self.options.build_tests
        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        copy(self, "LICENSE", src=self.source_folder, dst=os.path.join(self.package_folder, "licenses"))

    def package_info(self):
        self.cpp_info.libs = ["regeocode"]
        self.cpp_info.set_property("cmake_target_name", "regeocode::lib")
        self.cpp_info.set_property("pkg_config_name", "regeocode")
