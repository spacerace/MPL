#----------------------
# STRLIB.MAK make file
#----------------------

strlib.dll : strlib.obj strlib.def
     link strlib libentry, strlib.dll /align:16, NUL, /nod sdllcew libw, strlib
     rc strlib.dll

strlib.obj : strlib.c
     cl -c -ASw -Gsw -Ow -W2 -Zp strlib.c
