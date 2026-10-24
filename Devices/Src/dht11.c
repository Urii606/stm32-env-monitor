#include <stddef.h>

#include "debug_pins.h"
#include "dht11.h"
#include "stm32f411xx_dwt_delay.h"

#define DHT11_TIMEOUT_US 100U
/* Timing window to distinguish bit '0' (26-28us) from bit '1' (70us) */
#define DHT11_BIT_THRESHOLD_US 40U
/* Minimum LOW duration required by DHT11 to detect start signal */
#define DHT11_START_PULSE_LOW_US 18000U
/* Delay before releasing bus to sample presence response*/
#define DHT11_WAIT_FOR_RESPONSE_US 30U

static void s_DHT11_Start(const DHT11_Handle_t *dev);
static DHT11_Status_t s_DHT11_Check_Response(const DHT11_Handle_t *dev);
static DHT11_Status_t s_DHT11_Read_Byte(const DHT11_Handle_t *dev, uint8_t *byte);

DHT11_Status_t DHT11_Init(const DHT11_Handle_t *dev) {
    if (dev == NULL || dev->pGPIOx == NULL || dev->pin_number > 15) {
        return DHT11_ERR_PARAM;
    }

    /* Configure GPIO as Open-Drain with Pull-Up to meet 1-Wire bus electrical specifications */
    GPIO_Handle_t gpio = {.pGPIOx = dev->pGPIOx,
                          .GPIO_PinConfig = {.GPIO_PinMode = GPIO_MODE_OUT,
                                             .GPIO_PinOPType = GPIO_OP_TYPE_OD,
                                             .GPIO_PinPuPdControl = GPIO_PIN_PU,
                                             .GPIO_PinSpeed = GPIO_SPEED_FAST,
                                             .GPIO_PinNumber = dev->pin_number}};

    GPIO_PeripheralClockControl(gpio.pGPIOx, ENABLE);
    GPIO_Init(&gpio);

    /* Release line to High-Z Idle state */
    GPIO_WriteToOutputPin(gpio.pGPIOx, dev->pin_number, GPIO_PIN_SET);

    return DHT11_OK;
}

DHT11_Status_t DHT11_Read(const DHT11_Handle_t *dev, DHT11_Data_t *data) {

    if (dev == NULL || data == NULL) {
        return DHT11_ERR_PARAM;
    }

    /* Disable interrupts during time-critical 1-Wire communication */
    DISABLE_GLOBAL_INTERRUPTS();

    DEBUG_PIN_HIGH();

    s_DHT11_Start(dev);

    if (s_DHT11_Check_Response(dev) == DHT11_ERR_TIMEOUT) {
        /* Always restore interrupts before early return to prevent system lockout */
        ENABLE_GLOBAL_INTERRUPTS();
        DEBUG_PIN_LOW();
        return DHT11_ERR_TIMEOUT;
    }

    uint8_t raw_data[5] = {0};
    for (uint8_t i = 0; i < 5; i++) {
        if (s_DHT11_Read_Byte(dev, &raw_data[i]) == DHT11_ERR_TIMEOUT) {
            ENABLE_GLOBAL_INTERRUPTS();
            DEBUG_PIN_LOW();
            return DHT11_ERR_TIMEOUT;
        }
    }

    /* Re-enable interrupts immediately after bit polling completes */
    ENABLE_GLOBAL_INTERRUPTS();
    DEBUG_PIN_LOW();

    /* Verify 8-bit checksum: sum of first 4 bytes must match byte 5 */
    if (raw_data[4] != (uint8_t)(raw_data[0] + raw_data[1] + raw_data[2] + raw_data[3])) {
        return DHT11_ERR_CHECKSUM;
    }

    data->humidity = raw_data[0];
    data->temperature = raw_data[2];
    return DHT11_OK;
}

static void s_DHT11_Start(const DHT11_Handle_t *dev) {
    /* Drive line LOW to wake up sensor controller */
    GPIO_WriteToOutputPin(dev->pGPIOx, dev->pin_number, GPIO_PIN_RESET);
    Delay_Us(DHT11_START_PULSE_LOW_US);

    /* Release line HIGH and wait for sensor handshake */
    GPIO_WriteToOutputPin(dev->pGPIOx, dev->pin_number, GPIO_PIN_SET);
    Delay_Us(DHT11_WAIT_FOR_RESPONSE_US);
}

static DHT11_Status_t s_DHT11_Check_Response(const DHT11_Handle_t *dev) {
    uint32_t timeout = 0;

    /* Poll for sensor response falling edge */
    while (GPIO_ReadFromInputPin(dev->pGPIOx, dev->pin_number) == GPIO_PIN_SET) {
        Delay_Us(1);
        timeout++;
        if (timeout > DHT11_TIMEOUT_US) {
            return DHT11_ERR_TIMEOUT;
        }
    }

    timeout = 0;
    /* Wait for 80us LOW presence pulse to complete */
    while (GPIO_ReadFromInputPin(dev->pGPIOx, dev->pin_number) == GPIO_PIN_RESET) {
        Delay_Us(1);
        timeout++;
        if (timeout > DHT11_TIMEOUT_US) {
            return DHT11_ERR_TIMEOUT;
        }
    }

    timeout = 0;
    /* Wait for 80us HIGH response pulse to complete before data transmission */
    while (GPIO_ReadFromInputPin(dev->pGPIOx, dev->pin_number) == GPIO_PIN_SET) {
        Delay_Us(1);
        timeout++;
        if (timeout > DHT11_TIMEOUT_US) {
            return DHT11_ERR_TIMEOUT;
        }
    }

    return DHT11_OK;
}

static DHT11_Status_t s_DHT11_Read_Byte(const DHT11_Handle_t *dev, uint8_t *byte) {
    *byte = 0;

    /* DHT11 transmits MSB first */
    for (uint8_t i = 0; i < 8; i++) {
        uint32_t timeout = 0;

        /* Wait for bit preamble (50us LOW level) to end */
        while (GPIO_ReadFromInputPin(dev->pGPIOx, dev->pin_number) == GPIO_PIN_RESET) {
            Delay_Us(1);
            timeout++;
            if (timeout > DHT11_TIMEOUT_US) {
                return DHT11_ERR_TIMEOUT;
            }
        }

        /* Sample line after 40us: '0' bit will already be LOW, '1' bit remains HIGH */
        Delay_Us(DHT11_BIT_THRESHOLD_US);
        if (GPIO_ReadFromInputPin(dev->pGPIOx, dev->pin_number) == GPIO_PIN_SET) {
            *byte |= (1 << (7 - i));
        }

        timeout = 0;
        /* Wait for remainder of HIGH pulse to synchronize with next bit */
        while (GPIO_ReadFromInputPin(dev->pGPIOx, dev->pin_number) == GPIO_PIN_SET) {
            Delay_Us(1);
            timeout++;
            if (timeout > DHT11_TIMEOUT_US) {
                return DHT11_ERR_TIMEOUT;
            }
        }
    }

    return DHT11_OK;
}
