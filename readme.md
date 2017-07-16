TODO: fill 

### Features
- cleaner, faster code with c++11 features (requires arduino sw 1.6.x or higher, >=1.6.10 recommended)
- hardware-dependencies are combined in "platform.h"
   - supported: arduino zero, teensy, sam3x, pic32, [ATtiny](https://github.com/damellis/attiny), esp8266, nrf51822, raspberry (...)
   - tested architectures: atmega328 @ 16 MHz / arduino Uno, teensy3.2
   - for portability and tests the hub can be compiled on a PC with the supplied mock-up functions in platform.h
### History
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

## How to use

### Help

### Connecting the master with the bus: 

![Onewire-Schematic](http://wiki.lvl1.org/images/1/15/Onewire.gif)

[read more](http://wiki.lvl1.org/DS1820_Temp_sensor)

### Parasite Power with two wires

![Parasite-Power-Schematic](http://i.stack.imgur.com/0MeGL.jpg)

**Note:** you will need to activate power-argument on the send-routines or lower the pullup-resistor.

[read more](http://electronics.stackexchange.com/questions/193300/digital-ic-that-draws-power-from-data-pins)
