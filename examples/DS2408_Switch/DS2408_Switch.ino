/*
 * DS2408 8-Channel Addressable Switch
 *
 * Writte by Glenn Trewitt, glenn at trewitt dot org
 *
 * Some notes about the DS2408:
 *   - Unlike most input/output ports, the DS2408 doesn't have mode bits to
 *       set whether the pins are input or output.  If you issue a read command,
 *       they're inputs.  If you write to them, they're outputs.
 *   - For reading from a switch, you should use 10K pull-up resistors.
 */

#include <OneWire.h>

OneWire oneWire(10); // on pin 10 (a 1k to 4.7K resistor is necessary)

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
    for(uint8_t index = 0; index < 8; index++)
    {
        Serial.print(" ");
        Serial.print(device_rom[index], HEX);
    }

    if (device_rom[7] != oneWire.crc8(device_rom, 7))
    {
        Serial.println(" - ERROR: CRC is not valid!");
        return;
    }

    // start of device dependent code

    Serial.print(" - Chip = ");
    switch (device_rom[0])
    {
        case 0x29:
            Serial.println("DS2408");
            break;
        default:
            Serial.println("not a DS2408");
            return;
    }

    const bool device_is_present = oneWire.reset();
    if (!device_is_present) return;
    oneWire.select(device_rom);

    uint8_t device_data[13];  // Put everything in the buffer so we can compute CRC easily.
    device_data[0] = 0xF0;    // Read PIO Registers
    device_data[1] = 0x88;    // LSB address
    device_data[2] = 0x00;    // MSB address
    oneWire.write(device_data, 3);
    oneWire.read(device_data + 3, 10);     // 3 cmd bytes, 6 data bytes, 2 0xFF, 2 CRC16
    oneWire.reset();

    Serial.print("  data =");
    for(uint8_t index = 0; index < 11; index++)
    {
        Serial.print(" ");
        Serial.print(device_data[index], HEX);
    }

    if (!oneWire.check_crc16(device_data, 11, &device_data[11]))
    {
        Serial.print(" - ERROR: CRC not valid");
    }
    Serial.println();
}
