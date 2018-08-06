# makefile for the SB_TEST driver program

MODEL = S
LLIB = c:\lib\local
LINC = c:\include\local
LIBS = $(LLIB)\$(MODEL)sbuf.lib $(LLIB)\$(MODEL)bios.lib $(LLIB)\$(MODEL)dos.lib

sb_test.obj:	sb_test.c $(LINC)\sbuf.h sb_test.h

sb_test.exe:	sb_test.obj $(LIBS)
	link $*, $*, nul, $(LIBS);
