# makefile for the ANSI library

LINC = c:\include\local
LLIB = c:\lib\local

# --- memory model information ---
# edit the MODEL line to choose the desired memory model
# [C=compact, L=large (and huge), M=medium, S=small) 
MODEL = S		# small model
ANSI = $(MODEL)ansi	# target library

# --- objects ---
OBJS = ansi_cpr.obj ansi_tst.obj colornum.obj setattr.obj userattr.obj

# --- compile sources ---
ansi_cpr.obj:	ansi_cpr.c $(LINC)\ansi.h

ansi_tst.obj:	ansi_tst.c $(LINC)\ansi.h $(LINC)\video.h

colornum.obj:	colornum.c $(LINC)\ansi.h $(LINC)\ibmcolor.h

setattr.obj:	setattr.c $(LINC)\ansi.h $(LINC)\ibmcolor.h

userattr.obj:	userattr.c $(LINC)\ansi.h $(LINC)\ibmcolor.h

$(LLIB)\$(ANSI).lib:	$(OBJS)
	del $(LLIB)\$(ANSI).lib
	lib $(LLIB)\$(ANSI) +$(OBJS);
