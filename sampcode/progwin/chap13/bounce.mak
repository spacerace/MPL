#----------------------
# BOUNCE.MAK make file
#----------------------

bounce.exe : bounce.obj bounce.def
     link bounce, /align:16, NUL, /nod slibcew libw, bounce
     rc bounce.exe

bounce.obj : bounce.c
     cl -c -Gsw -Ow -W2 -Zp bounce.c
