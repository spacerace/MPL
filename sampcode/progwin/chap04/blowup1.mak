#-----------------------
# BLOWUP1.MAK make file
#-----------------------

blowup1.exe : blowup1.obj blowup1.def
     link blowup1, /align:16, NUL, /nod slibcew libw, blowup1
     rc blowup1.exe

blowup1.obj : blowup1.c
     cl -c -Gsw -Ow -W2 -Zp blowup1.c
