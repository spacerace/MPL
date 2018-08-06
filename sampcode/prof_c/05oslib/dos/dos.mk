# makefile for the DOS library

LINC = c:\include\local
LLIB = c:\lib\local

# --- memory model information ---
# edit the MODEL line to choose the desired memory model
# [C=compact, L=large (and huge), M=medium, S=small) 
MODEL = L		# small model
DOS = $(MODEL)dos.lib	# target library

# --- objects ---
OBJS = drvpath.obj first_fm.obj getdrive.obj getkey.obj keyready.obj next_fm.obj setdta.obj ver.obj

# --- compile sources ---
drvpath.obj:	drvpath.c $(LINC)\doslib.h

first_fm.obj:	first_fm.c $(LINC)\doslib.h

getdrive.obj:	getdrive.c $(LINC)\doslib.h

getkey.obj:	getkey.c $(LINC)\std.h $(LINC)\doslib.h $(LINC)\keydefs.h

keyready.obj:	keyready.c $(LINC)\doslib.h

next_fm.obj:	next_fm.c $(LINC)\doslib.h

setdta.obj:	setdta.c $(LINC)\doslib.h

ver.obj:	ver.c $(LINC)\doslib.h


# --- create and install the library ---
$(LLIB)\$(DOS):	$(OBJS)
	del $(LLIB)\$(DOS)
	lib $(LLIB)\$(DOS) +$(OBJS);
