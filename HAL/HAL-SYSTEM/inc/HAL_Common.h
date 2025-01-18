#ifndef __HAL_COMM_H
#define __HAL_COMM_H

#include "stm32f10x.h"
#include "../../HAL-RCC/inc/stm32f10x_rcc.h"
#include "../../HAL-GPIO/inc/hal_gpio_config.h"
#include "../../HAL-GPIO/inc/stm32f10x_gpio.h"
#include "../../HAL-UART/inc/stm32f10x_usart.h"
#include "../../HAL-UART/inc/hal_usart2_config.h"

typedef enum {
    HAL_OK = 0,         // Operation completed successfully
    HAL_ERROR = 1,      // General error occurred
    HAL_BUSY = 2,       // Peripheral is busy
    HAL_TIMEOUT = 3,    // Operation timed out
    HAL_INVALID = 4,    // Invalid parameter or operation
    HAL_UNSUPPORTED = 5 // Operation not supported
} HAL_StatusTypeDef;

void HAL_config_MCU(void);

#endif /* __HAL_COMM_H */
