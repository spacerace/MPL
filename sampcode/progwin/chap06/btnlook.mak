#-----------------------
# BTNLOOK.MAK make file
#-----------------------

btnlook.exe : btnlook.obj btnlook.def
     link btnlook, /align:16, NUL, /nod slibcew libw, btnlook
     rc btnlook.exe

btnlook.obj : btnlook.c
     cl -c -Gsw -Ow -W2 -Zp btnlook.c
