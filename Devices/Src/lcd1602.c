
#include <string.h>

#include "lcd1602.h"
#include "stm32f411xx_dwt_delay.h"

/* PCF8574T to HD44780 pin mapping */
#define LCD_EN 0x04 /* P2: Enable bit (Falling edge triggers data latch) */
#define LCD_RW 0x02 /* P1: Read/Write bit (0 = Write) */
#define LCD_RS 0x01 /* P0: Register Select (0 = Command, 1 = Data) */
#define LCD_BL 0x08 /* P3: Backlight control */

LCD1602_Status_t LCD1602_Init(LCD1602_Handle_t *dev) {
    if (dev == NULL) {
        return LCD1602_ERR_PARAM;
    }

    Delay_Us(50000); /* Wait >40ms after VCC rises to 2.7V */

    /* Software reset sequence per HD44780 datasheet */
    LCD1602_Send4Bits(dev, 0x30, 0);
    Delay_Us(5000);
    LCD1602_Send4Bits(dev, 0x30, 0);
    Delay_Us(1000);
    LCD1602_Send4Bits(dev, 0x30, 0);
    Delay_Us(1000);
    LCD1602_Send4Bits(dev, 0x20, 0);
    Delay_Us(1000);

    /* Set 4-bit interface, 2 display lines, 5x8 font */
    LCD1602_SendCommand(dev, 0x28);
    /* Display ON, Cursor OFF, Blink OFF */
    LCD1602_SendCommand(dev, 0x0C);
    /* Clear display */
    LCD1602_SendCommand(dev, 0x01);
    Delay_Us(2000); /* Clear display command requires >1.52ms execution time */
    /* Entry mode: Auto-increment cursor, no shift */
    LCD1602_SendCommand(dev, 0x06);

    return LCD1602_OK;
}

LCD1602_Status_t LCD1602_WriteByte(LCD1602_Handle_t *dev, uint8_t data) {
    if (dev == NULL) {
        return LCD1602_ERR_PARAM;
    }

    if (I2C_MasterSendData(dev->pI2CHandle, &data, 1, dev->i2c_address, ENABLE) != I2C_OK) {
        return LCD1602_ERR_I2C;
    }

    return LCD1602_OK;
}

LCD1602_Status_t LCD1602_Send4Bits(LCD1602_Handle_t *dev, uint8_t data_nible, uint8_t rs_flag) {
    LCD1602_Status_t status;
    uint8_t byte_to_send = data_nible | rs_flag | LCD_BL | LCD_EN;

    status = LCD1602_WriteByte(dev, byte_to_send);
    if (status != LCD1602_OK) {
        return status;
    }

    byte_to_send &= ~LCD_EN;
    return LCD1602_WriteByte(dev, byte_to_send);
}

LCD1602_Status_t LCD1602_SendCommand(LCD1602_Handle_t *dev, uint8_t cmd) {
    if (dev == NULL) {
        return LCD1602_ERR_PARAM;
    }
    LCD1602_Status_t status;
    uint8_t high_nibble = cmd & 0xF0;
    uint8_t low_nibble = (cmd << 4) & 0xF0;
    status = LCD1602_Send4Bits(dev, high_nibble, 0);
    if (status != LCD1602_OK) {
        return status;
    }

    Delay_Us(50);
    return LCD1602_Send4Bits(dev, low_nibble, 0);
}

LCD1602_Status_t LCD1602_SendData(LCD1602_Handle_t *dev, uint8_t data) {
    LCD1602_Status_t status;
    if (dev == NULL) {
        return LCD1602_ERR_PARAM;
    }
    uint8_t high_nibble = data & 0xF0;
    uint8_t low_nibble = (data << 4) & 0xF0;
    status = LCD1602_Send4Bits(dev, high_nibble, LCD_RS);
    if (status != LCD1602_OK) {
        return status;
    }
    Delay_Us(50);
    return LCD1602_Send4Bits(dev, low_nibble, LCD_RS);
}

LCD1602_Status_t LCD1602_PrintString(LCD1602_Handle_t *dev, char *str) {
    if (dev == NULL) {
        return LCD1602_ERR_PARAM;
    }

    LCD1602_Status_t status;

    for (uint32_t i = 0; str[i] != '\0'; i++) {
        status = LCD1602_SendData(dev, (uint8_t)str[i]);
        if (status != LCD1602_OK) {
            return status;
        }
    }
    return LCD1602_OK;
}

LCD1602_Status_t LCD1602_SetCursor(LCD1602_Handle_t *dev, uint8_t row, uint8_t col) {
    if (dev == NULL || row > 1 || col > 20)
        return LCD1602_ERR_PARAM;

    uint8_t base_addr = 0;
    if (row == 0) {
        base_addr = 0x00;
    } else {
        base_addr = 0x40;
    }
    base_addr += col;

    return LCD1602_SendCommand(dev, 0x80 | base_addr);
}