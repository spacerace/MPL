#
# make file	for	diserver.c
#

COPT=/Lp /W3 /Zpiel /G2s /I$(INCLUDE) /Alfw

errexit.obj: errexit.c errexit.h
	cl /c $(COPT) errexit.c


diserver.exe: diserver.c diserver.mak di.h errexit.obj
	cl $(COPT) diserver.c errexit /link /co /noi llibcmt
	markexe windowcompat diserver.exe





