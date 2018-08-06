#-----------------------
# STRPROG.MAK make file
#-----------------------

strprog.exe : strprog.obj strprog.res strprog.def
     link strprog, /align:16, NUL, /nod slibcew libw, strprog
     rc strprog.res

strprog.obj : strprog.c strprog.h
     cl -c -Gsw -Ow -W2 -Zp strprog.c

strprog.res : strprog.rc strprog.h
     rc -r strprog.rc
