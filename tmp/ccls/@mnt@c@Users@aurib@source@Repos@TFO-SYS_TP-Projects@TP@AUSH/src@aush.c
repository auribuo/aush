#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>
#include "util.h"
#include "types.h"

//#define DEBUG_AUSH
#define NO_PIPE
//#define GREET_MSG

void exeChild(char *, char *[], int *);
void spawnProc(int in, int out, char *cmd, char **args, AUSH_CMD_FLAG flag, int *lastRet);

int main(int argc, char *argv[])
{
#ifdef GREET_MSG
	printf("%s\n", GREET);
#endif
	AUSH_CMD_FLAG flag = none_f;
	int lastRet = 0;
	char *userInput = (char *)malloc((MAXCMD * sizeof(char)) + 1);
	do
	{
		int inputRes = getUserInput(userInput);

		fflush(stdout);

		if (strlen(userInput) >= MAXCMD)
		{
			printf("Input buffer too big.\n");
		}
		else if (userInput != NULL && strcmp(userInput, "\0") != 0 && inputRes == 0)
		{
			int argCount = countChar(userInput, ' ') + 1;
			char *args[argCount + 1];

			pipe_t pipeContainer = parseInput(userInput, args, argCount, &flag);

			args[argCount] = (char *)0;

#ifdef DEBUG_AUSH
			printf("Tokens created:\n");
			if (pipeContainer.isPipe)
			{
				printf("Lhand: (");
				for (int i = 0; i < pipeContainer.leftArgc + 1; i++)
					printf("[%s]", pipeContainer.lefthandPipeParsed[i]);
				printf(") Flag: %d\nRhand: (", pipeContainer.lefthandFlag);
				for (int i = 0; i < pipeContainer.rightArgc + 1; i++)
					printf("[%s]", pipeContainer.righthandPipeParsed[i]);
				printf(") Flag: %d\n", pipeContainer.righthandFlag);
				printf("Global flag: %d\n", flag);
			}
			else
			{
				printf("(");
				for (int i = 0; i < argCount + 1; i++)
					printf("[%s]", args[i]);
				printf(")\n");
			}
#endif

			switch (flag)
			{
#ifndef NO_PIPE
			case pipe_f:
			{
				int in, fd[2];
				in = 0;
				int _ = pipe(fd);
				spawnProc(in, fd[1], pipeContainer.lefthandPipeParsed[0], pipeContainer.lefthandPipeParsed, pipeContainer.lefthandFlag, &lastRet);
				close(fd[1]);
				in = fd[0];
				if (in != 0)
				{
					dup2(in, 0);
				}
				switch (pipeContainer.righthandFlag)
				{
				case exit_f:
				{
					free(userInput);
					exit(0);
					//killpg(0, SIGTERM);
				}
				case retLastExit_f:
				{
					char buf[] = {(char)(lastRet + 48), '\0'};
					args[0] = "echo";
					args[1] = buf;
					args[2] = (char *)0;
					exeChild(args[0], args, &lastRet);
					break;
				}
				case cd_f:
				{
					args[0] = "cd";
					args[2] = (char *)0;
					exe(args[0], args, &lastRet);
					break;
				}
				case default_f:
				{
					exeChild(pipeContainer.righthandPipeParsed[0], pipeContainer.righthandPipeParsed, &lastRet);
					break;
				}
				default:
				{
					fprintf(stderr, "Internal error!\n");
					killpg(0, SIGTERM);
				}
				}
			}
#endif
			case exit_f:
			{
				free(userInput);
				exit(0);
				//killpg(0, SIGTERM);
			}
			case retLastExit_f:
			{
				char buf[] = {(char)(lastRet + 48), '\0'};
				args[0] = "echo";
				args[1] = buf;
				args[2] = (char *)0;
				exeChild(args[0], args, &lastRet);
				break;
			}
			case cd_f:
			{
				args[0] = "cd";
				args[2] = (char *)0;
				lastRet = exe(args[0], args);
				break;
			}
			case default_f:
			{
				exeChild(args[0], args, &lastRet);
                if (lastRet != 0)
                {
                    fprintf(stderr, "AUSH: no such command: %s\n", args[0]);
                }
                break;
			}
			case none_f:
				break;
			default:
			{
				fprintf(stderr, "Internal error!\n");
				killpg(0, SIGTERM);
			}
			}
		}
	} while (1);
}

void exeChild(char *cmd, char *args[], int *lastRet)
{
	fflush(stdout);
	switch (fork())
	{
	case -1:
	{
		exit(1);
		break;
	}
	case 0:
	{
		exit(exe(cmd, args));
	}
	default:
	{
		wait(lastRet);
		break;
	}
	}
}

void spawnProc(int in, int out, char *cmd, char **args, AUSH_CMD_FLAG flag, int *lastRet)
{
	pid_t pid;
	char *opt[3];
	if ((pid = fork()) == 0)
	{
		if (in != 0)
		{
			dup2(in, 0);
			close(in);
		}
		if (out != 1)
		{
			dup2(out, 1);
			close(out);
		}
		switch (flag)
		{
		case exit_f:
		{
			//killpg(0, SIGTERM);
			exit(0);
		}
		case retLastExit_f:
		{
			char buf[] = {(char)(*lastRet + 48), '\0'};
			opt[0] = "echo";
			opt[1] = buf;
			opt[2] = (char *)0;
			exeChild(opt[0], opt, lastRet);
			break;
		}
		case cd_f:
		{
			opt[0] = "cd";
			opt[1] = args[1];
			opt[2] = (char *)0;
			*lastRet = exe(opt[0], opt);
			break;
		}
		case default_f:
		{
			exeChild(cmd, args, lastRet);
			break;
		}
		default:
		{
			fprintf(stderr, "Internal error!\n");
			killpg(0, SIGTERM);
		}
		}
	}
}
