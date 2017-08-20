#include "platform.h"

#ifdef ONEWIRE_FALLBACK_BASIC_FNs

uint32_t micros() { return 0; }; // original arduino-fn takes about 3 µs to process @ 16 MHz

void cli() { };
void sei() { };

void noInterrupts() { };

void interrupts() { };

#endif // ONEWIRE_FALLBACK_BASIC_FNs



