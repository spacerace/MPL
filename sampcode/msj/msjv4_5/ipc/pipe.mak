#
# make file	for	pipe.c
#

COPT=/Lp /W3 /Zpiel /G2s /I$(INCLUDE) /Alfw

pipe.exe: pipe.c pipe.mak
	cl $(COPT) pipe.c /link /co /noi llibcmt
	markexe windowcompat pipe.exe








