/*
 * hal_usart2_config.c
 *
 * This source file contains the implementation of functions for configuring 
 * and initializing USART2 communication in an embedded system. It includes:
 *
 * - Redirecting standard I/O (printf) to USART2 for debugging and communication.
 * - Configuration of USART2 parameters such as baud rate, data format, and interrupt handling.
 *
 * The file ensures proper initialization of USART2 and prepares it for reliable 
 * data transmission and reception.
 */

//#include "../inc/stm32f10x_usart.h"
//#include "../../HAL-RCC/inc/stm32f10x_rcc.h"
#include "../../HAL-UART/inc/hal_usart2_config.h"


#define  UART_TIMEOUT    (uint32_t) 3000
#define  PRIORITY_GROUP  (uint32_t)0x300

/**
 * @brief Transmits a string of data via the specified UART interface.
 *
 * Sends characters from the provided string through the UART. Checks for null
 * pointers and implements a timeout mechanism to prevent blocking indefinitely.
 *
 * @param UARTx Pointer to the USART peripheral (e.g., USART1, USART2).
 * @param data  Null-terminated string to be transmitted.
 * 
 * @return SUCCESS if data is transmitted successfully, ERROR otherwise.
 */

ErrorStatus UART_WriteData(USART_TypeDef *UARTx, const char* data)
{
    ErrorStatus outcome = SUCCESS;
    uint16_t index = 0;
    uint16_t timeout = 0;
    if(!data || !UARTx)
    {
        outcome = ERROR;
    }
    else
    {
        while(data[index])
        {
            // Wait until the USART transmit data register is empty or timeout occurs
            while (USART_GetFlagStatus(UARTx, USART_FLAG_TXE) == RESET) 
            {
                ++timeout;

                if(timeout > UART_TIMEOUT)
                {
                    break;
                }
            }

            if(timeout > UART_TIMEOUT)
            {
                break;
            }
            USART_SendData(UARTx, (char) data[index]);
        }
    }
		return outcome;
}

/**
 * @brief Configures and initializes USART2 for communication.
 *        Sets baud rate, data format, and enables interrupts.
 */
void HAL_USART2_Config(void)
{
    USART_InitTypeDef USART2_Config;

    //enable clock for USART2 to prepare it for initialization
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    //configure USART2 parameters
    USART2_Config.USART_BaudRate            = USART_BAUD_RATE;           // Set baud rate (defined elsewhere)
    USART2_Config.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // No hardware flow control
    USART2_Config.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;  // Enable both TX and RX modes
    USART2_Config.USART_Parity              = USART_Parity_No;           // No parity check
    USART2_Config.USART_StopBits            = USART_StopBits_1;          // Use 1 stop bit
    USART2_Config.USART_WordLength          = USART_WordLength_8b;       // 8-bit word length
    USART_Init(USART2, &USART2_Config);                                  // Initialize USART2 with the configuration

    //enable the RXNE (Receive Data Register Not Empty) interrupt
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  

    //configure NVIC for USART2 interrupts
    NVIC_SetPriorityGrouping(PRIORITY_GROUP); //set priority grouping
    NVIC_SetPriority(USART2_IRQn, USART_NVIC_PERIORITY); //Set interrupt priority
    NVIC_EnableIRQ(USART2_IRQn);                   //enable USART2 interrupt in NVIC

    //enable USART2 for communication
    USART_Cmd(USART2, ENABLE);
}

