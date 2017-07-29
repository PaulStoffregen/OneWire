// Platform specific I/O definitions

#ifndef ONEWIRE_PLATFORM_H
#define ONEWIRE_PLATFORM_H

// determine gcc version, will produce number like 40803 for gcc 4.8.3
#if defined(__GNUC__)
#define ONEWIRE_GCC_VERSION ( (__GNUC__ * 10000) + (__GNUC_MINOR__ * 100) + __GNUC_PATCHLEVEL__)
#else
#define ONEWIRE_GCC_VERSION 0
#endif

#if defined(ARDUINO) && (ARDUINO>=100)
#include <Arduino.h>
#endif

#if defined(__AVR__) /* arduino (all with atmega, atiny) */
#include <util/crc16.h>
#define PIN_TO_BASEREG(pin)             (portInputRegister(digitalPinToPort(pin)))
#define PIN_TO_BITMASK(pin)             (digitalPinToBitMask(pin))
#define DIRECT_READ(base, mask)         (((*(base)) & (mask)) ? 1 : 0)

#if ONEWIRE_USE_PULL_UP // also set pin high, after changing it to input -> activate PU
#define DIRECT_MODE_INPUT(base, mask)   ((*((base)+1)) &= ~(mask));((*((base)+2)) |= (mask))
#else
#define DIRECT_MODE_INPUT(base, mask)   ((*((base)+1)) &= ~(mask))
#endif

#define DIRECT_MODE_OUTPUT(base, mask)  ((*((base)+1)) |= (mask))
#define DIRECT_WRITE_LOW(base, mask)    ((*((base)+2)) &= ~(mask))
#define DIRECT_WRITE_HIGH(base, mask)   ((*((base)+2)) |= (mask))
#define DELAY_MICROSECONDS(us)		    delayMicroseconds(us)
using io_reg_t = uint8_t; // define special data type for register-access

#elif defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK66FX1M0__) || defined(__MK64FX512__) /* teensy 3.2 to 3.6 */

#define PIN_TO_BASEREG(pin)             (portOutputRegister(pin))
#define PIN_TO_BITMASK(pin)             (1)
#define DIRECT_READ(base, mask)         (*((base)+512))
#define DIRECT_MODE_INPUT(base, mask)   (*((base)+640) = 0)
#define DIRECT_MODE_OUTPUT(base, mask)  (*((base)+640) = 1)
#define DIRECT_WRITE_LOW(base, mask)    (*((base)+256) = 1)
#define DIRECT_WRITE_HIGH(base, mask)   (*((base)+128) = 1)
#define DELAY_MICROSECONDS(us)		    delayMicroseconds(us)
using io_reg_t = uint8_t; // define special data type for register-access

#elif defined(__MKL26Z64__) /* teensy LC */

#define PIN_TO_BASEREG(pin)             (portOutputRegister(pin))
#define PIN_TO_BITMASK(pin)             (digitalPinToBitMask(pin))
#define DIRECT_READ(base, mask)         ((*((base)+16) & (mask)) ? 1 : 0)
#define DIRECT_MODE_INPUT(base, mask)   (*((base)+20) &= ~(mask))
#define DIRECT_MODE_OUTPUT(base, mask)  (*((base)+20) |= (mask))
#define DIRECT_WRITE_LOW(base, mask)    (*((base)+8) = (mask))
#define DIRECT_WRITE_HIGH(base, mask)   (*((base)+4) = (mask))
#define DELAY_MICROSECONDS(us)		    delayMicroseconds(us)
using io_reg_t = uint8_t; // define special data type for register-access

#elif defined(__SAM3X8E__) || defined(__SAM3A8C__) || defined(__SAM3A4C__) /* arduino due */
// Arduino 1.5.1 may have a bug in delayMicroseconds() on Arduino Due.
// http://arduino.cc/forum/index.php/topic,141030.msg1076268.html#msg1076268
// If you have trouble with OneWire on Arduino Due, please check the
// status of delayMicroseconds() before reporting a bug in OneWire!
#define PIN_TO_BASEREG(pin)             (&(digitalPinToPort(pin)->PIO_PER))
#define PIN_TO_BITMASK(pin)             (digitalPinToBitMask(pin))
#define DIRECT_READ(base, mask)         (((*((base)+15)) & (mask)) ? 1 : 0)
#define DIRECT_MODE_INPUT(base, mask)   ((*((base)+5)) = (mask))
#define DIRECT_MODE_OUTPUT(base, mask)  ((*((base)+4)) = (mask))
#define DIRECT_WRITE_LOW(base, mask)    ((*((base)+13)) = (mask))
#define DIRECT_WRITE_HIGH(base, mask)   ((*((base)+12)) = (mask))
#define DELAY_MICROSECONDS(us)		    delayMicroseconds(us)
#ifndef PROGMEM
#define PROGMEM
#endif
#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const uint8_t *)(addr))
#endif
using io_reg_t = uint32_t; // define special data type for register-access

#elif defined(__PIC32MX__)

#define PIN_TO_BASEREG(pin)             (portModeRegister(digitalPinToPort(pin)))
#define PIN_TO_BITMASK(pin)             (digitalPinToBitMask(pin))
#define DIRECT_READ(base, mask)         (((*(base+4)) & (mask)) ? 1 : 0)  //PORTX + 0x10
#define DIRECT_MODE_INPUT(base, mask)   ((*(base+2)) = (mask))            //TRISXSET + 0x08
#define DIRECT_MODE_OUTPUT(base, mask)  ((*(base+1)) = (mask))            //TRISXCLR + 0x04
#define DIRECT_WRITE_LOW(base, mask)    ((*(base+8+1)) = (mask))          //LATXCLR  + 0x24
#define DIRECT_WRITE_HIGH(base, mask)   ((*(base+8+2)) = (mask))          //LATXSET + 0x28
#define DELAY_MICROSECONDS(us)		    delayMicroseconds(us)
using io_reg_t = uint32_t; // define special data type for register-access

#if ONEWIRE_USE_PULL_UP
#error "PULL UP feature is not yet implemented or tested for your microcontroller"
#endif

#elif defined(ARDUINO_ARCH_ESP8266) /* nodeMCU, ESPduino, ... */
// Special note: I depend on the ESP community to maintain these definitions and
// submit good pull requests.  I can not answer any ESP questions or help you
// resolve any problems related to ESP chips.  Please do not contact me and please
// DO NOT CREATE GITHUB ISSUES for ESP support.  All ESP questions must be asked
// on ESP community forums.
#define PIN_TO_BASEREG(pin)             ((volatile uint32_t*) GPO)
#define PIN_TO_BITMASK(pin)             (1 << pin)
#define DIRECT_READ(base, mask)         ((GPI & (mask)) ? 1 : 0)    //GPIO_IN_ADDRESS
#define DIRECT_MODE_INPUT(base, mask)   (GPE &= ~(mask))            //GPIO_ENABLE_W1TC_ADDRESS
#define DIRECT_MODE_OUTPUT(base, mask)  (GPE |= (mask))             //GPIO_ENABLE_W1TS_ADDRESS
#define DIRECT_WRITE_LOW(base, mask)    (GPOC = (mask))             //GPIO_OUT_W1TC_ADDRESS
#define DIRECT_WRITE_HIGH(base, mask)   (GPOS = (mask))             //GPIO_OUT_W1TS_ADDRESS
#define DELAY_MICROSECONDS(us)		    delayMicroseconds(us)
using io_reg_t = uint32_t; // define special data type for register-access

#elif defined(__SAMD21G18A__) /* arduino zero */

#define PIN_TO_BASEREG(pin)             portModeRegister(digitalPinToPort(pin))
#define PIN_TO_BITMASK(pin)             (digitalPinToBitMask(pin))
#define DIRECT_READ(base, mask)         (((*((base)+8)) & (mask)) ? 1 : 0)
#define DIRECT_MODE_INPUT(base, mask)   ((*((base)+1)) = (mask))
#define DIRECT_MODE_OUTPUT(base, mask)  ((*((base)+2)) = (mask))
#define DIRECT_WRITE_LOW(base, mask)    ((*((base)+5)) = (mask))
#define DIRECT_WRITE_HIGH(base, mask)   ((*((base)+6)) = (mask))
#define DELAY_MICROSECONDS(us)		    delayMicroseconds(us)
using io_reg_t = uint32_t; // define special data type for register-access

#elif defined(NRF52) /* arduino primo */

#define PIN_TO_BASEREG(pin)             (0)
#define PIN_TO_BITMASK(pin)             (pin)
#define DIRECT_READ(base, pin)          nrf_gpio_pin_read(pin)
#define DIRECT_WRITE_LOW(base, pin)     nrf_gpio_pin_clear(pin)
#define DIRECT_WRITE_HIGH(base, pin)    nrf_gpio_pin_set(pin)
#define DIRECT_MODE_INPUT(base, pin)    nrf_gpio_cfg_input(pin, NRF_GPIO_PIN_NOPULL)
#define DIRECT_MODE_OUTPUT(base, pin)   nrf_gpio_cfg_output(pin)
#define DELAY_MICROSECONDS(us)		    delayMicroseconds(us)
#define io_reg_t uint32_t     /* TODO: the tool chain is old .... check for updates, last 2017-07 */
// using io_reg_t = uint32_t; // define special data type for register-access

#elif defined(NRF51) || defined(__RFduino__) /* red bear blend, should be good for all nrf51x chips */

#if defined(TARGET_NRF51822)
#include <nRF51822_API.h>
#endif

#define PIN_TO_BASEREG(pin)             (0)
#define PIN_TO_BITMASK(pin)             (pin)
#define DIRECT_READ(base, pin)          nrf_gpio_pin_read(pin)
#define DIRECT_WRITE_LOW(base, pin)     nrf_gpio_pin_clear(pin)
#define DIRECT_WRITE_HIGH(base, pin)    nrf_gpio_pin_set(pin)
#define DIRECT_MODE_INPUT(base, pin)    nrf_gpio_cfg_input(pin, NRF_GPIO_PIN_NOPULL)
#define DIRECT_MODE_OUTPUT(base, pin)   nrf_gpio_cfg_output(pin)
#define DELAY_MICROSECONDS(us)		    nrf_delay_us(us) // TODO: only needed because of faulty redbear-delay()-implementation
#define io_reg_t uint32_t     /* TODO: the tool chain is old .... check for updates, last 2017-07 */
// using io_reg_t = uint32_t; // define special data type for register-access

#elif defined(__arc__) || defined(__ARDUINO_ARC__) /* Arduino101/Genuino101 specifics */

#include "scss_registers.h"
#include "portable.h"
#include "avr/pgmspace.h"

#define GPIO_ID(pin)			(g_APinDescription[pin].ulGPIOId)
#define GPIO_TYPE(pin)			(g_APinDescription[pin].ulGPIOType)
#define GPIO_BASE(pin)			(g_APinDescription[pin].ulGPIOBase)
#define DIR_OFFSET_SS			0x01
#define DIR_OFFSET_SOC			0x04
#define EXT_PORT_OFFSET_SS		0x0A
#define EXT_PORT_OFFSET_SOC		0x50

/* GPIO registers base address */
#define PIN_TO_BASEREG(pin)		((volatile uint32_t *)g_APinDescription[pin].ulGPIOBase)
#define PIN_TO_BITMASK(pin)		pin
#define io_reg_t uint32_t     /* TODO: the tool chain is old .... check for updates, last 2017-07 */
//using io_reg_t = uint32_t; // define special data type for register-access

#if ONEWIRE_USE_PULL_UP
#error "PULL UP feature is not yet implemented or tested for your microcontroller"
#endif

static inline __attribute__((always_inline))
IO_REG_TYPE directRead(volatile IO_REG_TYPE *base, IO_REG_TYPE pin)
{
    IO_REG_TYPE ret;
    if (SS_GPIO == GPIO_TYPE(pin)) {
        ret = READ_ARC_REG(((IO_REG_TYPE)base + EXT_PORT_OFFSET_SS));
    } else {
        ret = MMIO_REG_VAL_FROM_BASE((IO_REG_TYPE)base, EXT_PORT_OFFSET_SOC);
    }
    return ((ret >> GPIO_ID(pin)) & 0x01);
}

static inline __attribute__((always_inline))
void directModeInput(volatile IO_REG_TYPE *base, IO_REG_TYPE pin)
{
    if (SS_GPIO == GPIO_TYPE(pin)) {
        WRITE_ARC_REG(READ_ARC_REG((((IO_REG_TYPE)base) + DIR_OFFSET_SS)) & ~(0x01 << GPIO_ID(pin)),
            ((IO_REG_TYPE)(base) + DIR_OFFSET_SS));
    } else {
        MMIO_REG_VAL_FROM_BASE((IO_REG_TYPE)base, DIR_OFFSET_SOC) &= ~(0x01 << GPIO_ID(pin));
    }
}

static inline __attribute__((always_inline))
void directModeOutput(volatile IO_REG_TYPE *base, IO_REG_TYPE pin)
{
    if (SS_GPIO == GPIO_TYPE(pin)) {
        WRITE_ARC_REG(READ_ARC_REG(((IO_REG_TYPE)(base) + DIR_OFFSET_SS)) | (0x01 << GPIO_ID(pin)),
            ((IO_REG_TYPE)(base) + DIR_OFFSET_SS));
    } else {
        MMIO_REG_VAL_FROM_BASE((IO_REG_TYPE)base, DIR_OFFSET_SOC) |= (0x01 << GPIO_ID(pin));
    }
}

static inline __attribute__((always_inline))
void directWriteLow(volatile IO_REG_TYPE *base, IO_REG_TYPE pin)
{
    if (SS_GPIO == GPIO_TYPE(pin)) {
        WRITE_ARC_REG(READ_ARC_REG(base) & ~(0x01 << GPIO_ID(pin)), base);
    } else {
        MMIO_REG_VAL(base) &= ~(0x01 << GPIO_ID(pin));
    }
}

static inline __attribute__((always_inline))
void directWriteHigh(volatile IO_REG_TYPE *base, IO_REG_TYPE pin)
{
    if (SS_GPIO == GPIO_TYPE(pin)) {
        WRITE_ARC_REG(READ_ARC_REG(base) | (0x01 << GPIO_ID(pin)), base);
    } else {
        MMIO_REG_VAL(base) |= (0x01 << GPIO_ID(pin));
    }
}

#define DIRECT_READ(base, pin)		    directRead(base, pin)
#define DIRECT_MODE_INPUT(base, pin)	directModeInput(base, pin)
#define DIRECT_MODE_OUTPUT(base, pin)	directModeOutput(base, pin)
#define DIRECT_WRITE_LOW(base, pin)	    directWriteLow(base, pin)
#define DIRECT_WRITE_HIGH(base, pin)	directWriteHigh(base, pin)
#define DELAY_MICROSECONDS(us)		    delayMicroseconds(us)

#else // any unknown architecture, including PC

#include <inttypes.h>

#define PIN_TO_BASEREG(pin)             (0)
#define PIN_TO_BITMASK(pin)             (pin)
#define DIRECT_READ(base, pin)          digitalRead(pin)
#define DIRECT_WRITE_LOW(base, pin)     digitalWrite(pin, LOW)
#define DIRECT_WRITE_HIGH(base, pin)    digitalWrite(pin, HIGH)
#define DIRECT_MODE_INPUT(base, pin)    pinMode(pin,INPUT)
#define DIRECT_MODE_OUTPUT(base, pin)   pinMode(pin,OUTPUT)
#define DELAY_MICROSECONDS(us)		    delayMicroseconds(us)
using io_reg_t = uint32_t; // define special data type for register-access

#warning "OneWire. Fallback mode. Using API calls for pinMode,digitalRead and digitalWrite. Operation of this library is not guaranteed on this architecture."

#endif

#ifndef ARDUINO
#define ONEWIRE_FALLBACK_BASIC_FNs
#define ONEWIRE_FALLBACK_ADDITIONAL_FNs // to load up Serial below
#endif

#ifdef ARDUINO_attiny
#define ONEWIRE_FALLBACK_ADDITIONAL_FNs // to load up Serial below
#endif

#ifdef ONEWIRE_FALLBACK_BASIC_FNs

/////////////////////////////////////////// EXTRA PART /////////////////////////////////////////
// this part is loaded if no proper arduino-environment is found (good for external testing)
// these used functions are mockups
////////////////////////////////////////////////////////////////////////////////////////////////

#define INPUT 1
#define INPUT_PULLUP 1
#define OUTPUT 0
#define HIGH 1
#define LOW 0

static bool mockup_pin_value[256];

template<typename T1>
bool digitalRead(const T1 pin) { return (mockup_pin_value[pin & 0xFF]!=0); }; // mock up outputs

template<typename T1, typename T2>
void digitalWrite(const T1 pin, const T2 value) { mockup_pin_value[pin & 0xFF] = value; };

template<typename T1, typename T2>
void pinMode(const T1 pin, const T2 value) { mockup_pin_value[pin & 0xFF] = value; };

template<typename T1>
T1 digitalPinToPort(const T1 pin) { return pin; };

template<typename T1>
T1* portInputRegister(const T1 port) { return port; };

template<typename T1>
T1 digitalPinToBitMask(const T1 pin) { return pin; };

constexpr uint32_t microsecondsToClockCycles(const uint32_t micros) { return (100*micros); }; // mockup, emulate 100 MHz CPU

template<typename T1>
void delayMicroseconds(const T1 micros) { };

/// the following fn are no templates and need to be defined in platform.cpp

uint32_t micros(); // takes about 3 µs to process @ 16 MHz

void cli();

void sei();

void noInterrupts();

void interrupts();

template<typename T1>
T1 pgm_read_byte(const T1* address)
{
    return *address;
}

#endif

#ifdef ONEWIRE_FALLBACK_ADDITIONAL_FNs // Test to make it work on aTtiny85, 8MHz
/// README: use pin2 or pin3 for Attiny, source: https://github.com/gioblu/PJON/wiki/ATtiny-interfacing

#ifndef BIN
#define BIN 1
#endif

#ifndef HEX
#define HEX 2
#endif

static class serial {
private:

    uint32_t speed;

public:

    void print(...) { };

    void println(...) { };

    void flush() { };

    void begin(const uint32_t speed_baud) { speed = speed_baud; };

} Serial;

template<typename T1, typename T2>
void memset(T1* const address, const T1 initValue, const T2 bytes)
{
    const T2 iterations = bytes/sizeof(T1);
    for (T2 counter = 0; counter<iterations; ++counter)
    {
        address[counter] = (initValue);
    }
}

template<typename T1, typename T2>
void memcpy(T1* const destination, const T1* const source, const T2 bytes)
{
    const T2 iterations = bytes/sizeof(T1);
    for (T2 counter = 0; counter<iterations; ++counter)
    {
        destination[counter] = source[counter];
    }
}

template<typename T1, typename T2>
bool memcmp(const T1* const source_A, const T1* const source_B, const T2 bytes) // return true if string is different
{
    const T2 iterations = bytes/sizeof(T1);
    for (T2 counter = 0; counter<iterations; ++counter)
    {
        if (source_A[counter]!=source_B[counter]) return true;
    };
    return false;
}

void delay(uint32_t time_millis);

uint32_t millis();

void wdt_reset();

void wdt_enable(...);

#ifndef PROGMEM
#define PROGMEM
#endif

#endif

#endif //ONEWIRE_PLATFORM_H
