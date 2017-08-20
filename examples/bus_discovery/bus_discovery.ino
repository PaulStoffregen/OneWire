// OneWire Bus Discovery
//
// http://www.pjrc.com/teensy/td_libs_OneWire.html
//

#include <OneWire.h>

OneWire oneWire(10);  // on pin 10 (a 1k to 4.7K resistor is necessary)

void setup()
{
    Serial.begin(115200);
}

void loop()
{
    /// start general bus search: safer for unknown bus-devices

    uint8_t device_rom[8];

    delay(50);

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

    if (device_rom[7] != oneWire.crc8(device_rom, 7))
    {
        Serial.println(" - ERROR: CRC not valid!");
        return;
    }

    /// start of device dependent code

    // the first ROM byte indicates which chip
    Serial.print(" - Chip = ");
    switch (device_rom[0])
    {
        case 0x01:
            Serial.println("DS1990, DS2401 or DS2411");
            break;
        case 0x05:
            Serial.println("DS2405");
            break;
        case 0x09:
            Serial.println("DS2502");  // 512 byte
            break;
        case 0x0B:
            Serial.println("DS2505");
            break;
        case 0x0F:
            Serial.println("DS2506");
            break;
        case 0x10:
            Serial.println("DS18S20");  // or old DS1820
            break;
        case 0x11:
            Serial.println("DS2501");  // 64 byte
            break;
        case 0x13:
            Serial.println("DS2503");  // 512 byte
            break;
        case 0x1D:
            Serial.println("DS2423");
            break;
        case 0x20:
            Serial.println("DS2450");
            break;
        case 0x22:
            Serial.println("DS1822");
            break;
        case 0x26:
            Serial.println("DS2438");
            break;
        case 0x28:
            Serial.println("DS18B20");
            break;
        case 0x29:
            Serial.println("DS2408");
            break;
        case 0x2C:
            Serial.println("DS2890");
            break;
        case 0x2D:
            Serial.println("DS2431");
            break;
        case 0x33:
            Serial.println("DS2432");
            break;
        case 0x3A:
            Serial.println("DS2413");
            break;
        case 0x81:
            Serial.println("DS1990A");
            break;
        case 0x89:
            Serial.println("DS2502");
            break;
        case 0x91:
            Serial.println("DS2501");  // 64 byte, alternative ID
            break;
        case 0xFC:
            Serial.println("BAE0910");
            break;
        default:
            Serial.println("unknown");
            return;
    }


}
