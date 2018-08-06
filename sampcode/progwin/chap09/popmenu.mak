#-----------------------
# POPMENU.MAK make file
#-----------------------

popmenu.exe : popmenu.obj popmenu.def popmenu.res
     link popmenu, /align:16, NUL, /nod slibcew libw, popmenu
     rc popmenu.res

popmenu.obj : popmenu.c popmenu.h
     cl -c -Gsw -Ow -W2 -Zp popmenu.c

popmenu.res : popmenu.rc popmenu.h
     rc -r popmenu.rc
