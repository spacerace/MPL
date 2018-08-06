#------------------------
# MFRESORC.MAK make file
#------------------------

mfresorc.exe : mfresorc.obj mfresorc.def mfresorc.res
     link mfresorc, /align:16, NUL, /nod slibcew libw, mfresorc
     rc mfresorc.res

mfresorc.obj : mfresorc.c
     cl -c -Gsw -Ow -W2 -Zp mfresorc.c

mfresorc.res : mfresorc.rc mylogo.wmf
     rc -r mfresorc.rc
