#-----------------------
# SHOWPOP.MAK make file
#-----------------------

showpop.exe : showpop.obj showpop.def
     link showpop, /align:16, NUL, /nod slibcew libw, showpop
     rc showpop.exe

showpop.obj : showpop.c
     cl -c -Gsw -Ow -W2 -Zp showpop.c
