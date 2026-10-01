#pragma once

#include "stm32f411xx_usart_driver.h"

typedef enum
{
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR
}LogLevel_t;

void Logger_Init(USART_Handle_t* p_usart);
void Logger_Log(LogLevel_t log_level,const char* msg);
