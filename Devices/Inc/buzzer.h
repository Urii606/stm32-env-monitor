#pragma once

#include "stm32f411xx_gpio_driver.h"
#include <stdint.h>

/** Logic level that turns the buzzer on. */
typedef enum {
    BUZZER_ACTIVE_LOW,  /**< Buzzer sounds when pin is low. */
    BUZZER_ACTIVE_HIGH  /**< Buzzer sounds when pin is high. */
} Buzzer_ActiveLevel_t;

/** Buzzer configuration: GPIO port, pin and active level. */
typedef struct {
    GPIO_RegDef_t *pGPIOx;
    uint8_t pin_number;
    Buzzer_ActiveLevel_t active_state;
} Buzzer_Handle_t;

typedef enum {
    BUZZER_OK,
    BUZZER_ERR_PARAM  /**< NULL pointer or pin number out of range. */
} Buzzer_Status_t;

/** Configures the pin as output and turns the buzzer off. Call once at startup. */
Buzzer_Status_t Buzzer_Init(Buzzer_Handle_t *buzzer_h);

/** Turns the buzzer on. */
Buzzer_Status_t Buzzer_On(const Buzzer_Handle_t *buzzer_h);

/** Turns the buzzer off. */
Buzzer_Status_t Buzzer_Off(const Buzzer_Handle_t *buzzer_h);

/** Inverts the current pin state. */
Buzzer_Status_t Buzzer_Toggle(const Buzzer_Handle_t *buzzer_h);