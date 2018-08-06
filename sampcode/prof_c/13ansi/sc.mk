# makefile for the SC program

MODEL = S
LINC = c:\include\local
LLIB = c:\lib\local
LIBS = $(LLIB)\$(MODEL)ansi.lib $(LLIB)\$(MODEL)util.lib \
 $(LLIB)\$(MODEL)bios.lib $(LLIB)\$(MODEL)dos.lib
OBJS = sc_cmds.obj menumode.obj parse.obj

sc_cmds.obj:	sc_cmds.c $(LINC)\ansi.h

menumode.obj:	menumode.c $(LINC)\ansi.h $(LINC)\ibmcolor.h $(LINC)\keydefs.h

parse.obj:	parse.c $(LINC)\ansi.h $(LINC)\ibmcolor.h

sc.lib:		$(OBJS)
	del $*.lib
	lib $* +$(OBJS);

sc.obj:		sc.c $(LINC)\ansi.h $(LINC)\ibmcolor.h $(LINC)\keydefs.h

sc.exe:		sc.obj sc.lib $(LIBS)
	link $*, $*, nul, sc.lib $(LIBS);
