#------------------------
# SYSMETS3.MAK make file
#------------------------

sysmets3.exe : sysmets3.obj sysmets3.def
     link sysmets3, /align:16, NUL, /nod slibcew libw, sysmets3
     rc sysmets3.exe

sysmets3.obj : sysmets3.c sysmets.h
     cl -c -Gsw -Ow -W2 -Zp sysmets3.c
