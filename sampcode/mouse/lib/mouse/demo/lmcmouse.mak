lmcmouse.obj:	lmcmouse.c
	msc /AL lmcmouse;

lmcmouse.exe:	lmcmouse.obj
	link lmcmouse+subs,,,..\mouse
