#include "HAL/HAL-SYSTEM/inc/stm32f10x.h"
#include "Command_Line_App/UART_command_line/UART_Command_Line.h"
#include ""
#include "HAL/HAL-SYSTEM/inc/HAL_Common.h"
#include <stdio.h>
#include <string.h>




int main()
{
	HAL_config_MCU();
	while(1)
	{
		extract_command_and_params_from_xml();
	}
}

/*********************************************************
* @breif USART2 Interrupt Service Routine (ISR)
*
* @param none
*
* @retval none
*/



