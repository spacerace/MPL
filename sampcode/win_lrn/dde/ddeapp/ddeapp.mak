
ddeapp.res: ddeapp.rc ddeapp.ico ddeapp.h
    rc -r ddeapp.rc

ddeapp.obj: ddeapp.c ddeapp.h
   cl -d -W2 -c -AM -Gsw -Od -Zpei ddeapp.c

ddeapp.exe: ddeapp.obj ddeapp.res ddeapp.def
    link4 ddeapp,/align:16,/map/co,mlibw,ddeapp.def
    rc ddeapp.res

