#include "stm32f10x.h"
#include "Headers/UART_Command_Line.h"
#include "HAL-SYSTEM/inc/HAL_Common.h"
#include <stdio.h>
#include <string.h>

extern const char* UART_Message[];

//IncommingCommandBuffer is a place in memory where incomming command is saved
char IncommingCommandBuffer[MAX_USER_COMMAND_LENGTH];
//this flag shows if it is time to process incomming command or not
incommingCommandBufferStatus CommandBufferStatus = Empty;
//this structure is used for configuring commands and their corresponding callback functions
struct CommandEntry commandList[] = {
     {"set LED_Value ", SetLedValue},
     {"get heater_value ", GetHeaterValue},
      // Add more command entries
};

//ErrorHandler holds the status of error
ErrorStatus ErrorHandler = SUCCESS;
//this structure is utilized to store parsed input string content
struct incommingCommandContents CommandContent;
//this flag is used to indicate the validity of the input command
CommandValidation_Handler CommandValidation;
//callback function
CommandCallback callbackFunction;
//arguments separator
const char argumentDelimeter[] = ";"; 
//the CallBack Function index in the commandList array will be stored in callbackFunctionIndex
uint8_t callbackFunctionIndex;
uint32_t i;
int main()
{
	HAL_config_MCU();
	while(1)
	{
		//check if we have a full message
		if(Full == CommandBufferStatus)
		{
		    // Process the command, callbackFunctionIndex is holding the index of callback function corresponding to the user command
             callbackFunctionIndex = ParseCommand(IncommingCommandBuffer, &CommandContent, commandList, argumentDelimeter, &CommandValidation, &ErrorHandler);
			if(ErrorHandler == SUCCESS)
			{					
			    //CommandValidation shows that if user command is valid
			    if(Valid == CommandValidation)
                {
					   //calling callback function corresponding to the user command
                    callbackFunction = commandList[callbackFunctionIndex].callback;
                    ErrorHandler = callbackFunction(&CommandContent);
					if(ErrorHandler == ERROR)
					{
						printf(UART_Message[ERR_CALLBACK]);
					}
                }
				  //if user command is invalid then "invalid command" will be send on UART
                else
				{
                    printf(UART_Message[ERR_INVALID_COMMAND]);
                }
		    }
			else
			{
                 printf(UART_Message[ERR_PARSING_COMMAND]);
            }
			//clearing the buffer for the next command
			memset(IncommingCommandBuffer, 0 , sizeof(IncommingCommandBuffer));
			//make CommandBufferStatus empty for another user command
			CommandBufferStatus = Empty;
		}
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

