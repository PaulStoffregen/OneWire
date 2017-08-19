OneWire Master Library
======================

[![Build Status](https://travis-ci.org/orgua/OneWire.svg?branch=master)](https://travis-ci.org/orgua/OneWire)

### Features
- emulates a bus master in a generic way, the lib is handling most of the timing-constrains and the hardware-access
- clean, small, fast code with c++11 features (requires arduino sw 1.6.x or higher)
- proper naming of variables make it easier to read the code
- hardware-dependencies are combined in "platform.h" (see next section)
   - for portability and tests the lib can be compiled on a PC with the supplied mock-up functions in platform.h
- extended and easy understandable examples
- command to enable or disable parasitic bus powering by the master (was implemented wrong before)
- compiler-switch to use pull-ups of micro controller 
- improved documentation in doxygen style
- decided on one code style - Allman braces - was closest to main style in original

### Supported and tested Hardware
- embedded real life test
   - setup: run ds18x20-master, enabled internal pull up (if possible), arduino 1.8.3, Windows 10 and the board-library named in the brackets
   - Arduino Uno ([Arduino AVR Boards](https://github.com/arduino/Arduino/tree/master/hardware/arduino/avr))
   - Arduino Due ([Arduino SAMD Boards (32-bits ARM Cortex-M3)](https://github.com/arduino/ArduinoCore-sam)) -> NOTE: internal PU is not strong enough to power bus
   - Arduino MKRZero ([Arduino SAMD Boards (32-bits ARM Cortex-M0+)](https://github.com/arduino/ArduinoCore-samd)) -> NOTE: internal PU is not strong enough to power bus
   - Teensy 3.2 ([teensyduino](https://github.com/PaulStoffregen/cores))
   - Wemos D1 Mini ESP32S ([esp32](https://github.com/espressif/arduino-esp32)) -> NOTE: internal PU is not strong enough to power bus
   - Wemos Wifi & BT ESP32 ([esp32](https://github.com/espressif/arduino-esp32)) -> NOTE: internal PU is not strong enough to power bus
   - Wemos D1 R2 ([esp8266](https://github.com/esp8266/Arduino))
   - nodeMCU 1.0 ESP-12E ([esp8266](https://github.com/esp8266/Arduino))
- Travis CI (automated Continuous Integration) for different platforms
   - Arduino Uno ([Arduino AVR Boards](https://github.com/arduino/Arduino/tree/master/hardware/arduino/avr))
   - Arduino Due ([Arduino SAMD Boards (32-bits ARM Cortex-M3)](https://github.com/arduino/ArduinoCore-sam))
   - Arduino Zero ([Arduino SAMD Boards (32-bits ARM Cortex-M0+)](https://github.com/arduino/ArduinoCore-samd))
   - Arduino 101 ([Intel Curie Boards](https://github.com/01org/corelibs-arduino101))
   - Teensy 3.0, 3.1, 3.2, LC, 3.5, 3.6 ([teensyduino](https://github.com/PaulStoffregen/cores))
   - generic ESP8266 ([esp8266](https://github.com/esp8266/Arduino))
   - nodeMCU V2 ([esp8266](https://github.com/esp8266/Arduino))
   - espduino ([esp8266](https://github.com/esp8266/Arduino))
   - ESP32 dev module ([esp32](https://github.com/espressif/arduino-esp32))
   - RFduino nrf51 ([RFduino](https://github.com/RFduino/RFduino))
   - ATtiny 84, 88 ([attiny](https://github.com/damellis/attiny)??)
   - Digispark tiny ([DigistumpArduino](https://github.com/digistump/DigistumpArduino))
- manual compile test: process passes 
   - setup: run ds18x20-master, arduino 1.8.3, Windows 10 and the board-library named in the brackets
   - Arduino Primo ([Arduino nRF52 Boards](https://github.com/arduino-org/arduino-core-nrf52))  
   - Teensy 2.0, 2.0++ ([teensyduino](https://github.com/PaulStoffregen/cores)) 
   - Digilent chipKIT UNO32 [pic32](https://github.com/chipKIT32/chipKIT-core)
   - RedBear [nRF51](https://github.com/RedBearLab/nRF51822-Arduino)
- if you successfully tested a new controller don't hesitate to open an issue on github to document it
- if you modified / extended the lib to support new hardware don't hesitate to open a pull request on github

### How to use
- understanding [onewire protocol](https://en.wikipedia.org/wiki/1-Wire) helps a lot during "how to"
- connect your microcontroller (onewire-master) with the onewire-bus
   - the onewire-bus should be set up as described in separate section below
- the examples are mostly self explanatory
   - the microcontroller acts as a master
   - it will search and select devices on the bus
   - master will read data from devices
   - data is processed and send via uart to the pc
- onewire.h has the following compiler-switches to enable special features
   - USE_CRC8_TABLE: will trade compilation size for speed
   - USE_PULL_UP: activate internal pull up of microcontroller (external bus PU most likely not needed then)

### HELP - What to do if things don't work as expected?
- check if your arduino software up to date (>v1.6.8)
- update this lib to the latest release (v3.0.0)
- check if clock-speed of the µC is set correctly (if possible) - test with simple blink example, 1sec ON should really need 1sec. timing is critical
- begin with a simple example like the ds18x20 (if possible) or the bus_discovery example
- check if your setup is right: see sections below
- is there more than one master on the bus? It won't work!
- has any other master ever worked with the slave device?
- if you can provide a recording via logic-analyzer (logic 8 or similar) there should be chance we can help you 
- if you checked all these points feel free to open an issue at [Github](https://github.com/orgua/OneWire) and describe your troubleshooting process
   - please provide the following basic info: which µC do you use, software versions, what device do you try to work with, what works, what doesn't
   
### Plans for the future
- update the interface, NOTE: these changes can be made without breaking old code, just use "deprecated" on old interface 
   - simplify bus powering as extra argument for reset and read as well, OR we set it on bus constructor
   - predefine standard onewire commands and use send(), skip() is not needed then
- unit tests
- overdrive and other features (alarm search)
- add or extend hardware support
   - stm32f4     https://github.com/arduino-org/arduino-core-stm32f4
   - stm32       https://github.com/rogerclarkmelbourne/Arduino_STM32
   - msp430 E    https://github.com/energia/Energia
   - msp430 arduino, platformIO speaks of a different framework
- add travis tests, currently not supported
   - NRF52 /* arduino primo */
   - NRF51 /* red bear blend, should be good for all nrf51x chips */
   - pic32 -> problem with toolchain in platformIO
   - teensy 2 -> crc-lib has linker problem in platformIO

### Development history
Version 3.0
- real life tested with several platforms (atmega, atsamD21, Atsam3x, esp8266, esp32, teensy)
- extend and fix hardware support (specially esp-boards, esp32, ...)
- added travis CI for a big bunch of controllers
- delayMicroseconds() hardware abstraction to allow proper nRF51-use (redbear messed up the delaymicroseconds()-implementation)
   - will be removed as soon redbear fixes its code
- overload write() to replace write_bytes() and read() to replace read_byte()
- update code documentation, doxygen style
- allow to use internal pull-up if micro controller has support for it -> most µC support INPUT_PULLUP in combination with pinMode(), others will get an error message or warning if feature is enabled but not supported / tested
- clean up compiler-switches
- better implementation to allow parasitic power to bus, otherwise master will act as open-drain-only! -> this was totally wrong before as the bus was powered for a short amount of time even if powering is disabled
- extend examples with ds2438 and a bus-discovery feature
- clean up examples, easier to understand, more similar to each other
- put hardware dependant code into platform.h
- better naming of variables and a lot of code cleanup
- simplify pin-access and interrupt-handling
- improve onewire class-interface (it was possible to copy the object, ...) 
- reduce size of data types if possible, more const correctness and much more explicit code
- get rid of most ancient c and cpp code and reduce language mix
- decide on one code style - Allman braces - was closest to main style in original
- a lot of how-to documentation for this lib inside this readme

Version 2.3
- Unknown chip fallback mode, Roger Clark
- Teensy-LC compatibility, Paul Stoffregen
- Search bug fix, Love Nystrom

Version 2.2
- Teensy 3.0 compatibility, Paul Stoffregen, paul@pjrc.com
- Arduino Due compatibility, http://arduino.cc/forum/index.php?topic=141030
- Fix DS18B20 example negative temperature
- Fix DS18B20 example's low res modes, Ken Butcher
- Improve reset timing, Mark Tillotson
- Add const qualifiers, Bertrik Sikken
- Add initial value input to crc16, Bertrik Sikken
- Add target_search() function, Scott Roberts

Version 2.1
- Arduino 1.0 compatibility, Paul Stoffregen
- Improve temperature example, Paul Stoffregen
- DS250x_PROM example, Guillermo Lovato
- PIC32 (chipKit) compatibility, Jason Dangel, dangel.jason AT gmail.com
- Improvements from Glenn Trewitt:
   - crc16() now works
   - check_crc16() does all of calculation/checking work.
   - Added read_bytes() and write_bytes(), to reduce tedious loops.
   - Added ds2408 example.
- Delete very old, out-of-date readme file (info is here)

Version 2.0
- Modifications by Paul Stoffregen, January 2010: http://www.pjrc.com/teensy/td_libs_OneWire.html
- Search fix from Robin James http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1238032295/27#27
- Use direct optimized I/O in all cases
- Disable interrupts during timing critical sections (this solves many random communication errors)
- Disable interrupts during read-modify-write I/O
- Reduce RAM consumption by eliminating unnecessary variables and trimming many to 8 bits
- Optimize both crc8 - table version moved to flash

Version before 2.0
- Modified to work with larger numbers of devices - avoids loop. Tested in Arduino 11 alpha with 12 sensors. 26 Sept 2008 -- Robin James
- Updated to work with arduino-0008 and to include skip() as of 2007/07/06. --RJL20
- Modified to calculate the 8-bit CRC directly, avoiding the need for the 256-byte lookup table to be loaded in RAM.  Tested in arduino-0010 -- Tom Pollard, Jan 23, 2008

### Connecting the master with the bus: 

- the picture does not show the power line (assumes parasitic powering)
- by activating the internal pull up of the microcontroller you most likely do not need external PU

![Onewire-Schematic](http://wiki.lvl1.org/images/1/15/Onewire.gif)

[read more](http://wiki.lvl1.org/DS1820_Temp_sensor)

### Parasitic Power for the bus slave devices

advantages: 
- reduce number of wires to the slave device

how to do it:
- you will need to activate power-argument on the send-routines or use power()-function of the master
- for low power devices like the ds2401 it should be possible to just lower the resistance of the pullup-resistor of the bus (~1k)
- most slave devices have the circuit shown in the lower picture already included (consult datasheet)
- note: you should only activate / use this feature if you know what you are doing
- possible safety-precaution: do not directly connect the master-pin to the bus, instead add ~200 Ohm between -> in case of shortage or any other bus failure there will only be ~16mA of current flowing 

![Parasite-Power-Schematic](http://i.stack.imgur.com/0MeGL.jpg)

[read more](http://electronics.stackexchange.com/questions/193300/digital-ic-that-draws-power-from-data-pins)
