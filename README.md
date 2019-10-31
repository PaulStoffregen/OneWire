# OneWire Library

## Description

OneWire lets you access 1-wire devices, such as temperature sensors and ibutton secure memory. For temperature sensors, the DallasTemperature library can be used with this library.

## Installation

### First Method

1. In the Arduino IDE, navigate to Sketch > Include Library > Manage Libraries
1. Then the Library Manager will open and you will find a list of libraries that are already installed or ready for installation.
1. Then search for EduIntro using the search bar.
1. Click on the text area and then select the zpecific version and install it.

### Second Method

1. Navigate to the Releases page.
1. Download the latest release.
1. Extract the zip file
1. In the Arduino IDE, navigate to Sketch > Include Library > Add .ZIP Library

## Requirements

![image](https://user-images.githubusercontent.com/36513474/67940921-62d28080-fbf6-11e9-9b07-1b620e3cdd8d.png)

OneWire requires a single 4.7K pullup resistor, connected between the pin and +5 volts. Then just connect each 1-wire device to the pin and ground. Some 1-wire devices can also connect to power, or get their power from the signal wire. Please refer to the specifications for the 1-wire devices you are using.

## Features

- ### Inexpensive

     The 1-wire temperature sensors have become particularly popular, because they're inexpensive and easy to use, providing calibrated digital temperature readings directly.

- ### More tolerant

    1-wire is more tolerant of long wires between sensor and Arduino

- ### Modes

    Many 1-Wire chips can operate in both parasitic and normal power modes

- ### Accession

    MicroLans can be accessed directly by an Arduino, using the mature Arduino OneWire library.

- ### Give back

    OneWire is free for everyone. The licensed document can be copied, redistributed and used in the projects, assignments or anywhere.

## Functions

- begin()
- reset()
- write_bit()
- read_bit()
- write()
- write_bytes()
- read()
- read_bytes()
- select()
- skip()
- depower()
- reset_search()
- target_search()
- search()
- crc8()
- check_crc16()

For further functions description visit [OneWire](https://www.pjrc.com/teensy/td_libs_OneWire.html)

## Example

There are many examples implemented where this library is used. You can find other examples from [Github-OneWire](https://github.com/PaulStoffregen/OneWire/tree/master/examples) and [Arduino-Reference](https://www.pjrc.com/teensy/td_libs_OneWire.html)

- ### DS2408_Switch

  Unlike the most input/output ports, the DS2408 doesn't have mode bits to set whether the pins are input or output.  If you issue a read command, they're inputs.  If you write to them, they're outputs.

  - For reading from a switch, you should use 10K pull-up resisters.

``` Cpp

#include <OneWire.h>
OneWire net(10);

void PrintBytes(const uint8_t* addr, uint8_t count, bool newline=false) {
  for (uint8_t i = 0; i < count; i++) {
    Serial.print(addr[i]>>4, HEX);
    Serial.print(addr[i]&0x0f, HEX);
  }
  if (newline)
    Serial.println();
}

void setup(void) {
  Serial.begin(9600);
}

void loop(void) {
  byte addr[8];

  if (!net.search(addr)) {
    Serial.print("No more addresses.\n");
    net.reset_search();
    delay(1000);
    return;
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.print("CRC is not valid!\n");
    return;
  }

  if (addr[0] != 0x29) {
    PrintBytes(addr, 8);
    Serial.print(" is not a DS2408.\n");
    return;
  }

  Serial.print("  Reading DS2408 ");
  PrintBytes(addr, 8);
  Serial.println();

  uint8_t buf[13];
  buf[0] = 0xF0;
  buf[1] = 0x88;
  buf[2] = 0x00;
  net.write_bytes(buf, 3);
  net.read_bytes(buf+3, 10);
  net.reset();

  if (!OneWire::check_crc16(buf, 11, &buf[11])) {
    Serial.print("CRC failure in DS2408 at ");
    PrintBytes(addr, 8, true);
    return;
  }
  Serial.print("  DS2408 data = ");
  Serial.println(buf[3], BIN);
}
```

## Contributing

If you want to contribute to this project:

- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell others about this library
- Contribute new protocols

Please read [CONTRIBUTING.md](https://github.com/PaulStoffregen/OneWire/blob/master/CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## Credits

The authors of this library are J. Studt, T. Pollard, R. James, G. Trewitt, J. Dangel, G. Lovato, P. Stoffregen, S. Roberts, B. Sikken, M. Tillotson, K. Butcher, R. Clark, L. Nystrom. This OneWire Library is maintained by P. Stoffregen

Based on previous work by:

- T. Tico
- I. Kravets
- Cbxbiker61
- P. Messina
- I. Grokhotkov
- F. Pillon
- M. Facchin
- A. Lentz
- R. Ondráček
- S. Denisov
- Emiley

## Current stable version

**version:**  v2.3.5

## License

Copyright (C) 2000 Dallas Semiconductor Corporation, All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

The software is provided "as is", without warranty of any kind, express or implied, including but not limited to the warranties of merchantability, fitness for a particular purpose and noninfringement. In no event shall Dallas Semiconductor be liable for any claim, damages or other liability, whether in an action of contract, tort or otherwise, arising from, out of or in connection with the software or the use or other dealings in the software.

Except as contained in this notice, the name of Dallas Semiconductor shall not be used except as stated in the Dallas Semiconductor Branding Policy.
