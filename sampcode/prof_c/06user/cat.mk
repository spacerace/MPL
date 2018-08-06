# makefile for CAT program

LIB = c:\lib
LLIB = c:\lib\local

cat.obj:	cat.c
	msc $*;

cat.exe:	cat.obj $(LLIB)\sutil.lib
	link $* $(LIB)\ssetargv, $*, nul, $(LLIB)\sutil;
