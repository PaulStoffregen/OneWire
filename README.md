#OneWire
This is a fork of Paul Stoffregen's OneWire library. I made a simple change, adding a begin method, allowing you to dynamically assign a onewire bus pin.

Usage:

In your globals:

OneWire oneWire;

In Setup (or other applicable function)

oneWire.begin(pin); // where pin is the gpio pin your onewire bus is connected to
 
