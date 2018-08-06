#-----------------------
# COLORS1.MAK make file
#-----------------------

colors1.exe : colors1.obj colors1.def
     link colors1, /align:16, NUL, /nod slibcew libw, colors1
     rc colors1.exe

colors1.obj : colors1.c
     cl -c -Gsw -Ow -W2 -Zp colors1.c
