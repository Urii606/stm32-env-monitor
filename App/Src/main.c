#include "buzzer.h"
#include "logger.h"
#include "console.h"
#include <stdint.h>

static void delay(void);

void SystemInit(void) {}

int main(void) {
    if(Console_Init() != CONSOLE_OK)
    {
        while(1){}
    }

    Buzzer_Handle_t buzzer = {.pGPIOx = GPIOA, .pin_number = 1, .active_state = BUZZER_ACTIVE_LOW};
   
    if (Buzzer_Init(&buzzer) != BUZZER_OK) {
        Logger_Log(LOG_LEVEL_ERROR, "Error(Buzzer != OK)");
        while (1) {
        }
    }
    Logger_Log(LOG_LEVEL_INFO, "System started successfully");
    while (1) {
        Buzzer_Toggle(&buzzer);
        Logger_Log(LOG_LEVEL_INFO, "Buzzer is toggling");
        delay();
    }
    return 0;
}

static void delay(void) {
    for (volatile uint32_t i = 0; i < 500000; i++)
        ;
}



