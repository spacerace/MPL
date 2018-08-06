#
# make file	for	diclient.c
#

COPT=/Lp /W3 /Zpiel /G2s /I$(INCLUDE) /Alfw

diclient.exe: diclient.c diclient.mak dirinfo.h
	cl $(COPT) diclient.c /link /co /noi llibcmt
	markexe windowcompat diclient.exe




