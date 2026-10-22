#include "buzzer.h"
#include "console.h"
#include "dht11.h"
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
    Delay_Init();

    Buzzer_Handle_t buzzer = {.pGPIOx = GPIOA, .pin_number = 1, .active_state = BUZZER_ACTIVE_LOW};
    const DHT11_Handle_t dht11 = {.pGPIOx = GPIOA, .pin_number = 2};
    DHT11_Data_t data = {0};
    char str_buf[4];

    if (Buzzer_Init(&buzzer) != BUZZER_OK) {
        Logger_Log(LOG_LEVEL_ERROR, "Error(Buzzer != OK)");
        while (1) {
        };
    }

    if (DHT11_Init(&dht11) != DHT11_OK) {
        Logger_Log(LOG_LEVEL_ERROR, "Error in DHT11_Init");
        while (1) {
        };
    }

    Logger_Log(LOG_LEVEL_INFO, "System started successfully");
    while (1) {
        if (DHT11_Read(&dht11, &data) != DHT11_OK) {
            Logger_Log(LOG_LEVEL_ERROR, "Error in dht11_read");
        }

        Logger_Log(LOG_LEVEL_INFO, "Humidity:");
        uint8_to_str(data.humidity, str_buf);
        Logger_Log(LOG_LEVEL_INFO, str_buf);
        Logger_Log(LOG_LEVEL_INFO, "Temperature:");
        uint8_to_str(data.temperature, str_buf);
        Logger_Log(LOG_LEVEL_INFO, str_buf);
        if (data.temperature > 0) {
            Buzzer_Toggle(&buzzer);
            Logger_Log(LOG_LEVEL_INFO, "Buzzer is toggling");
        }
        Delay_Us(2000000);
    }
    return 0;
}

void uint8_to_str(uint8_t val, char *str) {
    str[0] = (val / 10) + '0'; // Десятки
    str[1] = (val % 10) + '0'; // Одиниці
    str[2] = '\0';             // Null-terminator
}
