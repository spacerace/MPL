#------------------------
# CHECKER1.MAK make file
#------------------------

checker1.exe : checker1.obj checker1.def
     link checker1, /align:16, NUL, /nod slibcew libw, checker1
     rc checker1.exe

checker1.obj : checker1.c
     cl -c -Gsw -Ow -W2 -Zp checker1.c
