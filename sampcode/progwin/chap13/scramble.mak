#------------------------
# SCRAMBLE.MAK make file
#------------------------

scramble.exe : scramble.obj scramble.def
     link scramble, /align:16, NUL, /nod slibcew libw, scramble
     rc scramble.exe

scramble.obj : scramble.c
     cl -c -Gsw -Ow -W2 -Zp scramble.c
