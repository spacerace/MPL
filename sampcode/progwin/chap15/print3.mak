#----------------------
# PRINT3.EXE make file
#----------------------

print3.exe : print.obj print3.obj print3.def print.res
     link print3 print, /align:16, NUL, /nod slibcew libw, print3
     rc print.res print3.exe

print.obj : print.c
     cl -c -Gsw -Ow -W2 -Zp print.c

print3.obj : print3.c
     cl -c -Gsw -Ow -W2 -Zp print3.c

print.res : print.rc
     rc -r print.rc
