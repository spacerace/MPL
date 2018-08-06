.c.obj:
   cl /c /AL /Gs /Oailt /Zp1 /W3 $*.c

tskalloc.obj:  tskalloc.c tsk.h tsklocal.h tskconf.h

tsksnap.obj:   tsksnap.c tsk.h tsklocal.h tskconf.h

tprt.obj:     tprt.c tsk.h prt.h tskconf.h

tprt.exe:     tprt.obj tskalloc.obj tsksnap.obj ctaskms.lib
   link tprt+tskalloc+tsksnap,,,ctaskms/M/ST:16000;


