#
# make file for pmaccess.c
#

#COPT=/Lp /W3 /Zp /Zl /G2s /Ox /I$(INCLUDE) /Alfw
COPT=/Lp /W3 /Zpiel /G2s /I$(INCLUDE) /Alfw /Od


kbd.obj: kbd.c kbd.h msgs.h msgq.h kbddefs.h button.h
    cl $(COPT) /c kbd.c


mou.obj: mou.c mou.h msgs.h msgq.h moudefs.h button.h errexit.h
    cl $(COPT) /c mou.c


msgq.obj: msgq.c msgq.h
    cl $(COPT) /c msgq.c


button.obj: button.c button.h
    cl $(COPT) /c button.c


errexit.obj: errexit.c errexit.h
    cl /c $(COPT) errexit.c


pmaccess.obj: pmaccess.c pmaccess.h moudefs.h kbddefs.h msgq.h msgs.h mou.h kbd.h
    cl /c $(COPT) pmaccess.c


pmaccess.exe: pmaccess.c pmaccess.mak msgq.obj button.obj errexit.obj \
            mou.obj kbd.obj
    cl $(COPT) pmaccess msgq button errexit kbd mou /link /co /noi llibcmt
    markexe windowcompat pmaccess.exe




