
#include "UART_isr.h"

/**
 * @brief Initialize a new message by allocating memory for the raw buffer.
 *
 * @param mem_blocks Number of memory blocks to allocate
 * @param char_index Pointer to the character index
 * @param received_char First character of the new message
 *
 * @retval bool True if the ISR should exit, False to continue processing
 */
bool start_new_message(uint32_t mem_blocks, uint32_t *char_index, char received_char)
{
    bool exit_isr = false;  // Flag to track if ISR should exit early

    // Validate parameters
    if (char_index == NULL || mem_blocks == 0)
    {
        exit_isr = true; // Exit ISR due to invalid input
    }
    else
    {
        // Attempt to allocate memory for the raw UART buffer
        g_uart_xml_raw_buffer = (char *)MemoryPool_AllocatePages(mem_blocks);

        if (g_uart_xml_raw_buffer)
        {
            // Clear the allocated buffer
            memset(g_uart_xml_raw_buffer, 0, sizeof(g_uart_xml_raw_buffer));

            // Store the first received character in the buffer
            g_uart_xml_raw_buffer[(*char_index)++] = received_char;

            // Null-terminate the string
            g_uart_xml_raw_buffer[*char_index] = '\0';
        }
        else
        {
            // Memory allocation failed; reset state
            *char_index = 0;
            exit_isr = true; // Exit ISR due to memory allocation failure
        }
    }

    return exit_isr;
}


/**
 * @brief Validate the presence of the parent tag in the received XML string.
 *
 * @param mem_blocks Number of memory blocks allocated
 * @param char_index Pointer to the character index
 *
 * @retval bool True if the ISR should exit, False to continue processing
 */
bool validate_parent_tag(uint32_t mem_blocks, uint32_t *char_index)
{
    bool exit_isr = false;  // Flag to track if ISR should exit early

    // Validate parameters
    if (char_index == NULL || mem_blocks == 0)
    {
        exit_isr = true; // Exit ISR due to invalid input
    }
    else
    {
        // Check if the opening <UCL> tag exists in the buffer
        if (find_tag_location(g_uart_xml_raw_buffer, XML_PARENT_TAG, OPEN_TAG) == NULL)
        {
            // Free memory if the tag is not found
            if (g_uart_xml_raw_buffer)
            {
                MemoryPool_FreePages((char *)g_uart_xml_raw_buffer, mem_blocks);
            }

            // Reset the character index
            *char_index = 0;
            exit_isr = true; // Exit ISR due to invalid parent tag
        }
    }

    return exit_isr;
}

/**
 * @brief Process each received character and check for message completeness.
 *
 * @param received_char The character received from UART
 * @param char_index Pointer to the character index
 * @param mem_blocks Number of memory blocks allocated
 *
 * @retval None
 */
void process_received_char(char received_char, uint32_t *char_index, uint32_t mem_blocks)
{
    bool exit_isr = false;  // Flag to track if ISR should exit early

    // Validate parameters
    if (char_index == NULL || mem_blocks == 0)
    {
        exit_isr = true; // Exit ISR due to invalid input
    }
    else
    {
        // Store the received character in the buffer
        g_uart_xml_raw_buffer[(*char_index)++] = received_char;

        // Null-terminate the string
        g_uart_xml_raw_buffer[*char_index] = '\0';

        // Check if the received string contains the closing </UCL> tag
        if (find_tag_location(g_uart_xml_raw_buffer, XML_PARENT_TAG, CLOSE_TAG))
        {
            // Ensure the semaphore is unlocked before processing
            if (g_semaphore == SEMAPHORE_UNLOCKED)
            {
                // Lock the semaphore to signal data processing
                acquire_semaphore(&g_semaphore);

                // Process the complete message
                process_complete_message(mem_blocks);
            }

            // Reset the character index
            *char_index = 0;
        }
    }

    // Exit ISR if necessary
    if (exit_isr)
    {
        return;
    }
}


/**
 * @brief Process a complete XML message and prepare it for the main application.
 *
 * @param mem_blocks Number of memory blocks allocated
 *
 * @retval None
 */
void process_complete_message(uint32_t mem_blocks)
{
    bool exit_isr = false;  // Flag to track if ISR should exit early

    // Validate parameters
    if (mem_blocks == 0)
    {
        exit_isr = true; // Exit ISR due to invalid memory block size
    }
    else
    {
        // Allocate memory for the main buffer
        g_uart_xml_main_buffer = (char *)MemoryPool_AllocatePages(mem_blocks);

        if (g_uart_xml_main_buffer)
        {
            // Clear the main buffer
            memset(g_uart_xml_main_buffer, 0, sizeof(g_uart_xml_main_buffer));

            // Copy the received data to the main buffer
            memcpy(g_uart_xml_main_buffer, g_uart_xml_raw_buffer, strlen(g_uart_xml_raw_buffer) + 1);
        }

        // Free the raw buffer as it is no longer needed
        MemoryPool_FreePages((char *)g_uart_xml_raw_buffer, mem_blocks);
    }

    // Exit ISR if necessary
    if (exit_isr)
    {
        return;
    }
}

/**
 * @brief Reset the state of the raw buffer and free allocated memory.
 *
 * @param mem_blocks Number of memory blocks allocated
 * @param char_index Pointer to the character index
 *
 * @retval None
 */
void reset_buffer_state(uint32_t mem_blocks, uint32_t *char_index)
{
    bool exit_isr = false;  // Flag to track if ISR should exit early

    // Validate parameters
    if (char_index == NULL || mem_blocks == 0)
    {
        exit_isr = true; // Exit ISR due to invalid input
    }
    else
    {
        // Free the raw buffer if it exists
        if (g_uart_xml_raw_buffer)
        {
            MemoryPool_FreePages((char *)g_uart_xml_raw_buffer, mem_blocks);
        }

        // Reset the character index
        *char_index = 0;
    }

    // Exit ISR if necessary
    if (exit_isr)
    {
        return;
    }
}


/**
 * @brief USART2 Interrupt Service Routine (ISR)
 *
 * Handles UART data reception, allocates memory for incoming messages, processes XML, 
 * and manages memory to avoid fragmentation.
 *
 * @param None
 * @retval None
 */
void USART2_IRQHandler(void)
{
    static uint32_t char_index = 0;                // Tracks the current position in the received buffer
    const uint32_t MEM_BLOCK_NO = 8;              // Number of memory blocks for the raw buffer
    const uint32_t CHECK_PARENT_TAG = 7;          // Position to validate the XML parent tag

    // Check if the RXNE (Receive Data Register Not Empty) flag is set
    if (SET == USART_GetFlagStatus(USART2, USART_FLAG_RXNE))
    {
        char received_char = (char)USART_ReceiveData(USART2); // Read received character

        // Start of a new message
        if (char_index == 0)
        {
            // Attempt to initialize a new message
            if (start_new_message(MEM_BLOCK_NO, &char_index, received_char))
                return; // Exit ISR if initialization failed
        }
        else if (char_index < (BLOCK_SIZE * MEM_BLOCK_NO))
        {
            // If a valid buffer exists
            if (g_uart_xml_raw_buffer)
            {
                // Validate parent tag after receiving sufficient characters
                if (char_index == CHECK_PARENT_TAG)
                {
                    if (validate_parent_tag(MEM_BLOCK_NO, &char_index))
                    {
                        return; // Exit ISR if validation fails
                    }
                }

                // Process the current received character
                process_received_char(received_char, &char_index, MEM_BLOCK_NO);
            }
            else
            {
                // Reset the state if no buffer is allocated
                reset_buffer_state(MEM_BLOCK_NO, &char_index);
            }
        }
        else
        {
            // Reset state if buffer limit is exceeded
            reset_buffer_state(MEM_BLOCK_NO, &char_index);
        }
    }
}

