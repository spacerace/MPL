# makefile for SOUNDS program

LLIB = c:\lib\local
LLIBS = $(LLIB)\sbios.lib $(LLIB)\sdos.lib $(LLIB)\sutil.lib

sounds.obj:	sounds.c
	msc $*;

sounds.exe:	sounds.obj sound.obj $(LLIBS)
	link $* sound, $*, nul, $(LLIBS);
