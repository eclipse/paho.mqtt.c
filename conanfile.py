from conans import ConanFile, CMake, tools


class PahocConan(ConanFile):
    name = "paho.mqtt.c"
    version = "1.3.1"
    license = "Eclipse Public License - v 1.0"
    url = "https://github.com/eclipse/paho.mqtt.c"
    description = """The Eclipse Paho project provides open-source client implementations of MQTT
and MQTT-SN messaging protocols aimed at new, existing, and emerging applications for the Internet
of Things (IoT)"""
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "SSL": [True, False], "asynchronous": [True, False]}
    default_options = "shared=False", "SSL=False", "asynchronous=False"
    generators = "cmake"
    exports_sources = "*"

    def requirements(self):
        if self.options.SSL:
            self.requires("OpenSSL/1.0.2n@conan/stable")

    def build(self):
        tools.replace_in_file("CMakeLists.txt", "PROJECT(\"paho\" C)", '''PROJECT("paho" C)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()''')
        tools.replace_in_file("CMakeLists.txt", "ADD_SUBDIRECTORY(test)", "") #  Disable tests
        tools.replace_in_file("CMakeLists.txt",
                              "ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE -DWIN32_LEAN_AND_MEAN -MD)",
                              "ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE -DWIN32_LEAN_AND_MEAN)") #  Allow other runtimes
        cmake = CMake(self)
        cmake.definitions["PAHO_BUILD_DOCUMENTATION"] = False
        cmake.definitions["PAHO_BUILD_SAMPLES"] = False
        cmake.definitions["PAHO_BUILD_DEB_PACKAGE"] = False
        cmake.definitions["PAHO_BUILD_STATIC"] = not self.options.shared
        cmake.definitions["PAHO_WITH_SSL"] = self.options.SSL
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy("*e*l-v10", dst="licenses")
        self.copy("*.h", dst="include", src="src")
        self.copy("*paho*.dll", dst="bin", keep_path=False)
        self.copy("*paho*.dylib", dst="lib", keep_path=False)
        self.copy("*paho*.so*", dst="lib", keep_path=False)
        self.copy("*paho*.a", dst="lib", keep_path=False)
        self.copy("*paho*.lib", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = []

        if self.options.shared and self:
            if self.options.asynchronous:
                if self.options.SSL:
                    self.cpp_info.libs.append("paho-mqtt3as")
                else:
                    self.cpp_info.libs.append("paho-mqtt3a")
            else:
                if self.options.SSL:
                    self.cpp_info.libs.append("paho-mqtt3cs")
                else:
                    self.cpp_info.libs.append("paho-mqtt3c")
        else:
            if self.options.asynchronous:
                if self.options.SSL:
                    self.cpp_info.libs.append("paho-mqtt3as")
                else:
                    self.cpp_info.libs.append("paho-mqtt3a")
            else:
                if self.options.SSL:
                    self.cpp_info.libs.append("paho-mqtt3cs")
                else:
                    self.cpp_info.libs.append("paho-mqtt3c")

        if self.settings.os == "Windows":
            if not self.options.shared:
                self.cpp_info.libs.append("ws2_32")
                if self.settings.compiler == "gcc":
                    self.cpp_info.libs.append("wsock32") # (MinGW) needed?
        else:
            if self.settings.os == "Linux":
                self.cpp_info.libs.append("c")
                self.cpp_info.libs.append("dl")
                self.cpp_info.libs.append("pthread")
            elif self.settings.os == "FreeBSD":
                self.cpp_info.libs.append("compat")
                self.cpp_info.libs.append("pthread")
            else:
                self.cpp_info.libs.append("c")
                self.cpp_info.libs.append("pthread")

    def configure(self):
        del self.settings.compiler.libcxx
