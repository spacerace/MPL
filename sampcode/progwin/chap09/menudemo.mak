#------------------------
# MENUDEMO.MAK make file
#------------------------

menudemo.exe : menudemo.obj menudemo.def menudemo.res
     link menudemo, /align:16, NUL, /nod slibcew libw, menudemo
     rc menudemo.res

menudemo.obj : menudemo.c menudemo.h
     cl -c -Gsw -Ow -W2 -Zp menudemo.c

menudemo.res : menudemo.rc menudemo.h
     rc -r menudemo.rc
