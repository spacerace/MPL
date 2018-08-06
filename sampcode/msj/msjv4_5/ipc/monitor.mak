#
# make file	for	monitor.c
#

COPT=/Lp /W3 /Zpiel /G2s /I$(INCLUDE) /Alfw

monitor.exe: monitor.c monitor.mak
	cl $(COPT) monitor.c /link /co /noi llibcmt
	markexe	windowcompat monitor.exe







