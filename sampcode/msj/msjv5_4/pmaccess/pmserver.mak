#--------------------
# pmserver make file
#--------------------
COPT=/Lp /W3 /Zpiel /G2sw /I$(INCLUDE) /Od /Alfw

errexit.obj: errexit.c errexit.h
	cl /c $(COPT) errexit.c

msgq.obj: msgq.c msgq.h
	cl $(COPT) /c msgq.c

client.obj: client.c client.h pmsvmsgs.h
     cl /c $(COPT) client.c

pmserver.obj : pmserver.c pmserver.h pmsvmsgs.h msgq.h client.h
     cl /c $(COPT) pmserver.c

pmserver.exe : pmserver.obj pmserver.def pmserver.mak errexit.obj \
               msgq.obj client.obj
     link @pmserver.lnk

