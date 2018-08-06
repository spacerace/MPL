# makefile for the SETMYDIR program

LLIB=c:\lib\local

setmydir.obj:	setmydir.c
	msc $*;

setmydir.exe:	setmydir.obj $(LLIB)\sutil.lib
	link $*, $*, nul, $(LLIB)\sutil;
