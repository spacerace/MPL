#------------------------
# CHECKER2.MAK make file
#------------------------

checker2.exe : checker2.obj checker2.def
     link checker2, /align:16, NUL, /nod slibcew libw, checker2
     rc checker2.exe

checker2.obj : checker2.c
     cl -c -Gsw -Ow -W2 -Zp checker2.c
