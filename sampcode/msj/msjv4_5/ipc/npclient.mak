#
# make file	for	npclient.c
#

#COPT=/Lp /W3 /Zp /Zl /G2s /Ox /I$(INCLUDE) /Alfw
COPT=/Lp /W3 /Zpiel /G2s /I$(INCLUDE) /Alfw

npclient.exe:	npclient.c npclient.mak
	cl $(COPT) npclient.c /link /co /noi llibcmt
	markexe	windowcompat npclient.exe






