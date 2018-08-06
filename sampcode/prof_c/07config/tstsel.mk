# makefile for the TSTSEL program

LINC=c:\include\local
LLIB=c:\lib\local

tstsel.obj:	tstsel.c $(LINC)\std.h
	msc $*;

tstsel.exe:	tstsel.obj $(LLIB)\sutil.lib
	link $*, $*, nul, $(LLIB)\sutil;
