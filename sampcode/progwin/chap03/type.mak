#--------------------
# TYPE.MAK make file
#--------------------

type.exe : type.obj type.def
     link type, /align:16, NUL, /nod slibcew libw, type
     rc type.exe

type.obj : type.c
     cl -c -Gsw -Ow -W2 -Zp type.c
