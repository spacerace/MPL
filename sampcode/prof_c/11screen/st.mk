# makefile for ScreenTest (ST) program

LINC = c:\include\local
LLIB = c:\lib\local

# --- memory model information ---
# edit the MODEL line to choose the desired memory model
# [C=compact, L=large (and huge), M=medium, S=small) 
MODEL = S	# small model

LIBS = $(LLIB)\$(MODEL)bios.lib $(LLIB)\$(MODEL)dos.lib

# --- compile sources ---
swap_int.obj:	swap_int.c

cpblk.obj:	cpblk.c

st.obj:		st.c $(LINC)\std.h $(LINC)\video.h

# --- create the executable program ---
st.exe:		st.obj swap_int.obj cpblk.obj $(LIBS)
	link $* swap_int cpblk, $*, nul, $(LIBS);
