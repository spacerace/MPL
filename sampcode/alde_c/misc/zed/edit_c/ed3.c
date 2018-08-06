/* ED3.C */

#include "ed0.c"
#include "ed1.ccc" 
int readfile = -1;	/* separate read and write files */
int writefile = -1;	/* -1 for inactive, file channel no. for active */
char rfilename[SYSFNMAX];	/* file names */
char wfilename[SYSFNMAX];
char databuf[MAXLEN1];		/* single buffer replaces redundant buffers */
fileclear()		/* initializes filenames to zero length */
{
	rfilename[0] = EOS;
	wfilename[0] = EOS;
}
append(args) char *args;
{
int file;
int n;
int topline;
char locfn[SYSFNMAX];
	if (name1(args,locfn)==ERR) {
		return;
	}
	if (locfn[0]==EOS) {
		message("no file argument");
		return;
	}
	if ((file=sysopen(locfn,"r"))==ERR) {
		message("file not found");
		return;
	}
	while ((n=readline(file,databuf,MAXLEN))>=0) {
		if (n>MAXLEN) {
			message("line truncated");
			n=MAXLEN;
		}
		if (bufins(databuf,n)==ERR) {
			break;
		}
		if (bufdn()==ERR) {
			break;
		}
	}
	sysclose(file);
	topline=max(1,bufln()-SCRNL2);
	bufout(topline,2,SCRNL2);
	bufgo(topline);
}
change(args,begin,end) char *args; int begin,end;
{
char oldline[MAXLEN1];
char newline[MAXLEN1];
char oldpat[MAXLEN1];
char newpat[MAXLEN1];
int from, to, col, n, k;
	if (get2args(args,&from,&to)==ERR) {
		if (check2mark(begin,end,&from,&to)==ERR){
			sysabort();
			return;
		}
	}
	scr_chr_attr(SETINTENSE);
	fmtsout("search mask ?	",0);
	getcmnd(oldpat,15);
	fmtcrlf();
	if(oldpat[0]==EOS) {
		return;
	}
	pmtline();
	scr_chr_attr(SETINTENSE);
	fmtsout("change mask ?	",0);
	getcmnd(newpat,15);
	fmtcrlf();
	while (from<=to) {
		if (chkkey()==YES) {
			break;
		}
		if (bufgo(from++)==ERR) {
			sysabort();
			break;
		}
		if (bufatbot()==YES) {
			sysabort();
			break;
		}
		n=bufgetln(oldline,MAXLEN);
		n=min(n,MAXLEN);
		oldline[n]=EOS;
		if (oldpat[0]=='^') {
			if (amatch(oldline,oldpat+1,NO)==YES) {
				k=replace(oldline,newline,
					oldpat+1,newpat,0);
				if (k==ERR) {
					sysabort();
					return;
				}
				fmtcrlf();
				putdec(bufln(),5);
				fmtsout(newline,5);
				outdeol();
				bufrepl(newline,k);
			}
			continue;
		}
		if ((col=amatch(oldline,oldpat,YES))>=0) {
			k=replace(oldline,newline,
				oldpat,newpat,col);
			if (k==ERR) {
				sysabort();
				return;
			}
			fmtcrlf();
			putdec(bufln(),5);
			fmtsout(newline,5);
			outdeol();
			bufrepl(newline,k);
		}
	}
	fmtcrlf();
}
clear()
{
	if (chkbuf()==YES) {
		outclr();
		outxy(0,SCRNL1);
		bufnew();
		message("buffer cleared");
		return(YES);
	}
	else{
		return(NO);
	}
}
delete(args,begin,end) char *args; int begin,end;
{
int from, to;
	if(get2args(args,&from,&to)==ERR) {
		if (check2mark(begin,end,&from,&to)==ERR){
			sysabort();
			return;
		}
	}
	if (from>to) {
		return;
	}
	if (edpick(from,to)==OK){
		message ("deleted lines saved in pick buffer");
	}
	if (bufgo(from)==ERR) {
		return;
	}
	if (bufndel(to-from+1)==ERR) {
		sysabort();
		return;
	}
	bufout(bufln(),1,SCRNL1);
}
find()
{
	return(suurch(bufln()+1,HUGE,YES));
}
list(args,begin,end) char *args; int begin,end;
{
int n;
int from, to, line, oldline;
	oldline=bufln();
	if (get2args(args,&from,&to)==ERR) {
		if (check2mark(begin,end,&from,&to)==ERR) {
			sysabort();
			return;
		}
	}
	line=from;
	while (line<=to) {
		fmtassn(NO);
		if (chkkey()==YES) {
			break;
		}
		fmtassn(YES);
		if (bufgo(line++)!=OK) {
			break;
		}
		if (bufatbot()) {
			sysabort();
			break;
		}
		n=bufgetln(databuf,MAXLEN1);
		n=min(n,MAXLEN);
		databuf[n]=CR;
		fmtsout(databuf,0);
		fmtcrlf();
	}
	fmtassn(NO);
	bufgo(oldline);
}
extract(args,begin,end) char *args; int begin,end;
	/* write indicated line range to writefile */
{
/*char args1[SCRNW1];
char *argp;
int file;
char locfn[SYSFNMAX];	*/
int oldline, from, to, n;
	if (writefile == -1) {
		message("file not opened");
		return;
	}
	oldline = bufln();
	if (get2args(args,&from,&to)==ERR) {/* from and to are line range */
		if (check2mark(begin,end,&from,&to)==ERR){
			return;
		}
	}
/*	message("enter file name");  */ /* request the file name */
/*	getcmnd(args1,0);
	argp=skipbl(args1);
	if (syschkfn(argp)==ERR) {
		return;
	}
	syscopfn(argp,locfn);
	if (locfn[0]==EOS) {
		message("no file argument");
		return;
	}
	if ((file=sysopen(locfn,"w"))==ERR){
		return;
	}	*/
	if (bufgo(from)==ERR){
	   /*	  sysclose(file);	*/
		return;
	}
	while ((bufatbot()==NO)&(bufln()<=to)) {	/* do the writing */
		n=bufgetln(databuf,MAXLEN);
		n=min(n,MAXLEN);
		if (pushline(writefile,databuf,n)==ERR){
			break;
		}
		if (bufdn()==ERR) {
			break;
		}
	}
	bufgo(oldline);
/*	  sysclose(file);	*/  /* close the file */
}
openf (args,flag) char *args; int flag;
{
/* open a file for reading */
char locfn [SYSFNMAX];
int n;
int file;
int topline;
	if (readfile > 0) {		/* check for open readfile */
		message("read file still open");
		return;
	}
	if (flag==YES) {
		if (name1(args,locfn)==ERR) {
			return;
		}
	}
	else  {
		if (name3(args,locfn)==ERR) {
			return;
		}
	}
	if (locfn[0]==EOS) {
		message("no file argument");
		return;
	}
	if (chkbuf()==NO) {
		return;
	}
	if ((file=sysopen(locfn,"r"))==ERR) {
		message("file not found");
		return;
	}
	syscopfn(locfn, rfilename);	/* make the file name the readfile */
	readfile = file;
	pmtrfile(rfilename);
	bufnew();
}
getit (args) char *args;	/* new: add n lines to buffer if room */
{
int n;
int topline;
int nlines,npoint;
	if (readfile == -1) {
		message ("no read file");
		return;
	}
	pmtrfile(rfilename);
	if (getarg(args,&nlines)==ERR) {
		return;
	}
	if (nlines<1) {
		return;
	}
	if (bufgo(HUGE)==ERR) {
		return;
	}
	npoint=1;
	while (npoint<=nlines) {	/* add the lines */
		npoint++;
		if ((n=readline(readfile,databuf,MAXLEN))>=0) {
			if (n>MAXLEN) {
				message("line truncated");
				n=MAXLEN;
			}
			if (buf1ins(databuf,n)==ERR) {
				bufgo(1);
				topline=max(1,bufln()-SCRNL2);
				bufout(topline,2,SCRNL2);
				bufgo(topline);
				return;
			}
			if (bufdn()==ERR) {
				break;
			}
		}
		else {
			npoint = HUGE;	   /* reached end of file - close it */
			sysclose(readfile);
			readfile = -1;
			rfilename[0] = EOS;
			pmtrfile(rfilename);
		}
	}
	bufgo(1);
	topline=max(1,bufln()-SCRNL2);
	bufout(topline,2,SCRNL2);
	bufgo(topline);
}
rest (args) char *args; /* new: try to read rest of readfile into buffer */
{
int n;
int topline;
	if (readfile == -1) {
		message ("no read file");
		return;
	}
	pmtrfile(rfilename);
/* if buffer has been changed and not saved, give option of clearing, */
/* otherwise add to end of buffer */
	if(bufchng()==YES) {
		scr_chr_attr(SETINTENSE);

		fmtsout("buffer not saved. clear? ",0);
		scr_chr_attr(SETFAINT);
		pmtline();
		if (tolower(syscout(syscin()))=='y'){
			outclr();
			outxy(0,SCRNL1);
			bufnew();
			message("buffer cleared");
		}
		else {
			bufgo(HUGE);
		}
	}
	else{
		outclr();
		outxy(0,SCRNL1);
		bufnew();
		message("buffer cleared");

	}
	while ((n=readline(readfile,databuf,MAXLEN))>=0) {
		if (n>MAXLEN) {
			message("line truncated");
			n=MAXLEN;
		}
		if (buf1ins(databuf,n)==ERR) {
			bufgo(1);
			topline=max(1,bufln()-SCRNL2);
			bufout(topline,2,SCRNL2);
			bufgo(topline);
			return;
		}
		if (bufdn()==ERR) {
			break;
		}
	}
	sysclose(readfile);	/* close file if all read */
	readfile = -1;
	rfilename[0] = EOS;
	pmtrfile(rfilename);
	bufgo(1);
	topline=max(1,bufln()-SCRNL2);
	bufout(topline,2,SCRNL2);
	bufgo(topline);
}
rename(args) char *args;	/* new: change name of writefile */
{
	if (writefile == -1) {
		message("no write file - call name");
		return;
	}
	sysclose(writefile);
	writefile = -1;
	wfilename[0]=EOS;
	pmtwfile(wfilename);
	if (name0(args,wfilename)==ERR) {
		return;
	}
	pmtwfile(wfilename);
}
name(args) char *args;		/* altered definition: name the writefile */
{
	if (writefile > 0) {
		message("write file open");
		return;
	}
	if (name0(args,wfilename)==ERR){
		return;
	}
	pmtwfile(wfilename);
}
name0(args,wfilename) char *args, *wfilename;	/* names new writefile */
{
int file;
	args=skiparg(args);
	args=skipbl(args);
	if (syschkfn(args)==ERR) {
		return(ERR);
	}
	syscopfn(args,wfilename);
	if ((file=sysopen(wfilename,"r"))!=ERR) {
		sysclose(file);
		message("disk file exists");
		return(ERR);
	}
	if ((file=syscreate(wfilename))==ERR) {
		return(ERR);
	}
	writefile = file;
	return(OK);
}
delname(args) char *args;	/* new: similar to old resave command, */
{				/* but doesn't automatically write file */
	if (writefile > 0) {
		message("write file open");
		return;
	}
	if (name2(args,wfilename)==ERR){
		return;
	}
	pmtwfile(wfilename);
}
name2(args,wfilename) char *args, *wfilename;	/* open writefile: no error */
{						/* file exists */
int file;
	args=skiparg(args);
	args=skipbl(args);
	if (syschkfn(args)==ERR) {
		return(ERR);
	}
	syscopfn(args,wfilename);
	if ((file=sysopen(wfilename,"w"))==ERR) {
		return(ERR);
	}
	writefile = file;
	return(OK);
}
name1(args,filename) char *args, *filename;
{
	args=skiparg(args);
	args=skipbl(args);
	if (syschkfn(args)==ERR) {
		return(ERR);
	}
	syscopfn(args,filename);
	return(OK);
}
name3(args,filename) char *args, *filename;
{
	if (syschkfn(args)==ERR) {
		return(ERR);
	}
	syscopfn(args,filename);
	return(OK);
}
count(args) char *args;  /* get repeat count for buffer macro replay */
{
int n;
	if (getarg(args,&n)==ERR) {
		return;
	}
	syscount(n);
}
writel(args) char *args;	/* new: write first n lines of buffer to */
{				/* writefile and delete from buffer */
int n, to;
	if (writefile == -1) {
		message("file not opened");
		return;
	}
	if (getarg(args,&to)==ERR) {
		return;
	}
	if (bufgo(1)==ERR) {
		sysclose(writefile);
		writefile = -1;
		wfilename[0] = EOS;
		pmtwfile(wfilename);
		return;
	}
	while ((bufatbot()==NO)&(bufln()<=to)) {
		n=bufgetln(databuf,MAXLEN);
		n=min(n,MAXLEN);
		if (pushline(writefile,databuf,n)==ERR) {
			return;
		}
		if (bufdn()==ERR) {
			return;
		}
	}
	if (bufgo(1)==ERR) {
		return;
	}
	if (bufndel(to)==ERR){
		return;
	}
	bufout(bufln(),1,SCRNL1);
	if (bufatbot()) {
		bufsaved();
	}
	bufgo(1);
}
save()
{
int n, oldline;
	if (writefile == -1) {			 /* changed to writefile */
		message("file not opened");	 /* file assumed already open */
		return;
	}
	oldline=bufln();
	if (bufgo(1)==ERR) {
		sysclose(writefile);	/* close the writefile if error */
		writefile = -1;
		wfilename[0]=EOS;
		pmtwfile(wfilename);
		return;
	}
	while (bufatbot()==NO) {
		n=bufgetln(databuf,MAXLEN);
		n=min(n,MAXLEN);
		if (pushline(writefile,databuf,n)==ERR) {
			break;
		}
		if (bufdn()==ERR) {
			break;
		}
	}
	if (bufatbot()) {
		bufsaved();
	}
	bufgo(oldline);
					/* writefile no longer closed */
}
closeread()				/* new: close the readfile */
{
	if (readfile!= -1) {
		sysclose(readfile);
		readfile= -1;
		rfilename[0]=EOS;
		pmtrfile(rfilename);
	}
}
closewrite()				/* new: close the writefile */
{
	if (writefile != -1) {
		sysclose(writefile);
		writefile= -1;
		wfilename[0]=EOS;
		pmtwfile(wfilename);
	}
}
search (args,begin,end) char *args; int begin,end;
{
int from, to;
	if (get2args(args,&from,&to)==ERR) {
		if (check2mark(begin,end,&from,&to)==ERR) {
			sysabort();
			return;
		}
	}
	suurch(from,to,NO);
}
suurch(from,to,flag) int from, to, flag;   /* old "search1" */
{
					/* now uses module buffer */
char pat [MAXLEN1];
int col, n;
	scr_chr_attr(SETINTENSE);
	fmtsout("search mask ?	",0);
	getcmnd(pat,15);
	fmtcrlf();
	if (pat[0]==EOS) {
		return (-1);
	}
	while (from<=to) {
		if (chkkey()==YES) {
			break;
		}
		if (bufgo(from++)==ERR) {
			sysabort();
			break;
		}
		if (bufatbot()==YES) {
			sysabort();
			break;
		}
		n=bufgetln(databuf,MAXLEN);
		n=min(n,MAXLEN);
		databuf[n]=EOS;
		if (pat[0]=='^') {
			if (amatch(databuf,pat+1,NO)==0) {
				if (flag==NO) {
					fmtcrlf();
					putdec(bufln(),5);
					fmtsout(databuf,5);
					outdeol();
				}
				else {
					return(0);
				}
			}
			continue;
		}
		if ((col=amatch(databuf,pat,YES))>=0) {
			if (flag==NO) {
				fmtcrlf();
				putdec(bufln(),5);
				fmtsout(databuf,5);
				outdeol();
			}
			else {
				return(col);
			}
		}
	}
	if (flag==YES) {
		return(-1);
	}
	else {
		fmtcrlf();
	}
}
tabs(args) char *args;
{
int n,junk;
	if (get2args(args,&n,&junk)==ERR) {
		sysabort();
		return;
	}
	fmtset(n);
}
chkbuf()
{
	if (bufchng()==NO) {
		return(YES);
	}
	scr_chr_attr(SETINTENSE);
	fmtsout("Buffer not saved.  Proceed  ? ",0);
	scr_chr_attr(SETFAINT);
	pmtline();
	if (tolower(syscout(syscin()))!='y') {
		fmtcrlf();
		message("cancelled");
		return(NO);
	}
	else {
		fmtcrlf();
		return(YES);
	}
}
message(s) char *s;
{
		scr_chr_attr(SETINTENSE);
		fmtsout(s,0);
		fmtcrlf();
		scr_chr_attr(SETFAINT);
}
getarg(args,val) char *args; int *val;	/* new: get a single argument */
{
	args=skiparg(args);
	args=skipbl(args);
	if (*args==EOS) {
		*val=1;
		return(ERR);
	}
	if (number(args,val)==NO) {
		message("bad argument");
		return(ERR);
	}
	return(OK);
}
/* new: get three arguments						*/
get3args(args,val1,val2,val3) char *args; int *val1, *val2,*val3;
{
	args=skiparg(args);
	args=skipbl(args);
	if (*args==EOS) {
		return(ERR);
	}
	if (number(args,val1)==NO) {
		message("bad argument");
		return(ERR);
	}
	args=skiparg(args);
	args=skipbl(args);
	if(*args==EOS) {
		return(ERR);
	}
	if (number(args,val2)==NO) {
		message("bad argument");
		return(ERR);
	}
	args=skiparg(args);
	args=skipbl(args);
	if(*args==EOS) {
		return(ERR);
	}
	if (number(args,val3)==NO) {
		message("bad argument");
		return(ERR);
	}
	else {
		return(OK);
	}
}
get2args(args,val1,val2) char *args; int *val1, *val2;
{
	args=skiparg(args);
	args=skipbl(args);
	if (*args==EOS) {
/*		*val1=1;
		*val2=HUGE;   */
		return(ERR);
	}
	if (number(args,val1)==NO) {
		message("bad argument");
		return(ERR);
	}
	args=skiparg(args);
	args=skipbl(args);
	if(*args==EOS) {
	/*	*val2=HUGE;  */
		return(ERR);
	}
	if (number(args,val2)==NO) {
		message("bad argument");
		return(ERR);
	}
	else {
		return(OK);
	}
}
skiparg(args) char *args;
{
	while ((*args!=EOS)&(*args!=' ')) {
		args++;
	}
	return(args);
}
skipbl(args) char *args;
{
	while(*args==' ') {
		args++;
	}
	return(args);
}
chkkey()
{
int c;
	c=syscstat();
	if (c==-1) {		/* note correction from original version */
		return(NO);
	}
	else if (c==' ') {
		pmtline();
		if (syscin()==' ') {
			return(NO);
		}
	}
	return(YES);
}
/*
amatch(line,pat,col) char *line, *pat; int col;
{
int k;
	k=0;
	while (pat[k]!=EOS) {
		if (pat[k]==line[col]) {
			k++;
			col++;
		}
		else if ((pat[k]=='?')&(line[col]!=EOS)) {
			k++;
			col++;
		}
		else {
			return(NO);
		}
	}
	return(YES);
}
*/
replace(oldline,newline,oldpat,newpat,col)
char *oldline, *newline, *oldpat, *newpat; int col;
{
int k;
char *tail, *pat;
	k=0;
	while (k<col) {
		newline[k++]= *oldline++;
	}
	tail=oldline;
	pat=oldpat;
	while (*pat++!=EOS) {
		tail++;
	}
	while (*newpat!=EOS) {
		if (k>MAXLEN-1) {
			message("new line too long");
			sysabort();
			return(ERR);
		}
		if (*newpat!='?') {
			newline[k++]= *newpat++;
			continue;
		}
		while (*oldpat!='?') {
			if (*oldpat==EOS) {
				message(
				"too many ?'s in change mask"
				);
				sysabort();
				return(ERR);
			}
			oldpat++;
			oldline++;
		}
		newline[k++]= *oldline++;
		oldpat++;
		newpat++;
	}
	while (*tail!=EOS) {
		if (k>MAXLEN-1) {
			message("new line too long");
			sysabort();
			return(ERR);
		}
		newline[k++]= *tail++;
	}
	newline[k]=EOS;
	return(k);
}
copyit (args,begin,end,dest) char *args; int begin,end,dest;
	/* new: copies "lenn" lines from "from"   */
{				/* to "to" */
int from,to,lenn;
int topline;
	if (get3args(args,&from,&to,&lenn)==ERR) {
		if (check3mark(begin,end,dest,&from,&to,&lenn)==ERR) {
			sysabort();
			return;
		}
	}
	if (to == from) {
		return;
	}
	if ((to>from)&(to<=(from+lenn))) {
		message("interleaving not permitted");
		sysabort();
		return;
	}
	bufcopy(from,to,lenn);
	bufgo(1);
	topline=max(1,bufln()-SCRNL2);
	bufout(topline,2,SCRNL2);
	bufgo(topline);
}
moveit (args,begin,end,dest) char *args; int begin,end,dest;

	/* new: moves "lenn" lines from "from" */
{				/* to "to" - same as copyit except deletes */
int from,to,lenn;		/* old lines */
int topline;
	if (get3args(args,&from,&to,&lenn)==ERR) {
		if (check3mark(begin,end,dest,&from,&to,&lenn)==ERR) {
			sysabort();
			return;
		}
	}
	if (to == from) {
		return;
	}
	if ((to>from)&(to<=(from+lenn))) {
		message("interleaving not permitted");
		sysabort();
		return;
	}
	bufcopy(from,to,lenn);
	if (to>from){
		bufgo(from);
		bufndel(lenn);
	}
	else {
		bufgo(from+lenn);
		bufndel(lenn);
	}
	bufgo(1);
	topline=max(1,bufln()-SCRNL2);
	bufout(topline,2,SCRNL2);
	bufgo(topline);
}
check2mark(begin,end,from,to) int begin,end,*from,*to;
{
	if ((begin==(-1))|(end==(-1))){
		return(ERR);
	}
	if	(begin>end) {
		return(ERR);
	}
	*from=begin;
	*to=end;
	return(OK);
}
check3mark(begin,end,dest,from,to,lenn) int begin,end,dest;
	int *from,*to,*lenn;
{
	if ((begin==(-1))|(end==(-1))){
		return(ERR);
	}
	if (begin>end) {
		return(ERR);
	}
	*from=begin;
	*to=dest;
	*lenn=end-begin;
}






