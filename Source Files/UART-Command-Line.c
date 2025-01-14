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
#include <stdlib.h>
#include <stdio.h>

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



/**
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
 * @brief Function to extract a value from an XML string between specific tags
 * 
 * @param xml: Pointer to the XML string
 * @param tag: Pointer to the tag name whose value needs to be extracted
 * @param tag_value: Pointer to a buffer where the extracted value will be stored
 * @param value_size: Size of the buffer for tag_value
 * 
 * @retval XML_Parser_Status_t: Status of the extraction (e.g., success or error)
 */
XML_Parser_Status_t extract_value_from_xml(const char *xml, 
                                           const char *tag, 
                                           char *tag_value, 
                                           size_t value_size) 
{
    // Initialize the return value to XML_OK (success)
    XML_Parser_Status_t outcome = XML_OK;

    // Pointers to store the opening and closing tags
    char *open_tag = NULL;
    char *close_tag = NULL;
    // Pointers to mark the start and end of the tag value in the XML string
    char *start = NULL;
    char *end = NULL;
    // Variable to store the length of the extracted tag value
    size_t tag_length = 0;

    // Memory allocation size for open_tag and close_tag
    uint8_t memory_size = 32;
    
    // Check if input parameters are valid
    if (!xml || !tag || !tag_value || value_size == 0) 
    {
        outcome = EMPTY_PARAMS; // Set outcome to error status for empty parameters
    } 
    else 
    {
        // Allocate memory for opening and closing tags
        open_tag = (char *)malloc(memory_size);
        close_tag = (char *)malloc(memory_size);

        // Check if memory allocation was successful
        if (open_tag && close_tag) 
        {
            // Format the opening and closing tags
            snprintf(open_tag, memory_size, "<%s>", tag);
            snprintf(close_tag, memory_size, "</%s>", tag);

            // Find the opening tag in the XML string
            start = strstr(xml, open_tag);
            // Find the closing tag in the XML string
            end = strstr(start, close_tag);
            
            // Ensure both tags are found
            if (start && end) 
            {
                start += strlen(open_tag); // Move the pointer to the start of the value
                tag_length = end - start;  // Calculate the length of the value

                // Check if the extracted value fits in the provided buffer
                if (tag_length >= value_size) 
                {
                    outcome = BAD_XML; // Set outcome to error status if value is too large
                } 
                else 
                {
                    // Copy the extracted value into the provided buffer
                    strncpy(tag_value, start, tag_length);
                    tag_value[tag_length] = '\0'; // Null-terminate the string
                    outcome = XML_OK; // Set outcome to success
                }
            } 
            else 
            {
                outcome = BAD_XML; // Set outcome to error if tags are not found
            }
            // Free the allocated memory for tags
            free(open_tag);
            free(close_tag);
        } 
        else 
        {
            outcome = BAD_XML; // Set outcome to error if memory allocation fails
        }
    }
    // Return the outcome of the operation
    return outcome;
}

/**
 * @brief Looks for the specified command in the global command list.
 *
 * This function searches through the global command list to find a matching
 * command. If the command is found, it returns its index; otherwise, it 
 * returns INVALID_CALLBACK_FUNCTION.
 *
 * @param cmd Pointer to the command string to search for.
 * @return uint8_t Index of the found command or INVALID_CALLBACK_FUNCTION if not found.
 */
uint8_t find_command_in_list(const char* cmd)
{
    bool    operation_outcome = false;  //tracks whether the command was found.
    uint8_t cmd_list_index = 0;         //index for iterating through the command list.
    
    //check if both input command and global command list are valid.
    if (cmd && g_cmd_list)
    {
        //iterate through the command list until the end (null command).
        while (g_cmd_list[cmd_list_index].cmd)
        {
            //compare the current command in the list with the input command.
            if (strcmp(g_cmd_list[cmd_list_index].cmd, cmd) == 0)
            {
                operation_outcome = true;  //mark that the command is found.
                break;                     //exit the loop as the command is found.
            }
            ++cmd_list_index;  //move to the next command in the list.
        }
        
        //if the command was not found, set index to INVALID_CALLBACK_FUNCTION.
        if (!operation_outcome)
        {
            cmd_list_index = INVALID_CALLBACK_FUNCTION;
        }
    }
    // If either the input command or command list is invalid, return INVALID_CALLBACK_FUNCTION.
    else
    {
        cmd_list_index = INVALID_CALLBACK_FUNCTION;
    }
    
    return cmd_list_index;  //return the index of the command or INVALID_CALLBACK_FUNCTION.
}

/**
 * @brief Extracts a command and its parameter from an input XML string.
 *
 * This function processes an XML string to extract the value of the `CMD` tag 
 * and its associated parameter. It returns a structure containing the command, 
 * its parameter, and the index of the corresponding callback function.
 *
 * @param xml Pointer to the input XML string.
 * @return struct XMLDataExtractionResult A structure containing:
 *         - `cmd`: The extracted command.
 *         - `param`: The extracted parameter.
 *         - `callback_index`: Index of the callback function, or an error code if unsuccessful.
 */
struct XMLDataExtractionResult extract_command_and_params_from_xml(const char *xml)
{
    struct XMLDataExtractionResult outcome; //stores the results of XML parsing.
    
    uint8_t cmd_index = 0;                  //index to track commands in the list.
    XML_Parser_Status_t parser_status = XML_OK; //status of XML parsing operations.

    char* tag_value = NULL;     //pointer to temporarily store extracted tag values.
    size_t memory_size = 32;    //initial memory size for tag value storage.

    memory_size *= sizeof(char);  //adjust memory size based on character size.
    
    //check if the input XML string is valid.
    if (xml)
    {
        //extract the command from the `CMD` tag in the XML string.
        parser_status = extract_value_from_xml(xml, XML_TAG_CMD, outcome.cmd, memory_size);

        //check if the command was successfully extracted.
        if (parser_status == XML_OK)
        {
            //search for the extracted command in the command list and get its callback index.
            outcome.callback_index = find_command_in_list(outcome.cmd);
            
            //validate that the callback index is valid.
            if (outcome.callback_index < INVALID_OPERATION)
            {
                //extract the parameter from the `PARAMETER` tag and store it in `outcome.param`.
                parser_status = extract_value_from_xml(xml, XML_TAG_PARAMETER, outcome.param, memory_size);
                
                //if parameter extraction fails, mark the callback index as invalid.
                if (parser_status != XML_OK)
                {
                    outcome.callback_index = INVALID_CALLBACK_FUNCTION;
                }
            }  
        }
        //if the `CMD` tag is missing or invalid, mark the callback index as invalid.
        else
        {
            outcome.callback_index = INVALID_CALLBACK_FUNCTION;
        }   
    }
    //if the input XML is null, mark the operation as invalid.
    else
    {
        outcome.callback_index = INVALID_OPERATION;
    }

    return outcome; // Return the result structure containing the command, parameter, and callback index.
}

