# makefile for the BIOS library

LINC = c:\include\local
LLIB = c:\lib\local

# --- memory model information ---
# edit the MODEL line to choose the desired memory model
# [C=compact, L=large (and huge), M=medium, S=small) 
MODEL = S		# small model
BIOS = $(MODEL)bios	# target library

# --- objects ---
O1 = clrscrn.obj clrw.obj curback.obj delay.obj drawbox.obj ega_info.obj
O2 = equipchk.obj getctype.obj getstate.obj getticks.obj kbd_stat.obj
O3 = memsize.obj palette.obj putcur.obj putfld.obj putstr.obj put_ch.obj
O4 = readca.obj readcur.obj readdot.obj scroll.obj setctype.obj setpage.obj
O5 = setvmode.obj writea.obj writec.obj writeca.obj writedot.obj writemsg.obj
O6 = writestr.obj writetty.obj

# --- compile sources ---
clrscrn.obj:	clrscrn.c $(LINC)\bioslib.h $(LINC)\std.h

clrw.obj:	clrw.c $(LINC)\bioslib.h $(LINC)\std.h

curback.obj:	curback.c $(LINC)\bioslib.h

delay.obj:	delay.c $(LINC)\timer.h

drawbox.obj:	drawbox.c $(LINC)\video.h

ega_info.obj:	ega_info.c $(LINC)\bioslib.h $(LINC)\video.h

equipchk.obj:	equipchk.c $(LINC)\bioslib.h $(LINC)\equip.h

getctype.obj:	getctype.c $(LINC)\bioslib.h $(LINC)\std.h

getstate.obj:	getstate.c $(LINC)\bioslib.h $(LINC)\std.h

getticks.obj:	getticks.c $(LINC)\bioslib.h

kbd_stat.obj:	kbd_stat.c $(LINC)\bioslib.h $(LINC)\keybdlib.h

memsize.obj:	memsize.c $(LINC)\bioslib.h $(LINC)\std.h

palette.obj:	palette.c $(LINC)\bioslib.h

putcur.obj:	putcur.c $(LINC)\bioslib.h $(LINC)\std.h

putfld.obj:	putfld.c $(LINC)\bioslib.h

putstr.obj:	putstr.c $(LINC)\bioslib.h

put_ch.obj:	put_ch.c $(LINC)\bioslib.h

readca.obj:	readca.c $(LINC)\bioslib.h $(LINC)\std.h

readcur.obj:	readcur.c $(LINC)\bioslib.h $(LINC)\std.h

readdot.obj:	readdot.c $(LINC)\bioslib.h $(LINC)\std.h

scroll.obj:	scroll.c $(LINC)\bioslib.h $(LINC)\std.h

setctype.obj:	setctype.c $(LINC)\bioslib.h

setpage.obj:	setpage.c $(LINC)\bioslib.h $(LINC)\std.h $(LINC)\video.h

setvmode.obj:	setvmode.c $(LINC)\bioslib.h $(LINC)\std.h

writea.obj:	writea.c $(LINC)\std.h

writec.obj:	writec.c $(LINC)\bioslib.h $(LINC)\std.h

writeca.obj:	writeca.c $(LINC)\bioslib.h $(LINC)\std.h

writedot.obj:	writedot.c $(LINC)\bioslib.h $(LINC)\std.h

writemsg.obj:	writemsg.c $(LINC)\std.h

writestr.obj:	writestr.c $(LINC)\std.h

writetty.obj:	writetty.c $(LINC)\bioslib.h $(LINC)\std.h


# --- create and install the library ---
$(LLIB)\$(BIOS).lib:	$(O1) $(O2) $(O3) $(O4) $(O5) $(O6)
	del $(LLIB)\$(BIOS).lib
	lib $(LLIB)\$(BIOS) +$(O1);
	lib $(LLIB)\$(BIOS) +$(O2);
	lib $(LLIB)\$(BIOS) +$(O3);
	lib $(LLIB)\$(BIOS) +$(O4);
	lib $(LLIB)\$(BIOS) +$(O5);
	lib $(LLIB)\$(BIOS) +$(O6);
	del $(LLIB)\$(BIOS).bak
