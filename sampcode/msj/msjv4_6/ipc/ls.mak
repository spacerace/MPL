#
# make file	for	ls.c
#

COPT=/Lp /W3 /Zpiel /G2s /I$(INCLUDE) /Alfw /J

di.obj: di.c di.h
	cl $(COPT) /c di.c

errexit.obj: errexit.c errexit.h
	cl /c $(COPT) errexit.c

ls.exe: ls.c ls.mak di.obj di.h errexit.c errexit.h
	cl $(COPT) ls.c di errexit /link /co /noi llibcmt
	markexe windowcompat ls.exe



