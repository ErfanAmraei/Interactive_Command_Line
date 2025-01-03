/*
 * hal_gpio_config.c
 *
 * This header file provides the declaration and implementation of the 
 * HAL_GPIO_Config function, which is responsible for configuring GPIO pins 
 * used for USART2 communication.
 *
 * This function initializes the necessary GPIO settings, including enabling 
 * the clocks, configuring pin modes, and remapping USART2 functionality.
 */



#include "hal_gpio_config.h"

/**
 * @brief Configures GPIO pins for USART2 communication.
 *        - PA2: TX (Alternate Function Push-Pull)
 *        - PA3: RX (Floating Input)
 */
void HAL_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_USART_Config;

    // Enable clock for GPIOA to access its pins
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // Enable alternate function remapping for USART2
    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);

    // Configure PA2 as TX (Transmit) with Alternate Function Push-Pull mode
    GPIO_USART_Config.GPIO_Mode  = GPIO_Mode_AF_PP;  // Alternate Function Push-Pull
    GPIO_USART_Config.GPIO_Speed = GPIO_Speed_50MHz; // High-speed configuration
    GPIO_USART_Config.GPIO_Pin   = GPIO_Pin_2;       // Select pin PA2
    GPIO_Init(GPIOA, &GPIO_USART_Config);           // Initialize GPIOA Pin 2

    // Configure PA3 as RX (Receive) with Floating Input mode
    GPIO_USART_Config.GPIO_Mode  = GPIO_Mode_IN_FLOATING; // Input floating
    GPIO_USART_Config.GPIO_Pin   = GPIO_Pin_3;            // Select pin PA3
    GPIO_Init(GPIOA, &GPIO_USART_Config);                // Initialize GPIOA Pin 3
}




