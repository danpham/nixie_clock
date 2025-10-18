/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "hv5622.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"


/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/


/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/


/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/
static spi_device_handle_t hv5622_spi;

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/


/******************************************************************
 * 6. Functions definitions
******************************************************************/
void hv5622_init(void)
{
    spi_bus_config_t buscfg = {
        .mosi_io_num = HV5622_PIN_MOSI,
        .miso_io_num = -1,
        .sclk_io_num = HV5622_PIN_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 8
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = (uint32_t)(1U * 1000U * 1000U), // 1 MHz
        .mode = 3,
        .spics_io_num = -1, // no CS, handled by LE
        .queue_size = 1,
    };

    ESP_ERROR_CHECK(spi_bus_initialize(HV5622_SPI_HOST, &buscfg, SPI_DMA_DISABLED));
    ESP_ERROR_CHECK(spi_bus_add_device(HV5622_SPI_HOST, &devcfg, &hv5622_spi));

    // Configure control GPIOs
    gpio_reset_pin(HV5622_PIN_LE);
    gpio_set_direction(HV5622_PIN_LE, GPIO_MODE_OUTPUT);

    gpio_reset_pin(HV5622_PIN_BL);
    gpio_set_direction(HV5622_PIN_BL, GPIO_MODE_OUTPUT);
    
    // Initial state
    gpio_set_level(HV5622_PIN_LE, 1);
    gpio_set_level(HV5622_PIN_BL, 0);
}

void hv5622_send64(uint64_t data)
{
    uint64_t inverted = ~data; // invert bits

    spi_transaction_t t = {
        .length = 64, // 64 bits
        .tx_buffer = &inverted,
    };

    ESP_ERROR_CHECK(spi_device_transmit(hv5622_spi, &t));

    // latch
    gpio_set_level(HV5622_PIN_LE, 0);
    gpio_set_level(HV5622_PIN_LE, 1);
}

void hv5622_set_output(int pin, bool value)
{
    uint64_t hv5622_buffer = 0;
    
    if (pin < (int)0 || pin >= (int)64) return; // safety check

    if (value) {
        hv5622_buffer |= (1ULL << (uint64_t)pin);
    } else {
        hv5622_buffer &= ~(1ULL << (uint64_t)pin);
    }

    hv5622_send64(hv5622_buffer);
}
