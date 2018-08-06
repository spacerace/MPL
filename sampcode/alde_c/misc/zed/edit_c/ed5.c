/* ED5.C */

#include "ed0.c"
#include "ed1.ccc"
int fmtrev;
int fmttab;
int fmtdev;
int fmtwidth;
int fmtcol[MAXLEN1];
tablength()  
{
	return(fmttab);
}
fmtytab()
{
	fmtrev=YES;
}
fmtntab()
{
	fmtrev=NO;
}
fmtassn(listflag) int listflag;
{
	if (listflag==YES) {
		fmtdev=YES;
		fmtwidth=LISTW;
	}
	else {
		fmtdev=NO;
		fmtwidth=SCRNW1;
	}
}
fmtadj(buf,minind,maxind) char *buf; int minind,maxind;
{
int k;
	fmtcol[0]=0;
	k=minind;
	while (k<maxind) {
		if (buf[k]==CR) {
			break;
		}
		fmtcol[k+1]=fmtcol[k]+fmtchlen(buf[k],fmtcol[k]);
		k++;
	}
}
fmtlen(buf,i) char *buf; int i;
{
	return(fmtcol[i]);
}
fmtsubs(buf,i,j) char *buf; int i,j;
{
int k;
	if (fmtcol[i]>=fmtwidth) {
		return;
	}
	outxy(fmtcol[i],outyget());
	while (i<j) {
		if (buf[i]==CR) {
			break;
		}
		if (fmtcol[i+1]>fmtwidth) {
			break;
		}
		fmtoutch(buf[i],fmtcol[i]);
		i++;
	}
	if (fmtcol[i]<SCRNW1) {    /* modified to allow full screen width */
	outdeol();
	}
}
fmtsout(buf,offset) char *buf; int offset;
{
char c;
int col,k;
	col=0;
	while (c= *buf++) {
		if (c==CR) {
			break;
		}
		k=fmtchlen(c,col);
		if ((col+k+offset)>fmtwidth) {
			break;
		}
		fmtoutch(c,col);
		col=col+k;
	}
	return(col);		/* fmtsout() now returns column value */
}
fmtchlen(c,col) char c; int col;
{
	if (c==TAB) {
		return(fmttab-(col%fmttab));
	}
	else if ((c<32)&(fmtdev==NO)) {
		return(2);
	}
	else {
		return(1);
	}
}
fmtoutch(c,col) char c; int col;
{
int k;
	if (c==TAB) {
		k=fmtchlen(TAB,col);
		if ((fmtdev==NO)&(fmtrev==YES)) {
/*show tab characters in reverse video */
			scr_chr_attr(SETREVERSE);
		}
		while ((k--)>0) {
			fmtchdev(' ');
		}
		if ((fmtdev==NO)&(fmtrev==YES)) {
			scr_chr_attr(SETNORMAL);
		}
	}
	else if (c<32) {
/* show control characters in reverse video */
		if (fmtdev==NO) {
			scr_chr_attr(SETREVERSE);
			fmtchdev(c+64);
			scr_chr_attr(SETNORMAL);
		}
		else {
			fmtchdev('^');
			fmtchdev(c+64);
		}
	}
	else {
		fmtchdev(c);
	}
}
fmtchdev(c) char c;
{
	if (fmtdev==YES) {
		syslout(c);
	}
	else {
		outchar(c);
	}
}
fmtcrlf()
{
	if (fmtdev==YES) {
		syslout(CR);
		syslout(LF);
	}
	else {
		outxy(0,SCRNL1);
		syscout(CR);
		syscout(LF);
	}
}
fmtset(n) int n;
{
	fmttab=max(1,n);
}
