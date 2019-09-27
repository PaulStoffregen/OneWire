/*
Created by Chris Morgan based on the nodemcu project driver.
Copyright 2017 Chris Morgan <chmorgan@gmail.com>

Ported to ESP32 RMT peripheral for low-level signal generation by Arnim Laeuger.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Much of the code was inspired by Derek Yerger's code, though I don't
think much of that remains.  In any event that was..
    (copyleft) 2006 by Derek Yerger - Free to distribute freely.

The CRC code was excerpted and inspired by the Dallas Semiconductor
sample code bearing this copyright.
//---------------------------------------------------------------------------
// Copyright (C) 2000 Dallas Semiconductor Corporation, All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL DALLAS SEMICONDUCTOR BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name of Dallas Semiconductor
// shall not be used except as stated in the Dallas Semiconductor
// Branding Policy.
//--------------------------------------------------------------------------
*/


#include "owb_rmt.h"

#include "driver/rmt.h"
#include "driver/gpio.h"
#include "esp_log.h"

#undef OW_DEBUG


// bus reset: duration of low phase [us]
#define OW_DURATION_RESET 480
// overall slot duration
#define OW_DURATION_SLOT 75
// write 1 slot and read slot durations [us]
#define OW_DURATION_1_LOW    2
#define OW_DURATION_1_HIGH (OW_DURATION_SLOT - OW_DURATION_1_LOW)
// write 0 slot durations [us]
#define OW_DURATION_0_LOW   65
#define OW_DURATION_0_HIGH (OW_DURATION_SLOT - OW_DURATION_0_LOW)
// sample time for read slot
#define OW_DURATION_SAMPLE  (15-2)
// RX idle threshold
// needs to be larger than any duration occurring during write slots
#define OW_DURATION_RX_IDLE (OW_DURATION_SLOT + 2)


static const char * TAG = "owb_rmt";


// flush any pending/spurious traces from the RX channel
static void onewire_flush_rmt_rx_buf(const OneWireBus * bus)
{
    void *p;
    size_t s;

    while ((p = xRingbufferReceive(bus->rb, &s, 0)))
    {
        ESP_LOGD(TAG, "flushing entry");
        vRingbufferReturnItem(bus->rb, p);
    }
}

owb_status owb_rmt_reset(const OneWireBus *bus, bool *is_present)
{
    rmt_item32_t tx_items[1];
    bool _is_present = false;
    owb_status res = OWB_STATUS_OK;

    tx_items[0].duration0 = OW_DURATION_RESET;
    tx_items[0].level0 = 0;
    tx_items[0].duration1 = 0;
    tx_items[0].level1 = 1;

    uint16_t old_rx_thresh;
    rmt_get_rx_idle_thresh(bus->rx_channel, &old_rx_thresh);
    rmt_set_rx_idle_thresh(bus->rx_channel, OW_DURATION_RESET+60);

    onewire_flush_rmt_rx_buf(bus);
    rmt_rx_start(bus->rx_channel, true);
    if (rmt_write_items(bus->tx_channel, tx_items, 1, true) == ESP_OK)
    {
        size_t rx_size;
        rmt_item32_t* rx_items = (rmt_item32_t *)xRingbufferReceive(bus->rb, &rx_size, 100 / portTICK_PERIOD_MS);

        if (rx_items)
        {
            if (rx_size >= (1 * sizeof(rmt_item32_t)))
            {
#ifdef OW_DEBUG
                ESP_LOGI(TAG, "rx_size: %d", rx_size);

                for (int i = 0; i < (rx_size / sizeof(rmt_item32_t)); i++)
                {
                    ESP_LOGI(TAG, "i: %d, level0: %d, duration %d", i, rx_items[i].level0, rx_items[i].duration0);
                    ESP_LOGI(TAG, "i: %d, level1: %d, duration %d", i, rx_items[i].level1, rx_items[i].duration1);
                }
#endif

                // parse signal and search for presence pulse
                if ((rx_items[0].level0 == 0) && (rx_items[0].duration0 >= OW_DURATION_RESET - 2))
                {
                    if ((rx_items[0].level1 == 1) && (rx_items[0].duration1 > 0))
                    {
                        if (rx_items[1].level0 == 0)
                        {
                            _is_present = true;
                        }
                    }
                }
            }

            vRingbufferReturnItem(bus->rb, (void *)rx_items);
        }
        else
        {
            // time out occurred, this indicates an unconnected / misconfigured bus
            ESP_LOGE(TAG, "rx_items == 0");
            res = OWB_STATUS_HW_ERROR;
        }
    }
    else
    {
        // error in tx channel
        ESP_LOGE(TAG, "Error tx");
        res = OWB_STATUS_HW_ERROR;
    }

    rmt_rx_stop(bus->rx_channel);
    rmt_set_rx_idle_thresh(bus->rx_channel, old_rx_thresh);

    *is_present = _is_present;

    ESP_LOGD(TAG, "_is_present %d", _is_present);

    return res;
}

static rmt_item32_t _encode_write_slot(uint8_t val)
{
    rmt_item32_t item;

    item.level0 = 0;
    item.level1 = 1;
    if (val)
    {
        // write "1" slot
        item.duration0 = OW_DURATION_1_LOW;
        item.duration1 = OW_DURATION_1_HIGH;
    }
    else
    {
        // write "0" slot
        item.duration0 = OW_DURATION_0_LOW;
        item.duration1 = OW_DURATION_0_HIGH;
    }

    return item;
}

/** NOTE: The data is shifted out of the low bits, eg. it is written in the order of lsb to msb */
owb_status owb_rmt_write_bits(const OneWireBus * bus, uint8_t out, int number_of_bits_to_write)
{
    rmt_item32_t tx_items[number_of_bits_to_write + 1];

    if (number_of_bits_to_write > 8)
    {
        return OWB_STATUS_TOO_MANY_BITS;
    }

    // write requested bits as pattern to TX buffer
    for (int i = 0; i < number_of_bits_to_write; i++)
    {
        tx_items[i] = _encode_write_slot(out & 0x01);
        out >>= 1;
    }

    // end marker
    tx_items[number_of_bits_to_write].level0 = 1;
    tx_items[number_of_bits_to_write].duration0 = 0;

    owb_status status;

    if (rmt_write_items(bus->tx_channel, tx_items, number_of_bits_to_write+1, true) == ESP_OK)
    {
        status = OWB_STATUS_OK;
    }
    else
    {
        status = OWB_STATUS_HW_ERROR;
        ESP_LOGE(TAG, "rmt_write_items() failed");
    }

    return status;
}

static rmt_item32_t _encode_read_slot(void)
{
    rmt_item32_t item;

    // construct pattern for a single read time slot
    item.level0    = 0;
    item.duration0 = OW_DURATION_1_LOW;   // shortly force 0
    item.level1    = 1;
    item.duration1 = OW_DURATION_1_HIGH;  // release high and finish slot
    return item;
}

/** NOTE: Data is read into the high bits, eg. each bit read is shifted down before the next bit is read */
owb_status owb_rmt_read_bits(const OneWireBus * bus, uint8_t *in, int number_of_bits_to_read)
{
    rmt_item32_t tx_items[number_of_bits_to_read + 1];
    uint8_t read_data = 0;
    owb_status res = OWB_STATUS_OK;

    if (number_of_bits_to_read > 8)
    {
        ESP_LOGE(TAG, "_read_bits() OWB_STATUS_TOO_MANY_BITS");
        return OWB_STATUS_TOO_MANY_BITS;
    }

    // generate requested read slots
    for (int i = 0; i < number_of_bits_to_read; i++)
    {
        tx_items[i] = _encode_read_slot();
    }

    // end marker
    tx_items[number_of_bits_to_read].level0 = 1;
    tx_items[number_of_bits_to_read].duration0 = 0;

    onewire_flush_rmt_rx_buf(bus);
    rmt_rx_start(bus->rx_channel, true);
    if (rmt_write_items(bus->tx_channel, tx_items, number_of_bits_to_read+1, true) == ESP_OK)
    {
        size_t rx_size;
        rmt_item32_t* rx_items = (rmt_item32_t *)xRingbufferReceive(bus->rb, &rx_size, portMAX_DELAY);

        if (rx_items)
        {
#ifdef OW_DEBUG
            for (int i = 0; i < rx_size / 4; i++)
            {
                ESP_LOGI(TAG, "level: %d, duration %d", rx_items[i].level0, rx_items[i].duration0);
                ESP_LOGI(TAG, "level: %d, duration %d", rx_items[i].level1, rx_items[i].duration1);
            }
#endif

            if (rx_size >= number_of_bits_to_read * sizeof(rmt_item32_t))
            {
                for (int i = 0; i < number_of_bits_to_read; i++)
                {
                    read_data >>= 1;
                    // parse signal and identify logical bit
                    if (rx_items[i].level1 == 1)
                    {
                        if ((rx_items[i].level0 == 0) && (rx_items[i].duration0 < OW_DURATION_SAMPLE))
                        {
                            // rising edge occured before 15us -> bit 1
                            read_data |= 0x80;
                        }
                    }
                }
                read_data >>= 8 - number_of_bits_to_read;
            }

            vRingbufferReturnItem(bus->rb, (void *)rx_items);
        }
        else
        {
            // time out occurred, this indicates an unconnected / misconfigured bus
            ESP_LOGE(TAG, "rx_items == 0");
            res = OWB_STATUS_HW_ERROR;
        }
    }
    else
    {
        // error in tx channel
        ESP_LOGE(TAG, "Error tx");
        res = OWB_STATUS_HW_ERROR;
    }

    rmt_rx_stop(bus->rx_channel);

    *in = read_data;
    return res;
}

static owb_status _uninitialize(const OneWireBus *bus)
{
    rmt_driver_uninstall(bus->tx_channel);
    rmt_driver_uninstall(bus->rx_channel);

    return OWB_STATUS_OK;
}

owb_status owb_rmt_init(OneWireBus *bus, uint8_t gpio_num,
                        rmt_channel_t tx_channel, rmt_channel_t rx_channel)
{
    owb_status status = OWB_STATUS_HW_ERROR;

    // Ensure the RMT peripheral is not already running
    // Note: if using RMT elsewhere, don't call this here, call it at the start of your prgoram instead.
    periph_module_disable(PERIPH_RMT_MODULE);
    periph_module_enable(PERIPH_RMT_MODULE);

    bus->tx_channel = tx_channel;
    bus->rx_channel = rx_channel;
    bus->gpio = gpio_num;

#ifdef OW_DEBUG
    ESP_LOGI(TAG, "RMT TX channel: %d", bus->tx_channel);
    ESP_LOGI(TAG, "RMT RX channel: %d", bus->rx_channel);
#endif

    rmt_config_t rmt_tx;
    rmt_tx.channel = bus->tx_channel;
    rmt_tx.gpio_num = (gpio_num_t)gpio_num;
    rmt_tx.mem_block_num = 1;
    rmt_tx.clk_div = 80;
    rmt_tx.tx_config.loop_en = false;
    rmt_tx.tx_config.carrier_en = false;
    rmt_tx.tx_config.idle_level = RMT_IDLE_LEVEL_HIGH;
    rmt_tx.tx_config.idle_output_en = true;
    rmt_tx.rmt_mode = RMT_MODE_TX;
    if (rmt_config(&rmt_tx) == ESP_OK)
    {
        if (rmt_driver_install(rmt_tx.channel, 0, ESP_INTR_FLAG_LOWMED | ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_SHARED) == ESP_OK)
        {
            rmt_config_t rmt_rx;
            rmt_rx.channel = bus->rx_channel;
            rmt_rx.gpio_num = (gpio_num_t)gpio_num;
            rmt_rx.clk_div = 80;
            rmt_rx.mem_block_num = 1;
            rmt_rx.rmt_mode = RMT_MODE_RX;
            rmt_rx.rx_config.filter_en = true;
            rmt_rx.rx_config.filter_ticks_thresh = 30;
            rmt_rx.rx_config.idle_threshold = OW_DURATION_RX_IDLE;
            if (rmt_config(&rmt_rx) == ESP_OK)
            {
                if (rmt_driver_install(rmt_rx.channel, 512, ESP_INTR_FLAG_LOWMED | ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_SHARED) == ESP_OK)
                {
                    rmt_get_ringbuf_handle(bus->rx_channel, &bus->rb);
                    status = OWB_STATUS_OK;
                }
                else
                {
                    ESP_LOGE(TAG, "failed to install rx driver");
                }
            }
            else
            {
                status = OWB_STATUS_HW_ERROR;
                ESP_LOGE(TAG, "failed to configure rx, uninstalling rmt driver on tx channel");
                rmt_driver_uninstall(rmt_tx.channel);
            }
        }
        else
        {
            ESP_LOGE(TAG, "failed to install tx driver");
        }
    }
    else
    {
        ESP_LOGE(TAG, "failed to configure tx");
    }

    // attach GPIO to previous pin
    if (gpio_num < 32)
    {
        GPIO.enable_w1ts = (0x1 << gpio_num);
    }
    else
    {
        GPIO.enable1_w1ts.data = (0x1 << (gpio_num - 32));
    }

    // attach RMT channels to new gpio pin
    // ATTENTION: set pin for rx first since gpio_output_disable() will
    //            remove rmt output signal in matrix!
    rmt_set_pin(bus->rx_channel, RMT_MODE_RX, (gpio_num_t)gpio_num);
    rmt_set_pin(bus->tx_channel, RMT_MODE_TX, (gpio_num_t)gpio_num);

    // force pin direction to input to enable path to RX channel
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[gpio_num]);

    // enable open drain
    GPIO.pin[gpio_num].pad_driver = 1;

    return status;
}

