#---------------------------------------------------------------------------
# PBX Makefile
#
# Author:  Brendan Dixon
#          Microsoft, inc.
#          LAN Manager Developer Support
#---------------------------------------------------------------------------

# Macros -------------------------------------------------------------------
TARG   = pbxsrv.exe

CFLAGS = /G2rs /MT /W3
LFLAGS = /NOD

LIBS   = os2.lib     \
         llibcmt.lib \
         lan.lib

CFILES = pbxsrv.c   \
         init.c     \
         makepipe.c \
         pipemgr.c  \
         router.c   \
         signals.c  \
         perror.c

HFILES = pbxsrv.h \
         pbxpkt.h

# Inference Rules ----------------------------------------------------------
$(TARG) : $(CFILES:.c=.obj) makefile
  link /NOI $(CFILES:.c=.obj),$*,NUL,$(LIBS),$*;

$(CFILES:.c=.obj) : $*.c $(HFILES)

clean   :
  -@del *.exe 2>nul
  -@del *.obj 2>nul
