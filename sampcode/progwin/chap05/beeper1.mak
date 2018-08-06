#-----------------------
# BEEPER1.MAK make file
#-----------------------

beeper1.exe : beeper1.obj beeper1.def
     link beeper1, /align:16, NUL, /nod slibcew libw, beeper1
     rc beeper1.exe

beeper1.obj : beeper1.c
     cl -c -Gsw -Ow -W2 -Zp beeper1.c
