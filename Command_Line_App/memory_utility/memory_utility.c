#include "memory_utility.h"
#include <stdlib.h>

//buffers for receiving and processing incoming XML data via UART
char* g_uart_xml_raw_buffer = NULL;  //temporary buffer for receiving raw UART data
char* g_uart_xml_main_buffer = NULL; // main buffer for processed XML data

/**
 * @brief Allocates memory for a pointer.
 * 
 * 
 * @param ptr Pointer to the memory pointer to be allocated (must not be NULL).
 * @param size The size of memory to allocate in bytes (must be greater than 0 and less than MAX_MEMORY_SIZE).
 * @return true if the memory allocation is successful, false otherwise.
 */
bool allocate_memory(void **ptr, uint16_t size) 
{

    bool outcome = false;

    //validate input parameters
    if (size == 0 || size > MAX_MEMORY_SIZE) 
    {
        outcome = false; //invalid size
    }
    else
    {
        //allocate memory
        *ptr = malloc(size);

        //check if memory allocation was successful
        if (*ptr == NULL) 
        {
            outcome =  false; //memory allocation failed
        }
        else
        {
            outcome = true;//memory successfully allocated
        }
    }

    return outcome; 
}
