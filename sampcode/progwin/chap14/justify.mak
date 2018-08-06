#-----------------------
# JUSTIFY.MAK make file
#-----------------------

justify.exe : justify.obj justify.def justify.res
     link justify, /align:16, NUL, /nod slibcew libw, justify
     rc justify.res

justify.obj : justify.c justify.h
     cl -c -Gsw -Ow -W2 -Zp justify.c

justify.res : justify.rc justify.asc justify.h
     rc -r justify.rc
