#------------------------
# PICKFONT.MAK make file
#------------------------

pickfont.exe : pickfont.obj pickfont.def pickfont.res
     link pickfont, /align:16, NUL, /nod slibcew libw, pickfont
     rc pickfont.res

pickfont.obj : pickfont.c pickfont.h
     cl -c -Gsw -Ow -W2 -Zp pickfont.c

pickfont.res : pickfont.rc pickfont.h
     rc -r pickfont.rc
