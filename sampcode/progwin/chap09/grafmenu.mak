#------------------------
# GRAFMENU.MAK make file
#------------------------

grafmenu.exe : grafmenu.obj grafmenu.def grafmenu.res
     link grafmenu, /align:16, NUL, /nod slibcew libw, grafmenu
     rc grafmenu.res

grafmenu.obj : grafmenu.c grafmenu.h
     cl -c -Gsw -Ow -W2 -Zp grafmenu.c

grafmenu.res : grafmenu.rc grafmenu.h \
               editlabl.bmp filelabl.bmp fontlabl.bmp bighelp.bmp
     rc -r grafmenu.rc
