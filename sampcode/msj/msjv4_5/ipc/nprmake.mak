#
# make file	for	nprmode.c
#

#COPT=/W3 /Zp /Zl /G2s /Ox /I$(INCLUDE)
COPT=/W3 /Zpiel /G2 /I$(INCLUDE)

nprmode.exe:	nprmode.c nprmode.mak
	cl $(COPT) nprmode.c /link /co

