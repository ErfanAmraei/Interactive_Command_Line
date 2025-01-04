/*
 * hal_common.c
 *
 * This source file contains common hardware abstraction layer (HAL) configurations.
 * It includes:
 *
 * - Initialization and configuration of general-purpose input/output (GPIO).
 * - Configuration and initialization of USART2 for communication purposes.
 * - Integration of core functions to prepare the microcontroller for reliable operation.
 *
 * The file serves as the entry point for configuring critical hardware components 
 * necessary for system functionality.
 */

#include "../inc/HAL_Common.h"

void HAL_config_MCU(void)
{
    HAL_GPIO_Config();
    HAL_USART2_Config();
}


