QBMOUSE.OBJ:	QBDEMO.BAS
	QB QBDEMO /O;

QBDEMO.EXE:	QBDEMO.OBJ
	LINK QBDEMO+SUBS,,,BCOM20+..\MOUSE
