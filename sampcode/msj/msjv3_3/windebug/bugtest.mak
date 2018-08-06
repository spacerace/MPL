# Figure 8
# ========

# BUGTEST MAKE File
# =================


STDFLAGS=-c -u -AS -FPa -Gsw -Os -Zep
BUGFLAGS=-c -u -AS -FPa -Gsw -Os -Zep -DDEBUG

bugtest.res: bugtest.rc bugtest.ico
   rc -r bugtest.rc

debug.obj: debug.h debug.c
   cl $(STDFLAGS) debug.c

bugtest.obj: bugtest.c
   cl $(BUGFLAGS) bugtest.c

bugtest.exe: bugtest.obj bugtest.def bugtest.res debug.obj
   link4 bugtest+debug /AL:16 /NOE,,,slibw,bugtest.def
   rc bugtest.res
