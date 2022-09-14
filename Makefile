##########################################################################################

PROG_1=aush#								change this to your .c/.cpp filename
LIB_1=util
LIB_2=types
CC=gcc# 									change this to your compiler
ARGS=-O -lreadline# 									change this to your arguments if you want any
BINDIR=bin$(SEP)#							destination folder (put and escaped \ at the end)
SRCDIR=src$(SEP)#							source folder (put an escaped \ at the end)
INST_DIR=/usr/local/bin
OTHER_RM=

##########################################################################################

ifeq ($(OS), Windows_NT)
	BUILD_OS=Windows
	EXT=.exe
	RM=del
	SEP=\\
else
	BUILD_OS=Linux
	EXT=
	RM=rm -f
	SEP=/
endif

ifeq ($(CC), gcc)
	SRC_EXT=.c
else
	SRC_EXT=.cpp
endif

all: $(SRCDIR)$(PROG_1).c $(SRCDIR)$(LIB_1).h $(SRCDIR)$(LIB_2).h
	$(CC) -o $(BINDIR)$(PROG_1)$(EXT) $(SRCDIR)$(PROG_1)$(SRC_EXT) $(SRCDIR)$(LIB_1).c $(ARGS) 

clean:
	$(RM) $(BINDIR)$(PROG_1)$(EXT)
	$(OTHER_RM)

install: $(SRCDIR)$(PROG_1).c $(SRCDIR)$(LIB_1).h $(SRCDIR)$(LIB_2).h
	$(CC) -o $(BINDIR)$(PROG_1)$(EXT) $(SRCDIR)$(PROG_1)$(SRC_EXT) $(SRCDIR)$(LIB_1).c $(ARGS)
	cp $(BINDIR)$(PROG_1)$(EXT) $(INST_DIR)

remove:
	rm -f $(INST_DIR)$(PROG_1)$(EXT)