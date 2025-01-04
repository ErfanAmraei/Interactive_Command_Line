#include "stm32f10x.h"
#include <stdlib.h>

//specifies the maximum length of command string
#define COMMAND_LENGTH                 (uint8_t) 30
//specifies the maximum length of every individual argument
#define ARGUMENT_LENGTH                (uint8_t) 3
//specifies the maximum number of commands that user can send
#define NUMBER_OF_DIFFERENT_COMMANDS   (uint8_t) 2
//specifies the maximum length of user string buffer	
#define MAX_USER_COMMAND_LENGTH        (uint8_t) 40 
//hex code of ")"
#define PRANTHESIS_HEX_CODE            (uint8_t) 0x29
	
//incommingCommandContents is used to save different parts of the input string
struct incommingCommandContents
{
   char Command[COMMAND_LENGTH];
   char FirstArgument[ARGUMENT_LENGTH];
   char SecondArgument[ARGUMENT_LENGTH];
   char ThirdArgument[ARGUMENT_LENGTH];
   char FourthArgument[ARGUMENT_LENGTH];
   char FifthArgument[ARGUMENT_LENGTH];
};


/******************************************************************************
* @brief defining a function pointer type named CommandCallback
*
* @param *CommandContent points to incommingCommandContents structure
*
* @retval none.
*/
typedef ErrorStatus (*CommandCallback)(struct incommingCommandContents *CommandContent);


/********************************************************************************
* @brief CommandEntry is used to configure input commands and callback functions
*/
struct CommandEntry
{
    const char command[COMMAND_LENGTH];
    CommandCallback callback;
};


/********************************************************************************
 * @brief CommandValidation_Handler is used to show the validity of input string
*/
typedef enum 
{
  Invalid = 0,
	Valid   = !Invalid,
}CommandValidation_Handler;

/********************************************************************************
 * @brief incommingCommandBufferStatus is used to show that the user string from UART is
 *        is full or empty
*/
typedef enum 
{
   Empty = 0,
   Full  = !Empty,
}incommingCommandBufferStatus;

//SetLedValue prototype
ErrorStatus SetLedValue(struct incommingCommandContents *CommandContent);
//GetHeaterValue
ErrorStatus GetHeaterValue(struct incommingCommandContents *CommandContent);
//splitingInputString prototype
ErrorStatus splitingInputString(char *incommingCommand, struct incommingCommandContents *CommandContent, const char *delimiter);
//ParseCommand prototype
uint8_t ParseCommand(char *incommingCommand, struct incommingCommandContents *CommandContent,struct CommandEntry* CommandList, const char *delimiter, CommandValidation_Handler *ValidationHandler, ErrorStatus *ErrorHandler);

