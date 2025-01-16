#include "../HAL-SYSTEM/inc/stm32f10x.h"
#include "../HAL-UART/inc/stm32f10x_usart.h"
#include "../../Command_Line_App/memory_utility/memory_utility.h"
#include "../../Command_Line_App/UART_command_line/UART_Command_Line.h"
#include "../../Command_Line_App/semaphore/semaphore.h"
#include <stdio.h>
#include <string.h>

void USART2_IRQHandler(void)
{
    static uint16_t counter = (uint16_t) 0x0000;     // Static counter to keep track of received characters
    static bool mem_alloc_status = false;            // Flag to store memory allocation status for the raw buffer

    // Check if the RXNE (Receive Data Register Not Empty) flag is set (indicating new data is available)
    if(SET == USART_GetFlagStatus(USART2, USART_FLAG_RXNE))
    {
        // If the counter is zero, indicating we are receiving the start of a new message
        if(counter == (uint16_t) 0x0000)
        {
            // Attempt to allocate memory for the raw UART buffer
            mem_alloc_status = allocate_memory(&g_uart_xml_raw_buffer, INPUT_BUFFER_LENGTH);

            // If memory allocation was successful
            if(mem_alloc_status)
            {
                // Clear the raw buffer to avoid leftover data from previous messages
                memset(g_uart_xml_raw_buffer, 0, sizeof(g_uart_xml_raw_buffer));
            }
        }
        else
        {
            // If memory was successfully allocated
            if(mem_alloc_status)
            {
                // Read a character from USART2 and store it in the raw buffer
                g_uart_xml_raw_buffer[counter] = (char) USART_ReceiveData(USART2);

                // Check if the received character is the newline character (end of message)
                if(g_uart_xml_raw_buffer[counter] == NEWLINE_ASCII)
                {
                    // If the semaphore is unlocked, indicating the main application can process data
                    if(g_semaphore == SEMAPHORE_UNLOCKED)
                    {
                        // Acquire the semaphore to signal that data processing will begin
                        acquire_semaphore(&g_semaphore);

                        // Allocate memory for the main buffer where the data will be copied
                        mem_alloc_status = allocate_memory(&g_uart_xml_main_buffer, INPUT_BUFFER_LENGTH);

                        // If memory allocation for the main buffer was successful
                        if(mem_alloc_status)
                        {
                            // Clear the main buffer to avoid leftover data from previous messages
                            memset(g_uart_xml_main_buffer, 0, sizeof(g_uart_xml_main_buffer));

                            // Copy the raw buffer content (the received message) into the main buffer
                            memcpy(g_uart_xml_main_buffer, g_uart_xml_raw_buffer, strlen(g_uart_xml_raw_buffer) + 1);
                        }

                        // Free the raw buffer to release memory as it is no longer needed
                        free(g_uart_xml_raw_buffer);
                    }
                    // Reset the counter and memory allocation flag to prepare for the next message
                    counter = (uint16_t) 0x0000;
                    mem_alloc_status = false;
                }
                else
                {
                    // Increment the counter to store the next received character in the buffer
                    ++counter;
                }
            }
            else
            {
                // If memory allocation failed, reset the counter and memory flag
                counter = (uint16_t) 0x0000;
                mem_alloc_status = false;
            }
        }
    }
}
