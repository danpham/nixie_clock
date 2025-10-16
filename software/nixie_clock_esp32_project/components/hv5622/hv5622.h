#ifndef HV5622_H
#define HV5622_H

/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include <stdint.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"


/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
#define HV5622_SPI_HOST   SPI2_HOST

// GPIOs (adapt according to your wiring)
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


/******************************************************************
 * 5. Functions prototypes (static only)
******************************************************************/


/******************************************************************
 * 6. Functions definitions (public API in .c)
******************************************************************/
void hv5622_init(void);
void hv5622_send64(uint64_t data);
void hv5622_set_output(int pin, bool value);

#endif // HV5622_H
