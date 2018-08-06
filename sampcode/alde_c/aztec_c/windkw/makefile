# MAKEFILE for AZTEC C.
# Uses Aztec's version of make
# you will have to modify it if you aren't using AZTEC make

#
# generate code for 80186/V20
CFLAGS=+1

CLIBS=-l/clibs/c

# *.c
# setscrn.c       window.c        wndemo.c        

windsrc=window.c
windasm=vlib.asm        
windobj=window.o vlib.o conio.o

$(windobj) : window.h

wndemo.com : wndemo.o $(windobj)
	ln -o $@ $*.o $(windobj) $(CLIBS)
wtest.com : wtest.o $(windobj)
	ln -t -o $@ $*.o $(windobj) $(CLIBS)
	\\atron\\aztoat <$*.sym >$*.map
setscrn.com : setscrn.o $(windobj)
	ln -o $@ $*.o $(windobj) $(CLIBS)
