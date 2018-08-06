#--------------------
# pmserver make file
#--------------------
COPT=/Lp /W3 /Zpiel /G2sw /I$(INCLUDE) /Od /Alfw

errexit.obj: errexit.c errexit.h
	cl /c $(COPT) errexit.c

msgq.obj: msgq.c msgq.h
	cl $(COPT) /c msgq.c

pmserver.obj : pmserver.c pmserver.h
     cl /c $(COPT) pmserver.c

pmserver.exe : pmserver.obj pmserver.def errexit.obj msgq.obj
     link pmserver errexit msgq, /align:16 /co, NUL,os2 llibcmt, pmserver

