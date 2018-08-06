diabox.obj: diabox.c  diabox.h
     cl -d -W2 -c -Asnw -Gsw -Od -Zdpe diabox.c

diabox.res: diabox.rc diabox.h
     rc -r diabox.rc

diabox.exe:  diabox.obj diabox.def diabox.res
     link4 diabox,diabox/align:16/map/li,diabox,slibw/noe,diabox.def
     rc diabox.res
     mapsym diabox

