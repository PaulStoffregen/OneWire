#include <Wire.h>
#include <OneWire.h>

OneWire oneWire(0, true); // DS2482 on I2C BUS
OneWire oneWire(10);  // on pin 10 (a 4.7K resistor is necessary)

void printAddress(uint8_t deviceAddress[8])
{
  Serial.print("{ ");
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    Serial.print("0x");
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    if (i < 7) Serial.print(", ");

  }
  Serial.print(" }");
}

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  Serial.print("\nChecking for I2C devices... ");
  if (oneWire.checkPresence())
  {
    Serial.println("DS2482-100 present");
    oneWire.deviceReset();
  }
  else
    Serial.println("No DS2482-100 present");

  Serial.print("\nChecking for 1-Wire devices...  ");
  if (oneWire.reset())
  {
    Serial.println("Devices present");
  }
  else
    Serial.println("\tNo devices");

  Serial.println("\nSearching 1-Wire bus...");
  uint8_t currAddress[8];
  while (oneWire.search(currAddress))
  {
    Serial.print("\tFound device: ");
    printAddress(currAddress);
    Serial.println();
  }

  oneWire.reset_search();

  delay(5000);
}