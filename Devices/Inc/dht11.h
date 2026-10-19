#pragma once

#include "stm32f411xx_gpio_driver.h"

// Disable global interrupts
#define DISABLE_GLOBAL_INTERRUPTS()  __asm volatile ("cpsid i" : : : "memory")
// Enable global interrupts
#define ENABLE_GLOBAL_INTERRUPTS()  __asm volatile ("cpsie i" : : : "memory")

typedef struct
{
    GPIO_RegDef_t *pGPIOx;
    uint8_t pin_number;
}DHT11_Handle_t;

typedef struct 
{
    uint8_t temperature;
    uint8_t humidity;
}DHT11_Data_t;

typedef enum 
{
    DHT11_OK,
    DHT11_ERR_TIMEOUT,
    DHT11_ERR_CHECKSUM,
    DHT11_ERR_PARAM
}DHT11_Status_t;

DHT11_Status_t DHT11_Init(const DHT11_Handle_t *dev);
DHT11_Status_t DHT11_Read(const DHT11_Handle_t *dev,DHT11_Data_t *data);