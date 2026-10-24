#pragma once

#include "stm32f411xx_gpio_driver.h"

/* Define your debug pin here (e.g., PB1) */
#define DEBUG_GPIO_PORT    GPIOB
#define DEBUG_PIN_NUMBER   1

#ifdef DEBUG
    /* In Debug mode: compile actual GPIO operations */
    #define DEBUG_PIN_INIT() do { \
        GPIO_Handle_t dbg_pin = { \
            .pGPIOx = DEBUG_GPIO_PORT, \
            .GPIO_PinConfig = { \
                .GPIO_PinMode = GPIO_MODE_OUT, \
                .GPIO_PinOPType = GPIO_OP_TYPE_PP, \
                .GPIO_PinPuPdControl = GPIO_NO_PUPD, \
                .GPIO_PinSpeed = GPIO_SPEED_FAST, \
                .GPIO_PinNumber = DEBUG_PIN_NUMBER \
            } \
        }; \
        GPIO_PeripheralClockControl(DEBUG_GPIO_PORT, ENABLE); \
        GPIO_Init(&dbg_pin); \
        GPIO_WriteToOutputPin(DEBUG_GPIO_PORT, DEBUG_PIN_NUMBER, GPIO_PIN_RESET); \
    } while(0)

    #define DEBUG_PIN_HIGH()   GPIO_WriteToOutputPin(DEBUG_GPIO_PORT, DEBUG_PIN_NUMBER, GPIO_PIN_SET)
    #define DEBUG_PIN_LOW()    GPIO_WriteToOutputPin(DEBUG_GPIO_PORT, DEBUG_PIN_NUMBER, GPIO_PIN_RESET)

#else
    /* In Release mode: compile to absolute nothing (zero overhead) */
    #define DEBUG_PIN_INIT()   ((void)0)
    #define DEBUG_PIN_HIGH()   ((void)0)
    #define DEBUG_PIN_LOW()    ((void)0)
#endif