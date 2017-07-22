#ifndef OneWire_h
#define OneWire_h

#if defined(ARDUINO) && ARDUINO>=100
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




class OneWire {
private:

    io_reg_t pin_bitMask;
    volatile io_reg_t* pin_baseReg;

    /// search state
    uint8_t search_rom_array[8];
    uint8_t search_last_discrepancy;
    uint8_t search_last_family_discrepancy;
    bool search_last_device_flag;

public:

    /// ONEWIRE COMMANDS
    static const uint8_t   CMD_SEARCH_ROM              { 0xF0 };
    static const uint8_t   CMD_CONDITIONAL_SEARCH      { 0xEC };
    static const uint8_t   CMD_SKIP_ROM                { 0xCC };
    static const uint8_t   CMD_SKIP_ROM_OVERDRIVE      { 0x3C };
    static const uint8_t   CMD_MATCH_ROM               { 0x55 };
    static const uint8_t   CMD_MATCH_ROM_OVERDRIVE     { 0x69 };
    static const uint8_t   CMD_READ_ROM                { 0x0F };
    static const uint8_t   CMD_READ_ROM_OLD            { 0x33 };
    static const uint8_t   CMD_ALARM_SEARCH            { 0xEC };
    static const uint8_t   CMD_RESUME                  { 0xA5 };
    // TODO: should be constexpr, but NRF5x (redbear, ardu primo) have old toolchain

    /// constructor for the bus master
    ///
    /// \param pin  number of the pin as stated by controller-framework
    explicit OneWire(uint8_t pin);

    /// instantiate a default destructor
    ~OneWire() = default;                             // nothing special to do here

    OneWire(const OneWire& ow) = delete;             // disallow copy constructor
    OneWire& operator=(OneWire& ow) = delete;        // disallow copy assignment
    OneWire& operator=(const OneWire& ow) = delete;  // disallow copy assignment

#if (ONEWIRE_GCC_VERSION>40900) // is needed because of an old tool chain for nrf5x
    OneWire(OneWire&& ow) = default;               // default move constructor
    OneWire& operator=(OneWire&& ow) = delete;       // disallow move assignment
#endif

    /// Perform a 1-Wire reset cycle. Will wait up to 250uS for the bus to come high,
    /// if it doesn't then it is broken or shorted
    ///
    /// \return     true = a device responds with a presence pulse.
    ///             false = there is no device or the bus is shorted or otherwise held low for more than 250uS
    bool reset();

    /// Issue a 1-Wire rom select command. Will need a bus reset first.
    ///
    /// \param rom_array    provide pointer to ROM of slave device
    void select(const uint8_t rom_array[8]);

    /// Issue a 1-Wire rom skip command, to address all on bus. normaly this should only be used if ONE slave device
    /// is on the bus. otherwise all present device would respond
    ///
    void skip();

    /// Write a bit.
    ///
    /// \param value        bit value to write
    /// \param power        true = the wire is held high at the end for parasitically powered devices.
    ///                         You are responsible for eventually depowering it by calling depower() or doing another read or write.
    ///                     false = high impedance mode after writing
    void write_bit(bool value, bool power = false);

    /// Write a byte.
    ///
    /// \param value        byte to write
    /// \param power        true = the wire is held high at the end for parasitically powered devices.
    ///                         You are responsible for eventually depowering it by calling depower() or doing another read or write.
    ///                     false = high impedance mode after writing
    void write(uint8_t value, bool power = false);

    /// Write an array of bytes
    ///
    /// \param data_array   pointer to array
    /// \param data_size    size of array
    /// \param power        true = the wire is held high at the end for parasitically powered devices.
    ///                         You are responsible for eventually depowering it by calling depower() or doing another read or write.
    ///                     false = high impedance mode after writing
    void write(const uint8_t data_array[], uint16_t data_size, bool power = false);

    /// deprecated "got replaced by overloaded version of write(), will be deleted in future version"
    void write_bytes(const uint8_t data_array[], uint16_t data_size, bool power = false)
    {   write(data_array, data_size, power); };

    /// Read a bit.
    ///
    /// \return     bit that has been read
    bool read_bit();

    /// Read a byte.
    ///
    /// \return     byte that has been read
    uint8_t read();

    /// Read an array of bytes
    ///
    /// \param data_array   pointer to array
    /// \param data_size    size of array
    void read(uint8_t data_array[], uint16_t data_size);

    /// deprecated "got replaced by overloaded version of read(), will be deleted in future version"
    void read_bytes(uint8_t data_array[], uint16_t data_size)
    {   read(data_array, data_size); };

    /// activate parasitic bus powering by master
    /// - write_bytes() , write() and write_bit() have these features already build in,
    /// - but reset() and read() do not --> use this after issuing
    ///
    void power();

    /// Stop forcing power onto the bus. You only need to do this if
    /// you used the 'power' flag to write() or used a write_bit() call
    /// and aren't about to do another read or write. You would rather
    /// not leave this powered if you don't have to, just in case
    /// someone shorts your bus.
    ///
    void depower();

    /// Clear the search state so that it will start from the beginning again.
    /// Not needed to call prior the first search, though it does not matter.
    ///
    void reset_search();

    /// Setup the search to find the device type 'family_code' on the next call
    /// to search(*newAddr) if it is present.
    ///
    /// \param family_code  device type to search for on next search() run
    void target_search(uint8_t family_code);

    /// Look for the next device. It might be a good idea to check the CRC to make sure you didn't
    /// get garbage.  The order is deterministic. You will always get the same devices in the same order.
    ///
    /// \param new_rom_array    pointer to array of data, will be filled with found ROM
    /// \param search_mode      true = normal search mode
    ///                         false = conditional search
    /// \return                 true = a new address has been returned.
    ///                         false = might mean that the bus is shorted, there are no devices, or you have already retrieved all of them.
    bool search(uint8_t new_rom_array[8], bool search_mode = true);

    /// Compute a Dallas Semiconductor 8 bit CRC, these are used in the
    /// ROM and scratchpad registers.
    ///
    /// \param data_array   pointer to array of data to check
    /// \param data_size    size of data to check (usually one byte smaller than array size, because last byte is crc itself)
    /// \param crc_init     initial crc-value, normally left blank (set to zero)
    /// \return             calculated crc
    static uint8_t crc8(const uint8_t data_array[], uint8_t data_size, uint8_t crc_init = 0);

    /// Compute the 1-Wire CRC16 and compare it against the received CRC.
    ///
    /// \param data_array       Array of bytes to checksum.
    /// \param data_size        size of data to check (usually one byte smaller than array size, because last byte is crc itself)
    /// \param inverted_crc     The two CRC16 bytes in the received data. This should just point into the received data,
    ///                         *not* at a 16-bit integer.
    /// \param crc              initial crc-value, normally left blank (set to zero)
    /// \return                 true = crc matches / is correct
    static bool check_crc16(const uint8_t data_array[], uint16_t data_size, const uint8_t* inverted_crc, uint16_t crc = 0);

    /// Compute a Dallas Semiconductor 16 bit CRC.  This is required to check
    /// the integrity of data received from many 1-Wire devices.  Note that the
    /// CRC computed here is *not* what you'll get from the 1-Wire network,
    /// for two reasons:
    ///   1) The CRC is transmitted bitwise inverted.
    ///   2) Depending on the endian-ness of your processor, the binary
    ///      representation of the two-byte return value may have a different
    ///      byte order than the two bytes you get from 1-Wire.
    ///
    /// \param data_array       Array of bytes to checksum.
    /// \param data_size        size of data to check (usually one byte smaller than array size, because last byte is crc itself)
    /// \param crc_init         initial crc-value, normally left blank (set to zero)
    /// \return                 calculated crc
    static uint16_t crc16(const uint8_t data_array[], uint16_t data_size, uint16_t crc_init = 0);

};

#endif
