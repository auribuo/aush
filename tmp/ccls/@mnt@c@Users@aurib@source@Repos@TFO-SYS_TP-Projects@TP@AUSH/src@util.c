#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <pwd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "util.h"
#include "types.h"

#define USE_READLINE
#define USE_CUSTOMPROMPT

static char wd[FILENAME_MAX];
static const char *aushCMD[] = {"exit", "ret?", "cd"};
static const char *sepArgs = " ";
static const char *sepPipe = "|";
static const char *sepDir = "/";

int createTokens(char *, char **, int len);
pipe_t createTokensPipe(char *, char *, char *, char **, char **);
int isPipe(char *);
int startswith(char *, char *);
int endswith(char *, char *);
void removeWhitespaces(char *);
void replace(char *, char, char);

/**
 * @brief Create Tokens
 * 
 * Takes an input string and parses tokens by separating the input strings at each whitespace character
 * 
 * @param input The input string to create the tokens from
 * @param out The array to store the parsed arguments 
 * @param len The amount of arguments in the input string
 * @return int The amount of tokens created
 */
int createTokens(char *input, char *out[], int len)
{
	int i = 1;
	if (len == 1)
	{
		out[0] = input;
	}
	else
	{
		char *inpcpy = strdup(input);

		char *arg;

		for (int i = 0; i < len && (arg = strsep(&inpcpy, sepArgs)) != NULL; i++)
			out[i] = arg;
	}
	return i;
}

/**
 * @brief Creates Tokens for a pipe
 * 
 * Takes in an input and creates a pipe struct and assignes both left and righthand strings. The single pipe instructions are not parsed
 * 
 * @param input The input string
 * @param lhand The lefthand init string for the pipe struct
 * @param rhand The righthand init string for the pipe struct
 * @param lhandParsed The lefthand init string array for the pipe struct
 * @param rhandParsed The righthand init string array for the pipe struct
 * @return pipe_t Returns an initialised pipe struct with the parsed string arrays
 */
pipe_t createTokensPipe(char *input, char *lhand, char *rhand, char **lhandParsed, char **rhandParsed)
{
	pipe_t res = {1, lhand, rhand, lhandParsed, rhandParsed, none_f, none_f, 0, 0};

	if (!isPipe(input))
	{
		res.isPipe = 0;
		return res;
	}
	else
	{
		char *inpcpy = strdup(input);

		char *arg;

		arg = strsep(&inpcpy, sepPipe);
		if (arg != NULL)
		{
			res.lefthandPipe = arg;
		}
		arg = strsep(&inpcpy, sepPipe);
		if (arg != NULL)
		{
			res.righthandPipe = arg;
		}
		return res;
	}
}

/**
 * @brief Is input a pipe
 * 
 * Checks wether the given string is a pipe by checking if the | char is contained
 * 
 * @param string The string to check
 * @return int 1 if the string is a pipe else 0
 */
int isPipe(char *string)
{
	return countChar(string, '|') == 1 ? 1 : 0;
}

/**
 * @brief Counts chr in input
 * 
 * Counts the occurences of the char chr in the string input. The search is case sensitive
 * 
 * @param input The input string
 * @param chr The char to search
 * @return int The amount of times chr occures in input
 */
int countChar(char *input, char chr)
{
	int len = strlen(input);
	int res = 0;

	for (int i = 0; i < len; i++)
	{
		if (input[i] == chr)
		{
			res++;
		}
	}

	return res;
}

/**
 * @brief Parses the input
 * 
 * Takes an input string and parses the string returning a full pipe struct if the input was a pipe else it writes the args into the out array.
 * The pipe functionality is not yet implemented and will lead to a crash
 * 
 * @param cmd The input to parse
 * @param out The array of args to write to if the input isn't a pipe
 * @param argCount The amount of arguments in the input
 * @param flag The pointer to the flag to set according to the parsing
 * @return pipe_t Returns a filled or empty pipe struct
 */
pipe_t parseInput(char *cmd, char *out[], int argCount, AUSH_CMD_FLAG *flag)
{
	pipe_t res;
	if (cmd == NULL || strcmp(cmd, "\n") == 0)
	{
		*flag = none_f;
	}
	else
	{
		int pipeCmdCount = MAXCMD / 2;

		char lhand[pipeCmdCount];
		char rhand[pipeCmdCount];
		char *lhandParsed[pipeCmdCount];
		char *rhandParsed[pipeCmdCount];

		res = createTokensPipe(cmd, lhand, rhand, lhandParsed, rhandParsed);

		res.leftArgc = countChar(res.lefthandPipe, ' ') + 1;
		res.rightArgc = countChar(res.righthandPipe, ' ') + 1;

		removeWhitespaces(res.lefthandPipe);
		removeWhitespaces(res.righthandPipe);

		if (!res.isPipe)
		{
			int opDone = 0;
			createTokens(cmd, out, argCount);
			for (int i = 0; i < AUSH_COUNT && !opDone; i++)
			{
				if (strcmp(out[0], aushCMD[i]) == 0)
				{
					opDone = 1;
					setFlag(out[0], flag);
				}
			}
			if (!opDone)
			{
				*flag = default_f;
			}
		}
		else
		{
			fprintf(stderr, "Not yet implemented!\n");
			*flag = none_f;
			return res;
			createTokens(res.lefthandPipe, res.lefthandPipeParsed, res.leftArgc);
			createTokens(res.righthandPipe, res.righthandPipeParsed, res.rightArgc);

			int leftParsed = 0;
			int rightParsed = 0;

			for (int i = 0; i < AUSH_COUNT && !leftParsed; i++)

			{
				if (strcmp(res.lefthandPipeParsed[0], aushCMD[i]) == 0)
				{
					leftParsed = 1;
					setFlag(res.lefthandPipeParsed[0], &res.lefthandFlag);
				}
			}
			if (!leftParsed)
			{
				res.lefthandFlag = default_f;
			}

			for (int i = 0; i < AUSH_COUNT && !rightParsed; i++)
			{
				if (strcmp(res.righthandPipeParsed[0], aushCMD[i]) == 0)
				{
					rightParsed = 1;
					setFlag(res.righthandPipeParsed[0], &res.righthandFlag);
				}
			}
			if (!rightParsed)
			{
				res.righthandFlag = default_f;
			}

			res.lefthandPipeParsed[res.leftArgc + 1] = (char *)0;
			res.righthandPipeParsed[res.rightArgc + 1] = (char *)0;

			*flag = pipe_f;
		}
		return res;
	}
}

/**
 * @brief Is chr in input
 * 
 * @param input The string to check
 * @param chr The char to check for
 * @return int 1 if the string contains chr else 0
 */
int contains(char *input, char chr)
{
	int res = countChar(input, chr);
	return res > 0 ? 1 : 0;
}

/**
 * @brief Set the Flag
 * 
 * Sets the flag according to the input. Only sets flags for shell specific commands
 * 
 * @param cmd The command to check
 * @param flag The pointer to the flag to set
 */
void setFlag(char *cmd, AUSH_CMD_FLAG *flag)
{
	if (strcmp(cmd, "exit") == 0)
	{
		*flag = exit_f;
	}
	else if (strcmp(cmd, "ret?") == 0)
	{
		*flag = retLastExit_f;
	}
	else if (strcmp(cmd, "cd") == 0)
	{
		*flag = cd_f;
	}
	else
	{
		*flag = none_f;
	}
}

/**
 * @brief Executes a command
 * 
 * Executes the command with the passed arguments and sets the return value. The args array has to be null terminated
 * 
 * @param cmd The command to execute
 * @param args The argument list. The first is the same as cmd and the last has to be NULL
 * @param lastRet The pointer to the integer holding the last exit code
 */
int exe(char *cmd, char *args[])
{
	if (strcmp(cmd, "cd") == 0)
	{
		int chDirRes = chdir(args[1]);
		if (chDirRes != 0)
		{
			fprintf(stderr, "%s%s\n", "cd: no such file or directory: ", args[1]);
			return 1;
		}
		return 0;
	}
	else
	{
		return execvp(cmd, args);
	}
}

/**
 * @brief Gets the User Input
 * 
 * Promps for the input using readline() from GNU libreadline and copies the input into out
 * 
 * @param out The pointer to copy the input to
 * @return int 1 if the input was taken correctly else 0
 */
int getUserInput(char *out)
{
	int ret = 0;
	char *prompt = getPrompt();

#ifdef USE_READLINE
#ifdef USE_CUSTOMPROMPT
	char *buf = readline(prompt);
#else
	char *buf = readline(">>> ");
#endif
#endif
#ifndef USE_READLINE
	printf("%s", prompt);
	char buf[MAXCMD];
	char *_ = fgets(buf, MAXCMD, stdin);
	replace(buf, '\n', '\0');
#endif

	if (strlen(buf) != 0)
	{
		add_history(buf);
		strcpy(out, buf);
		ret = 0;
	}
	else
	{
		ret = 1;
	}
	free(prompt);

#ifdef USE_READLINE
	free(buf);
#endif

	return ret;
}

/**
 * @brief Gets the Prompt
 * 
 * Creates the shell prompt and returns the pointer to it. The string is allocated with malloc() so you want to free() it after
 * 
 * @return char* The shell prompt
 */
char *getPrompt()
{
	char *wdRes = getcwd(wd, FILENAME_MAX);
	char *wdCpy = strdup(wd);
	char wdShort[MAXCMD];
	int len = countChar(wd, '/');
	if (len > 4)
	{
		char *dirs[len];
		char *dir;
		for (int i = 0; i < len && (dir = strsep(&wdCpy, sepDir)) != NULL; i++)
			dirs[i] = dir;

		char *promptDirs[5];
		promptDirs[0] = "..";
		for (int i = 4, j = len - 1; i >= 1 && j >= 0; i--, j--)
			promptDirs[i] = dirs[j];

		int totLen = 0;
		for (int i = 0; i < 5; i++)
			totLen += strlen(promptDirs[i]);

		sprintf(wdShort, "%s/%s/%s/%s/%s", promptDirs[0], promptDirs[1], promptDirs[2], promptDirs[3], promptDirs[4]);
	}

	struct passwd *usrAcc = getpwuid(getuid());
	char hostname[HOST_NAME_MAX + 1];
	gethostname(hostname, sizeof(hostname));

	int colLen = strlen(CLR_RESET);
	int promptLen = colLen * 2 + colLen * 2 + strlen(usrAcc->pw_name) + strlen(hostname) + 1 + colLen * 2 + strlen(":") + 1 + colLen * 2 + strlen(len > 4 ? wdShort : wd) + 1 + colLen * 2 + strlen(CMD_PROMPT) + 3;

	char *res = malloc((promptLen + 10) * sizeof(char));

	sprintf(res, "%s%s%s@%s%s%s%s%s%s%s%s%s%s%s%s %s %s", STY_BOLD, CLR_RED, usrAcc->pw_name, hostname, CLR_RESET, STY_NBOLD, CLR_GREEN, ":", CLR_RESET, STY_BOLD, CLR_CYAN, len > 4 ? wdShort : wd, CLR_RESET, STY_NBOLD, CLR_GREEN, CMD_PROMPT, CLR_RESET);
	return res;
}

/**
 * @brief Does str start with seq
 * 
 * Checks wether the string str starts exactly with the string seq
 * 
 * @param str The string to check
 * @param seq The starting char sequence to compare to
 * @return int 1 if str starts with seq else 0
 */
int startswith(char *str, char *seq)
{
	if (strlen(seq) > strlen(str))
	{
		return 0;
	}
	if (strlen(seq) == strlen(str) && strcmp(seq, str) == 0)
	{
		return 1;
	}

	for (int i = 0; i < strlen(seq); i++)
	{
		if (str[i] != seq[i])
		{
			return 0;
		}
	}
	return 1;
}

/**
 * @brief Does str end with seq
 * 
 * Checks wether the string str ends exactly with the string seq
 * 
 * @param str The string to check
 * @param seq The ending char sequence to compare to
 * @return int 1 if str ends with seq else 0
 */
int endwith(char *str, char *seq)
{
	if (strlen(seq) > strlen(str))
	{
		return 0;
	}
	if (strlen(seq) == strlen(str) && strcmp(seq, str) == 0)
	{
		return 1;
	}

	for (int i = strlen(str); i >= strlen(str) - strlen(seq); i--)
	{
		if (str[i] != seq[i])
		{
			return 0;
		}
	}
	return 1;
}

/**
 * @brief Removes spaces around str
 * 
 * Removes leading and trailing whitespace characters from the string s
 * 
 * @param str The string to remove the spaces from
 */
void removeWhitespaces(char *str)
{
	int i;
	int begin = 0;
	int end = strlen(str) - 1;

	while (isspace((unsigned char)str[begin]))
		begin++;

	while ((end >= begin) && isspace((unsigned char)str[end]))
		end--;

	for (i = begin; i <= end; i++)
		str[i - begin] = str[i];

	str[i - begin] = '\0';
}

void replace(char *str, char c, char repl)
{
	for (int i = 0; i < strlen(str); i++)
		if (str[i] == c)
			str[i] = repl;
}