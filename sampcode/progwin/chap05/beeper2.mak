#-----------------------
# BEEPER2.MAK make file
#-----------------------

beeper2.exe : beeper2.obj beeper2.def
     link beeper2, /align:16, NUL, /nod slibcew libw, beeper2
     rc beeper2.exe

beeper2.obj : beeper2.c
     cl -c -Gsw -Ow -W2 -Zp beeper2.c
