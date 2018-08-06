#----------------------------------------------------------------------------
# makefile : cldemo make file
#
# Copyright (c) Microsoft Corporation, 1990-
#----------------------------------------------------------------------------

all: cltest.exe

cltest.res: cltest.rc cldemo.h
    rc -r cltest.rc

cldemo.obj: cldemo.c cldemo.h
    cl -c  -AS -Gsw -Od -Zi -Zpe cldemo.c

client.obj: client.c client.h
    cl -c  -AS -Gsw -Od -Zi -Zpe client.c

cltest.exe: cldemo.obj client.obj cltest.def cltest.res
    link /CO /NOD /NOE cldemo client, cltest,, libw slibcew ..\..\..\dll\debug\ecd, cltest.def
    rc cltest.res

