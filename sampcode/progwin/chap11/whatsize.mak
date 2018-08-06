#------------------------
# WHATSIZE.MAK make file
#------------------------

whatsize.exe : whatsize.obj whatsize.def
     link whatsize, /align:16, NUL, /nod slibcew libw, whatsize
     rc whatsize.exe

whatsize.obj : whatsize.c
     cl -c -Gsw -Ow -W2 -Zp whatsize.c
