/**
  ******************************************************************************
  * @file    UART-Command-Line.c
  * @author  Erfan Amraei
  * @date    10-August-2023
  * @brief   This file provides the UART and command Parsing and interepting
  *          firmware functions.
  ******************************************************************************/

#include "../../HAL/HAL-SYSTEM/inc/stm32f10x.h"
#include "UART_Command_Line.h"
#include "../memory_utility/memory_utility.h"
#include "../../HAL/HAL-UART/inc/hal_usart2_config.h"
#include <stdlib.h>
#include <stdio.h>

/**
 array of UART message strings used for logging and debugging purposes.
*/
static const char* UART_Message[] = 
{
   "Error: CommandContent pointer is null.\n",
   "Command received and processed.\n",
   "\nFirst Command: %s\n",
   "\nSecond Command: %s\n",
   NULL //proper termination for an array of pointers
};

#define XML_MSG_ARRAY_SIZE       (uint8_t) 4
#define INVALID_OPERATION_INDX   (uint8_t) 1

/*Array of strings representing various XML processing messages. 
 Each string corresponds to a specific XML parsing status, providing 
 human-readable error or status messages. The array ends with a NULL 
 pointer to indicate the end of the list.*/
static const char* XML_Proccessing_Messages[XML_MSG_ARRAY_SIZE] = 
{
    "\nNo command was found\n",  //message for when no valid command is detected in the XML
    "\nInvalid operation\n",     //message for an unsupported or invalid operation in the XML
    "\nBad XML\n",               //message for malformed or incorrect XML format
    NULL                         //sentinel value marking the end of the array
};

#define NUMBER_OF_COMMANDS  (uint8_t) 3
/*define a global array of CommandEntry structures, where each entry associates a command string 
 with a corresponding handler function. The array ends with a sentinel entry {NULL, NULL} to 
 indicate the end of the command list. 
 please note that g_cmd_list must be null terminated*/
static const struct CommandEntry g_cmd_list[NUMBER_OF_COMMANDS] = 
{
    {"LightOn", SetLedValue},   //command "LightOn" is handled by the SetLedValue function
    {"GetHeater", GetHeaterValue}, //command "GetHeater" is handled by the GetHeaterValue function
    {NULL, NULL}               //Sentinel entry marking the end of the command list
};


/**
* @brief Callback function to process and set LED value based on command.
*
* This function checks the input pointer, logs the incoming command, 
* and processes it to set the appropriate LED value. It returns an 
* error status indicating success or failure.
*
* @param [in] *CommandContent Pointer to the XMLDataExtractionResult structure.
*
* @retval SUCCESS if the command is successfully processed.
* @retval ERROR if the input pointer is null or processing fails.
*/
ErrorStatus SetLedValue(const struct XMLDataExtractionResult *CommandContent) 
{
    // Initialize outcome as ERROR to handle potential failures.
    ErrorStatus outcome = ERROR;
    
    // Check if the input pointer is valid; return ERROR if NULL.
    if (CommandContent == NULL) 
    {
        UART_WriteData(USART2, (const char*)UART_Message[ERR_NULL_POINTER]);
    }
    else
    {
      // Log the first command from the incoming structure.
      UART_WriteData(USART2, (const char*)UART_Message[FIRST_CMD]);
      UART_WriteData(USART2, (const char*)CommandContent->cmd);
      
      // Indicate that the command was received and processed.
      UART_WriteData(USART2, (const char*)UART_Message[CMD_PROCESSED]);
      
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
* @param [in] *CommandContent Pointer to the XMLDataExtractionResult structure.
*
* @retval SUCCESS if the command is successfully processed.
* @retval ERROR if the input pointer is null or processing fails.
*/
ErrorStatus GetHeaterValue(const struct XMLDataExtractionResult *CommandContent) 
{
    // Initialize outcome as ERROR to handle failure cases.
    ErrorStatus outcome = ERROR;
    
    // Validate the input pointer to ensure it is not null.
    if (CommandContent == NULL) 
    {
        // Log an error message for null pointer.
        UART_WriteData(USART2, (const char*)UART_Message[ERR_NULL_POINTER]);
    } 
    else 
    {
        // Log the second command from the incoming structure.
        UART_WriteData(USART2, (const char*)UART_Message[SECOND_CMD]);
        UART_WriteData(USART2, (const char*)CommandContent->cmd);
        
        // Indicate that the command was received and processed.
        UART_WriteData(USART2, (const char*)UART_Message[CMD_PROCESSED]);
        
        // Update outcome to SUCCESS as processing was successful.
        outcome = SUCCESS;
    }
    
    // Return the final outcome of the processing.
    return outcome;
}

/**
 * @brief Function to find the location of a tag in an XML string.
 *
 * @param xml: Pointer to the XML string
 * @param tag: Pointer to the tag name to locate
 * @param kind_of_tag: Specifies whether to find an opening tag (0) or a closing tag (1)
 * @return Pointer to the tag location in the XML string, or NULL if not found
 */
const char* find_tag_location(const char *xml, const char *tag, uint8_t kind_of_tag) 
{
	//allocated one memory block for the tag
    char *formatted_tag = NULL;
	
	//this buffer is going to hold the tag location in the xml string
	const char *tag_location;
	
    //validate input parameters
    if(!xml || !tag || kind_of_tag > CLOSE_TAG)
    {
        return NULL; // invalid input parameters
    }
    
		formatted_tag = (char *) MemoryPool_Allocate();
    
    if (!formatted_tag) 
    {
        return NULL; // Memory allocation failed
    }

    // Format the tag based on kind_of_tag (0: opening, 1: closing)
    if (kind_of_tag == OPEN_TAG) 
    {
        snprintf(formatted_tag, BLOCK_SIZE, "<%s>", tag);
    } 
    else 
    {
        snprintf(formatted_tag, BLOCK_SIZE, "</%s>", tag);
    }

    // Find the tag in the XML string
    tag_location = strstr(xml, formatted_tag);

    // Free allocated memory
    MemoryPool_Free((char *) formatted_tag);

    return tag_location; // Return the location of the tag, or NULL if not found
}

/**
 * @brief Function to extract a value from an XML string between specific tags
 *
 * @param xml: Pointer to the XML string
 * @param tag: Pointer to the tag name whose value needs to be extracted
 * @param tag_value: Pointer to a buffer where the extracted value will be stored
 * @param value_size: Size of the buffer for tag_value
 * @retval XML_Parser_Status_t: Status of the extraction (e.g., success or error)
 */
XML_Parser_Status_t extract_value_from_xml(const char *xml, const char *tag, 
                                           char *tag_value, size_t value_size) 
{
    // Initialize the return value to XML_OK (success)
    XML_Parser_Status_t outcome = XML_OK;
	
	  //pointer to the start and end point of the tag
    const char *start;
    const char *end;
	
		size_t tag_length = 0;

    // Check if input parameters are valid
    if (!xml || !tag || !tag_value || value_size == 0) 
    {
        return INVALID_OPERATION; // Invalid input parameters
    }
		
		start = find_tag_location(xml, tag, CLOSE_TAG);   // Find closing tag
		end   = find_tag_location(xml, tag, OPEN_TAG); // Find opening tag

    // Ensure both tags are found and in proper order
    if (start && end && end > start) 
    {
        start += strlen(tag) + 2; // Move the pointer past the opening tag (e.g., "<tag>")

   			tag_length = (size_t)(end - start); // Calculate the length of the value

        // Check if the extracted value fits in the provided buffer
        if (tag_length >= value_size) 
        {
            outcome = BAD_XML; // Value too large for buffer
        } 
        else 
        {
            // Copy the extracted value into the provided buffer
            strncpy(tag_value, start, tag_length);
            tag_value[tag_length] = '\0'; // Null-terminate the string
            outcome = XML_OK; // Extraction successful
        }
    } 
    else 
    {
        outcome = BAD_XML; // Tags not found or malformed XML
    }

    return outcome; // Return the outcome of the operation
}


/**
 * @brief Looks for the specified command in the global command list.
 *
 * This function searches through the global command list to find a matching
 * command. If the command is found, it returns its index; otherwise, it 
 * returns NO_COMMAND_FOUND.
 *
 * @param cmd Pointer to the command string to search for.
 * @return uint8_t Index of the found command or NO_COMMAND_FOUND if not found.
 */
uint8_t find_command_in_list(const char* cmd)
{
    bool    operation_outcome = false;  //tracks whether the command was found.
    uint8_t cmd_list_index = 0;         //index for iterating through the command list.
    
    //check if input command is valid.
    if (cmd)
    {
        //iterate through the command list until the end (null command).
        //please note that g_cmd_list i null terminated, so there is no way 
        //it overflows
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
        
        //if the command was not found, set index to NO_COMMAND_FOUND.
        if (!operation_outcome)
        {
            cmd_list_index = NO_COMMAND_FOUND;
        }
    }
    // If either the input command or command list is invalid, return NO_COMMAND_FOUND.
    else
    {
        cmd_list_index = INVALID_OPERATION;
    }
    
    return cmd_list_index;  //return the index of the command or NO_COMMAND_FOUND.
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
    
    XML_Parser_Status_t parser_status = XML_OK; //status of XML parsing operations.

    size_t memory_size = 0;    //initial memory size for tag value storage.

    memory_size = CMD_AND_PARAM_LENGTH * sizeof(char);  //adjust memory size based on character size.
    
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
            if (outcome.callback_index < NO_COMMAND_FOUND)
            {
                //extract the parameter from the `PARAMETER` tag and store it in `outcome.param`.
                parser_status = extract_value_from_xml(xml, XML_TAG_PARAMETER, outcome.param, memory_size);
                
                //if parameter extraction fails, mark the callback index as invalid.
                if (parser_status != XML_OK)
                {
                    outcome.callback_index = parser_status;
                }
            } 
        }
        //if the `CMD` tag is missing or invalid, mark the callback index as invalid.
        else
        {
            outcome.callback_index = parser_status;
        }   
    }
    //if the input XML is null, mark the operation as invalid.
    else
    {
        outcome.callback_index = INVALID_OPERATION;
    }

    return outcome; // Return the result structure containing the command, parameter, and callback index.
}

/**
 * @brief Executes the appropriate callback function or prints an error message 
 *        based on the result of XML data extraction.
 * 
 * @param commandContent Pointer to an XMLDataExtractionResult structure containing 
 *                       the extracted command and callback index.
 * 
 * This function determines whether to call a callback function from the global 
 * command list (g_cmd_list) or print an error message based on the contents 
 * of the commandContent structure.
 */
void execute_callback_functions(const struct XMLDataExtractionResult *commandContent)
{
    uint8_t index = 0; // Initialize index to store the calculated error message index or callback index
    
    // Check if the commandContent pointer is NULL
    if (!commandContent)
    {
        // Calculate the index for the "Invalid operation" error message
        index =  INVALID_OPERATION - NO_COMMAND_FOUND;
        
        //validate index
        if(index < XML_MSG_ARRAY_SIZE)
        {
           // Print the corresponding error message to the UART port
           UART_WriteData(USART2, (const char*) XML_Proccessing_Messages[index]);
        }
        //if index is not valid then return invalid operation message
        else
        {
            UART_WriteData(USART2, (const char*) XML_Proccessing_Messages[INVALID_OPERATION_INDX]);
        }
    }

    else if(commandContent->callback_index < NUMBER_OF_COMMANDS)
    {
        // Call the corresponding callback function from the global command list
        // using the callback index provided in commandContent
        g_cmd_list[commandContent->callback_index].callback(commandContent);
    }
    // Check if the callback index is greater than the maximum valid callback index
    else if (commandContent->callback_index < NO_OF_PARSER_MESSAGES)
    {
        // Calculate the index for the specific error message based on the callback index
        index = commandContent->callback_index - NO_COMMAND_FOUND;
        
        //validate index
        if(index < XML_MSG_ARRAY_SIZE)
        {
           // Print the corresponding error message to the UART port
           UART_WriteData(USART2, (const char*) XML_Proccessing_Messages[index]);
        }
        //if index is not valid then return invalid operation message
        else
        {
            UART_WriteData(USART2, (const char*) XML_Proccessing_Messages[INVALID_OPERATION_INDX]);
        }
    }
    else
    {
        // Calculate the index for the "Invalid operation" error message
        index =  INVALID_OPERATION - NO_COMMAND_FOUND;
        
        //validate index
        if(index < XML_MSG_ARRAY_SIZE)
        {
           // Print the corresponding error message to the UART port
           UART_WriteData(USART2, (const char*) XML_Proccessing_Messages[index]);
        }
        //if index is not valid then return invalid operation message
        else
        {
            UART_WriteData(USART2, (const char*) XML_Proccessing_Messages[INVALID_OPERATION_INDX]);
        }
        
    }
    
}


