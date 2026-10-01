#include "logger.h"
#include <string.h>

static void Logger_PrintString(const char *msg);

static USART_Handle_t *s_p_usart = NULL; // saving state

void Logger_Init(USART_Handle_t *p_usart) { s_p_usart = p_usart; }
void Logger_Log(LogLevel_t log_level, const char *msg) {
    const char *prefix = NULL;
    switch (log_level) {
    case LOG_LEVEL_INFO:
        prefix = "[INFO] ";
        break;
    case LOG_LEVEL_WARNING:
        prefix = "[WARNING] ";
        break;
    case LOG_LEVEL_ERROR:
        prefix = "[ERROR] ";
        break;
    default:
        prefix = "[UNKNOWN] ";
        break;
    }
    Logger_PrintString(prefix);
    Logger_PrintString(msg);
    Logger_PrintString("\r\n");
}

static void Logger_PrintString(const char *str) {

    if (str == NULL) {
        str = "(null)";
    }
    uint32_t len = strlen(str);
    for (uint32_t i = 0; i < len; i++) {
        USART_SendData(s_p_usart, (uint8_t *)&str[i], 1);
    }
}