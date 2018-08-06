#
# make file	for	qclient.c
#

COPT=/Lp /W3 /Zpiel /G2s /I$(INCLUDE) /Alfw

qclient.exe: qclient.c qclient.mak q.h
	cl $(COPT) qclient.c /link /co /noi llibcmt
	markexe windowcompat qclient.exe



