# makefile for the DSPYTYPE demo program

LINC = c:\include\local
LLIB = c:\lib\local
LIBS = $(LLIB)\$(MODEL)bios.lib $(LLIB)\$(MODEL)util.lib 

# --- memory model information ---
# edit the MODEL line to choose the desired memory model
# [C=compact, L=large (and huge), M=medium, S=small) 
MODEL = S	# small model

# --- compile source ---
dspytype.obj:	dspytype.c $(LINC)\bioslib.h

# --- create the DSPYTYPE program ---
dspytype.exe:	dspytype.obj $(LIBS)
	link $*, $*, nul, $(LIBS);
