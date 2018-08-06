#
# make file	for	dipop.c
#

#COPT=/Lp /W3 /Zp /Zl /G2s /Ox /I$(INCLUDE)	/Alfw
COPT=/Lp /W3 /Zpiel /G2s /I$(INCLUDE) /Alfw	 /Od

di.obj: di.c di.h
	cl $(COPT) /c di.c


msgq.obj: msgq.c msgq.h
	cl $(COPT) /c msgq.c


button.obj: button.c button.h
	cl $(COPT) /c button.c


errexit.obj: errexit.c errexit.h
	cl /c $(COPT) errexit.c


dipop.exe: dipop.c dipop.mak di.obj di.h moudefs.h kbddefs.h \
			dipop.h msgq.h msgq.obj button.obj button.h errexit.obj errexit.h
	cl $(COPT) dipop.c di msgq button errexit /link /co /noi llibcmt
	markexe	windowcompat dipop.exe




