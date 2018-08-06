lmcega.obj:	lmcega.c
	msc /AL /Ze lmcega;

lmcega.exe:	lmcega.obj
	link lmcega,,,..\ega;
