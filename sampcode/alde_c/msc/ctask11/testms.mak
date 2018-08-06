.c.obj:
   cl /c /AL /Gs /Oailt /Zp1 /W3 $*.c

tskalloc.obj:  tskalloc.c tsk.h tsklocal.h tskconf.h

tsksnap.obj:   tsksnap.c tsk.h tsklocal.h tskconf.h

test.obj:     test.c tsk.h tskconf.h

test.exe:     test.obj tskalloc.obj tsksnap.obj ctaskms.lib
   link test+tskalloc+tsksnap,,,ctaskms/M/ST:16000;
