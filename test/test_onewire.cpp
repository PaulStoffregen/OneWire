#include <ArduinoUnitTests.h>
#include "../OneWire.h"

#define PIN 10

int delayToBit(unsigned long* timestamps, int lowIndex) {
  int duration = timestamps[lowIndex + 1] - timestamps[lowIndex];
  if (duration <= 15) return 1;
  if (duration >= 60) return 0;
  return -1; // indicating error
}

// use GODMODE to test pins
GodmodeState* state = GODMODE();

unittest_setup()
{
  // reset pin state before each test
  state->reset();
}

unittest(no_pin_noise_on_init)
{
  int initalHistorySize = state->digitalPin[PIN].historySize();
  OneWire ow(PIN);
  assertEqual(initalHistorySize, state->digitalPin[PIN].historySize());
}

unittest(failed_search)
{
  OneWire ow(PIN);
  byte addr[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  assertFalse(ow.search(addr)); // this had better fail; we mocked nothing
  assertEqual(0, (int)OneWire::crc8(addr, 7)); // should take no data
}

unittest(write_a_0)
{
  bool actual[5];          // to store actual values written to pin
  unsigned long timing[5]; // to store event timestamps
  int numEntries;

  // write a zero and verify
  OneWire ow(PIN);
  ow.write_bit(0);
  assertEqual(3, state->digitalPin[PIN].historySize());
  numEntries = state->digitalPin[PIN].toArray(actual, 5);
  assertEqual(3, numEntries);  // initial state plus low-high
  assertEqual(LOW, actual[1]);
  assertEqual(HIGH, actual[2]);

  // assert that the timing indicates a 0
  state->digitalPin[PIN].toTimestampArray(timing, 5);
  assertEqual(0, delayToBit(timing, 1));


  // same thing but for writing a 1
  state->reset();
  ow.write_bit(1);
  assertEqual(3, state->digitalPin[PIN].historySize());
  state->digitalPin[PIN].toArray(actual, 5);
  assertEqual(LOW, actual[1]);
  assertEqual(HIGH, actual[2]);

  // assert that the timing indicates a 1
  state->digitalPin[PIN].toTimestampArray(timing, 5);
  assertEqual(1, delayToBit(timing, 1));
}

unittest(write_a_byte_no_power)
{
  OneWire ow(PIN);

  // without power
  ow.write(0xAC, 0);
  assertEqual(18, state->digitalPin[PIN].historySize());
  bool expected[18] = {
    LOW,
    LOW, HIGH,
    LOW, HIGH,
    LOW, HIGH,
    LOW, HIGH,
    LOW, HIGH,
    LOW, HIGH,
    LOW, HIGH,
    LOW, HIGH,  // 8 of these
    LOW
  };
  bool actual[18];
  unsigned long timing[18]; // to store event timestamps

  // convert history queue into an array so we can verify it.
  state->digitalPin[PIN].toArray(actual, 18);

  // verify each element
  for (int i = 0; i < 18; ++i) {
    assertEqual(expected[i], actual[i]);
  }

  // verify transmitted data, apparently it goes across little-endian
  // 0xAC -> 10101100 becomes 00110101
  state->digitalPin[PIN].toTimestampArray(timing, 18);
  assertEqual(0, delayToBit(timing, 1));
  assertEqual(0, delayToBit(timing, 3));
  assertEqual(1, delayToBit(timing, 5));
  assertEqual(1, delayToBit(timing, 7));
  assertEqual(0, delayToBit(timing, 9));
  assertEqual(1, delayToBit(timing, 11));
  assertEqual(0, delayToBit(timing, 13));
  assertEqual(1, delayToBit(timing, 15));
}

// and now a blatant copy-paste
unittest(write_a_byte_with_power)
{
  OneWire ow(PIN);

  // without power
  ow.write(0xAC, 1);
  assertEqual(17, state->digitalPin[PIN].historySize());
  bool expected[17] = {
    LOW,
    LOW, HIGH,
    LOW, HIGH,
    LOW, HIGH,
    LOW, HIGH,
    LOW, HIGH,
    LOW, HIGH,
    LOW, HIGH,
    LOW, HIGH,  // 8 of these.  power stays on -- high
  };
  bool actual[17];

  // convert history queue into an array so we can verify it.
  int copied = state->digitalPin[PIN].toArray(actual, 17);
  assertEqual(17, copied);

  // verify each element
  for (int i = 0; i < copied; ++i) {
    assertEqual(expected[i], actual[i]);
  }
}


unittest_main()
