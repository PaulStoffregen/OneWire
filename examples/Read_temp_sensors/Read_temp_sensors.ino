#include <OneWire.h>
#include <DallasTemperature.h>

// This is required for the Arduino IDE + DS2482
#include <Wire.h>

// When instantiated with no parameters, uses I2C address 18
//OneWire oneWire(0, true);
OneWire oneWire(10); // Use address D10

DallasTemperature sensors(&oneWire);

void printAddress(DeviceAddress deviceAddress)
{
  Serial.print("{ ");
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    Serial.print("0x");
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    if (i<7) Serial.print(", ");
    
  }
  Serial.print(" }");
}

void setup()
{
  Serial.begin(9600);
  Serial.println("DS18B20 search");

  sensors.begin();
  
  DeviceAddress currAddress;
  uint8_t numberOfDevices = sensors.getDeviceCount();
  
  for (int i=0; i<numberOfDevices; i++)
  {
    sensors.getAddress(currAddress, i);
    printAddress(currAddress);
    Serial.println();  
  }
}
  
void loop()
{
  Serial.println("Requesting temperatures...");
  sensors.requestTemperatures();
  
  DeviceAddress currAddress;
  uint8_t numberOfDevices = sensors.getDeviceCount();
  
  for (int i=0; i<numberOfDevices; i++)
  {
    sensors.getAddress(currAddress, i);
    printAddress(currAddress);
    Serial.print(": ");
    Serial.print(sensors.getTempCByIndex(i));
    Serial.println();  
  }
  
  delay(10000);
}
