/**
 * @file    logger.c
 * @brief   Application logging service implementation.
 *          Decoupled from micro-controller peripherals via the BSP console interface.
 */

#include "logger.h"
#include "console.h"
#include <stdio.h>
#include <string.h>

/* Maximum single-line capacity including prefix, payload, CRLF, and null terminator */
#define LOG_MAX_LINE_LEN 128U

void Logger_Log(LogLevel_t log_level, const char *msg) {
    /* Guard clause: prevent undefined behavior from null pointer dereferencing */
    if (msg == NULL) {
        msg = "(null)";
    }

    const char *prefix = NULL;
    char line_buffer[LOG_MAX_LINE_LEN];

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

    /* Format line into a single atomic frame */
    snprintf(line_buffer, sizeof(line_buffer), "%s%s\r\n", prefix, msg);

    /* Dispatch payload synchronously to the hardware transport */
    Console_Write(line_buffer, strlen(line_buffer));
}