#
# make file	for	npserver.c
#

#COPT=/Lp /W3 /Zp /Zl /G2s /Ox /I$(INCLUDE)	/Alfw
COPT=/Lp /W3 /Zpiel /G2s /I$(INCLUDE) /Alfw

npserver.exe:	npserver.c npserver.mak
	cl $(COPT) npserver.c /link /co /noi llibcmt
	markexe	windowcompat npserver.exe






