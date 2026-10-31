#pragma once
#include "stm32f411xx_i2c_driver.h"

/**
 * @brief LCD1602 device handle structure for I2C communication.
 */
typedef struct {
    I2C_Handle_t *pI2CHandle; /**< Pointer to configured I2C peripheral handle */
    uint8_t i2c_address;     /**< I2C slave address of PCF8574 backpack */
} LCD1602_Handle_t;

/**
 * @brief LCD1602 driver status codes.
 */
typedef enum {
    LCD1602_OK,          /**< Operation completed successfully */
    LCD1602_ERR_TIMEOUT, /**< Timeout during transfer */
    LCD1602_ERR_PARAM,   /**< Invalid input parameter or NULL pointer */
    LCD1602_ERR_I2C      /**< I2C bus transmission error */
} LCD1602_Status_t;

/**
 * @brief Initializes LCD1602 display in 4-bit mode via PCF8574 I2C expander.
 * @param dev Pointer to LCD1602 device handle.
 * @return LCD1602_OK on success, or error code on failure.
 */
LCD1602_Status_t LCD1602_Init(LCD1602_Handle_t *dev);

/**
 * @brief Writes a raw 8-bit value to PCF8574 I2C expander port.
 * @param dev Pointer to LCD1602 device handle.
 * @param data 8-bit data representing pin states on expander.
 * @return LCD1602_OK on success, or error code on failure.
 */
LCD1602_Status_t LCD1602_WriteByte(LCD1602_Handle_t *dev, uint8_t data);

/**
 * @brief Transmits 4 bits of data/command with latching pulse on EN pin.
 * @param dev Pointer to LCD1602 device handle.
 * @param data_nible High 4 bits containing data/command nibble (0xF0 masked).
 * @param rs_flag RS pin state (0 for command, LCD_RS for data).
 * @return LCD1602_OK on success, or error code on failure.
 */
LCD1602_Status_t LCD1602_Send4Bits(LCD1602_Handle_t *dev, uint8_t data_nible, uint8_t rs_flag);

/**
 * @brief Sends a command byte to HD44780 controller.
 * @param dev Pointer to LCD1602 device handle.
 * @param cmd 8-bit command byte.
 * @return LCD1602_OK on success, or error code on failure.
 */
LCD1602_Status_t LCD1602_SendCommand(LCD1602_Handle_t *dev, uint8_t cmd);

/**
 * @brief Sends a data byte (character ASCII) to display RAM.
 * @param dev Pointer to LCD1602 device handle.
 * @param data ASCII character code.
 * @return LCD1602_OK on success, or error code on failure.
 */
LCD1602_Status_t LCD1602_SendData(LCD1602_Handle_t *dev, uint8_t data);

/**
 * @brief Prints a null-terminated string at current cursor position.
 * @param dev Pointer to LCD1602 device handle.
 * @param str Pointer to null-terminated C-string.
 * @return LCD1602_OK on success, or error code on failure.
 */
LCD1602_Status_t LCD1602_PrintString(LCD1602_Handle_t *dev, char *str);

/**
 * @brief Sets display cursor position.
 * @param dev Pointer to LCD1602 device handle.
 * @param row Target line index (0 for Line 1, 1 for Line 2).
 * @param col Target column index (0 to 15 for 16x2 display).
 * @return LCD1602_OK on success, or error code on failure.
 */
LCD1602_Status_t LCD1602_SetCursor(LCD1602_Handle_t *dev, uint8_t row, uint8_t col);