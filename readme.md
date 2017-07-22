TODO: fill 

### Features
- cleaner, smaller, faster code with c++11 features (requires arduino sw 1.6.x or higher, >=1.6.10 recommended)
- hardware-dependencies are combined in "platform.h"
   - supported: arduino zero, teensy, sam3x, pic32, [ATtiny](https://github.com/damellis/attiny), esp8266, nrf51822, raspberry (...)
   - tested architectures: atmega328 @ 16 MHz / arduino Uno, teensy3.2
   - for portability and tests the hub can be compiled on a PC with the supplied mock-up functions in platform.h
- use command to enable or disable parasitic bus powering by the master (was implemented wrong before)
- use compiler-switch to use pull-ups of micro controller 

### History
- allow to use pull-up if micro controller has support for it -> most support INPUT_PULLUP used on pinMode(), others will get an errormessage if feature is enables
- clean up compiler-switches
- better implementation to allow parasitic power to bus, otherwise master will act as open-drain-only! -> this was totally wrong before as the bus was powered for a short amount of time even if powering is disabled
- some µC enable pin-pullups when writing pin=high when pin is in input-mode -> use it, does not harm other
- extend examples with ds2438 and a bus-discovery feature
- clean up examples, easier to understand, more similar to each other
- put hardware dependant code into platform.h
- better naming of variables and a lot of code cleanup
- simplify pin-access and interrupt-handling
- improve onewire class-interface (it was possible to copy the object, ...) 
- reduce size of datatypes if possible, more const correctness and more explicit code
- get rid of plain old c and decide on one code style

### tested hardware
- run as master: 
   - Arduino Uno (Arduino AVR Boards)
   - Teensy 3.2 (teensyduino)
- compile process: 
   - Teensy LC (teensyduino)
   - Teensy 3.6 (teensyduino)
   - Arduino Zero native USB (Arduino SAMD Boards (32-bits ARM Cortex-M0+))
   - Arduino Primo (Arduino nRF52 Boards)
   - Arduino Due native USB (Arduino SAMD Boards (32-bits ARM Cortex-M3))
   - ATtiny 84 (attiny)
   - generic ESP8266 (esp8266)
   
### todo
- https://github.com/sandeepmistry/arduino-nRF5

## How to use

### Help

### Connecting the master with the bus: 

![Onewire-Schematic](http://wiki.lvl1.org/images/1/15/Onewire.gif)

[read more](http://wiki.lvl1.org/DS1820_Temp_sensor)

### Parasite Power with two wires

![Parasite-Power-Schematic](http://i.stack.imgur.com/0MeGL.jpg)

**Note:** you will need to activate power-argument on the send-routines or lower the pullup-resistor.

[read more](http://electronics.stackexchange.com/questions/193300/digital-ic-that-draws-power-from-data-pins)
