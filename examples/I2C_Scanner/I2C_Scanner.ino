#include <Wire.h>

#define YUN

#ifdef YUN
  #include <Bridge.h>
  #include <Console.h>
  #define PRINTI_INIT Bridge.begin()
  #define PRINTLN Console.println
  #define PRINT Console.print
#else
  #define PRINTI_INIT Serial.begin(9600)
  #define PRINTLN Serial.println
  #define PRINT Serial.print
#endif

void setup()
{
  Wire.begin();

  PRINTI_INIT;
  PRINTLN("\nI2C Scanner");
}


void loop()
{
  byte error, address;
  int nDevices;

  PRINTLN("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      PRINT("I2C device found at address 0x");
      if (address<16) 
        PRINT("0");
      PRINT(address,HEX);
      PRINTLN("  !");

      nDevices++;
    }
    else if (error==4) 
    {
      PRINT("Unknow error at address 0x");
      if (address<16) 
        PRINT("0");
      PRINTLN(address,HEX);
    }    
  }
  if (nDevices == 0)
    PRINTLN("No I2C devices found\n");
  else
    PRINTLN("done\n");

  delay(5000);           // wait 5 seconds for next scan
}
