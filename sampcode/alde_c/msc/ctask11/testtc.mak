.c.obj:
   tcc -c -d -G -O -ml -Z $*.c

tskalloc.obj:  tskalloc.c tsk.h tsklocal.h tskconf.h

tsksnap.obj:   tsksnap.c tsk.h tsklocal.h tskconf.h

test.obj:      test.c tsk.h tskconf.h

test.exe:      test.obj tskalloc.obj tsksnap.obj ctasktc.lib
   tlink /m c:\turboc\lib\c0l test+tskalloc+tsksnap,test,test,ctasktc c:\turboc\lib\cl


