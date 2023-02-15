
# Author: diplfranzhoepfinger 
# reference:  https://docs.espressif.com/projects/arduino-esp32/en/latest/esp-idf_component.html
# URL: https://github.com/PaulStoffregen/OneWire
# DATE: 15.02.2023

idf_component_register(
    SRCS "OneWire.cpp"
    INCLUDE_DIRS "."
    PRIV_REQUIRES arduino
    )
