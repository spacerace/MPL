#------------------------
# CHECKER3.MAK make file
#------------------------

checker3.exe : checker3.obj checker3.def
     link checker3, /align:16, NUL, /nod slibcew libw, checker3
     rc checker3.exe

checker3.obj : checker3.c
     cl -c -Gsw -Ow -W2 -Zp checker3.c
