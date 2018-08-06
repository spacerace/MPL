#------------------------
# MFCREATE.MAK make file
#------------------------

mfcreate.exe : mfcreate.obj mfcreate.def
     link mfcreate, /align:16, NUL, /nod slibcew libw, mfcreate
     rc mfcreate.exe

mfcreate.obj : mfcreate.c
     cl -c -Gsw -Ow -W2 -Zp mfcreate.c
