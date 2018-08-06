#-----------------------
# MDIDEMO.MAK make file
#-----------------------

mdidemo.exe : mdidemo.obj mdidemo.def mdidemo.res
     link mdidemo, /align:16, NUL, /nod slibcew libw, mdidemo
     rc mdidemo.res

mdidemo.obj : mdidemo.c mdidemo.h
     cl -c -Gsw -Ow -W2 -Zp mdidemo.c

mdidemo.res : mdidemo.rc mdidemo.h
     rc -r mdidemo.rc
