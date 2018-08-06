#-----------------------
# LINEDDA.MAK make file
#-----------------------

linedda.exe : linedda.obj linedda.def
     link linedda, /align:16, NUL, /nod slibcew libw, linedda
     rc linedda.exe

linedda.obj : linedda.c
     cl -c -Gsw -Ow -W2 -Zp linedda.c
