#----------------------
# PRINT1.MAK make file
#----------------------

print1.exe : print.obj print1.obj print1.def
     link print1 print, /align:16, NUL, /nod slibcew libw, print1
     rc print1.exe

print.obj : print.c
     cl -c -Gsw -Ow -W2 -Zp print.c

print1.obj : print1.c
     cl -c -Gsw -Ow -W2 -Zp print1.c
