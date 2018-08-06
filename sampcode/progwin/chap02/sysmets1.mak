#------------------------
# SYSMETS1.MAK make file
#------------------------

sysmets1.exe : sysmets1.obj sysmets1.def
     link sysmets1, /align:16, NUL, /nod slibcew libw, sysmets1
     rc sysmets1.exe

sysmets1.obj : sysmets1.c sysmets.h
     cl -c -Gsw -Ow -W2 -Zp sysmets1.c
