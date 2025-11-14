#ifndef UART_H
#define UART_H

/******************************************************************
 * 1. Included files (microcontroller ones then user defined ones)
******************************************************************/
#include "driver/uart.h"

/******************************************************************
 * 2. Define declarations (macros then function macros)
******************************************************************/
#define UART_PORT          UART_NUM_1
#define UART_TX_PIN        12
#define UART_RX_PIN        11
#define UART_BAUD_RATE     115200
#define UART_BUF_SIZE      1024

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
void uart_init(void);
void uart_write(const char *str, size_t len);

#endif // UART_H
