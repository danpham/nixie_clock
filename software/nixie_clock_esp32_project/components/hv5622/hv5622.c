/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#ifdef STATIC_ANALYSIS
#include "../test/common/esp_stub.h"
#endif
#include "hv5622.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
#define HV5622_SPI_HOST   SPI2_HOST
#define HV5622_PIN_MOSI   7    /* SPI_MOSI */
#define HV5622_PIN_SCLK   6    /* SPI_CLK */
#define HV5622_PIN_LE     2    /* GPIO2 */
#define HV5622_PIN_BL     1    /* GPIO1 */

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

/**
 * @brief Initialize the HV5622 shift register.
 *
 * Sets up SPI communication, configures LE and BL control pins,
 * and sets their initial states.
 */
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
        .clock_speed_hz = (uint32_t)(1U * 1000U * 1000U), /* 1 MHz */
        .mode = 3,
        .spics_io_num = -1, /* no CS, handled by LE */
        .queue_size = 1,
    };

    esp_err_t ret = spi_bus_initialize(HV5622_SPI_HOST, &buscfg, SPI_DMA_DISABLED);
    ESP_ERROR_CHECK(spi_bus_add_device(HV5622_SPI_HOST, &devcfg, &hv5622_spi));

    if (ret == ESP_OK) {
        /* Configure control GPIOs */
        gpio_reset_pin(HV5622_PIN_LE);
        gpio_set_direction(HV5622_PIN_LE, GPIO_MODE_OUTPUT);

        gpio_reset_pin(HV5622_PIN_BL);
        gpio_set_direction(HV5622_PIN_BL, GPIO_MODE_OUTPUT);
        
        /* Initial state */
        gpio_set_level(HV5622_PIN_LE, 1);
        gpio_set_level(HV5622_PIN_BL, 0);
    }
}

/**
 * @brief Send 64-bit data to the HV5622 shift register.
 *
 * The data is inverted before transmission. After sending via SPI,
 * the LE pin is toggled to update the outputs.
 *
 * @param data 64-bit value to send.
 */
void hv5622_send64(uint64_t data)
{
    uint64_t inverted = ~data; /* invert bits */

    spi_transaction_t t = {
        .length = 64, /* 64 bits */
        .tx_buffer = &inverted,
    };

    ESP_ERROR_CHECK(spi_device_transmit(hv5622_spi, &t));

    /* latch */
    gpio_set_level(HV5622_PIN_LE, 0);
    gpio_set_level(HV5622_PIN_LE, 1);
}