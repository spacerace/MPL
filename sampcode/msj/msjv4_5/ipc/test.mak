#
# make file	for	test.c
#

COPT=/Lp /W3 /Zpiel /G2s /I$(INCLUDE) /Alfw

test.exe: test.c test.mak
	cl $(COPT) test.c /link /co /noi llibcmt
	markexe windowcompat test.exe









