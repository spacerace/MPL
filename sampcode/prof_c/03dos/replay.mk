# makefile for the REPLAY program

LIB=c:\lib
LLIB=c:\lib\local

replay.obj:	replay.c

replay.exe:	replay.obj $(LLIB)\sutil
	link $* $(LIB)\ssetargv, $*, nul, $(LLIB)\sutil;
