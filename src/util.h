#ifndef __AUSH_UTIL__
#define __AUSH_UTIL__

#include "types.h"

#define AUSH_COUNT 3           ///< Amount of shell specific commands
#define CLR_RED "\x1b[31m"     ///< Color format code for red
#define CLR_GREEN "\x1b[32m"   ///< Color format code for green
#define CLR_YELLOW "\x1b[33m"  ///< Color format code for yellow
#define CLR_BLUE "\x1b[34m"    ///< Color format code for blue
#define CLR_MAGENTA "\x1b[35m" ///< Color format code for magenta
#define CLR_CYAN "\x1b[36m"    ///< Color format code for cyan
#define CLR_RESET "\x1b[00m"   ///< Color format code for resetting the color
#define STY_BOLD "\033[01m"    ///< Style format code for bold
#define STY_NBOLD "\033[22m"   ///< Style format code for not bold

#define GREET "\
 ____________\n\
< hi aurelio >\n\
 ------------\n\
        \\   ^__^\n\
         \\  (oo)\\_______\n\
            (__)\\       )\\/\\\n\
                ||----w |\n\
                ||     ||\n"

#define CMD_PROMPT "$" ///< Prompt symbol in the shell

pipe_t parseInput(char *, char *[], int, AUSH_CMD_FLAG *);
int countChar(char *, char);
int contains(char *, char);
void setFlag(char *, AUSH_CMD_FLAG *);
int exe(char *, char *[]);
int getUserInput(char *);
char *getPrompt();
void removeWhitespaces(char *);

#endif
