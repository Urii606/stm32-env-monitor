#include "logger.h"
#include "stm32f411xx.h"
#include "stm32f411xx_gpio_driver.h"
#include "stm32f411xx_usart_driver.h"
#include <stdint.h>

#define EXPECTED_MSG_LEN 22

static USART_Handle_t usart1_handle;
static uint8_t rx_buffer[32];
static volatile uint8_t rx_complete = 0;

static void Console_USART_Init(void) {
    GPIO_Handle_t usart_pins;
    usart_pins.pGPIOx = GPIOA;
    usart_pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    usart_pins.GPIO_PinConfig.GPIO_PinAltFunMode = 7;
    usart_pins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    usart_pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
    usart_pins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;

    GPIO_PeripheralClockControl(GPIOA, ENABLE);

    usart_pins.GPIO_PinConfig.GPIO_PinNumber = 9;
    GPIO_Init(&usart_pins);

    usart_pins.GPIO_PinConfig.GPIO_PinNumber = 10;
    GPIO_Init(&usart_pins);

    usart1_handle.pUSARTx = USART1;
    usart1_handle.USART_Config.USART_Baud = USART_STD_BAUD_9600;
    usart1_handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
    usart1_handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
    usart1_handle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
    usart1_handle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;
    usart1_handle.USART_Config.USART_Mode = USART_MODE_TXRX;

    USART_PeriClockControl(USART1, ENABLE);
    USART_Init(&usart1_handle);

    USART_IRQInterruptConfig(IRQ_NO_USART1, ENABLE);
    USART_IRQPriorityConfig(IRQ_NO_USART1, 15);

    USART_PeripheralControl(USART1, ENABLE);
}

void SystemInit(void) {
// Тут можна увімкнути FPU (Coprocessor CP10/CP11), якщо він використовується:
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2)); /* set CP10 and CP11 Full Access */
#endif
}

void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 4000; i++) {
        __asm volatile("nop");
    }
}

int main(void) {
    Console_USART_Init();
    Logger_Init(&usart1_handle);

    while (USART_ReceiveDataIT(&usart1_handle, rx_buffer, EXPECTED_MSG_LEN) != USART_READY)
        ;

    Logger_Log(LOG_LEVEL_INFO, "System booted");

    while (rx_complete != 1) {
        __asm volatile("nop");
    }
    while (1) {
    }
    return 0;
}

void USART1_IRQHandler(void) { USART_IRQHandling(&usart1_handle); }

void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle, uint8_t ApEv) {
    if (ApEv == USART_EVENT_RX_CMPLT) {
        rx_complete = 1;
    }
}