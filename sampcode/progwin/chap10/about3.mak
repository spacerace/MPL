#----------------------
# ABOUT3.MAK make file
#----------------------

about3.exe : about3.obj about3.def about3.res
     link about3, /align:16, NUL, /nod slibcew libw, about3
     rc about3.res

about3.obj : about3.c about3.h
     cl -c -Gsw -Ow -W2 -Zp about3.c

about3.res : about3.rc about3.h about3.ico
     rc -r about3.rc
