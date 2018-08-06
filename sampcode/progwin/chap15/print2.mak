#----------------------
# PRINT2.MAK make file
#----------------------

print2.exe : print.obj print2.obj print2.def
     link print2 print, /align:16, NUL, /nod slibcew libw, print2
     rc print2.exe

print.obj : print.c
     cl -c -Gsw -Ow -W2 -Zp print.c

print2.obj : print2.c
     cl -c -Gsw -Ow -W2 -Zp print2.c
