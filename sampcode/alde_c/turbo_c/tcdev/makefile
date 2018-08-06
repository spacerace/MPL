.asm.obj:
	masm /mx /D__TINY__ $*;

.c.obj:
	tcc -c -mt -O -Z $*

mono.sys: mono.obj tcdev.obj
	tlink tcdev + mono,mono,,c:\turboc\lib\cs/m;
	exe2bin mono.exe mono.sys
