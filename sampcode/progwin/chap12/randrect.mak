#------------------------
# RANDRECT.MAK make file
#------------------------

randrect.exe : randrect.obj randrect.def
     link randrect, /align:16, NUL, /nod slibcew libw, randrect
     rc randrect.exe

randrect.obj : randrect.c
     cl -c -Gsw -Ow -W2 -Zp randrect.c
