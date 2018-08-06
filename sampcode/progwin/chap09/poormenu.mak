#------------------------
# POORMENU.MAK make file
#------------------------

poormenu.exe : poormenu.obj poormenu.def
     link poormenu, /align:16, NUL, /nod slibcew libw, poormenu
     rc poormenu.exe

poormenu.obj : poormenu.c
     cl -c -Gsw -Ow -W2 -Zp poormenu.c
