#makefile for SHOWTABS program

LINC = c:\include\local
LLIB = c:\lib\local

showtabs.obj:	 showtabs.c $(LINC)\std.h
	msc $*;

showtabs.exe:	 showtabs.obj $(LLIB)\sutil.lib
	link $*, $*, nul, $(LLIB)\sutil;
