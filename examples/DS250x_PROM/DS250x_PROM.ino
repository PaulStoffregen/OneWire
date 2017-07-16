/*
DS250x add-only programmable memory reader w/SKIP ROM.
 
 The DS250x is a 512/1024bit add-only PROM(you can add data but cannot change the old one) that's used mainly for device identification purposes
 like serial number, mfgr data, unique identifiers, etc. It uses the Maxim 1-wire bus.
 
 This sketch will use the SKIP ROM function that skips the 1-Wire search phase since we only have one device connected in the bus on digital pin 6.
 If more than one device is connected to the bus, it will fail.
 Sketch will not verify if device connected is from the DS250x family since the skip rom function effectively skips the family-id byte readout.
 thus it is possible to run this sketch with any Maxim OneWire device in which case the command CRC will most likely fail.
 Sketch will only read the first page of memory(32bits) starting from the lower address(0000h), if more than 1 device is present, then use the sketch with search functions.
 Remember to put a 4.7K pullup resistor between pin 6 and +Vcc
 
 To change the range or ammount of data to read, simply change the data array size, LSB/MSB addresses and for loop iterations
 
 This example code is in the public domain and is provided AS-IS.
 
 Built with Arduino 0022 and PJRC OneWire 2.0 library http://www.pjrc.com/teensy/td_libs_OneWire.html
 
 created by Guillermo Lovato <glovato@gmail.com>
 march/2011
 
 */

#include <OneWire.h>

OneWire oneWire(10);  // on pin 10 (a 1k to 4.7K resistor is necessary)

void setup()
{
    Serial.begin(115200);
}

void loop()
{
    /// start general bus search: safer for unknown bus-devices
    // alternative 1: directly address device (onewire.select()) if you know the ROM
    // alternative 2: if only ONE device is present: issue skip-rom-cmd skip() after reset()

    uint8_t device_rom[8];

    if (!oneWire.search(device_rom))
    {
        Serial.println("No more addresses.");
        oneWire.reset_search();
        delay(250);
        return;
    }

    Serial.print("ROM =");
    for (uint8_t index = 0; index < 8; index++)
    {
        Serial.print(" ");
        Serial.print(device_rom[index], HEX);
    }

    if (OneWire::crc8(device_rom, 7) != device_rom[7])
    {
        Serial.println(" ERROR: CRC is not valid!");
        return;
    }

    // start of device dependent code

    switch (device_rom[0])
    {
        case 0x11:
        case 0x91:
            Serial.println(" Chip = DS2501");  // 64 byte
            break;
        case 0x09:
            Serial.println(" Chip = DS2502");  // 512 byte
            break;
        case 0x13:
            Serial.println(" Chip = DS2503");  // 512 byte
            break;
        case 0x28:
            Serial.println(" Chip = DS2505"); // 2048 byte
            break;
        case 0x22:
            Serial.println(" Chip = DS2506"); // 8192 byte
            break;
        default:
            Serial.println(" ERROR: not a DS18x20 family device.");
            return;
    }

    const bool device_is_present = oneWire.reset();
    if (!device_is_present) return;
    oneWire.select(device_rom);

    // array with the commands to initiate a read, DS250x devices expect 3 bytes to start a read: command,LSB&MSB addresses
    uint8_t cmd_read[3] = { 0xF0, 0x00, 0x00 };  // 0xF0 is the Read Data command, followed by 00h 00h as starting address(the beginning, 0000h)
    oneWire.write_bytes(cmd_read, 3, true);        // Read data command, leave ghost power on

    const uint8_t crc_read = oneWire.read();             // DS250x generates a CRC for the command we sent, we assign a read slot and store it's value
    const uint8_t crc_calc = OneWire::crc8(cmd_read, 3);  // We calculate the CRC of the commands we sent using the library function and store it

    if (crc_calc != crc_read)
    {      // Then we compare it to the value the ds250x calculated, if it fails, we print debug messages and abort
        Serial.println("Invalid command CRC!");
        Serial.print("Calculated CRC:");
        Serial.println(crc_calc, HEX);    // HEX makes it easier to observe and compare
        Serial.print("DS250x readback CRC:");
        Serial.println(crc_read, HEX);
        return;                      // Since CRC failed, we abort the rest of the loop and start over
    }

    Serial.println("Data is: ");   // For the printout of the data
    uint8_t device_data[32];                  // container for the data from device
    for (uint8_t index = 0; index < 32; index++)
    {    // Now it's time to read the PROM data itself, each page is 32 bytes so we need 32 read commands
        device_data[index] = oneWire.read();         // we store each read byte to a different position in the data array
        Serial.print(device_data[index], HEX);       // printout in ASCII
        Serial.print(" ");           // blank space
    }
    Serial.println();
    delay(5000);                    // Delay so we don't saturate the serial output

}

















