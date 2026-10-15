#pragma once

/**
 * @brief Severity levels for application logging events.
 */
typedef enum {
    LOG_LEVEL_INFO,    /**< General informational runtime events */
    LOG_LEVEL_WARNING, /**< Recoverable anomalies or warnings */
    LOG_LEVEL_ERROR    /**< Critical hardware or logic failures */
} LogLevel_t;

/**
 * @brief Formats a diagnostic message with severity prefix and emits it to the console.
 * 
 * @note This function is stateless and relies on the BSP console layer being initialized.
 * 
 * @param log_level Severity classification of the log entry.
 * @param msg       Null-terminated payload string. Passing NULL safely renders "(null)".
 */
void Logger_Log(LogLevel_t log_level, const char *msg);