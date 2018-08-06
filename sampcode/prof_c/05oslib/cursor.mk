# makefile for CURSOR program

LLIB=c:\lib\local

cursor.obj:	cursor.c
	msc $*;

cursor.exe:	cursor.obj $(LLIB)\sbios.lib  $(LLIB)\sdos.lib
	link $*, $*, nul, $(LLIB)\sbios $(LLIB)\sdos;
