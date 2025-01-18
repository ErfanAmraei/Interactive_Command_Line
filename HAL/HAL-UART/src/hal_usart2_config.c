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

#include "../inc/stm32f10x_usart.h"

// Initializes the standard I/O (printf) mechanism for embedded systems 
// by defining the required structures for standard input, output, and error streams.
struct __FILE { int handle; };  // Custom definition for FILE structure to work with printf in embedded systems

FILE __stdout;                 // Standard output stream
FILE __stdin;                  // Standard input stream
FILE __stderr;                 // Standard error stream

// Redirects printf output to USART2. 
// When printf is called, this function will execute, sending each character through USART2.
int fputc(int character, FILE *f)  
{
    // wait until the USART2 transmit data register is empty (ready to transmit).
    while (!USART_GetFlagStatus(USART2, USART_FLAG_TXE));  

    // send the character through USART2.
    USART_SendData(USART2, character);

    // return the transmitted character as confirmation.
    return character;  
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
    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4); //set priority grouping
    NVIC_SetPriority(USART2_IRQn, USART_NVIC_PERIORITY); //Set interrupt priority
    NVIC_EnableIRQ(USART2_IRQn);                   //enable USART2 interrupt in NVIC

    //enable USART2 for communication
    USART_Cmd(USART2, ENABLE);
}

