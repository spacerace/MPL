STDFLAGS=-c -u -AS -FPa -Gsw -Os -Zep

spyglass.res: spyglass.rc spyglass.h spyglass.ico
   rc -r spyglass.rc

spyglass.obj: spyglass.c spyglass.h
   cl $(STDFLAGS) spyglass.c

spyglass.exe: spyglass.obj spyglass.res spyglass.def
   link4 spyglass /AL:16 /NOE,,,slibw,spyglass.def
   rc spyglass.res
