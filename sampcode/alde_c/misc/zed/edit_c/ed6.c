/* ED6.C */

#include "ed0.c"
#include "ed1.ccc"
int outx,outy;
outxget()
{
	return(outx);
}
outyget()
{
	return(outy);
}
outchar(c) char c;
{
	syscout(c);
	outx++;
	return(c);
}
outxy(x,y) int x,y;
{
	outx=x;
	outy=y;
	scr_rowcol(y,x);
}
outclr()
{
	scr_clr();
}
outdelln()
{
	outxy(0,outy);
	outdeol();
}
outdeol()
{
	scr_clrl();
}
outuphas()
{
	return(YES);
}
outdnhas()
{
	return(YES);
}
outsup()
{
	outxy(0,SCRNL1);
	sysdelay();
	scr_scup(); 
}
outsdn()
{
	outxy(0,0);
	sysdelay();
	scr_scdn();
}
