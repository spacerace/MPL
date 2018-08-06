
biff.obj: biff.c biff.h
    cl -d -c -AS -Gsw -Od -Zpei -W3 biff.c

bigg.obj: bigg.c biff.h
    cl -d -c -AS -Gsw -Od -Zpei -W3 bigg.c

biff.res: biff.rc biff.ico biff.h
    rc -r biff.rc

biff.exe: biff.obj bigg.obj biff.res biff.def
    link4 biff bigg,/align:16,/co,slibw,biff.def
    rc biff.res

