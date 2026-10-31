#include "bsp.h"
#include "buzzer.h"
#include "console.h"
#include "dht11.h"
#include "lcd1602.h"
#include "logger.h"
#include "stm32f411xx_dwt_delay.h"
#include <stdint.h>

void SystemInit(void) {}

void uint8_to_str(uint8_t val, char *str);

int main(void) {
    if (Console_Init() != CONSOLE_OK) {
        while (1) {
        }
    }
    BSP_Init();

    DHT11_Data_t data = {0};
    char str_buf[4];
    uint32_t last_dht_read_time = 0;

    Logger_Log(LOG_LEVEL_INFO, "System started successfully");
    while (1) {
        uint32_t current_time = GetTimeUs();

        if ((current_time - last_dht_read_time) >= 2000000UL) {
            if (DHT11_Read(&g_dht, &data) != DHT11_OK) {
                Logger_Log(LOG_LEVEL_ERROR, "Error in dht11_read");
            }
            Logger_Log(LOG_LEVEL_INFO, "Humidity:");
            uint8_to_str(data.humidity, str_buf);
            Logger_Log(LOG_LEVEL_INFO, str_buf);
            if (LCD1602_SetCursor(&g_lcd, 0, 0) != LCD1602_OK ||
                LCD1602_PrintString(&g_lcd, str_buf) != LCD1602_OK) {
                Logger_Log(LOG_LEVEL_ERROR, "Error in LCD1602_PrintString");
            }
            Logger_Log(LOG_LEVEL_INFO, "Temperature:");
            uint8_to_str(data.temperature, str_buf);
            Logger_Log(LOG_LEVEL_INFO, str_buf);
            if (LCD1602_SetCursor(&g_lcd, 1, 0) != LCD1602_OK ||
                LCD1602_PrintString(&g_lcd, str_buf) != LCD1602_OK) {
                Logger_Log(LOG_LEVEL_ERROR, "Error in LCD1602_PrintString");
            }
            if (data.humidity > 93) {
                if (Buzzer_Toggle(&g_buzzer) != BUZZER_OK) {
                    Logger_Log(LOG_LEVEL_ERROR, "Error in Buzzer_Toggle");
                }
                Logger_Log(LOG_LEVEL_INFO, "Buzzer is toggling");
            }

            last_dht_read_time = current_time;
        }
    }

    return 0;
}

void uint8_to_str(uint8_t val, char *str) {
    str[0] = (val / 10) + '0';
    str[1] = (val % 10) + '0';
    str[2] = '\0';
}

void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv) {};
