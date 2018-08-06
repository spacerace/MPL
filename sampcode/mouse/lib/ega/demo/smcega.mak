smcega.obj:	smcega.c
	msc smcega;

smcega.exe:	smcega.obj
	link smcega,,,..\ega;
