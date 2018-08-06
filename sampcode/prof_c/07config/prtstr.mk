# makefile for the PRTSTR program

LINC=c:\include\local
LLIB=c:\lib\local

prtstr.obj:	prtstr.c $(LINC)\std.h
	msc $*;

prtstr.exe:	prtstr.obj $(LLIB)\sutil.lib
	link $*, $*, nul, $(LLIB)\sutil;
