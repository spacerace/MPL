hello.res: hello.rc hello.ico hello.h
    rc -r hello.rc

hello.obj: hello.c hello.h
    cl -d -c -u -W2 -Asnw -Gsw -Od -Zped hello.c

hello.exe: hello.obj hello.res hello.def
    link4 hello, hello/align:16, hello/map/li, slibw , hello.def
    mapsym hello
    rc hello.res
