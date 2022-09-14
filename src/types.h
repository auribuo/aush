#ifndef __AUSH_TYPES__
#define __AUSH_TYPES__

/**
 * The max length of a command that can be input 
 */
#define MAXCMD 100

/**
 * @brief Flags for internal command states
 * 
 * 
 * This flags are set after parsing a command to get the type of command to execute
 */
typedef enum AUSH_Flags
{
	/**
	 * @brief Unset
	 * 
	 * This flag is the default / unset value
	 */
	none_f,

	/**
	 * @brief Exit invoced
	 * 
	 * This flag is set when the user inputs the exit command, because this one is treated differently
	 */
	exit_f,

	/**
	 * @brief Return last exit code
	 * 
	 * 
	 * This flag is set if the user input the ret? command. Currently this command is work in progress
	 */
	retLastExit_f,

	/**
	 * @brief Change dir
	 * 
	 * This flag is set if the user input the cd command, because this one is treated differently
	 */
	cd_f,

	/**
	 * @brief Pipe
	 * 
	 * This flag is set if the user input a pipe and the shell must then act accordingly. This feature is not implemented yet
	 */
	pipe_f,

	/**
	 * @brief Default
	 * 
	 * This flag is set when the user inputs any not shell specific command hat can be normally executed via execvp
	 */
	default_f,
} AUSH_CMD_FLAG;

/**
 * @brief Pipe structure
 * 
 * This struct holds the data for a single pipe
 */
typedef struct Pipe_s
{
	/**
	 * @brief Is this a pipe
	 * 
	 * Used for checking if the input command was pared to a pipe. If this value is 1 all the others are null
	 */
	int isPipe;

	/**
	 * @brief Left hand operand of the pipe
	 * 
	 * This is a ptr to the string holding the unparsed lefthand side of the pipe
	 */
	char *lefthandPipe;

	/**
	 * @brief Right hand operand of the pipe
	 * 
	 * This is a ptr to the string holding the unparsed righthand side of the pipe
	 */
	char *righthandPipe;

	/**
	 * @brief Left hand operand of the pipe (parsed)
	 * 
	 * This is a ptr to the string array holding the parsed lefthand side of the pipe
	 */
	char **lefthandPipeParsed;

	/**
	 * @brief Right hand operand of the pipe (parsed)
	 * 
	 * This is a ptr to the string array holding the parsed righthand side of the pipe
	 */
	char **righthandPipeParsed;

	/**
	 * @brief Left hand flag
	 * 
	 * This is the flag set for the lefthand side of the pipe
	 */
	AUSH_CMD_FLAG lefthandFlag;

	/**
	 * @brief Right hand flag
	 * 
	 * This is the flag set for the righthand side of the pipe
	 */
	AUSH_CMD_FLAG righthandFlag;

	/**
	 * @brief Left hand argument count
	 * 
	 * The amount of arguments in the parsed lefthand string array
	 */
	int leftArgc;

	/**
	 * @brief Right hand argument count
	 * 
	 * The amount of arguments in the parsed righthand string array
	 */
	int rightArgc;
} pipe_t;

#endif
