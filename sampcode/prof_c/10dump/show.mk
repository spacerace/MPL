# makefile for the SHOW program

LINC = c:\include\local
LIB = c:\lib
LLIB = c:\lib\local

show.obj:	show.c $(LINC)\std.h
	msc $*;

showit.obj:	showit.c $(LINC)\std.h
	msc $*;

show.exe:	show.obj showit.obj $(LLIB)\sutil.lib
	link $* showit $(LIB)\ssetargv, $*, nul, $(LLIB)\sutil;
