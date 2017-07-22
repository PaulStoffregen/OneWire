OneWire Master Library
======================

### Features compared to lib 2.3.3
- cleaner, smaller, faster code with c++11 features (requires arduino sw 1.6.x or higher, >=1.6.10 recommended)
- proper naming of variables make it easier to read the code
- hardware-dependencies are combined in "platform.h"
   - supported: arduino zero, teensy, sam3x, pic32, [ATtiny](https://github.com/damellis/attiny), esp8266, nrf51822, raspberry (...)
   - for portability and tests the hub can be compiled on a PC with the supplied mock-up functions in platform.h
- more and better understandable examples
- use command to enable or disable parasitic bus powering by the master (was implemented wrong before)
- use compiler-switch to use pull-ups of micro controller 
- improved documentation

### Tested hardware
- run as ds18x20-master: 
   - Arduino Uno (Arduino AVR Boards)
   - Teensy 3.2 (teensyduino)
- compile process passes: 
   - Teensy LC (teensyduino)
   - Teensy 3.6 (teensyduino)
   - Arduino Zero native USB (Arduino SAMD Boards (32-bits ARM Cortex-M0+))
   - Arduino Primo (Arduino nRF52 Boards)
   - Arduino Due native USB (Arduino SAMD Boards (32-bits ARM Cortex-M3))
   - ATtiny 84 (attiny)
   - generic ESP8266 (esp8266)

### How to use
- understanding [onewire protocol](https://en.wikipedia.org/wiki/1-Wire) help a lot
- connect your microcontroller (onewire-master) with the onewire-bus
- the onewire-bus should be set up as described below
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
   
### Development history
Version 3.0
- allow to use internal pull-up if micro controller has support for it -> most support INPUT_PULLUP used on pinMode(), others will get an errormessage if feature is enables
- clean up compiler-switches
- better implementation to allow parasitic power to bus, otherwise master will act as open-drain-only! -> this was totally wrong before as the bus was powered for a short amount of time even if powering is disabled
- some µC enable pin-pullups when writing pin=high when pin is in input-mode -> use it, does not harm other
- extend examples with ds2438 and a bus-discovery feature
- clean up examples, easier to understand, more similar to each other
- put hardware dependant code into platform.h
- better naming of variables and a lot of code cleanup
- simplify pin-access and interrupt-handling
- improve onewire class-interface (it was possible to copy the object, ...) 
- reduce size of data types if possible, more const correctness and more explicit code
- get rid of plain old c and decide on one code style

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

### Plans for the future
- update documentation, maybe doxygen
- update the interface, NOTE: these changes can be made without breaking old code, just use "deprecated" on old interface 
  - use overloaded functions read and write_bytes(), write(), write_bit() become just write()
  - simplify bus powering as extra argument for reset and read as well, OR we set it on bus constructor
  - predefine standard onewire commands and use send(), skip() is not needed then
- unit tests
- CI

### Connecting the master with the bus: 

- the shown picture does not show the power line (assumes parasitic powering)
- by activating the internal pull up of the microcontroller you most likely do not need external PU

![Onewire-Schematic](http://wiki.lvl1.org/images/1/15/Onewire.gif)

[read more](http://wiki.lvl1.org/DS1820_Temp_sensor)

### Parasitic Power for the bus slave devices

advantages: 
- reduce number of wires to the slave device

how to do it:
- you will need to activate power-argument on the send-routines or use power()-function of the master
- for low power devices like the ds2401 it should be possible to just lower the resistance of the pullup-resistor of the bus and 
- most slave devices have the circuit shown in the lower picture already included (consult datasheet)

![Parasite-Power-Schematic](http://i.stack.imgur.com/0MeGL.jpg)

[read more](http://electronics.stackexchange.com/questions/193300/digital-ic-that-draws-power-from-data-pins)
