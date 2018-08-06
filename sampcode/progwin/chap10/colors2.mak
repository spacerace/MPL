#-----------------------
# COLORS2.MAK make file
#-----------------------

colors2.exe : colors2.obj colors2.def colors2.res
     link colors2, /align:16, NUL, /nod slibcew libw, colors2
     rc colors2.res

colors2.obj : colors2.c
     cl -c -Gsw -Ow -W2 -Zp colors2.c

colors2.res : colors2.rc
     rc -r colors2.rc
