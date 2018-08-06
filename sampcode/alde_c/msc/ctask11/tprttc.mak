.c.obj:
   tcc -c -d -G -O -ml -Z $*.c

tskalloc.obj:  tskalloc.c tsk.h tsklocal.h tskconf.h

tsksnap.obj:   tsksnap.c tsk.h tsklocal.h tskconf.h

tprt.obj:      tprt.c tsk.h prt.h tskconf.h

tprt.exe:      tprt.obj tskalloc.obj tsksnap.obj ctasktc.lib
   tlink /m c:\turboc\lib\c0l tprt+tskalloc+tsksnap,tprt,tprt,ctasktc c:\turboc\lib\cl

