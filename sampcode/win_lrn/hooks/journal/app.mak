
cp=cl -d -c -AS -Gsw -Os -Zpe

.c.obj:
    $(cp) $*.c

APP.obj: APP.c APP.h

wndproc.obj: wndproc.c APP.h

APPpaint.obj: APPpaint.c APP.h

APP.res: APP.rc APP.ico APP.h
    rc -r APP.rc

APP.exe: APP.obj wndproc.obj APPpaint.obj APP.res APP.def
    link4 @APP.rsp
    mapsym APP
    rc APP.res
