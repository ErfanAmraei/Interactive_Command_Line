#ifndef UCL_H
#define UCL_H

#include "stm32f10x.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define XML_PARENT_TAG       (char *)"UCL"
#define XML_TAG_CMD          (char *)"CMD"
#define XML_TAG_PARAMETER    (char *)"PARAM"

#define CMD_AND_PARAM_LENGTH           (uint8_t) 32 //command and parameter can be maximum 32 bytes long

#define OPEN_TAG     (uint8_t) 0
#define CLOSE_TAG    (uint8_t) 1

/**
 * @brief Enum to define the indexes of the UART_Message array
*/
typedef enum 
{
    ERR_NULL_POINTER,     // Index 0: "Error: CommandContent pointer is null.\n"
    CMD_PROCESSED,        // Index 1: "Command received and processed.\n"
    FIRST_CMD,            // Index 5: "\nFirst Command: %s\n"
    SECOND_CMD,           // Index 6: "\nSecond Command: %s\n"
    UART_MESSAGES_COUNT   // Total number of messages (useful for iteration)
} UART_MessageIndex;

/**
 * @brief Structure to hold the result of extracting data from an XML message.
 */
struct XMLDataExtractionResult
{
   uint8_t callback_index;            /*index of the callback function to handle the command. */
   char cmd[CMD_AND_PARAM_LENGTH];    /*pointer to the extracted XML command as a null-terminated string.*/
   char param[CMD_AND_PARAM_LENGTH];  /*pointer to the extracted XML command parameter as a null-terminated string.*/
};

/**
* @brief defining a function pointer type named CommandCallback
*
* @param *CommandContent points to incommingCommandContents structure
*
* @retval none.
*/
typedef ErrorStatus (*CommandCallback)(struct XMLDataExtractionResult *CommandContent);

/**
* @brief CommandEntry is used to configure input commands and callback functions
*/
struct CommandEntry
{
    const char *cmd;
    CommandCallback callback;
};

/**
 * @enum XML_Parser_Status_t
 * @brief Defines the possible outcomes of the XML parsing function.
 */
typedef enum 
{
   XML_OK = 0xFA,              // Indicates that the XML parsing was successful
   NO_COMMAND_FOUND = 0xFB,    // Indicates that no valid command was found in the XML
   INVALID_OPERATION = 0xFC,   // Indicates that an invalid operation or unsupported command was encountered
   BAD_XML = 0xFD,             // Indicates that the XML string is malformed or the expected tags were missing
   NO_OF_PARSER_MESSAGES = 0xFF // Represents the total number of parser status messages; used as a limit or marker
} XML_Parser_Status_t;


/******************************function prototypes*******************************/
//SetLedValue prototype
ErrorStatus SetLedValue(const struct XMLDataExtractionResult *CommandContent);
//GetHeaterValue
ErrorStatus GetHeaterValue(const struct XMLDataExtractionResult *CommandContent);

XML_Parser_Status_t extract_value_from_xml(const char *xml, const char *tag, 
                                           char *tag_value, size_t value_size);

uint8_t find_command_in_list(const char* cmd);

struct XMLDataExtractionResult extract_command_and_params_from_xml(const char *xml);

void execute_callback_functions(const struct XMLDataExtractionResult *commandContent);

const char* find_tag_location(const char *xml, const char *tag, uint8_t kind_of_tag);

#endif //End of UCL_H
