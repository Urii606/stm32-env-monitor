#pragma once

#include "buzzer.h"
#include "dht11.h"
#include "lcd1602.h"
#include "stm32f411xx_i2c_driver.h"

extern LCD1602_Handle_t g_lcd;
extern DHT11_Handle_t g_dht;
extern Buzzer_Handle_t g_buzzer;
extern I2C_Handle_t g_i2c1;

/**
 * @brief Initializes board hardware peripherals, clocks, and driver handles.
 */
void BSP_Init(void);