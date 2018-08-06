#----------------------
# PRINT4.MAK make file
#----------------------

print4.exe : print.obj print4.obj print4.def print.res
     link print4 print, /align:16, NUL, /nod slibcew libw, print4
     rc print.res print4.exe

print.obj : print.c
     cl -c -Gsw -Ow -W2 -Zp print.c

print4.obj : print4.c
     cl -c -Gsw -Ow -W2 -Zp print4.c

print.res : print.rc
     rc -r print.rc
