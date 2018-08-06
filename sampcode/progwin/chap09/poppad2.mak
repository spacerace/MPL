#-----------------------
# POPPAD2.MAK make file
#-----------------------

poppad2.exe : poppad2.obj poppad2.def poppad2.res
     link poppad2, /align:16, NUL, /nod slibcew libw, poppad2.def
     rc poppad2.res

poppad2.obj : poppad2.c poppad2.h
     cl -c -Gsw -Ow -W2 -Zp poppad2.c

poppad2.res : poppad2.rc poppad2.h poppad2.ico
     rc -r poppad2.rc
