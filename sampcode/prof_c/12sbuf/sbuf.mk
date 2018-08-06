# makefile for the screen buffer (SBUF) library

LLIB = c:\lib\local
LINC = c:\include\local

# --- memory model information ---
# edit the MODEL line to choose the desired memory model
# [C=compact, L=large (and huge), M=medium, S=small) 
MODEL = S		# small model
SBUF = $(MODEL)sbuf	# target library

# --- objects ---
OBJ1 = sb_box.obj sb_fill.obj sb_init.obj sb_move.obj sb_new.obj
OBJ2 = sb_put.obj sb_read.obj sb_scrl.obj sb_show.obj sb_write.obj

# --- compile sources ---
sb_box.obj:	sb_box.c $(LINC)\sbuf.h $(LINC)\video.h $(LINC)\box.h

sb_fill.obj:	sb_fill.c $(LINC)\sbuf.h

sb_init.obj:	sb_init.c $(LINC)\sbuf.h

sb_move.obj:	sb_move.c $(LINC)\sbuf.h

sb_new.obj:	sb_new.c $(LINC)\sbuf.h

sb_put.obj:	sb_put.c $(LINC)\sbuf.h

sb_read.obj:	sb_read.c $(LINC)\sbuf.h

sb_scrl.obj:	sb_scrl.c $(LINC)\sbuf.h

sb_show.obj:	sb_show.c $(LINC)\sbuf.h

sb_write.obj:	sb_write.c $(LINC)\sbuf.h

# --- create and install the library ---
$(LLIB)\$(SBUF).lib:	$(OBJ1) $(OBJ2)
	del $(LLIB)\$(SBUF).lib
	lib $(LLIB)\$(SBUF) +$(OBJ1);
	lib $(LLIB)\$(SBUF) +$(OBJ2);
	del $(LLIB)\$(SBUF).bak
