#------------------------
# CLIPVIEW.MAK make file
#------------------------

clipview.exe : clipview.obj clipview.def
     link clipview, /align:16, NUL, /nod slibcew libw, clipview
     rc clipview.exe

clipview.obj : clipview.c
     cl -c -Gsw -Ow -W2 -Zp clipview.c
