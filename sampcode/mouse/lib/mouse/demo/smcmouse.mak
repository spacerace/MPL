smcmouse.obj:	smcmouse.c
	msc smcmouse;

smcmouse.exe:	smcmouse.obj
	link smcmouse+subs,,,..\mouse
