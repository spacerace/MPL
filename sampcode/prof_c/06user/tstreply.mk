# makefile for the TSTREPLY program

LLIB = c:\lib\local
LIBS = $(LLIB)\sbios.lib $(LLIB)\sdos.lib $(LLIB)\sutil.lib

getreply.obj:	getreply.c
	msc $*;

tstreply.obj:	tstreply.c
	msc $*;

tstreply.exe:	tstreply.obj getreply.obj sound.obj $(LIBS)
	link $* getreply sound, $*, nul, $(LIBS);
