#include <ArduinoUnitTests.h>
#include "../OneWire.h"

unittest(crc8)
{
  byte seq1[8] = { 0,  0,  0,  0,  0,  0,  0,  0 };
  byte seq2[8] = { 0,  1,  2,  3,  4,  5,  6,  7 };
  byte seq3[8] = { 0, 10, 20, 30, 40, 50, 60, 70 };

  assertEqual(0, (int)OneWire::crc8(seq1, 7));

  assertEqual(216, (int)OneWire::crc8(seq2, 4));
  assertEqual(244, (int)OneWire::crc8(seq2, 5));
  assertEqual( 42, (int)OneWire::crc8(seq2, 6));
  assertEqual(128, (int)OneWire::crc8(seq2, 7));

  assertEqual( 63, (int)OneWire::crc8(seq3, 4));
  assertEqual( 30, (int)OneWire::crc8(seq3, 5));
  assertEqual(128, (int)OneWire::crc8(seq3, 6));
  assertEqual(145, (int)OneWire::crc8(seq3, 7));
}

unittest(crc16)
{
  byte seq1[16] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
  };

  byte seq2[16] = {
    0xF0, 0xF1, 0xF2, 0xF3,
    0xF4, 0xF5, 0xF6, 0xF7,
    0xF8, 0xF9, 0xFA, 0xFB,
    0xFC, 0xFD, 0xFE, 0xFF
  };

  assertEqual(0, (int)OneWire::crc16(seq1, 11));

  assertEqual(55581, (int)OneWire::crc16(seq2, 11));
  assertEqual(35416, (int)OneWire::crc16(seq2, 12));
  assertEqual(48011, (int)OneWire::crc16(seq2, 13));
  assertEqual(58938, (int)OneWire::crc16(seq2, 14));
}


unittest_main()
