/**
 * @file    console.c
 * @brief   Board Support Package (BSP) console driver for STM32F411.
 *          Provides blocking UART transmission with RAM mirror logging
 *          and interrupt-driven continuous reception.
 */

#include "console.h"
#include "stm32f411xx_gpio_driver.h"
#include "stm32f411xx_usart_driver.h"
#include <stddef.h>

/* ========================================================================== */
/*                             Configuration & Constants                      */
/* ========================================================================== */

#define CONSOLE_RAM_BUF_SIZE 256U

/* GPIO Pinout (USART1 on PA9 / PA10) */
#define CONSOLE_GPIO_PORT GPIOA
#define CONSOLE_PIN_TX 9
#define CONSOLE_PIN_RX 10
#define CONSOLE_ALTF_PIN 7
#define CONSOLE_GPIO_SPEED GPIO_SPEED_FAST
#define CONSOLE_GPIO_MODE GPIO_MODE_ALTFN
#define CONSOLE_PIN_OP_TYPE GPIO_OP_TYPE_PP
#define CONSOLE_PIN_PUPD_CONTROL GPIO_NO_PUPD

/* USART1 Framing: 115200 8N1 */
#define CONSOLE_USART_PORT USART1
#define CONSOLE_USART_MODE USART_MODE_TXRX
#define CONSOLE_USART_BAUD USART_STD_BAUD_115200
#define CONSOLE_USART_NO_STOPBITS USART_STOPBITS_1
#define CONSOLE_USART_WORD_LENGTH USART_WORDLEN_8BITS
#define CONSOLE_USART_PARITY USART_PARITY_DISABLE
#define CONSOLE_USART_HW_FLOW USART_HW_FLOW_CTRL_NONE

#define CONSOLE_IRQ_PRIORITY NVIC_IRQ_PRIO6

/* Pre-build boundary checks */
#if (CONSOLE_PIN_TX > 15) || (CONSOLE_PIN_RX > 15)
#error "Console pin number out of range: must be between 0 and 15"
#endif

_Static_assert(CONSOLE_USART_BAUD > 0, "Baud rate must be non-zero");

/* ========================================================================== */
/*                                Private State                               */
/* ========================================================================== */

/* In-memory TX ring buffer: mirrors output for debugger inspection (Live Watch)
 * when a physical USB-to-UART bridge is not attached. */
static char s_ram_buffer[CONSOLE_RAM_BUF_SIZE];
static uint32_t s_write_idx = 0;

/* Single-byte staging buffer populated by the MCAL interrupt service routine */
static uint8_t s_rx_byte = 0;

/* RX ring buffer and index. Marked volatile as they are modified inside the ISR
 * and accessed asynchronously. */
static volatile char s_rx_buffer[CONSOLE_RAM_BUF_SIZE];
static volatile uint32_t s_rx_idx = 0;

/* Persistent hardware instance descriptor stored in static storage */
static USART_Handle_t s_usart_handle = {
    .pUSARTx = CONSOLE_USART_PORT,
    .USART_Config = {.USART_Mode = CONSOLE_USART_MODE,
                     .USART_Baud = CONSOLE_USART_BAUD,
                     .USART_NoOfStopBits = CONSOLE_USART_NO_STOPBITS,
                     .USART_WordLength = CONSOLE_USART_WORD_LENGTH,
                     .USART_ParityControl = CONSOLE_USART_PARITY,
                     .USART_HWFlowControl = CONSOLE_USART_HW_FLOW}};

/* ========================================================================== */
/*                              Public API                                    */
/* ========================================================================== */

Console_Status_t Console_Init(void) {
    s_write_idx = 0;
    s_rx_idx = 0;

    /* Ephemeral GPIO configuration stack object */
    GPIO_Handle_t gpio = {.pGPIOx = CONSOLE_GPIO_PORT,
                          .GPIO_PinConfig = {.GPIO_PinMode = CONSOLE_GPIO_MODE,
                                             .GPIO_PinAltFunMode = CONSOLE_ALTF_PIN,
                                             .GPIO_PinOPType = CONSOLE_PIN_OP_TYPE,
                                             .GPIO_PinPuPdControl = CONSOLE_PIN_PUPD_CONTROL,
                                             .GPIO_PinSpeed = CONSOLE_GPIO_SPEED}};

    if (s_usart_handle.pUSARTx == NULL) {
        return CONSOLE_ERR;
    }

    /* 1. Power on GPIO clock gating and configure pins */
    GPIO_PeripheralClockControl(gpio.pGPIOx, ENABLE);

    gpio.GPIO_PinConfig.GPIO_PinNumber = CONSOLE_PIN_TX;
    GPIO_Init(&gpio);

    gpio.GPIO_PinConfig.GPIO_PinNumber = CONSOLE_PIN_RX;
    GPIO_Init(&gpio);

    /* 2. Power on USART clock gating and initialize hardware */
    USART_PeriClockControl(s_usart_handle.pUSARTx, ENABLE);
    USART_Init(&s_usart_handle);
    USART_PeripheralControl(s_usart_handle.pUSARTx, ENABLE);

    /* Hardware read-back: verify that the USART Enable (UE) bit is set */
    if ((s_usart_handle.pUSARTx->CR1 & (1U << USART_CR1_UE)) == 0) {
        return CONSOLE_ERR;
    }

    /* 3. Configure NVIC line and arm the first non-blocking byte reception */
    USART_IRQInterruptConfig(IRQ_NO_USART1, ENABLE);
    USART_IRQPriorityConfig(IRQ_NO_USART1, CONSOLE_IRQ_PRIORITY);

    if (USART_ReceiveDataIT(&s_usart_handle, &s_rx_byte, sizeof(s_rx_byte)) == USART_BUSY_IN_RX) {
        return CONSOLE_ERR;
    }

    return CONSOLE_OK;
}

Console_Status_t Console_Write(const char *data, uint32_t len) {
    if (data == NULL || len == 0) {
        return CONSOLE_ERR;
    }

    /* Synchronous blocking transmission via polling */
    USART_SendData(&s_usart_handle, (uint8_t *)data, len);

    /* Mirror transmitted bytes into the RAM ring buffer */
    for (uint32_t i = 0; i < len; i++) {
        s_ram_buffer[s_write_idx] = data[i];
        s_write_idx++;
        if (s_write_idx >= CONSOLE_RAM_BUF_SIZE) {
            s_write_idx = 0;
        }
    }

    return CONSOLE_OK;
}

/* ========================================================================== */
/*                             Interrupt Handlers                             */
/* ========================================================================== */

/**
 * @brief Application event callback dispatched by the MCAL interrupt dispatcher.
 */
void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle, uint8_t ApEv) {
    if (ApEv == USART_EVENT_RX_CMPLT) {
        /* Store the received byte into the ring buffer with wrap-around guard */
        s_rx_buffer[s_rx_idx++] = (char)s_rx_byte;
        if (s_rx_idx >= CONSOLE_RAM_BUF_SIZE) {
            s_rx_idx = 0;
        }

        /* Re-arm interrupt-driven reception for the next incoming byte */
        USART_ReceiveDataIT(pUSARTHandle, &s_rx_byte, sizeof(s_rx_byte));
    } else if (ApEv == USART_EVENT_TX_CMPLT) {
        /* Optional: Handle TX complete event when using asynchronous transmission */
    }
}

/**
 * @brief Hardware ISR vector handler defined in startup assembly.
 */
void USART1_IRQHandler(void) { USART_IRQHandling(&s_usart_handle); }