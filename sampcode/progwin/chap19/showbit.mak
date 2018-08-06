#-----------------------
# SHOWBIT.MAK make file
#-----------------------

showbit.exe : showbit.obj showbit.def
     link showbit, /align:16, NUL, /nod slibcew libw, showbit
     rc showbit.exe

showbit.obj : showbit.c
     cl -c -Gsw -Ow -W2 -Zp showbit.c
