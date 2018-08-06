
dlgtemp.obj: dlgtemp.c  dlgbox.h
     cl  -W2 -d  -c  -AM  -Gsw  -Os -Zpe dlgtemp.c

dlgbox.obj: dlgbox.c  dlgbox.h
     cl -W2 -d  -c  -AM  -Gsw  -Os -Zpe dlgbox.c

dlgbox.res: dlgbox.rc dlgbox.h
     rc -r dlgbox.rc

dlgbox.exe:  dlgbox.obj dlgbox.def dlgbox.res dlgtemp.obj
     link4 dlgbox dlgtemp,dlgbox/align:16,dlgbox,mlibw,dlgbox.def
     rc dlgbox.res
