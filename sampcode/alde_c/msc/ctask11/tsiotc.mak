.c.obj:
   tcc -c -d -G -O -ml -Z $*.c

tskalloc.obj:  tskalloc.c tsk.h tsklocal.h tskconf.h

tsksnap.obj:   tsksnap.c tsk.h tsklocal.h tskconf.h

tsio.obj:     tsio.c tsk.h sio.h tskconf.h

tsio.exe:      tsio.obj tskalloc.obj tsksnap.obj ctasktc.lib
   tlink /m c:\turboc\lib\c0l tsio+tskalloc+tsksnap,tsio,tsio,ctasktc c:\turboc\lib\cl

