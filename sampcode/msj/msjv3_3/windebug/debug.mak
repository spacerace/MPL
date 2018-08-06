# Debug MAKE File
# ===============


CFLAGS=-c -u -Asnw -FPa -Gsw -Os -Zep 

debug.obj: debug.h debug.c
   cl $(CFLAGS) debug.c
