#------------------------
# FONTLIST.MAK make file
#------------------------

fontlist.exe : fontlist.obj fontlist.def fontlist.res
     link fontlist, /align:16, NUL, /nod slibcew libw, fontlist
     rc fontlist.res

fontlist.obj : fontlist.c fontlist.h
     cl -c -Gsw -Ow -W2 -Zp fontlist.c

fontlist.res : fontlist.rc fontlist.h
     rc -r fontlist.rc
