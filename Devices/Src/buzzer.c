#include "buzzer.h"
#include <stddef.h>

Buzzer_Status_t Buzzer_Init(Buzzer_Handle_t *buzzer_h) {
    if (buzzer_h == NULL || buzzer_h->pGPIOx == NULL || buzzer_h->pin_number > 15) {
        return BUZZER_ERR_PARAM;
    }

    GPIO_Handle_t gpio_handle = {0};
    gpio_handle.pGPIOx = buzzer_h->pGPIOx;
    gpio_handle.GPIO_PinConfig.GPIO_PinNumber = buzzer_h->pin_number;
    gpio_handle.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    gpio_handle.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    gpio_handle.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    gpio_handle.GPIO_PinConfig.GPIO_PinSpeed =
        GPIO_SPEED_LOW; // Low speed is enough for on/off control

    // The port clock must be enabled before configuring the pin
    GPIO_PeripheralClockControl(gpio_handle.pGPIOx, ENABLE);
    GPIO_Init(&gpio_handle);

    // Start silent, so the buzzer does not beep right after init
    Buzzer_Off(buzzer_h);
    return BUZZER_OK;
}

Buzzer_Status_t Buzzer_On(const Buzzer_Handle_t *buzzer_h) {
    if (buzzer_h == NULL || buzzer_h->pGPIOx == NULL) {
        return BUZZER_ERR_PARAM;
    }

    // Pin level depends on the buzzer type (see active_state)
    if (buzzer_h->active_state == BUZZER_ACTIVE_LOW) {
        GPIO_WriteToOutputPin(buzzer_h->pGPIOx, buzzer_h->pin_number, GPIO_PIN_RESET);
    } else {
        GPIO_WriteToOutputPin(buzzer_h->pGPIOx, buzzer_h->pin_number, GPIO_PIN_SET);
    }
    return BUZZER_OK;
}

Buzzer_Status_t Buzzer_Off(const Buzzer_Handle_t *buzzer_h) {
    if (buzzer_h == NULL || buzzer_h->pGPIOx == NULL) {
        return BUZZER_ERR_PARAM;
    }

    // Opposite level to Buzzer_On
    if (buzzer_h->active_state == BUZZER_ACTIVE_LOW) {
        GPIO_WriteToOutputPin(buzzer_h->pGPIOx, buzzer_h->pin_number, GPIO_PIN_SET);
    } else {
        GPIO_WriteToOutputPin(buzzer_h->pGPIOx, buzzer_h->pin_number, GPIO_PIN_RESET);
    }
    return BUZZER_OK;
}

Buzzer_Status_t Buzzer_Toggle(const Buzzer_Handle_t *buzzer_h) {
    if (buzzer_h == NULL || buzzer_h->pGPIOx == NULL) {
        return BUZZER_ERR_PARAM;
    }

    // Works for both active levels: it just inverts the current pin state
    GPIO_ToggleOutputPin(buzzer_h->pGPIOx, buzzer_h->pin_number);
    return BUZZER_OK;
}