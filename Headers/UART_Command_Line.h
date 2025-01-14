#include "stm32f10x.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define XML_PARENT_TAG       (char *)"UCL"
#define XML_TAG_CMD          (char *)"CMD"
#define XML_TAG_PARAMETER    (char *)"PARAM"

#define INVALID_CALLBACK_FUNCTION  (uint8_t)0xFF
#define INVALID_OPERATION          (uint8_t)0xFE

/**
 * @brief Enum to define the indexes of the UART_Message array
*/
typedef enum {
    ERR_NULL_POINTER,     // Index 0: "Error: CommandContent pointer is null.\n"
    CMD_PROCESSED,        // Index 1: "Command received and processed.\n"
    ERR_CALLBACK,         // Index 2: "\nError happened in callback functions\n"
    ERR_INVALID_COMMAND,  // Index 3: "\nInvalid command. You are not allowed to send such commands\n"
    ERR_PARSING_COMMAND,  // Index 4: "\nError happened when parsing user command\n"
    FIRST_CMD,            // Index 5: "\nFirst Command: %s\n"
    SECOND_CMD,           // Index 6: "\nSecond Command: %s\n"
    UART_MESSAGES_COUNT   // Total number of messages (useful for iteration)
} UART_MessageIndex;

/**
/* @brief array of UART message strings used for logging and debugging purposes.
*/
const char* UART_Message[] = 
{
   "Error: CommandContent pointer is null.\n",
   "Command received and processed.\n",
   "\nError happend in callback functions\n",
   "\nInvalid command. you are not allowed to send such commands\n",
   "\nError happend when parsing user command\n",
   "\nFirst Command: %s\n",
   "\nSecond Command: %s\n",
   NULL //proper termination for an array of pointers
};

/**
 * @brief Structure to hold the result of extracting data from an XML message.
 */
struct XMLDataExtractionResult
{
   uint8_t callback_index; /*index of the callback function to handle the command. */
   char *cmd;              /*pointer to the extracted XML command as a null-terminated string.*/
   char *param;            /*pointer to the extracted XML command parameter as a null-terminated string.*/
};

/**
* @brief defining a function pointer type named CommandCallback
*
* @param *CommandContent points to incommingCommandContents structure
*
* @retval none.
*/
typedef ErrorStatus (*CommandCallback)(struct incommingCommandContents *CommandContent);


/**
* @brief CommandEntry is used to configure input commands and callback functions
*/
struct CommandEntry
{
    const char *cmd;
    CommandCallback callback;
};


/**
 * @brief CommandValidation_Handler is used to show the validity of input string
*/
typedef enum 
{
  Invalid = 0,
	Valid   = !Invalid,
}CommandValidation_Handler;

/**
 * @brief incommingCommandBufferStatus is used to show that the user string from UART is
 *        is full or empty
*/
typedef enum 
{
   Empty = 0,
   Full  = !Empty,
}incommingCommandBufferStatus;

/**
 * @enum XML_Parser_Status_t
 * @brief Defines the possible outcomes of the XML parsing function.
 */
typedef enum 
{
   XML_OK = 0,        // Indicates that the XML parsing was successful
   BAD_XML,           // Indicates that the XML string is invalid, or the expected tags were not found
   NO_CMD,            // Indicates that the incomming command has not been found in the command list
   EMPTY_PARAMS       // Indicates that one or more input parameters are null or invalid
} XML_Parser_Status_t;


/**
 * @brief Parse_CMD_Result is used as the output of the compare_Incomming_CMD_with_CMD_Library
 */
struct Parse_CMD_Result
{
   ErrorStatus error;
   CommandValidation_Handler cmdValidation;
   uint8_t callbackFunctionIndex;
};

//SetLedValue prototype
ErrorStatus SetLedValue(struct incommingCommandContents *CommandContent);
//GetHeaterValue
ErrorStatus GetHeaterValue(struct incommingCommandContents *CommandContent);

XML_Parser_Status_t extract_value_from_xml(const char *xml, 
                                           const char *tag, 
                                           char *tag_value, 
                                           size_t value_size);

uint8_t find_command_in_list(const char* cmd);

struct XMLDataExtractionResult extract_command_and_params_from_xml(const char *xml);


struct CommandEntry g_cmd_list[] = 
{
   {"LightOn", SetLedValue},
   {"GetHeater", GetHeaterValue},
   {NULL, NULL}
};

