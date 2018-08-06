# makefile for the TIMER program

LINC=c:\include\local
LLIB=c:\lib\local

interval.obj:	interval.c
	msc $*;

timer.obj:	timer.c $(LINC)\std.h
	msc $*;

timer.exe:	timer.obj interval.obj $(LLIB)\sutil.lib
	link $* interval, $*, nul, $(LLIB)\sutil;
