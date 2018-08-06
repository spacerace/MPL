#----------------------
# ABOUT2.MAK make file
#----------------------

about2.exe : about2.obj about2.def about2.res
     link about2, /align:16, NUL, /nod slibcew libw, about2
     rc about2.res

about2.obj : about2.c about2.h
     cl -c -Gsw -Ow -W2 -Zp about2.c

about2.res : about2.rc about2.h about2.ico
     rc -r about2.rc
