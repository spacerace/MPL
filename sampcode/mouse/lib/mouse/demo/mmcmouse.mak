mmcmouse.obj:	mmcmouse.c
	msc /AM mmcmouse;

mmcmouse.exe:	mmcmouse.obj
	link mmcmouse+subs,,,..\mouse
