#--------------------
# HEAD.MAK make file
#--------------------

head.exe : head.obj head.def
     link head, /align:16, NUL, /nod slibcew libw, head
     rc head.exe

head.obj : head.c
     cl -c -Gsw -Ow -W2 -Zp head.c
