#
# make file	for	ssem.c
#

COPT=/Lp /W3 /Zpiel /G2s /I$(INCLUDE) /Alfw

ssem.exe: ssem.c ssem.mak
	cl $(COPT) ssem.c /link /co /noi llibcmt
	markexe	windowcompat ssem.exe






