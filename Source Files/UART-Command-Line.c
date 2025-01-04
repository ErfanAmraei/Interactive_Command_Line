/**
  ******************************************************************************
  * @file    UART-Command-Line.c
  * @author  Erfan Amraei
  * @date    10-August-2023
  * @brief   This file provides the UART and command Parsing and interepting
  *          firmware functions.
  ******************************************************************************/

#include "HAL-SYSTEM/inc/stm32f10x.h"

#include "UART_Command_Line.h"
#include <stdio.h>
#include <string.h>


/**
* @brief Callback function to process and set LED value based on command.
*
* This function checks the input pointer, logs the incoming command, 
* and processes it to set the appropriate LED value. It returns an 
* error status indicating success or failure.
*
* @param [in] *CommandContent Pointer to the incomingCommandContents structure.
*
* @retval SUCCESS if the command is successfully processed.
* @retval ERROR if the input pointer is null or processing fails.
*/
ErrorStatus SetLedValue(struct incommingCommandContents *CommandContent) 
{
    // Initialize outcome as ERROR to handle potential failures.
    ErrorStatus outcome = ERROR;
    
    // Check if the input pointer is valid; return ERROR if NULL.
    if (CommandContent == NULL) 
    {
      printf("Error: CommandContent pointer is null.\n");
    }
    else
    {
      // Log the first command from the incoming structure.
      printf("\nFirst Command: %s\n", CommandContent->Command);
      
      // Indicate that the command was received and processed.
      printf("Command received and processed.\n");
      
      // Set outcome to SUCCESS since the command was successfully logged.
      outcome = SUCCESS;
    }
    
    // Return the final outcome of the processing.
    return outcome;
}



/**********************************************************************
* @brief Callback function to retrieve the heater value based on command.
*
* This function validates the input pointer, logs the incoming command, 
* and processes it to retrieve the heater value. It returns an error 
* status indicating success or failure.
*
* @param [in] *CommandContent Pointer to the incomingCommandContents structure.
*
* @retval SUCCESS if the command is successfully processed.
* @retval ERROR if the input pointer is null or processing fails.
*/
ErrorStatus GetHeaterValue(struct incommingCommandContents *CommandContent) 
{
    // Initialize outcome as ERROR to handle failure cases.
    ErrorStatus outcome = ERROR;
    
    // Validate the input pointer to ensure it is not null.
    if (CommandContent == NULL) 
    {
        // Log an error message for null pointer.
        printf("Error: CommandContent pointer is null.\n");
    } 
    else 
    {
        // Log the second command from the incoming structure.
        printf("\nSecond Command: %s\n", CommandContent->Command);
        
        // Indicate that the command was received and processed.
        printf("Command received and processed.\n");
        
        // Update outcome to SUCCESS as processing was successful.
        outcome = SUCCESS;
    }
    
    // Return the final outcome of the processing.
    return outcome;
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
