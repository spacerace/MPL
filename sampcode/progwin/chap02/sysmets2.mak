#------------------------
# SYSMETS2.MAK make file
#------------------------

sysmets2.exe : sysmets2.obj sysmets2.def
     link sysmets2, /align:16, NUL, /nod slibcew libw, sysmets2
     rc sysmets2.exe

sysmets2.obj : sysmets2.c sysmets.h
     cl -c -Gsw -Ow -W2 -Zp sysmets2.c
