#pragma once

#include "stm32f411xx_gpio_driver.h"

/* Prevents ISR context switches during microsecond-critical 1-Wire bus timing */
#define DISABLE_GLOBAL_INTERRUPTS()  __asm volatile ("cpsid i" : : : "memory")
/* Restores normal execution flow after critical section */
#define ENABLE_GLOBAL_INTERRUPTS()  __asm volatile ("cpsie i" : : : "memory")

/* Hardware binding context for the sensor */
typedef struct
{
    GPIO_RegDef_t *pGPIOx;
    uint8_t pin_number;
}DHT11_Handle_t;

/* DHT11 hardware only outputs integral values; decimal parts are intentionally excluded */
typedef struct 
{
    uint8_t temperature;
    uint8_t humidity;
}DHT11_Data_t;

/* Operation status codes for driver error handling */
typedef enum 
{
    DHT11_OK,
    DHT11_ERR_TIMEOUT,
    DHT11_ERR_CHECKSUM,
    DHT11_ERR_PARAM
}DHT11_Status_t;

/* Configures GPIO as Open-Drain with Pull-Up and sets line to Idle state */
DHT11_Status_t DHT11_Init(const DHT11_Handle_t *dev);

/* Reads 40-bit frame, verifies checksum, and populates data structure */
DHT11_Status_t DHT11_Read(const DHT11_Handle_t *dev,DHT11_Data_t *data);