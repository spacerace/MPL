#-----------------------
# DEVCAPS.MAK make file
#-----------------------

devcaps2.exe : devcaps2.obj devcaps.obj devcaps2.res devcaps2.def
     link devcaps2 devcaps, /align:16, NUL, /nod slibcew libw, devcaps2
     rc devcaps2.res

devcaps2.obj : devcaps2.c devcaps2.h
     cl -c -Gsw -Ow -W2 -Zp devcaps2.c

devcaps.obj : devcaps.c
     cl -c -Gsw -Ow -W2 -Zp devcaps.c

devcaps2.res : devcaps2.rc devcaps2.h
     rc -r devcaps2.rc
