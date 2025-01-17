#include "../HAL-SYSTEM/inc/stm32f10x.h"
#include "../HAL-UART/inc/stm32f10x_usart.h"
#include "../../Command_Line_App/memory_utility/memory_utility.h"
#include "../../Command_Line_App/UART_command_line/UART_Command_Line.h"
#include "../../Command_Line_App/semaphore/semaphore.h"
#include <stdio.h>
#include <string.h>

/**
* @breif USART2 Interrupt Service Routine (ISR)
*
* @param none
*
* @retval none
*/
void USART2_IRQHandler(void)
{
    static uint32_t char_index = (uint32_t) 0x00;     // Static char_index to keep track of received characters
    const uint32_t  MEM_BLOCK_NO = (uint32_t) 0x08;   //8 blocks *32 bytes = 256 bytes
    const uint32_t  CHECK_PARENT_TAG = (uint32_t) 0x07; // check the parent tag after receiving the first 7 characters
    
    // Check if the RXNE (Receive Data Register Not Empty) flag is set (indicating new data is available)
    if(SET == USART_GetFlagStatus(USART2, USART_FLAG_RXNE))
    {
        // If the char_index is zero, indicating we are receiving the start of a new message
        if(char_index == (uint32_t) 0x0000)
        {
            // Attempt to allocate memory for the raw UART buffer
            g_uart_xml_raw_buffer = (char *) MemoryPool_AllocatePages(MEM_BLOCK_NO);

            // If memory allocation was successful
            if(g_uart_xml_raw_buffer)
            {
                // Clear the raw buffer to avoid leftover data from previous messages
                memset(g_uart_xml_raw_buffer, 0, sizeof(g_uart_xml_raw_buffer));

                // Read a character from USART2 and store it in the raw buffer
                g_uart_xml_raw_buffer[char_index] = (char) USART_ReceiveData(USART2);
                g_uart_xml_raw_buffer[char_index+1] = '\0'; // Null-terminate the string
                ++char_index;
            }
        }
        else if (char_index < (BLOCK_SIZE * MEM_BLOCK_NO))
        {
            //if memory was successfully allocated
            if(g_uart_xml_raw_buffer)
            {
                //checked if the input xml starts with <UCL> tag after seven characters have been received
                if(char_index == CHECK_PARENT_TAG)
                {
                    //looking for the <UCL> parant tag, if it doex no exists, then the input string is invalid
                    if(find_tag_location(g_uart_xml_raw_buffer, XML_PARENT_TAG, OPEN_TAG) == NULL)
                    {
                        //free the allocated memory
                        if(g_uart_xml_raw_buffer)
                        {
                            //free the raw buffer to release memory as it is no longer needed
                            MemoryPool_FreePages((char*) g_uart_xml_raw_buffer, MEM_BLOCK_NO);
                        }
                        
                        //reset the char_index 
                        char_index = (uint32_t) 0x00;

                        //exit from the ISR
                        return;
                    }
                }

                // Read a character from USART2 and store it in the raw buffer
                g_uart_xml_raw_buffer[char_index] = (char) USART_ReceiveData(USART2);

                // Null-terminate the string
                g_uart_xml_raw_buffer[char_index+1] = '\0'; 

                // Check if the received string contains </UCL>. it means the whole xml string is received
                if(find_tag_location(g_uart_xml_raw_buffer, XML_PARENT_TAG, CLOSE_TAG))
                {
                    // If the semaphore is unlocked, indicating the main application can process data
                    if(g_semaphore == SEMAPHORE_UNLOCKED)
                    {
                        // Acquire the semaphore to signal that data processing will begin
                        acquire_semaphore(&g_semaphore);

                        // Allocate memory for the main buffer where the data will be copied
                        g_uart_xml_main_buffer = (char *) MemoryPool_AllocatePages(MEM_BLOCK_NO);

                        // If memory allocation for the main buffer was successful
                        if(g_uart_xml_main_buffer)
                        {
                            // Clear the main buffer to avoid leftover data from previous messages
                            memset(g_uart_xml_main_buffer, 0, sizeof(g_uart_xml_main_buffer));

                            // Copy the raw buffer content (the received message) into the main buffer
                            memcpy(g_uart_xml_main_buffer, g_uart_xml_raw_buffer, strlen(g_uart_xml_raw_buffer) + 1);
                        }

                        // Free the raw buffer to release memory as it is no longer needed
                        MemoryPool_FreePages((char*) g_uart_xml_raw_buffer, MEM_BLOCK_NO);
                    }
                    // Reset the char_index and memory allocation flag to prepare for the next message
                    char_index = (uint32_t) 0x00;
                }
                else
                {
                    // Increment the char_index to store the next received character in the buffer
                    ++char_index;
                }
            }
            else
            {
                // If memory allocation failed, reset the char_index and memory flag
                char_index = (uint32_t) 0x00;
            }
        }
        else
        {
            if(g_uart_xml_raw_buffer)
            {
               // Free the raw buffer to release memory as it is no longer needed
               MemoryPool_FreePages((char*) g_uart_xml_raw_buffer, MEM_BLOCK_NO);
            }
            
            //reset the char_index and memory flag
            char_index = (uint32_t) 0x00;
            mem_alloc_status = false;
        }
    }
}
