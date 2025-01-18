#include "HAL/HAL-SYSTEM/inc/stm32f10x.h"
#include "Command_Line_App/UART_command_line/UART_Command_Line.h"
#include "Command_Line_App/memory_utility/memory_utility.h"
#include "Command_Line_App/semaphore/semaphore.h"
#include "HAL/HAL-SYSTEM/inc/HAL_Common.h"
#include <stdio.h>
#include <string.h>


static const uint32_t PAGES_NEEDED_FOR_EXTRACTED_DATA = (uint32_t) 0x05;

int main(void)
{
	HAL_config_MCU();
	MemoryPool_Init();
	while(1)
	{
		// Check if the semaphore is locked (indicating that the resource is in use).
		if (g_semaphore == SEMAPHORE_LOCKED) 
		{
			// Attempt to allocate memory from the memory pool to hold the extracted data.
			g_extracted_data = (XMLDataExtractionResult *) MemoryPool_AllocatePages(PAGES_NEEDED_FOR_EXTRACTED_DATA);

			// Check if the memory allocation was successful.
			if (g_extracted_data) 
			{
				// Extract command and parameters from the XML data in the UART buffer.
				// The function returns the extracted data and assigns it to the allocated memory.
				(*g_extracted_data) = extract_command_and_params_from_xml(g_uart_xml_main_buffer);

				// Execute the relevant callback functions, passing the extracted data as input.
				execute_callback_functions(g_extracted_data);

				// Deallocate the memory that was allocated earlier to prevent memory leaks.
				MemoryPool_FreePages((char *) g_extracted_data, PAGES_NEEDED_FOR_EXTRACTED_DATA);
			}

			// Release the semaphore to indicate that the resource is now available for use.
			release_semaphore(&g_semaphore);
		}
	}
}





