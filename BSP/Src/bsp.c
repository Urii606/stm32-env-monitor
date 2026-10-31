

/* Board support package*/
#include "bsp.h"
#include "debug_pins.h"
#include "logger.h"
#include "stm32f411xx_dwt_delay.h"

#define GPIO_I2C_ALTF_PIN 4
#define GPIO_I2C_SCL_PIN GPIO_PIN_NO_6
#define GPIO_I2C_SDA_PIN GPIO_PIN_NO_7

LCD1602_Handle_t g_lcd;
DHT11_Handle_t g_dht;
Buzzer_Handle_t g_buzzer;

I2C_Handle_t g_i2c1;

void BSP_Init(void) {
    Delay_Init();

    /* --- BUZZER CONFIGURATION --- */
    g_buzzer.pGPIOx = GPIOA;
    g_buzzer.pin_number = GPIO_PIN_NO_1;
    /* Hardware verified: module emit sound on 3.3V */
    g_buzzer.active_state = BUZZER_ACTIVE_HIGH;

    if (Buzzer_Init(&g_buzzer) != BUZZER_OK) {
        Logger_Log(LOG_LEVEL_ERROR, "Error(Buzzer != OK)");
        while (1);
    }

    /* --- DHT11 CONFIGURATION --- */
    g_dht.pGPIOx = GPIOA;
    g_dht.pin_number = GPIO_PIN_NO_2;
    if (DHT11_Init(&g_dht) != DHT11_OK) {
        Logger_Log(LOG_LEVEL_ERROR, "Error in DHT11_Init");
        while (1) {
        };
    }

    /* --- I2C1 GPIO CONFIGURATION (PB6 = SCL, PB7 = SDA) --- */
    GPIO_Handle_t gpio_i2c = {
        .pGPIOx = GPIOB,
        .GPIO_PinConfig = {
            .GPIO_PinMode = GPIO_MODE_ALTFN,
            .GPIO_PinOPType = GPIO_OP_TYPE_OD,
            /* Enable internal pull-ups as a fallback if external resistors are weak */
            .GPIO_PinPuPdControl = GPIO_PIN_PU,
            .GPIO_PinSpeed = GPIO_SPEED_FAST,
            .GPIO_PinAltFunMode = GPIO_I2C_ALTF_PIN}};
    GPIO_PeripheralClockControl(GPIOB, ENABLE);

    gpio_i2c.GPIO_PinConfig.GPIO_PinNumber = GPIO_I2C_SCL_PIN;
    GPIO_Init(&gpio_i2c);
    gpio_i2c.GPIO_PinConfig.GPIO_PinNumber = GPIO_I2C_SDA_PIN;
    GPIO_Init(&gpio_i2c);

    /* --- I2C1 PERIPHERAL CONFIGURATION --- */
    g_i2c1.pI2Cx = I2C1;
    g_i2c1.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;
    /* MCU's own slave address (not used in Master mode, but driver requires it) */
    g_i2c1.I2C_Config.I2C_DeviceAddress = 0x61;
    g_i2c1.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;
    I2C_PeripheralClockControl(I2C1, ENABLE);
    I2C_Init(&g_i2c1);
    I2C_PeripheralControl(I2C1, ENABLE);

    /* --- LCD1602 CONFIGURATION --- */
    g_lcd.pI2CHandle = &g_i2c1;
    /* Slave address of the PCF8574 module (depends on A0-A2 solder jumpers) */
    g_lcd.i2c_address = 0x27;

    if (LCD1602_Init(&g_lcd) != LCD1602_OK) {
        Logger_Log(LOG_LEVEL_ERROR, "Error in lcd init");
    }
    DEBUG_PIN_INIT();
}
