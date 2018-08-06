/*
>>:yam7.c 04-Mar-84 (diff 08-11-81)
 * Logfile sprinf fixed to write correct length record
 * closerx fixed so as not to pad a file abnormal due to write errors
 * File open and close stuff and more ...
 * This file assumes operation on a CP/M system
 */

#include <YAM.H>

/* dpb dph blocks from CP/M Interface Guide Sect 6.5 */

struct dpb {	/* CP/M Version 2 Disk Parameter Block */
	unsigned dpb_spt;	/* sectors per track */
	char dpb_bsh;		/* block shift factor */
	char dpb_blm;
	char dpb_exm;		/* Extent Mask */
	unsigned dpb_dsm;	/* Highest block number on this disk */
	unsigned dpb_drm;	/* total number of directory entries -1 */
	unsigned dpb_al;	/* bit field corresponding to direc blocks */
	unsigned dpb_cks;	/* size of the directory check vector */
	unsigned dpb_off;	/* number of reserved tracks on this disk */
};

char *call();

struct dph {	/* CP/M Version 2 Disk Parameter Header */
	char **dph_xlt;		/* logical to physical xlat vector */
	int dph_ooo;
	char *dph_dirbuf;	/* 128 byte scratchpad for directory use */
	struct dpb *dph_dpb;	/* disk param block for this type of disk */
	char **dph_csv;		/* scratch area for detecting changed disks */
	char *dph_alv;		/* pointer to bit vector alloc map for disk */
};

struct fcb {	/* CP/M Version 2 fcb AS SEEN BY THE USER */
	char	dr;		/* drive number */
	char	fname[8];	/* fname[1] used by TAG2 */
	char	ftype[3];	/* ftype[1] 8th bit set for $SYS */
	char	ex;		/* file extent normally 0 */
	char	s1;		/* reserved for bdos's benefit */
	char	s2;		/* likewise, =0 on call to open,make,search */
	char	rc;		/* record count for extent[ex]  0...128 */
	char	dmap[16];
	char	cr;		/* current record, initialized to 0 by usr */
	unsigned recn;		/* highest record number */
	char	recovf;		/* overflow of above */
};

/* following are BIOS calls */
#define CONST 2			/* bios console char ready */
#define CONIN 3			/* bios cons char input */
#define CONOUT 4		/* bios cons char output */

/* following are BDOS calls */
#define BDOS 5			/* address used to call bdos */
#define SELDSK 14		/* bdos select disk 0=default disk */
#define SRCH 17 		/* bdos search for file pattern*/
#define SRCHNXT 18		/* search for next occurrence */
#define GETDEFDISK 25		/* get current disk  (0-15) */
#define SETDMA 26		/* set address for read, write, etc. */
#define GETALLOCP 27		/* get address of allocation vector */
#define SETATTRIB 30		/* update file attributes */
#define GETDPBP 31		/* get DPB address for disk */
#define SETGETUSER 32		/* set or get user number */
#define COMPFILSIZ 35		/* compute file size into recn and recovf */
#define UFNSIZE 15		/* a:foobar12.urk\0 is 15 chars */

openrx(name)
char *name;
{

#ifdef RESTRICTED
	char *s;
	if(s=cisubstr(name, ".com"))	/* upload .com files as .obj */
		strcpy(s, ".OBJ");
	if(cisubstr(name, "$$$"))
		return ERROR;		/* don't allow upload of $$$.sub */
	for(s=name; *s; )
		if(*s++ > 'z')
			return ERROR;	/* no garbage names please */
#endif
	unspace(name);
	lprintf("'%s' ", name);		/* show the name right away */
	if(!Creamfile && fopen(name, fout) != ERROR) {
		fclose(fout);
		printf("Exists ", name);
#ifdef XMODEM
		return ERROR;
#else
		printf("Replace it (y/n)??");
		if(tolower(getchar())!= 'y')
			return ERROR;
#endif
	}
	if(fcreat(name, fout)==ERROR){
		printf("Can't create %s\n", name);
		return ERROR;
	}
	Rfile= TRUE;
	strcpy(Rname, name);
	Dumping= !Squelch;
	lprintf("Created%s\n", Dumping? "" : " recording OFF");
	return OK;
}

closerx(pad)
{
	if(Rfile) {
		if(!Quiet)
			lpstat("Closing %s", Rname);
#ifdef BDSC
		if(pad && (fout._nleft != ERROR))
		/* only if requested, and file not error aborted */
			do
				putc(CPMEOF, fout);
				while(fout._nleft % SECSIZ);
#endif
		fflush(fout);
		fclose(fout);
		Rfile=FALSE;
#ifdef LOGFILE
		logfile(Rname, pad?'*':'R');	/* record file xmsn */
#endif
	}
}

opentx(name)
char *name;
{
	int printf();
#ifdef XMODEM
	int lprintf();
#endif
	struct fcb *fp, *fcbaddr();
	int (*fnx)();
#ifndef CDOS
	unsigned spm, dminutes;
	struct fcb fstat;
	spm= Baudrate/23;
#endif
#ifdef XMODEM
	fnx=Batch? lprintf : printf;
#else
	fnx=printf;
#endif
	(*fnx)("'%s' ", name);
	unspace(name);


	if(fopen(name, fin)==ERROR){
		printf("Can't open %s\n", name);
		return ERROR;
	}
#ifdef RESTRICTED
	if(cisubstr(name, ".bad")
	 || (fp=fcbaddr(fin._fd))==ERROR
	 || (fp->fname[1] & 0200)	/* tag2 */
	 || (fp->ftype[1] & 0200)	/* $SYS */
	 ) {
		fclose(fin); printf("\n'%s' Not for Distribution\n", name);
		return ERROR;
	}
#endif
	Tfile= TRUE;
	strcpy(Tname, name);
#ifndef CDOS
	setfcb( &fstat, name);
	bdos(COMPFILSIZ, &fstat);
	dminutes= 1+((10*(1+fstat.recn))/spm)+(fstat.recn/20);
	(*fnx)("Open %u Sectors %u.%u Minutes\n",
	  fstat.recn, dminutes/10, dminutes%10);
#else
	(*fnx)("Open\n");
#endif
	return OK;
}

/* closetx(status) call with status != 0 if incomplete file xmsn */
closetx(status)
{
	if(Tfile) {
		fclose(fin);
		if(!Quiet)
			lpstat("%s closed", Tname);
#ifdef LOGFILE
		if(!status)
			logfile(Tname, 's');		/* record file xmsn */
#endif
		Tfile=FALSE;
	}
}
/* search the phone file for name */
getphone(name, buffer)
char *name, *buffer;
{
	closetx(TRUE);

	if(fopen(PHONES, fin)==ERROR) {
		printf("Cannot open %s\n", PHONES);
		return ERROR;
	} else {
		while(fgets(buffer, fin))
			if(cmdeq(buffer, name)) {
				fclose(fin);
				return OK;
			}
	}
	printf("Can't find data for %s\n", name);
	fclose(fin);
	return ERROR;
}

/* change default disk and optionally, user number */
chdir(p)
char *p;
{
	unsigned newuser;

	if(Rfile||Tfile) {
		printf("Must close files first\n");
		return ERROR;
	}
	newuser=user; *p=toupper(*p);
	if(index(*p, DISKS)) {
		defdisk= *p - 'A';
		bdos(SELDSK, defdisk);
#ifdef CDOS
		return OK;
#else
		printdfr();
		if(!isdigit(p[1]))
			return;
		if((newuser=atoi(p+1)) <= MAXUSER) {
			bdos(SETGETUSER, newuser);
			user=newuser;
			return OK;
		}
#endif
	}
	printf("Disk %c and/or User %d Illegal\n", *p, newuser);
	return ERROR;
}

/* fetch default disk and user number */
initdd()
{
	Secpblk=SECPBLK;
	defdisk= bdos(GETDEFDISK,0);
#ifdef CDOS
	user=0;
#else
	user=bdos(SETGETUSER, 0377);
#endif
}

dolist(argc, argp)
char **argp;

{
	int listfile();
#ifdef XMODEM
	printf("^S pauses, ^K skips to next file, ^X terminates\n");
#endif
	expand(listfile, argc, argp);
	putch('\n');
}

listfile(name)
char *name;

{
	char *p;
	int c,d;

#ifdef XMODEM
	printf("\nListing '%s'\n\022", name);
#endif
	closetx(TRUE);
	if(opentx(name)==ERROR)
		return ERROR;
#ifdef USQ
	if((c=getw(fin))==RECOGNIZE)
		c=unsqueeze();
	else if(c != ERROR) {
		putcty(c);
		putcty(c>>8);	/* pdp-11 sequence */
#else
	else {
#endif
		while((c=getc(fin))!=EOF && c != CPMEOF) {
			if( !(c=putcty(c)))
				continue;
			if(c == CNTRLC || c == CAN || c == CNTRLK) {
				break;
			}
		}
	}
	if(c != CPMEOF)
		putchar('\n');
	/* record complete xmsn iff terminated by (CPM)EOF */
	closetx(c != CPMEOF);
#ifdef XMODEM
	sendline(024);	/* squelch in case user downloading */
#endif		
	/* cancel rest of files if ^C or ^X */
	if(c == CNTRLC || c == CAN)
		return ERROR;
	else
		return OK;
}

/* fill buf with count chars padding with ^Z for CPM */
filbuf(buf, count)
char *buf;
{
	register c, m;
	m=count;
	while((c=getc(fin))!=EOF) {
		*buf++ =c;
		if(--m == 0)
			break;
	}
	if(m==count)
		return 0;
	else
		while(--m>=0)
			*buf++ = 032;
	return count;
}

dodir(argc, argp)
char **argp;
{
	int pdirent();
	cfast=0;		/* counter for 4 across format */
	expand(pdirent, argc, argp);
#ifndef CDOS
	putch('\n');
	printdfr();
#endif
}

pdirent(name)
{
	printf("%-14s%c", name, (++cfast%DIRCOLS)?' ':'\n');
}
#ifndef CDOS
/* docomp does a directory listing showing sectors for each matched file
 * and computes total transmission time of matched files in batch mode
 * time is sum of:
 * 	number of files * open/close time (assumed 5 seconds)
 *	time to xmit and ACK each sector assuming no path delay or error
 *	disk i/o time at each end, not dependent on baud rate
 */
docomp(argc,argp)
char **argp;
{
	unsigned compsecs();
	unsigned spm;	/* sectors per minute-baud */
	unsigned dminutes;	/* tenths of minutes */
	cfast=Numsecs=Numblks=0;
	expand(compsecs, argc, argp);
	/* (Baudrate*60)/(10 bits each char * 136 chars) */
	spm= Baudrate/23;
	dminutes= Numfiles+((10*(Numfiles+Numsecs))/spm)+(Numsecs/20);
	printf("\n%u Files %u Blocks %u K\n",
	  Numfiles, Numblks, (Numblks*(Secpblk/8)));
	printf("%u Sectors %u.%u Minutes Xmsn Time at %u Baud\n",
	  Numsecs, dminutes/10, dminutes%10, Baudrate);

}
/* add file length (in CP/M 128 byte records) to Numsecs */
unsigned compsecs(ufn)
char *ufn;
{
	struct fcb fstat;
	printf("%-14s", ufn);
	unspace(ufn);
	setfcb( &fstat, ufn);
	bdos(COMPFILSIZ, &fstat);
	Numsecs += fstat.recn;
	Numblks += (fstat.recn+Secpblk)/Secpblk;
	printf("%4u%c",fstat.recn, (++cfast%DIRCOLS)?' ':'\n');
	return fstat.recn;
}
#endif

expand(fnx, argc, argp)
int (*fnx)();
char **argp;
{
	char name[PATHLEN], *s;
	Numfiles=0;

	if(argc<=0)
		return e1xpand(fnx, "*.*");
	else
		while(--argc>=0) {
			/* change b: to b:*.*  */
			strcpy(name, *argp++);
			if((s=index(':', name)) && *++s == 0)
				strcpy(s, "*.*");
			if(e1xpand(fnx, name)==ERROR)
				return ERROR;
		}
	return OK;
}

/*
 * e1xpand expands ambiguous pathname afnp
 * calling fnx for each.
 * Modified from: Parameter list builder by Richard Greenlaw
 *                251 Colony Ct. Gahanna, Ohio 43230
 */
e1xpand(fnx, afnp)
int (*fnx)();
char *afnp;	/* possible ambiguous file name*/
{
	struct fcb sfcb, *pfcb;
	FLAG first;
	char *p, *q, i, byteaddr;
	int filecount, m;
	char tbuf[SECSIZ];
	struct {
		char xYxx[UFNSIZE];		/* unambiguous file name */
	} *fp;
	int strcmp();


	/* build CPM fcb   */
	unspace(afnp);
	if(setfcb(&sfcb, afnp) == ERROR) {
		printf("%s is bad pattern\n", afnp);
		return ERROR;
	}

	if(Wrapped || (bufend-bufcq)<2048) {
		dumprxbuff();		/* I need the space for building the pathlist */
		clearbuff();		/* so the printer won't try to list dir's */
		bufmark=bufst;
	} else
		bufmark=bufcq;

	/* Search disk directory for all ufns which match afn*/
	for(fp=bufmark,filecount=0,first=TRUE;; fp++,filecount++) {
tryanother:
		bdos(SETDMA, tbuf);
		/* seems CP/M outta know whether to use SRCH or SRCHNXT !! */
		byteaddr=bdos(first? SRCH:SRCHNXT, &sfcb); first=FALSE;
		if(byteaddr==255)
			break;
		/* calculate pointer to filename fcb returned by bdos */
		pfcb = (tbuf + 32 * (byteaddr % 4));
#ifdef RESTRICTED
		/* check for $SYS or tag bit on 2nd byte of filename (TAG2) */
		if((pfcb->fname[1]&0200) ||(pfcb->ftype[1]&0200))
			goto tryanother;
#else
		/* check for $SYS with Sysfil flag */
		if((pfcb->ftype[1]&0200) && !Sysfil)
			goto tryanother;
#endif

		Numfiles++;
		p = fp;
		if(fp>bufend) {	/* Note: assumes some slop after bufend! */
			printf("Out of Memory for pathname expansion\n");
			return ERROR;
		}
		if(*(afnp+1) == ':') {
			/* Drive spec.*/
			*p++ = *afnp;
			*p++ = ':';
		}

		/*Copy filename from directory*/
		q = pfcb;
		for(i =8; i; --i)
			*p++ = (0177& *++q);
		*p++ = '.' ;

		/*Copy file extent*/
		for(i = 3; i; --i)
			*p++ = (0177& *++q);
		*p = '\0' ;

	}
	if(filecount==0) {
		printf("'%s' NOT FOUND\n", afnp);
		return ERROR;
	}

	qsort(bufmark, filecount, UFNSIZE, strcmp);

	for(fp=bufmark; --filecount>=0;) {
		p=fp++;
		/* execute desired function with real pathname */
		if((*fnx)(p)==ERROR)
			return ERROR;
	}
	return OK;
}

/*
 * cisubstr(string, token) searches for lower case token in string s
 * returns pointer to token within string if found, NULL otherwise
 */
char *cisubstr(s, t)
char *s,*t;
{
	char *ss,*tt;
	/* search for first char of token */
	for(ss=s; *s; s++)
		if(tolower(*s)==*t)
			/* compare token with substring */
			for(ss=s,tt=t; ;) {
				if(*tt==0)
					return s;
				if(tolower(*ss++) != *tt++)
					break;
			}
	return NULL;
}

/* copy string s onto itself deleting spaces "hello there" > "hellothere" */
unspace(s)
char *s;
{
	char *p;
	for(p=s; *s; s++)
		if(*s != ' ')
			*p++ = *s;
	*p++ =0;
}

#ifdef LOGFILE
/*
 * logfile keeps a record of files transmitted.
 * Mode is 's' for send file, 'r' for receive file, 't' for type file
 * Lifted from xcmodem by J. Wierda, R. Hart, and W. Earnest
*/

#define RPB 4	/* log records per buffer */
#define LRL 32	/* logical rec length LRL*RPB == SECSIZ */

logfile(name, mode)
char *name;
char mode;
{
	struct fcb *fcbaddr(), *fp;
	int fd, rnum, bnum, thedisk;
	char *i, *lrec, lbuf[SECSIZ+2];
	char date_str[6], time_str[6];

	/* find out what disk was used */

	thedisk = defdisk + 'a';
	if(i = index(':', name)) {
		thedisk = *name;
		name = ++i;
	}
	bdos(SETGETUSER, 0);	/* get it from user 0 */
	fd = open(LOGFILE,2);
	if(fd == ERROR) {	/* if file absent, create and initialize it */
		fd = creat(LOGFILE);
		if(fd == ERROR)
			return;
		else {
			rnum=1;
			fp = fcbaddr(fd);
			fp->fname[1] |= 0200;	/* set TAG2 bit */
			fp->ftype[1] |= 0200;	/* set SYS bit */
			bdos(SETATTRIB, fp);	/* and update attributes */
		}
	}
	else {
		read(fd, lbuf, 1);
		rnum = atoi(lbuf)+1;
	}
	setmem(lbuf, SECSIZ, 0);
	sprintf(lbuf, "%d\r\n", rnum);
	seek(fd,0,0);
	write(fd, lbuf, 1);		/* update last record number */
	bnum = rnum/RPB;
	rnum = (rnum%RPB)*LRL;
	seek(fd, bnum, 0);
	if(rnum == 0)
		setmem(lbuf, SECSIZ, 0);
	else
		read(fd, lbuf, 1);
	if (time(time_str)){
		sprintf(time_str,"fill");
	}
	if (date(date_str)){
		sprintf(date_str,"");
	}
	sprintf(&lbuf[rnum], "\r\n%c	%c%02d	%-12s	%5s	%5s",
	 mode, thedisk, user, name, time_str, date_str);
	seek(fd, bnum, 0);
	write(fd,lbuf,1);
	close(fd);
	bdos(SETGETUSER, user);
}

#ifdef LOGCLOCK

#include <CLOCK.H>

/* Real time system clock functions

   Extracted from DATEDEMO.C

   By Bill Bolton,
   Software Tools,
   P.O. Box 80,
   Newport Beach,
   NSW, 2106,
   AUSTRALIA

   Source Address TCY-396
   Phone (+61 2) 997-1018

   Version 1.1 for Godbout Systsem Support 1 clock 19/1/82

	char	date_str[5];	/* string (character array) for date */
	char	time_str[5];	/* string for time */
	date(srt) will fill a string with date formatted as follows:

	"11/5" (in Eurodate format)
*/

date(str)

char	*str;		/* pointer to date string */

{
	int	month[1];	/* month of the year, range 1 to 12 */
	int	mday[1];	/* day of the current month, range 0 to 31 */

	if (get_date(month,mday)){
		return(-1);
	} 
	sprintf(str,"%d/%d",*mday,*month);
	return(0);
}

/* get_date(month,mday) provides the basic data for formatting
   a date string, fetched from the clock board and converted to a useable
   set of values */

int get_date(month,mday)

int	*month;		/* pointer to current month */
int	*mday;		/* pointer to day of the month */

{
	if (inp(CDATA) == 0XFF )      /* no clock board present */
		return(-1);
	*month = ((read_digit(MONTH10) & 1) * 10) + read_digit(MONTH1);
	*mday = ((read_digit(DAY10) & 3) * 10) + read_digit(DAY1);
	outp(CLKCMD,0);
	return(0);
}


int read_digit(address)

int	address;

{
	int	instruct;

	instruct = address + CREAD;
	outp(CLKCMD,instruct);
	return (inp(CDATA));
}

/* time(str,format) fills a string with the time of day in the 
   following format :

		18:00
*/

time(str)

char	*str;		/* string to fill with time */

{
	int	t[4];

	if (read_clock(t)){
		return(-1);
	} 
	sprintf(str,"%d%d:%d%d",t[0],t[1],t[2],t[3]);
	return(0);
}

/* read_clock(t) fills an array with the time of day digits read from
   the clock board
*/

int read_clock(t)

int	*t;		/* array to store clock digits */

{
	int	ptr;	/* pointer into digit array */

	if (inp(CDATA) == 0XFF )	/* no clock board present */
		return(-1);
	t[0] = (read_digit(HOUR10) & 3);
	t[1] = read_digit(HOUR1);
	t[2] = (read_digit(MIN10) & 7);
	t[3] = read_digit(MIN1);
	outp(CLKCMD,0);
	return(0);
}

#else /* LOGCLOCK */

time(str)
char *str;

{
	return(-1);
}
 
date(str)
char *str;

{
	return(-1);
}

#endif /* LOGCLOCK */

#endif /* LOGFILE */


docrck(argc, argp)
char **argp;
{
	int crckfile();

	expand(crckfile, argc, argp);
}

/* Accumulate and print a "crck" for a file */
crckfile(name)
char *name;
{
	unsigned crck();
	char crbuf[SECSIZ]; int fd,st;

	printf("%14s ", name); unspace(name);
	if((fd=open(name, 0))==ERROR)
		return ERROR;

	oldcrc=0;
	while((st=read(fd, crbuf, 1)) ==1)
			oldcrc=crck(crbuf, SECSIZ, oldcrc);
	close(fd);
	if(st != 0)
		printf("READ ERROR\n");
	else
		printf("CRCK= %04x\n", oldcrc);
	return OK;
}

/* print number of free blocks on default disk */
printdfr()
{
	struct dpb *dp;

	dp=call(BDOS, 0, 0, GETDPBP, defdisk);
	Secpblk= 1 << dp->dpb_bsh;
	printf("%u kb Free on %c\n", getfree(defdisk), defdisk+'A');
}

/* return total free kilobytes of disk */
unsigned getfree(disk)
{
	struct dpb *dp;
	/* unsigned */ char v, c, *s;
	unsigned total, count;

	bdos(SELDSK, disk);
	dp=call(BDOS, 0, 0, GETDPBP, disk);
	s=call(BDOS, 0, 0, GETALLOCP, disk);
	total=0;
	count=dp->dpb_dsm+1;
	for(;;) {
		v= *s++;
		for(c=0200; c; c>>=1) {
			if((v & c)==0)
				++total;
			if(--count ==0) {
				bdos(SELDSK, defdisk);
				return total << dp->dpb_bsh-3;
			}
		}
	}
}

doerase(argc, argp)
char **argp;
{
	int erasefile();
	expand(erasefile, argc, argp);
#ifndef CDOS
	printdfr();
#endif
}

erasefile(ufn)
char *ufn;
{
	register c;
	unspace(ufn);
	printf("Erase %s (y/n/q)? ",ufn);
	c=getchar();
	putchar('\n');
	if(tolower(c)=='y')
		unlink(ufn);
	else if(tolower(c)=='n')
		return FALSE;
	else
		return ERROR;
	return FALSE;
}

/* end of YAM7.C */
