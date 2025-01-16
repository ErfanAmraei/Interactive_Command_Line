#include "HAL/HAL-SYSTEM/inc/stm32f10x.h"
#include "Command_Line_App/inc/UART_Command_Line.h"
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

void USART2_IRQHandler(void)
{
	static uint8_t Counter = (uint8_t) 0x00;
	//check RXNE flag for new message from USART2
	if(SET == USART_GetFlagStatus(USART2, USART_FLAG_RXNE))
	{
		//receive one character for USART2
		IncommingCommandBuffer[Counter] = USART_ReceiveData(USART2);
		////every incoming message must ends by ), this a sign of receiving the whole message
		if(IncommingCommandBuffer[Counter] == PRANTHESIS_HEX_CODE)
		{
			//IncommingCommandBuffer is full and ready for being Processed
			CommandBufferStatus = Full;
			Counter             = (uint8_t) 0x00;
		}
		//otherwise increment Counter to receive another character
		else
		{
			//counter can not goes beyond MAX_USER_COMMAND_LENGTH
			if(Counter < (MAX_USER_COMMAND_LENGTH-1))
			{
				++Counter;
			}
			//otherwise reset counter. it leads to returning "invalid command" to the user
			else
			{
				CommandBufferStatus = Full;
				Counter             = (uint8_t) 0x00;
			}
		}
	}
}

