# makefile for the NOTES program

LLIB=c:\lib\local

notes1.obj:	notes1.c
	msc $*;

notes1.exe:	notes1.obj 
	link $*, $*, nul, $(LLIB)\sutil;

notes2.obj:	notes2.c
	msc $*;

notes2.exe:	notes2.obj 
	link $*, $*, nul, $(LLIB)\sutil;
