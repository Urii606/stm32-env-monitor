#include <stddef.h>

#include "dht11.h"

DHT11_Status_t DHT11_Init(const DHT11_Handle_t *dev) {
    if (dev == NULL || dev->pin_number > 15) {
        return DHT11_ERR_PARAM;
    }

    GPIO_Handle_t gpio = {.pGPIOx = dev->pGPIOx,
                          .GPIO_PinConfig = {
                              .GPIO_PinMode = GPIO_MODE_OUT,
                              .GPIO_PinOPType = GPIO_OP_TYPE_OD,
                              .GPIO_PinPuPdControl = GPIO_PIN_PU,
                              .GPIO_PinSpeed=GPIO_SPEED_FAST,
                              .GPIO_PinNumber=dev->pin_number
                          }};
    GPIO_PeripheralClockControl(gpio.pGPIOx, ENABLE);
    GPIO_Init(&gpio);
    GPIO_WriteToOutputPin(gpio.pGPIOx, dev->pin_number, GPIO_PIN_SET);
    return DHT11_OK;
}
DHT11_Status_t DHT11_Read(const DHT11_Handle_t *dev, DHT11_Data_t *data) {}

static void delay_us(uint32_t us)
{
}