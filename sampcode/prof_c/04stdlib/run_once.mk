# makefile for the RUN_ONCE program

LLIB=c:\lib\local

run_once.obj:	run_once.c
	msc $*;

run_once.exe:	run_once.obj 
	link $*, $*, nul, $(LLIB)\sutil;
