
tcxldemo.exe: tcxldemo.obj tcxl.lib
    tlink c0s tcxldemo,tcxldemo,,tcxl cs /c/x

tcxldemo.obj: tcxldemo.c tcxldef.h tcxlkey.h tcxlvid.h tcxlwin.h tcxldemo.mak
    tcc -c -w tcxldemo

