.c.obj:
   cl /c /AL /Gs /Oailt /Zp1 /W3 $*.c

tskalloc.obj:  tskalloc.c tsk.h tsklocal.h tskconf.h

tsksnap.obj:   tsksnap.c tsk.h tsklocal.h tskconf.h

tsio.obj:     tsio.c tsk.h sio.h tskconf.h

tsio.exe:     tsio.obj tskalloc.obj tsksnap.obj ctaskms.lib
   link tsio+tskalloc+tsksnap,,,ctaskms/M/ST:16000;

