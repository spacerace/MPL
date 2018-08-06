cmcmouse.obj:	cmcmouse.c
	msc /AC cmcmouse;

cmcmouse.exe:	cmcmouse.obj
	link cmcmouse+subs,,,..\mouse
