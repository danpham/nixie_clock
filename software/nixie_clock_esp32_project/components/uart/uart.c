/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "../../main/esp_stub.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/
static SemaphoreHandle_t uart_mutex = NULL;
static const char UART_TAG[] = "UART";

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/

/******************************************************************
 * 6. Functions definitions
******************************************************************/

/**
 * @brief Initialize the UART peripheral.
 *
 * Configures the UART port, installs the UART driver, creates a
 * mutex for thread-safe transmission, and sets up an RX queue.
 *
 * UART parameters:
 * - Baud rate: UART_BAUD_RATE
 * - Data bits: 8
 * - Parity: Disabled
 * - Stop bits: 1
 * - Flow control: Disabled
 *
 * @return None
 */
void uart_init(void)
{
    esp_err_t ret;
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    if (uart_mutex == NULL) {
        uart_mutex = xSemaphoreCreateMutex();
    }

    /* Configure UART */
    ret = uart_param_config(UART_PORT, &uart_config);
    if (ret != ESP_OK) {
        ESP_LOGE(UART_TAG, "uart_param_config failed: %d", ret);
    }
    
    /* Configure TX/RX pins */
    ret = uart_set_pin(UART_PORT, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (ret != ESP_OK) {
        ESP_LOGE(UART_TAG, "uart_set_pin failed: %d", ret);
    }

    /* Install driver */
    ret = uart_driver_install(UART_PORT, UART_BUF_SIZE, UART_BUF_SIZE, 0, NULL, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(UART_TAG, "uart_driver_install failed: %d", ret);
    }
}

/**
 * @brief Thread-safe UART byte buffer transmission.
 *
 * @param data Pointer to the data buffer to send.
 * @param len Number of bytes to send.
 */
void uart_write(const char *str, size_t len)
{
    if (uart_mutex != NULL) {
        static const TickType_t UART_MUTEX_TIMEOUT = 100U;
        BaseType_t taken = xSemaphoreTake(uart_mutex, pdMS_TO_TICKS(UART_MUTEX_TIMEOUT));
        if (taken == pdTRUE) {
            uart_write_bytes(UART_PORT, str, len);
            xSemaphoreGive(uart_mutex);
        } else {
            ESP_LOGW(UART_TAG, "UART mutex timeout, write skipped");
        }
    }
}