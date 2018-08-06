#----------------------
# ABOUT1.MAK make file
#----------------------

about1.exe : about1.obj about1.def about1.res
     link about1, /align:16, NUL, /nod slibcew libw, about1
     rc about1.res

about1.obj : about1.c about1.h
     cl -c -Gsw -Ow -W2 -Zp about1.c

about1.res : about1.rc about1.h about1.ico
     rc -r about1.rc
