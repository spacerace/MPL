#-----------------------
# BLOWUP2.MAK make file
#-----------------------

blowup2.exe : blowup2.obj blowup2.def
     link blowup2, /align:16, NUL, /nod slibcew libw, blowup2
     rc blowup2.exe

blowup2.obj : blowup2.c
     cl -c -Gsw -Ow -W2 -Zp blowup2.c
