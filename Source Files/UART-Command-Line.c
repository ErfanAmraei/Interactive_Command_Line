/**
  ******************************************************************************
  * @file    UART-Command-Line.c
  * @author  Erfan Amraei
  * @date    10-August-2023
  * @brief   This file provides the UART and command Parsing and interepting
  *          firmware functions.
  ******************************************************************************/

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "UART_Command_Line.h"
#include <stdio.h>
#include <string.h>



//printf init
struct __FILE { int handle;} ;
FILE __stdout;
FILE __stdin;
FILE __stderr;
//when ever printf is called, fputs will be executed and sends all characters through USART2
int fputc(int character, FILE *f)  
{
	while(!USART_GetFlagStatus(USART2,USART_FLAG_TXE));	 													 
	USART_SendData(USART2,character);
	return character;	
}

/***********************************************************************
* @breif USART2_Configuration is used to prepare USART2 Configuration
*
* @param none
*
* @retval none
*/
void USART2_Configuration(void)
{
	GPIO_InitTypeDef   GPIO_USART_Config;
	USART_InitTypeDef  USART2_Config;
	//Enabling USART2 Clock
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	//Enabling GPIOA Clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//USART2 Alternate Function mapping
	GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
	//configuring of GPIOA Pin_2 for Transfering Data
	GPIO_USART_Config.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_USART_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_USART_Config.GPIO_Pin   = GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_USART_Config);
	
	//configuring of GPIOA Pin_2 for Receiving Data
	GPIO_USART_Config.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_USART_Config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_USART_Config.GPIO_Pin   = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_USART_Config);
	
	//USART2 Configuration
	USART2_Config.USART_BaudRate            = USART_BAUD_RATE;
	USART2_Config.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART2_Config.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;
	USART2_Config.USART_Parity              = USART_Parity_No;
	USART2_Config.USART_StopBits            = USART_StopBits_1;
	USART2_Config.USART_WordLength          = USART_WordLength_8b;
	USART_Init(USART2, &USART2_Config);
	//enabling Interrupt
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	//This function sets the priority grouping field
	NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	//set USART2 Periority in NVIC
	NVIC_SetPriority(USART2_IRQn, USART_NVIC_PERIORITY);
	//enabling USART2 interrupt in NVIC
	NVIC_EnableIRQ(USART2_IRQn);
	//enable USART2
	USART_Cmd(USART2, ENABLE);
}

/********************************************************************
* @brief defining first callback function
*
* @param *CommandContent points to incommingCommandContents structure
*
* @retval ErrorStatus indicates if input pointer is null.
*/
ErrorStatus SetLedValue(struct incommingCommandContents *CommandContent) 
{
	  //check the input pointer
	  assert_param(CommandContent);
    
    printf("\nFirst Command: %s\n",CommandContent->Command);
    printf("received and processed\n");
    // Implement setting LED value based on CommandContent
		//
		//
		return SUCCESS;
}


/**********************************************************************
* @brief defining second callback function
*
* @param *CommandContent points to incommingCommandContents structure
*
* @retval ErrorStatus indicates if input pointer is null.
*/
ErrorStatus GetHeaterValue(struct incommingCommandContents *CommandContent) 
{
	  //check the input pointer
	  if(CommandContent == NULL)
		{
			return ERROR;
		}
    printf("\nSecond Command: %s\n",CommandContent->Command);
    printf("received and processed\n");
    // Implement getting heater value based on CommandContent
		//
		//
		return SUCCESS;
}

/*****************************************************************************************************
* @brief splitingInputString is used to Tokenize input string to extract command and arguments
*
* @param *inputString is the input string from UART
*
* @param *CommandContent points to incommingCommandContents structure. it is called by reference
*
* @param *delimiter points to the arguments separator
*
* @retval ErrorStatus indicates if any of the input pointer is null.
*/
ErrorStatus splitingInputString(char *incommingCommand, struct incommingCommandContents *CommandContent, const char *delimiter)
{
	  //check the input pointers if any of them is null
	  if(incommingCommand == NULL || CommandContent == NULL || delimiter == NULL)
		{
			return ERROR;
		}
    // Initialize strtok with the input string and delimiter
    char *token = strtok(incommingCommand, delimiter);
    if(NULL != token)
    {
        sprintf(CommandContent->Command, "%s", token);
    }
    else
    {
			 memset(CommandContent->Command, 0, sizeof(CommandContent->Command));
    }

    token = strtok(NULL, delimiter); // Passing NULL continues splitting the same string
    if(NULL != token)
    {
        sprintf(CommandContent->FirstArgument, "%s", token);
    }
    else
    {
			  memset(CommandContent->FirstArgument, 0, sizeof(CommandContent->Command));
    }

    token = strtok(NULL, delimiter); // Passing NULL continues splitting the same string
    if(NULL != token)
    {
        sprintf(CommandContent->SecondArgument, "%s", token);
    }
    else
    {
       memset(CommandContent->SecondArgument, 0, sizeof(CommandContent->SecondArgument));
    }

    token = strtok(NULL, delimiter); // Passing NULL continues splitting the same string
    if(NULL != token)
    {
        sprintf(CommandContent->ThirdArgument, "%s", token);
    }
    else
    {
       memset(CommandContent->ThirdArgument, 0, sizeof(CommandContent->ThirdArgument));
    }

    token = strtok(NULL, delimiter); // Passing NULL continues splitting the same string
    if(NULL != token)
    {
        sprintf(CommandContent->FourthArgument, "%s", token);
    }
    else
    {
       memset(CommandContent->FourthArgument, 0, sizeof(CommandContent->FourthArgument));
    }
		
    token = strtok(NULL, delimiter); // Passing NULL continues splitting the same string
    if(NULL != token)
    {
        sprintf(CommandContent->FifthArgument, "%s", token);
    }
    else
    {
       memset(CommandContent->FifthArgument, 0, sizeof(CommandContent->FifthArgument));
    }
		return SUCCESS;
}

/*****************************************************************************************************
* @brief ParseCommand is used to parse input string and to extract commands and arguments
*
* @param *inputString is the input string from UART
*
* @param CommandList is a structure of commands and callback functions. input string is compared to 
*        CommandList[loopCounter].command to determine if it is a valid command
*
* @param *delimiter points to the arguments separator
*
* @param *ValidationHandler points to ahandler which is used for showing the user command validity.
*         it is called by reference.
*
* @param *ErrorHandler indicates if any of the input pointer is null. it is called by reference.
*
* @retval the index of callback function in CommandList[]. it is used for calling the corresponding
*         callback function
*/
uint8_t ParseCommand(char *incommingCommand, struct incommingCommandContents *CommandContent,
	                   struct CommandEntry* CommandList, const char *delimiter,
										 CommandValidation_Handler *ValidationHandler,ErrorStatus *ErrorHandler) 
{
    uint8_t loopCounter;
	  uint8_t CorespondingCallbackIndex;
	  char *SearchWithinSting;
    //this enum is used for determine whether the input string is valid or not
    *ValidationHandler = Invalid;
	  //check the input pointers
	  if((*incommingCommand) == NULL ||  CommandContent == NULL || CommandList == NULL || delimiter == NULL)
		{
			*ErrorHandler = ERROR;
		}
		else
		{
			*ErrorHandler = SUCCESS;
      //scaning input string for a valid command
      for (loopCounter = 0; loopCounter < NUMBER_OF_DIFFERENT_COMMANDS; ++loopCounter)
      {
			    SearchWithinSting = strstr(incommingCommand, CommandList[loopCounter].command);
          if(SearchWithinSting)
          {
            // Tokenize input to extract command and arguments
            *ErrorHandler = splitingInputString(incommingCommand, CommandContent, delimiter);
            //valid string was obsereved
            *ValidationHandler = Valid;
					  //the value of loopCounter is the index of the call back function coresponding to the user command
					  CorespondingCallbackIndex = loopCounter;
            break;
          }
				  else
				  {
					  *ValidationHandler = Invalid;
				  }
       }
        // return Index
		}
    return CorespondingCallbackIndex;
}
