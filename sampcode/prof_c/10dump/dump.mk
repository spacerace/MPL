# makefile for the DUMP utility

LINC = c:\include\local
LIB = c:\lib
LLIB = c:\lib\local

dump.obj:	dump.c $(LINC)\std.h
	msc $*;

hexdump.obj:	hexdump.c $(LINC)\std.h
	msc $*;

dump.exe:	dump.obj hexdump.obj $(LLIB)\sutil.lib
	link $* hexdump $(LIB)\ssetargv, $*, nul, $(LLIB)\sutil;
