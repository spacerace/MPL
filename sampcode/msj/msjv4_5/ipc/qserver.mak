#
# make file	for	qserver.c
#

COPT=/Lp /W3 /Zpiel /G2s /I$(INCLUDE) /Alfw

qserver.exe: qserver.c qserver.mak q.h
	cl $(COPT) qserver.c /link /co /noi llibcmt
	markexe windowcompat qserver.exe

