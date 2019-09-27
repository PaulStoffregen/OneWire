/*
 * MIT License
 *
 * Copyright (c) 2017 David Antliff
 * Copyright (c) 2017 Chris Morgan <chmorgan@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once
#ifndef OWB_RMT_H
#define OWB_RMT_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/ringbuf.h"
#include "driver/rmt.h"

typedef struct
{
  rmt_channel_t tx_channel;     ///< RMT channel to use for TX
  rmt_channel_t rx_channel;     ///< RMT channel to use for RX
  RingbufHandle_t rb;           ///< Ring buffer handle
  int gpio;                     ///< OneWireBus GPIO
  bool initialized;             ///< Flag indicating if the driver is init
} OneWireBus;

typedef enum
{
    OWB_STATUS_OK,
    OWB_STATUS_NOT_INITIALIZED,
    OWB_STATUS_PARAMETER_NULL,
    OWB_STATUS_DEVICE_NOT_RESPONDING,
    OWB_STATUS_CRC_FAILED,
    OWB_STATUS_TOO_MANY_BITS,
    OWB_STATUS_HW_ERROR
} owb_status;

/**
 * @brief Initialise the RMT driver.
 * @return OneWireBus*, pass this into the other OneWireBus public API functions
 */
owb_status owb_rmt_init(OneWireBus *bus, uint8_t gpio_num, rmt_channel_t tx_channel, rmt_channel_t rx_channel);

owb_status owb_rmt_reset(const OneWireBus *bus, bool *is_present);

owb_status owb_rmt_write_bits(const OneWireBus *bus, uint8_t out, int number_of_bits_to_write);

owb_status owb_rmt_read_bits(const OneWireBus *bus, uint8_t *in, int number_of_bits_to_read);

#endif // OWB_RMT_H
