// OneWire DS18S20, DS18B20, DS1822 Temperature Example
//
// http://www.pjrc.com/teensy/td_libs_OneWire.html
//
// The DallasTemperature library can do all this work for you!
// http://milesburton.com/Dallas_Temperature_Control_Library

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

    delay(50);

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

    if (oneWire.crc8(device_rom, 7) != device_rom[7])
    {
        Serial.println(" - ERROR: CRC is not valid!");
        return;
    }

    /// start of device dependent code

    // the first ROM byte indicates which chip
    Serial.print(" - Chip = ");
    uint8_t device_type = 0; // zero for all but 18S20

    switch (device_rom[0])
    {
        case 0x10:
            Serial.println("DS18S20");  // or old DS1820
            device_type = 1;
            break;
        case 0x28:
            Serial.println("DS18B20");
            break;
        case 0x22:
            Serial.println("DS1822");
            break;
        default:
            Serial.println("not in DS18x20 family");
            return;
    }

    bool device_is_present = oneWire.reset();
    if (!device_is_present) return;
    oneWire.select(device_rom);
    oneWire.write(0x44, 1);        // start conversion, with parasite power on at the end

    delay(1000);     // maybe 750ms is enough, maybe not
    // we might do a oneWire.depower() here, but the reset will take care of it.

    device_is_present = oneWire.reset();
    if (!device_is_present) return;
    oneWire.select(device_rom);
    oneWire.write(0xBE);         // Read Scratchpad

    uint8_t device_data[12];
    oneWire.read(device_data,9);

    Serial.print("  Data =");
    for (uint8_t index = 0; index < 9; index++) // we need 9 bytes
    {
        Serial.print(" ");
        Serial.print(device_data[index], HEX);
    }

    if (device_data[8] != oneWire.crc8(device_data, 8))
    {
        Serial.println(" - ERROR: CRC not valid!");
        return;
    }
    Serial.println();

    // Convert the device_data to actual temperature
    // because the result is a 16 bit signed integer, it should
    // be stored to an "int16_t" type, which is always 16 bits
    // even when compiled on a 32 bit processor.
    int16_t raw = (device_data[1] << 8) | device_data[0];
    if (device_type)
    {
        raw = raw << 3; // 9 bit resolution default
        if (device_data[7] == 0x10)
        {
            // "count remain" gives full 12 bit resolution
            raw = (raw & 0xFFF0) + 12 - device_data[6];
        }
    } else
    {
        const uint8_t cfg = (device_data[4] & 0x60);
        // at lower res, the low bits are undefined, so let's zero them
        if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
        else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
        else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
        //// default is 12 bit resolution, 750 ms conversion time
    }
    const float celsius = (float) raw / 16.0f;
    const float fahrenheit = celsius * 1.8f + 32.0f;
    Serial.print("  Temperature = ");
    Serial.print(celsius);
    Serial.print(" Celsius, ");
    Serial.print(fahrenheit);
    Serial.println(" Fahrenheit");
}
