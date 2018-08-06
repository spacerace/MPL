
# Figure 12
# =========


CFLAGS=-c -u -AS -FPa -Gsw -Os -Zep

maze.obj: maze.h maze.c
   cl $(CFLAGS) maze.c

maze.exe: maze.obj maze.def maze.lnk
   link4 @maze.lnk
