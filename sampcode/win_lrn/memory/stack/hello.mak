stack.obj: stack.asm
    masm -i\include -D?S stack.asm;

hello.obj: hello.c hello.h
    cl -d -c -AS -Gsw -Os -Zpe hello.c

hello.res: hello.rc hello.ico hello.h
    rc -r hello.rc

hello.exe: hello.obj hello.res hello.def stack.obj
    link4 hello stack,/align:16,/map,slibw,hello.def
    rc hello.res
