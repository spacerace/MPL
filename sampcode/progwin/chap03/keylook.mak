#-----------------------
# KEYLOOK.MAK make file
#-----------------------

keylook.exe : keylook.obj keylook.def
     link keylook, /align:16, NUL, /nod slibcew libw, keylook
     rc keylook.exe

keylook.obj : keylook.c
     cl -c -Gsw -Ow -W2 -Zp keylook.c
