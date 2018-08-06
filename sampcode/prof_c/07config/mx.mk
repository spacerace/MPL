# makefile for the MX program

LINC=c:\include\local
LLIB=c:\lib\local

mx.obj:		mx.c $(LINC)\std.h $(LINC)\printer.h
	msc $*;

mx.exe:		mx.obj $(LLIB)\sutil.lib
	link $*, $*, nul, $(LLIB)\sutil;
