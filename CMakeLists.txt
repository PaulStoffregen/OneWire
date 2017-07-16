cmake_minimum_required(VERSION 3.6)
project(OneWire)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        examples/DS18x20_Temperature/DS18x20_Temperature.ino
        examples/DS2408_Switch/DS2408_Switch.ino
        examples/DS250x_PROM/DS250x_PROM.ino
        src/platform.cpp
        src/OneWire.cpp
        )

add_executable(OneWire ${SOURCE_FILES})