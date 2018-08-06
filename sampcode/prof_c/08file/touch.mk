# makefile for the TOUCH program

LIB=c:\lib
LLIB=c:\lib\local

# --- program modules ---
touch.obj:	touch.c

# --- the executable program ---
touch.exe:	touch.obj $(LLIB)\sutil.lib
	link touch $(LIB)\ssetargv, touch, nul, $(LLIB)\sutil;
