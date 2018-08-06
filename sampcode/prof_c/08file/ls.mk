# makefile for the LS program

LIB=c:\lib
LLIB=c:\lib\local

# assumes that the first_fm, next_fm, setdta, getdrive, and drvpath object
# modules are contained in the DOS library and that the last_ch object module
# is contained in the UTIL library.
OBJS=ls.obj ls_dirx.obj ls_fcomp.obj ls_list.obj
LIBS=$(LLIB)\sutil.lib $(LLIB)\sdos.lib

ls_fcomp.obj:	ls_fcomp.c ls.h
	msc $*;

ls_dirx.obj:	ls_dirx.c ls.h
	msc $*;

ls_list.obj:	ls_list.c ls.h
	msc $*;

ls.obj:		ls.c ls.h
	msc $*;

ls.exe:		$(OBJS) $(LIBS)
	link $(OBJS) $(LIB)\ssetargv, $*, nul, $(LIBS);
