#pragma once
#include <stdint.h>


/** Result codes returned by the console API. */
typedef enum {
    CONSOLE_OK = 0,/**< Operation completed successfully */
    CONSOLE_ERR /**< Invalid argument or hardware setup failed */
} Console_Status_t;


/**
 * @brief 
 * Initializes the console: configures the GPIO pins and USART,
 *and starts interrupt-driven reception.
 * @return Console_Status_t:
  *                         -Console_OK:    All bytes were successfully transmitted
                            -Console_ERR:    The USART failed to start

 */
Console_Status_t Console_Init(void);

/**
 * @brief Transmit a raw byte buffer synchronously(blocking mode)
 * 
 * @param data Pointer to the source byte buffer to transmit. Must not be NULL.
 * @param len Number of bytes to transmit. Must be greater than 0.
 * @return Console_Status_t :
 *                          -Console_OK:    All bytes were successfully transmitted
                            -Console_ERR:   Invalid parameter (data is NULL or size is 0)
 */
Console_Status_t Console_Write(const char *data,uint32_t len);