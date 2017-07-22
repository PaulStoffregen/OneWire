#ifndef OneWire_h
#define OneWire_h

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>       // for delayMicroseconds, digitalPinToBitMask, etc
#endif

#include "platform.h"

// Select the table-lookup method of computing the 8-bit CRC
// by setting this to 1.  The lookup table enlarges code size by
// about 250 bytes.  It does NOT consume RAM (but did in very
// old versions of OneWire).  If you disable this, a slower
// but very compact algorithm is used.
#define ONEWIRE_USE_CRC8_TABLE 1

// micro controller feature to use internal pull-ups
// NOTE: will throw compiler-error if feature is not yet implemented or tested for your controller
#define ONEWIRE_USE_PULL_UP  0

// to make the bus more secure you can disable parasitic bus powering
// and switch to open drain only, just keep in mind to use the
// send-routines without power=true
// NOTE: you need to power the devices yourself, because the external
// PU-resistor might not do it completely

class OneWire
{
private:

    io_reg_t pin_bitMask;
    volatile io_reg_t *pin_baseReg;

    // search state
    uint8_t search_rom_array[8];
    uint8_t search_last_discrepancy;
    uint8_t search_last_family_discrepancy;
    bool search_last_device_flag;

public:

    explicit OneWire(uint8_t pin);

    ~OneWire() = default;                             // nothing special to do here

    OneWire(const OneWire &ow) = delete;             // disallow copy constructor
    OneWire &operator=(OneWire &ow) = delete;        // disallow copy assignment
    OneWire &operator=(const OneWire &ow) = delete;  // disallow copy assignment

#if (ONEWIRE_GCC_VERSION > 40900) // is needed because of old tool chain for arduino primo
    OneWire(OneWire &&ow) = default;               // default move constructor
    OneWire &operator=(OneWire &&ow) = delete;       // disallow move assignment
#endif

    // Perform a 1-Wire reset cycle. Returns 1 if a device responds
    // with a presence pulse.  Returns 0 if there is no device or the
    // bus is shorted or otherwise held low for more than 250uS
    bool reset();

    // Issue a 1-Wire rom select command, you do the reset first.
    void select(const uint8_t rom_array[8]);

    // Issue a 1-Wire rom skip command, to address all on bus.
    void skip();

    // Write a byte. If 'power' is one then the wire is held high at
    // the end for parasitically powered devices. You are responsible
    // for eventually depowering it by calling depower() or doing
    // another read or write.
    void write(uint8_t value, bool power = false);

    void write_bytes(const uint8_t data_array[], uint16_t data_size, bool power = false);

    // Read a byte.
    uint8_t read();

    void read_bytes(uint8_t data_array[], uint16_t data_size);

    // Write a bit. The bus is always left powered at the end, see
    // note in write() about that.
    void write_bit(bool value, bool power = false);

    // Read a bit.
    bool read_bit();

    // activate parasitic bus powering by master
    // write_bytes() , write() and write_bit() have these features already build in,
    // but reset() and read() do not --> use this after issuing
    void power();

    // Stop forcing power onto the bus. You only need to do this if
    // you used the 'power' flag to write() or used a write_bit() call
    // and aren't about to do another read or write. You would rather
    // not leave this powered if you don't have to, just in case
    // someone shorts your bus.
    void depower();


    // Clear the search state so that if will start from the beginning again.
    void reset_search();

    // Setup the search to find the device type 'family_code' on the next call
    // to search(*newAddr) if it is present.
    void target_search(uint8_t family_code);

    // Look for the next device. Returns 1 if a new address has been
    // returned. A zero might mean that the bus is shorted, there are
    // no devices, or you have already retrieved all of them.  It
    // might be a good idea to check the CRC to make sure you didn't
    // get garbage.  The order is deterministic. You will always get
    // the same devices in the same order.
    bool search(uint8_t new_rom_array[8], bool search_mode = true);


    // Compute a Dallas Semiconductor 8 bit CRC, these are used in the
    // ROM and scratchpad registers.
    static uint8_t crc8(const uint8_t data_array[], uint8_t data_size, uint8_t crc_init = 0);


    // Compute the 1-Wire CRC16 and compare it against the received CRC.
    // Example usage (reading a DS2408):
    //    // Put everything in a buffer so we can compute the CRC easily.
    //    uint8_t buf[13];
    //    buf[0] = 0xF0;    // Read PIO Registers
    //    buf[1] = 0x88;    // LSB address
    //    buf[2] = 0x00;    // MSB address
    //    WriteBytes(onewire, buf, 3);    // Write 3 cmd bytes
    //    ReadBytes(onewire, buf+3, 10);  // Read 6 data bytes, 2 0xFF, 2 CRC16
    //    if (!CheckCRC16(buf, 11, &buf[11])) {
    //        // Handle error.
    //    }     
    //          
    // @param input - Array of bytes to checksum.
    // @param len - How many bytes to use.
    // @param inverted_crc - The two CRC16 bytes in the received data.
    //                       This should just point into the received data,
    //                       *not* at a 16-bit integer.
    // @param crc - The crc starting value (optional)
    // @return True, iff the CRC matches.
    static bool check_crc16(const uint8_t data_array[], uint16_t data_size, const uint8_t *inverted_crc, uint16_t crc = 0);

    // Compute a Dallas Semiconductor 16 bit CRC.  This is required to check
    // the integrity of data received from many 1-Wire devices.  Note that the
    // CRC computed here is *not* what you'll get from the 1-Wire network,
    // for two reasons:
    //   1) The CRC is transmitted bitwise inverted.
    //   2) Depending on the endian-ness of your processor, the binary
    //      representation of the two-byte return value may have a different
    //      byte order than the two bytes you get from 1-Wire.
    // @param input - Array of bytes to checksum.
    // @param len - How many bytes to use.
    // @param crc - The crc starting value (optional)
    // @return The CRC16, as defined by Dallas Semiconductor.
    static uint16_t crc16(const uint8_t data_array[], uint16_t data_size, uint16_t crc_init = 0);

};

#endif
