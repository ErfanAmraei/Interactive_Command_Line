#ifndef UART_ISR_H
#define UART_ISR_H

#include "../HAL-SYSTEM/inc/stm32f10x.h"
#include "../HAL-UART/inc/stm32f10x_usart.h"
#include "../../Command_Line_App/memory_utility/memory_utility.h"
#include "../../Command_Line_App/UART_command_line/UART_Command_Line.h"
#include "../../Command_Line_App/semaphore/semaphore.h"
#include <stdio.h>
#include <string.h>

bool start_new_message(uint32_t mem_blocks, uint32_t *char_index, char received_char);
bool validate_parent_tag(uint32_t mem_blocks, uint32_t *char_index);
void process_received_char(char received_char, uint32_t *char_index, uint32_t mem_blocks);
void process_complete_message(uint32_t mem_blocks);
void reset_buffer_state(uint32_t mem_blocks, uint32_t *char_index);

#endif /*UART_ISR_H*/

