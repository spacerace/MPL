/* ED7.C */

#include "ed0.c"
#include "ed1.ccc"
char pmtln[MAXLEN];
char pmtrfn[SYSFNMAX];		/* now both read and write filenames */
char pmtwfn[SYSFNMAX];
pmtmess(s1,s2) char *s1, *s2;
{
int x,y;
	x=outxget();
	y=outyget();
	outxy(0,0);
	outdelln();
	pmt1line();
	pmt1col(x);
	scr_chr_attr(SETINTENSE);
	fmtsout(s1,outxget());
	fmtsout(s2,outxget());
	scr_chr_attr(SETFAINT);
	syscin();
	pmt1line();
	pmt1col(x);
	pmt1file(pmtrfn);
	pmt2file(pmtwfn);
	pmt1mode(pmtln);
	outxy(x,y);
}
pmtmode(s) char *s;
{
int x,y;
	x=outxget();
	y=outyget();
	outxy(0,0);
	outdelln();
	pmt1line();
	pmt1col(x);
	pmt1file(pmtrfn);
	pmt2file(pmtwfn);
	pmt1mode(s);
	outxy(x,y);
}
pmtrfile(s) char *s;	/*changed: readfile */
{
int x,y;
	x=outxget();
	y=outyget();
	outxy(0,0);
	outdelln();
	pmt1line();
	pmt1col(x);
	pmt1file(s);
	pmt2file(pmtwfn);  /* add writefile */
	pmt1mode(pmtln);
	outxy(x,y);
}
pmtwfile(s) char *s;	/* new: writefile */
{
int x,y;
	x=outxget();
	y=outyget();
	outxy(0,0);
	outdelln();
	pmt1line();
	pmt1col(x);
	pmt1file(pmtrfn);
	pmt2file(s);
	pmt1mode(pmtln);
	outxy(x,y);
}
pmtedit()
{
	pmtmode("edit:	 ");	/* add blanks to assure erasing "command" */
}
pmtline()
{
int x,y;
	x=outxget();
	y=outyget();
	outxy(0,0);
	outdelln();
	pmt1line();
	pmt1col(x);
	pmt1file(pmtrfn);	/* change: separate read and write files */
	pmt2file(pmtwfn);
	pmt1mode(pmtln);
	outxy(x,y);
}
pmtcol()
{
int x,y;
	x=outxget();
	y=outyget();
	pmt1col(x);
	outxy(x,y);
}
pmtcmnd(mode,buffer) char *mode, *buffer;
{
int x,y;
	x=outxget();
	y=outyget();
	pmt1mode(mode);
	getcmnd(buffer,outxget());
}
pmt1mode(s) char *s;
{
int i;
	outxy(60,0);		/* change of column location */
	scr_chr_attr(SETINTENSE);
	fmtsout(s,60);
	scr_chr_attr(SETFAINT);
	i=0;
	while (pmtln[i++]= *s++) {
		;
	}
}
pmt1file(s) char *s;
{
int i;
	outxy(25,0);
	scr_chr_attr(SETINTENSE);
	if (*s==EOS) {
		fmtsout("no rdfile",25);	/* change of message */
	}
	else {
		fmtsout(s,25);
	}
	scr_chr_attr(SETFAINT);
	i=0;
	while (pmtrfn[i++]= *s++) {
		;
	}
}
pmt2file(s) char *s;	/* new: write out writefile name or "no wrtfile" */
{
int i;
	outxy(40,0);
	scr_chr_attr(SETINTENSE);
	if (*s==EOS) {
		fmtsout("no wrtfile",40);
	}
	else {
		fmtsout(s,40);
	}
	scr_chr_attr(SETFAINT);
	i=0;
	while (pmtwfn[i++]= *s++) {
		;
	}
}
pmt1line()
{
	outxy(0,0);
	scr_chr_attr(SETINTENSE);
	fmtsout("line: ",0);
	putdec(bufln(),5);
	scr_chr_attr(SETFAINT);
}
pmt1col(x) int x;
{
	outxy(12,0);
	scr_chr_attr(SETINTENSE);
	fmtsout("column: ",12);
	putdec(x,3);
	scr_chr_attr(SETFAINT);
}
