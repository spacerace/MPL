

# Figure 8
# ========


# makefile for the GDEMO program

.c.obj:
	cl -c -W2 -Ze -AL $*.c

gdemo.obj:	gdemo.c

message.obj:	message.c

gdemo.exe:	gdemo.obj message.obj 
		link gdemo message, gdemo, nul, graphics;
