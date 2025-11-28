/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#ifdef STATIC_ANALYSIS
#include "../test/common/esp_stub.h"
#endif
#include "driver/ledc.h"
#include "esp_err.h"
#include "pwm.h"
#include "../config/config.h"
#include "esp_log.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
#define PWM_GPIO        1
#define PWM_FREQ_HZ     5000
#define PWM_RESOLUTION  LEDC_TIMER_8_BIT
#define PWM_CHANNEL     LEDC_CHANNEL_0
#define PWM_TIMER       LEDC_TIMER_0

/******************************************************************
 * 3. Typedef definitions (simple typedef, then enum and structs)
******************************************************************/

/******************************************************************
 * 4. Variable definitions (static then global)
******************************************************************/

/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/

/******************************************************************
 * 6. Functions definitions
******************************************************************/

/**
 * @brief Initialize the PWM peripheral.
 *
 * This function configures the LEDC timer and channel to generate
 * a PWM signal on the pin defined by `PWM_GPIO`.
 *
 * @note The PWM frequency is defined by `PWM_FREQ_HZ`.
 * @note Uses timer `LEDC_TIMER_0` and channel `LEDC_CHANNEL_0`.
 */
void pwm_init(void) {
    /* Configure PWM timer */
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .duty_resolution  = PWM_RESOLUTION,
        .timer_num        = PWM_TIMER,
        .freq_hz          = PWM_FREQ_HZ,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    /* Configure PWM channel */
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = PWM_CHANNEL,
        .timer_sel      = PWM_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = PWM_GPIO,
        .duty           = 255, /* 100% duty cycle for 8-bit resolution */
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel);
}

/**
 * @brief PWM update callback.
 *
 * This function retrieves the current configuration and applies
 * the configured PWM duty cycle to the LEDC channel. If the
 * configuration cannot be read, an error is logged.
 */
void pwm_callback(uint8_t* payload, uint8_t size) {
    (void)payload;
    (void)size;
    esp_err_t result = ESP_OK;
    config_t config;
    static const char PWM_TAG[] = "PWM";

    /* Get latest configuration */
    result = config_get_copy(&config);
    if (result == ESP_OK) {
        /* Apply duty cycle */
        ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL, config.dutycycle);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL);
    }
    else {
        ESP_LOGE(PWM_TAG, "Failed to get configuration");
    }
}