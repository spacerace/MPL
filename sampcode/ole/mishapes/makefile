
all: mishapes.exe

shapes.res: shapes.rc shapes.h
    rc -r shapes.rc

shapes.obj: shapes.c shapes.h
    cl -c  -AS -Gsw -Od -Zi -Zpe -W3 shapes.c


server.obj: server.c shapes.h
    cl -c  -AS -Gsw  -Od -Zi -Zpe -W3 server.c


mishapes.exe: shapes.obj server.obj shapes.def shapes.res
    link /CO /NOD /NOE shapes server,mishapes,, libw slibcew ..\..\srvr\debug\srvr.lib , shapes.def
    rc shapes.res mishapes.exe

