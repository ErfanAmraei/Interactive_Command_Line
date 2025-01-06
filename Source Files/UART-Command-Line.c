/**
  ******************************************************************************
  * @file    UART-Command-Line.c
  * @author  Erfan Amraei
  * @date    10-August-2023
  * @brief   This file provides the UART and command Parsing and interepting
  *          firmware functions.
  ******************************************************************************/

#include "HAL-SYSTEM/inc/stm32f10x.h"
#include "../Headers/UART_Command_Line.h"

extern const char* UART_Message[];
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
      printf(UART_Message[(uint8_t)ERR_NULL_POINTER]);
    }
    else
    {
      // Log the first command from the incoming structure.
      printf(UART_Message[(uint8_t)FIRST_CMD], CommandContent->Command);
      
      // Indicate that the command was received and processed.
      printf(UART_Message[(uint8_t)CMD_PROCESSED]);
      
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
        printf(UART_Message[(uint8_t)ERR_NULL_POINTER]);
    } 
    else 
    {
        // Log the second command from the incoming structure.
        printf(UART_Message[(uint8_t)SECOND_CMD], CommandContent->Command);
        
        // Indicate that the command was received and processed.
        printf(UART_Message[(uint8_t)CMD_PROCESSED]);
        
        // Update outcome to SUCCESS as processing was successful.
        outcome = SUCCESS;
    }
    
    // Return the final outcome of the processing.
    return outcome;
}


/**
* @brief parseCommandWithArguments is used to tokenize the input string to extract a command and its arguments.
*
* This function splits an input string into a command and up to NO_OF_CMD_ARGUMENTS arguments using 
* a specified delimiter. The results are stored in the incommingCommandContents structure.
*
* @param *incommingCommand: Pointer to the input string received (e.g., from UART).
* @param *CommandContent: Pointer to the structure where the command and arguments will be stored.
*
* @retval ErrorStatus: Returns SUCCESS if operation is successful, or ERROR if any input pointer is NULL.
*/
ErrorStatus parseCommandWithArguments(char *incommingCommand, struct incommingCommandContents *CommandContent) {

    // Check if any of the input pointers are NULL. Return ERROR if any is invalid.
    if (incommingCommand == NULL || CommandContent == NULL) 
    {
        return ERROR;
    }

    // Tokenize the input string to extract the command (first token).
    char *token = strtok(incommingCommand, DELIMETER);
    if (token != NULL) 
    {
        // Copy the command into the Command field of the structure.
        snprintf(CommandContent->Command, sizeof(CommandContent->Command), "%s", token);
    } 
    else 
    {
        // If no command is found, clear the Command field.
        memset(CommandContent->Command, 0, sizeof(CommandContent->Command));
    }

    // Loop through to extract and store each argument.
    for (int argumentIndex = 0; argumentIndex < NO_OF_CMD_ARGUMENTS; ++argumentIndex) 
    {
        token = strtok(NULL, DELIMETER); // Continue tokenizing the string for the next argument.
        if (token != NULL) 
        {
            // Copy the current token into the appropriate Arguments field.
            snprintf(CommandContent->Arguments[argumentIndex], sizeof(CommandContent->Arguments[argumentIndex]), "%s", token);
        } 
        else 
        {
            // If no token is found, clear the corresponding Arguments field.
            memset(CommandContent->Arguments[argumentIndex], 0, sizeof(CommandContent->Arguments[argumentIndex]));
        }
    }

    // If everything was processed successfully, return SUCCESS.
    return SUCCESS;
}


/**
 * @brief compare_Incomming_CMD_with_CMD_Library
 *        Compares the input string with a list of predefined commands and extracts the command and arguments.
 *
 * @param *incommingCommand Pointer to the input string received from UART.
 *                          This string contains the command to be parsed and validated.
 *
 * @param *CommandContent   Pointer to a structure (`incommingCommandContents`) that stores the parsed command
 *                          and its arguments. The structure is modified by reference.
 *
 * @param *CommandList      Pointer to an array of `CommandEntry` structures. Each element in this array contains
 *                          a command string and a corresponding callback function. The input command is compared
 *                          against these entries to validate and identify it.
 *
 * @retval struct Parse_CMD_Result
 *         Returns a structure containing:
 *         - `callbackFunctionIndex`: Index of the matching command in `CommandList`. This index can be used to
 *                                    call the corresponding callback function.
 *         - `cmdValidation`:         Indicates whether the command is valid (`Valid`) or invalid (`Invalid`).
 *         - `error`:                 Indicates whether any of the input pointers were null (`ERROR` or `SUCCESS`).
 */

struct Parse_CMD_Result compare_Incomming_CMD_with_CMD_Library(
                                               char *incommingCommand, 
                                               struct incommingCommandContents *CommandContent,
                                               struct CommandEntry* CommandList) 
{
    uint16_t loopCounter = 0; // Counter for iterating through the command list
    char *SearchWithinSting = NULL; // Pointer to hold the result of string search
    struct Parse_CMD_Result outcome; // Result structure to store the parsing outcome
    outcome.callbackFunctionIndex = 0; // Initialize callback function index to 0
    
    // Check if any input pointers are null
    if (incommingCommand == NULL || CommandContent == NULL || CommandList == NULL)
    {
        outcome.error = ERROR; // Set error flag if any pointer is null
    }
    else
    {
        outcome.error = SUCCESS; // Set success flag if pointers are valid
        
        // Calculate the number of commands in the CommandList array
        uint16_t numberOfCommands = sizeof(CommandList) / sizeof(CommandList[0]);
        
        // Scan through the command list to find a matching command
        for (loopCounter = 0; loopCounter < numberOfCommands; ++loopCounter)
        {
            // Search for the current command in the input string
            SearchWithinSting = strstr(incommingCommand, CommandList[loopCounter].command);
            
            if (SearchWithinSting) // If a matching command is found
            {
                // Parse the input string to extract command and arguments
                outcome.error = parseCommandWithArguments(incommingCommand, CommandContent);
                
                // Set command validation status to Valid
                outcome.cmdValidation = Valid;
                
                // Store the index of the matching command's callback function
                outcome.callbackFunctionIndex = loopCounter;
                
                break; // Exit the loop since a match was found
            }
            else
            {
                // Set command validation status to Invalid if no match is found
                outcome.cmdValidation = Invalid;
            }
        }
    }
    return outcome; // Return the result structure
}

