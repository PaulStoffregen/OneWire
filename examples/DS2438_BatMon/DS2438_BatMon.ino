// OneWire DS2438 Example
//
// You need : https://github.com/PaulStoffregen/OneWire

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
        Serial.println(" - ERROR: CRC not valid!");
        return;
    }

    // start of device dependent code

    Serial.print(" - Chip = ");
    switch (device_rom[0])
    {
        case 0x26:
            Serial.println("DS2438");
            break;
        default:
            Serial.println("not a DS2438");
            return;
    }

    bool device_is_present = oneWire.reset();
    if (!device_is_present) return;
    oneWire.select(device_rom);
    oneWire.write(0x44, 1);        // start T_conversion, with parasite power on at the end
    //ds.write(0xB4, 1);        // start V_conversion, with parasite power on at the end

    delay(1000);     // maybe 750ms is enough, maybe not

    device_is_present = oneWire.reset();
    if (!device_is_present) return;
    oneWire.select(device_rom);
    oneWire.write(0xBE);         // Read Scratchpad
    oneWire.write(0x00);         // request Page 0, measurements
    uint8_t device_data[9];
    oneWire.read(device_data,9);

    Serial.print("  data =");
    for(uint8_t index = 0; index < 9; index++)
    {
        Serial.print(" ");
        Serial.print(device_data[index], HEX);
    }

    if (device_data[8] != oneWire.crc8(device_data, 8))
    {
        Serial.println(" - ERROR: CRC not valid!");
        return;
    }

    // Convert the data to actual values
    int16_t raw = (device_data[2] << 8) | device_data[1];
    const float temperature = raw / 256.0f; // degC
    raw = ((device_data[4] << 8) & uint16_t(0x03)) | device_data[3];
    const float voltage = raw / 100.0f;   // V
    raw = (device_data[6] << 8) | (device_data[5]);
    const float current = raw / 32.0f;   // A

    // display data
    Serial.print("  Temp = ");
    Serial.print(temperature);
    Serial.print(" degC, Voltage = ");
    Serial.print(voltage);
    Serial.print(" V, Current = ");
    Serial.print(current);
    Serial.println(" A");
}
