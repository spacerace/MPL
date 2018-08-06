#------------------------
# NOPOPUPS.MAK make file
#------------------------

nopopups.exe : nopopups.obj nopopups.def nopopups.res
     link nopopups, /align:16, NUL, /nod slibcew libw, nopopups
     rc nopopups.res

nopopups.obj : nopopups.c nopopups.h
     cl -c -Gsw -Ow -W2 -Zp nopopups.c

nopopups.res : nopopups.rc nopopups.h
     rc -r nopopups.rc
