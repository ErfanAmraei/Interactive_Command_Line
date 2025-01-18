#ifndef __HAL_USART_CONF_H
#define __HAL_USART_CONF_H

#include "../../HAL-SYSTEM/inc/stm32f10x.h"
#include "../../HAL-RCC/inc/stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "../../HAL-SYSTEM/inc/misc.h"
#include <stdio.h>
#include <string.h>


#define USART_BAUD_RATE        (uint16_t) 9600
#define USART_NVIC_PERIORITY   (uint32_t) 0x00000000

void HAL_USART2_Config(void);

#endif /* __HAL_USART_CONF_H */
