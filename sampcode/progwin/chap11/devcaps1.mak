#------------------------
# DEVCAPS1.MAK make file
#------------------------

devcaps1.exe : devcaps1.obj devcaps.obj devcaps1.def devcaps1.res
     link devcaps1 devcaps, /align:16, NUL, /nod slibcew libw, devcaps1
     rc devcaps1.res

devcaps1.obj : devcaps1.c devcaps1.h
     cl -c -Gsw -Ow -W2 -Zp devcaps1.c

devcaps.obj : devcaps.c
     cl -c -Gsw -Ow -W2 -Zp devcaps.c

devcaps1.res : devcaps1.rc devcaps1.h
     rc -r devcaps1.rc
