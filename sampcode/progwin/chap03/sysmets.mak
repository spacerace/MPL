#-----------------------
# SYSMETS.MAK make file
#-----------------------

sysmets.exe : sysmets.obj sysmets.def
     link sysmets, /align:16, NUL, /nod slibcew libw, sysmets
     rc sysmets.exe

sysmets.obj : sysmets.c sysmets.h
     cl -c -Gsw -Ow -W2 -Zp sysmets.c
