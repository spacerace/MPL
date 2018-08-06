COPT=/Lp /W3 /Zpiel /G2s /I$(INCLUDE) /Alfw

sig.exe: sig.c sig
	cl $(COPT) sig.c /link /co /noi llibcmt
	markexe windowcompat sig.exe






