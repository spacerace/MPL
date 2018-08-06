# make file for tiler.exe

tiler.obj:  tiler.c
    msc -AS -Gcsw -Ox -u -W3 -Zdp $*;

tiler.res:  tiler.rc  tiler.ico
    rc -r tiler.rc

tiler.exe:  tiler.obj  tiler.res  tiler.def
    link4 tiler, tiler/align:16, tiler/map/line, slibw, tiler.def
    rc tiler.res
    mapsym tiler
