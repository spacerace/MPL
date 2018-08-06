# makefile for TEE program

LIB=c:\lib
LLIB=c:\lib\local

# --- program module ---
tee.obj:	tee.c
	msc $*;

# --- the executable program ---
tee.exe:	tee.obj $(LLIB)\sutil.lib
	link $* $(LIB)\ssetargv, $*, nul, $(LLIB)\sutil;
