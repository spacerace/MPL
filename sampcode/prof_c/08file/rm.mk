# makefile for the RM program

LIB=c:\lib
LLIB=c:\lib\local

# --- program modules ---
rm.obj:	rm.c
	msc $*;

# --- the executable program ---
rm.exe:	rm.obj $(LLIB)\sutil.lib
	link $* $(LIB)\ssetargv, $*, nul, $(LLIB)\sutil;
