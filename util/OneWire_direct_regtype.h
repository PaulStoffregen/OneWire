#ifndef OneWire_Direct_RegType_h
#define OneWire_Direct_RegType_h

#include <stdint.h>

// Platform specific I/O register type

#if (defined(__AVR_ATtiny804__) || defined(__AVR_ATtiny806__) || defined(__AVR_ATtiny807__) || defined(__AVR_ATtiny1604__) \
  || defined(__AVR_ATtiny1606__) || defined(__AVR_ATtiny1607__) || defined(__AVR_ATtiny1614__) || defined(__AVR_ATtiny1616__) \
  || defined(__AVR_ATtiny1617__) || defined(__AVR_ATtiny3216__) || defined(__AVR_ATtiny3217__) )

	#define IO_REG_TYPE PORT_t
	#define IO_BITMASK_TYPE uint8_t

#elif defined(__AVR__)
#define IO_REG_TYPE uint8_t

#elif defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK66FX1M0__) || defined(__MK64FX512__)
#define IO_REG_TYPE uint8_t

#elif defined(__IMXRT1052__) || defined(__IMXRT1062__)
#define IO_REG_TYPE uint32_t

#elif defined(__MKL26Z64__)
#define IO_REG_TYPE uint8_t

#elif defined(__SAM3X8E__) || defined(__SAM3A8C__) || defined(__SAM3A4C__)
#define IO_REG_TYPE uint32_t

#elif defined(__PIC32MX__)
#define IO_REG_TYPE uint32_t

#elif defined(ARDUINO_ARCH_ESP8266)
#define IO_REG_TYPE uint32_t

#elif defined(ARDUINO_ARCH_ESP32)
#define IO_REG_TYPE uint32_t
#define IO_REG_MASK_ATTR

#elif defined(ARDUINO_ARCH_STM32)
#define IO_REG_TYPE uint32_t

#elif defined(__SAMD21G18A__)
#define IO_REG_TYPE uint32_t

#elif defined(__ASR6501__)
#define IO_REG_TYPE uint32_t

#elif defined(RBL_NRF51822)
#define IO_REG_TYPE uint32_t

#elif defined(__arc__) /* Arduino101/Genuino101 specifics */
#define IO_REG_TYPE uint32_t

#elif defined(__riscv)
#define IO_REG_TYPE uint32_t

#else
#define IO_REG_TYPE unsigned int

#endif

// Define default IO_BITMASK_TYPE as same as IO_REG_TYPE
#ifndef IO_BITMASK_TYPE
#define IO_BITMASK_TYPE IO_REG_TYPE
#endif

#endif
