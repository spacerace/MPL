.c.obj:
   cl /c /AL /Gs /Oailt /Zp1 /Zl /W3 $*.c
   lib ctaskms.lib -+$*.obj;

.asm.obj:
   masm $*/MX;
   lib ctaskms.lib -+$*.obj;

tskmain.obj:   tskmain.c tsk.h tsklocal.h tskconf.h

tsksub.obj:    tsksub.c tsk.h tsklocal.h tskconf.h

tskflg.obj:    tskflg.c tsk.h tsklocal.h tskconf.h

tskcnt.obj:    tskcnt.c tsk.h tsklocal.h tskconf.h

tskrsc.obj:    tskrsc.c tsk.h tsklocal.h tskconf.h

tskmsg.obj:    tskmsg.c tsk.h tsklocal.h tskconf.h

tskpip.obj:    tskpip.c tsk.h tsklocal.h tskconf.h

tskwpip.obj:   tskwpip.c tsk.h tsklocal.h tskconf.h

tskbuf.obj:    tskbuf.c tsk.h tsklocal.h tskconf.h

tsksio.obj:    tsksio.c tsk.h sio.h tskconf.h

tskprt.obj:    tskprt.c tsk.h prt.h tskconf.h

tskasm.obj:    tskasm.asm tsk.mac

tsktim.obj:    tsktim.asm tsk.mac

tskdos.obj:    tskdos.asm tsk.mac

tskbios.obj:   tskbios.asm tsk.mac

tskkbd.obj:    tskkbd.asm tsk.mac

