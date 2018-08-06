#--------------------------------------
# CHECKERS.MAK make file, Version 0.40
#--------------------------------------

CC = cl -c -G2s -MT -Ow -W3 -Zi

checkers.exe : checkers.obj ckrjudge.obj ckrboard.obj \
               ckrmoves.obj ckrdraw.obj  ckrstrat.obj \
               ckrsetup.obj checkers.def checkers.res
     link @checkers.lnk
     rc checkers.res checkers.exe

checkers.obj : checkers.c checkers.h
     $(CC) checkers.c

ckrjudge.obj : ckrjudge.c checkers.h ckrmoves.h ckrstrat.h
     $(CC) ckrjudge.c

ckrboard.obj : ckrboard.c checkers.h ckrdraw.h
     $(CC) ckrboard.c

ckrmoves.obj : ckrmoves.c checkers.h ckrmoves.h
     $(CC) ckrmoves.c

ckrdraw.obj  : ckrdraw.c  checkers.h ckrdraw.h
     $(CC) ckrdraw.c

ckrstrat.obj : ckrstrat.c ckrstrat.h ckrmoves.h
     $(CC) ckrstrat.c

ckrsetup.obj : ckrsetup.c checkers.h ckrdraw.h
     $(CC) ckrsetup.c

checkers.res : checkers.rc checkers.h ckruhand.ptr ckrdhand.ptr
     rc -r checkers
