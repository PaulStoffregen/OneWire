cmake_minimum_required(VERSION 3.6)
project(OneWire)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        examples/bus_discovery/bus_discovery.ino
        examples/DS18x20_Temperature/DS18x20_Temperature.ino
        examples/DS2408_Switch/DS2408_Switch.ino
        examples/DS250x_PROM/DS250x_PROM.ino
        examples/DS2438_BatMon/DS2438_BatMon.ino
        src/platform.cpp
        src/OneWire.cpp
        )

add_executable(OneWire ${SOURCE_FILES})