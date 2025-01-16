#ifndef MEMORY_UTILITY_H
#define MEMORY_UTILITY_H

#include <stdbool.h>
#include <stdint.h>

//macro to define the maximum allowed memory size for allocation
#define MAX_MEMORY_SIZE (uint16_t) 1024

//macro to define the maximum allowed memory size for input xml buffer
#define INPUT_BUFFER_LENGTH  (uint16_t) 256 * sizeof(char)

//buffers for receiving and processing incoming XML data via UART
extern char* g_uart_xml_raw_buffer;  //temporary buffer for receiving raw UART data
extern char* g_uart_xml_main_buffer; // main buffer for processed XML data

/*************function prototypes**********************/
bool allocate_memory(void **ptr, uint16_t size);

#endif // MEMORY_UTILITY_H
