# makefile for the SHOWENV program

LLIB = c:\lib\local

showenv.obj:	showenv.c
	msc $*;

showenv.exe:	showenv.obj $(LLIB)\sutil.lib
	link $*, $*, nul, $(LLIB)\sutil;
