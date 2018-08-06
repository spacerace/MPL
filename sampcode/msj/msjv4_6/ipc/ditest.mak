#
# make file	for	dipop, diserver and ls
#

COPT=/Lp /W3 /Zpiel /G2s /I$(INCLUDE) /Alfw /DSTUB /FoDITEST /Od





errexit.obj: errexit.c errexit.h
	cl /c $(COPT) errexit.c

di.exe: di.c di.mak di.h errexit.obj
	cl $(COPT) di.c errexit /link /co /noi llibcmt
	markexe windowcompat di.exe





