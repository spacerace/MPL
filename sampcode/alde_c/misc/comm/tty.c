/*
 *	TTY.C		A quick general purpose terminal program.
 */

#include <stdio.h>
#include <osbind.h>
#ifdef dLibs
#include <time.h>
#endif

#define	ident	"TTY v0.71\033v\r\n"

#define	ABORT	(-3)
#define	CANCEL	(-2)

#define	AUX	1
#define	CON	2

#define	SOH	0x01
#define	STX	0x02
#define	EOT	0x04
#define	ACK	0x06
#define	NAK	0x15
#define	CAN	0x18
#define	CEE	0x43

#define	KEY_UNDO	0x6100		/* [undo]	*/
#define	KEY_HELP	0x6200		/* [help]	*/
#define	KEY_INS		0x5200		/* [insert]	*/
#define	KEY_CLR		0x4700		/* [clr/home]	*/
#define	KEY_CR		0x1C0D		/* [return]	*/
#define	KEY_LF		0x720D		/* [enter]	*/
#define	KEY_BS		0x0E08		/* [backspace]	*/
#define	KEY_DEL		0x537F		/* [delete]	*/
#define	KEY_BRK		0x2E03		/* [control][C]	*/
#define	KEY_F1		0x3B00		/* [f1]		*/
#define	KEY_F2		0x3C00		/* [f2]		*/
#define	KEY_F3		0x3D00		/* [f3]		*/
#define	KEY_F4		0x3E00		/* [f4]		*/
#define	KEY_F5		0x3F00		/* [f5]		*/
#define	KEY_F6		0x4000		/* [f6]		*/
#define	KEY_F7		0x4100		/* [f7]		*/
#define	KEY_F8		0x4200		/* [f8]		*/
#define	KEY_F9		0x4300		/* [f9]		*/
#define	KEY_F10		0x4400		/* [f10]	*/
#define	ALT_C		0x2E00		/* [alt][C]	*/
#define	ALT_A		0x1E00		/* [alt][A]	*/
#define	ALT_DASH	0x8200		/* [alt][-]	*/
#define	ALT_V		0x2F00		/* [alt][V]	*/
#define	ALT_T		0x1400		/* [alt][T]	*/
#define	ALT_E		0x1200		/* [alt][E]	*/
#define	ALT_F		0x2100		/* [alt][F]	*/
#define	ALT_R		0x1300		/* [alt][R]	*/
#define	ALT_S		0x1F00		/* [alt][S]	*/
#define	ALT_B		0x3000		/* [alt][B]	*/
#define	ALT_0		0x8100		/* [alt][0]	*/
#define	ALT_H		0x2300		/* [alt][H]	*/
#define	ALT_X		0x2D00		/* [alt][X]	*/
#define	ALT_G		0x2200		/* [alt][G]	*/

#define	ECODES	3		/* line echo settings (aka duplex) */
#define	FULLDUP	0		/*	no echo			*/
#define	HALFDUP	1		/*	local echo		*/
#define	HOSTMOD	2		/*	local & remote echo	*/

#define	XCODES	4		/* # of file xfer modes */
#define	ASCII	0
#define	XMODEM	1
#define	XMODEMC	2
#define	YMODEM	3

#define	BCODES	5		/* # of baud rate settings */
#define	B300	0
#define	B1200	1
#define	B2400	2
#define	B4800	3
#define	B9600	4

#define	HSIZ	(1<<11)		/* history size (2K) */
#define	HMASK	(HSIZ-1)

int	cd;			/* current state of carrier */
BYTE	hist[HSIZ];		/* history buffer */
int	hp = 0;			/* history position index */
BYTE	data[1200];		/* general purpose data buffer */
int	crcmode, kmode, ybatch;	/* file transfer mode settings */
int	pktnum;			/* file transfer packet counter */
int	noise;			/* line noise analysis value */
handle	capture = 0;		/* ascii capture file handle */
int	crtocrlf = FALSE;	/* convert <cr> --> <cr>+<lf> */
int	echo = FULLDUP;		/* echo mode (duplex) */
char	*ename[ECODES] =	/* echo mode names */
	{"full duplex", "half duplex", "host mode"};
int	xfer = XMODEMC;		/* file xfer mode */
char	*xname[XCODES] =	/* xfer mode names */
	{"ascii", "xmodem", "xmodem-crc", "ymodem"};
int	bcode = B300;		/* baud code (not set on entry) */
int	brate[BCODES] =		/* baud rate values */
	{9, 7, 4, 2, 1};
char	*bname[BCODES] =	/* baud code names */
	{"300", "1200", "2400", "4800", "9600"};
int	literal = FALSE;	/* literal mode flag */
int	debug = FALSE;		/* debugging mode flag */

struct iorec {
	char	*iobuf;			/* pointer to buffer */
	int	iobufsiz;		/* size of buffer */
	int	iobufhd;		/* head pointer */
	int	iobuftl;		/* tail pointer */
	int	iobuflo;		/* low water mark */
	int	iobufhi;		/* high water mark */
};

typedef struct {
    struct iorec in;			/*  0 Input buffer		*/
    struct iorec out;			/* 14 Output buffer		*/
    char	rsr_status;		/* 28 MFP(0x1C) Receiver status	*/
    char	tsr_status;		/* 29 MFP(0x1D) Transmit status	*/
    char	xoff_sent;		/* 30 TRUE if we sent XOFF	*/
    char	xoff_received;		/* 31 TRUE if we got XOFF	*/
    char	mode;			/* 32 Bit 0 XON, Bit 1 RTS mode	*/
    char	filler;			/* 33 Unused			*/
} IOREC;

#define	IBUFSIZ	1200
#define	OBUFSIZ	2

char	st_ibuf[IBUFSIZ];	/* our own input buffer		*/
char	st_obuf[OBUFSIZ];	/* and our own output buffer	*/

IOREC	*st_sysr;		/* ptr to system rs232 record	*/
IOREC	st_savr;		/* to save system rs232 record	*/

IOREC	st_myiorec = {		/* my own i/o record */
     st_ibuf, IBUFSIZ, 0, 0, (IBUFSIZ/4), (3*(IBUFSIZ/4)),
     st_obuf, OBUFSIZ, 0, 0, 0, 1,
     0, 0, 0, 0
};

#ifndef dLibs
int strncmp(str1, str2, limit)
register char *str1;
register char *str2;
register int limit;
/*
 *	Compare strings as with strcmp(), but limit comparison to the
 *	first <limit> characters.
 */
{
	for(; ((--limit) && (*str1 == *str2)); ++str1, ++str2)
		if(*str1 == '\0')
			return(0);
	return(*str1 - *str2);
}

/*
 * start_timer & time_since
 *
 * Use 200Hz system clock for an elapsed time function. -Dal
 */
long start_timer(timer)
long *timer;
{
	asm("	clr.l	-(sp)		");
	asm("	move.w	#$20,-(sp)	");
	asm("	trap	#1		");	/* enter supervisor mode */
	asm("	addq.l	#6,sp		");
	asm("	move.l	$4BA,-(sp)	");	/* save system clock value */
	asm("	move.l	d0,-(sp)	");
	asm("	move.w	#$20,-(sp)	");
	asm("	trap	#1		");	/* exit supervisor mode */
	asm("	addq.l	#6,sp		");
	asm("	move.l	(sp)+,d0	");
	asm("	move.l	$8(a6),a0	");	/* grab pointer to timer */
	asm("	move.l	d0,(a0)		");	/* return clock value */
}

long time_since(timer)
long *timer;
{
	asm("	clr.l	-(sp)		");
	asm("	move.w	#$20,-(sp)	");
	asm("	trap	#1		");	/* enter supervisor mode */
	asm("	addq.l	#6,sp		");
	asm("	move.l	$4BA,-(sp)	");	/* save system clock value */
	asm("	move.l	d0,-(sp)	");
	asm("	move.w	#$20,-(sp)	");
	asm("	trap	#1		");	/* exit supervisor mode */
	asm("	addq.l	#6,sp		");
	asm("	move.l	(sp)+,d0	");
	asm("	move.l	$8(a6),a0	");	/* grab pointer to timer */
	asm("	sub.l	(a0),d0		");	/* return elapsed time */
}
#endif

msleep(n)			/* do nothing for n-100ths of a second */
register int n;
{
	long t;

	n <<= 1;		/* multiply by 2 to get 200Hz ticks */
	start_timer(&t);
	while(time_since(&t) < n)
		;
}

rs232cd()			/* state of rs232 carrier detect line */
{
	register long ssp;
	register int *mfp, status;

	mfp = ((int *) 0xFFFFFA00L);		/* base address of MFP */
	ssp = Super(0L);			/* enter supervisor mode */
	status = *mfp;				/* get MFP status */
	Super(ssp);				/* return to user mode */
	return(!(status & 0x0002));		/* check for carrier */
}

snd_brk()			/* send a BREAK to the rs232 port */
{
	register long ssp;
	register char *tsr;

	tsr = ((char *) 0xFFFFFA2DL);		/* address of tsr */
	ssp = Super(0L);			/* enter supervisor mode */
	*tsr = 0x09;				/* turn break on */
	Super(ssp);				/* return to user mode */
	msleep(30);				/* 0.3 second BREAK */
	ssp = Super(0L);			/* enter supervisor mode */
	*tsr = 0x01;				/* turn break off */
	Super(ssp);				/* return to user mode */
}

hangup()			/* drop carrier by droppping DTR to modem */
{
	Ongibit(0x10);				/* kill DTR */
	msleep(50);				/* wait 1/2 a second */
	Offgibit(0xEF);				/* restore DTR */
}

/*
 * Buffered file input functions
 */
#define	Bsiz	1024		/* block read size */
#define	Fsiz	(2*Bsiz)+256	/* buffer size */
BYTE	Fbuf[Fsiz];		/* file character buffer */
BYTE	*Fp = &Fbuf[Fsiz];	/* next character to output from Fbuf[] */
int	Fcnt = 0;		/* number of characters in Fbuf[] */
handle	Fh;			/* file handle for buffered file */

Finit(fname)
char *fname;
{
	Fp = &Fbuf[Fsiz];
	Fcnt = 0;
	return(Fh = Fopen(fname, 0));
}

Fgetc()			/* get a character from file */
{
	register int n;

	if(Fcnt <= 0) {
		Fp -= Bsiz;
		if((n = Fread(Fh, ((long) Bsiz), Fp)) < 0)
			Bconws(CON, "<read error>\r\n");
		if(n <= 0)
			return(EOF);
		Fcnt += n;
	}
	--Fcnt;
	return(*Fp++);		/* MUST & with 0xFF if not UNSIGNED */
}

Fgetn(buf, n)		/* get 'n' characters from file */
register char *buf;
register int n;
{
	register int c, m;

	m = n;
	while(n--) {
		if((c = Fgetc()) == EOF) {
			m -= (++n);
			while(n--)
				*buf++ = '\0';
			break;
		}
		*buf++ = c;
	}
	return(m);
}

Fungetn(buf, n)
register char *buf;
register int n;
{
	register char *p;

	Fcnt += n;
	Fp -= n;
	for(p=Fp; n--; *p++ = *buf++)
		;
}

/*
 * Extra i/o functions including AUX: input with timeout
 */
openaux()		/* set up our own rs232 input and output buffers */
{
	while(Bconstat(AUX))		/* flush existing buffer */
		Bconin(AUX);
	st_sysr = (IOREC *)Iorec(0);
	st_savr = *st_sysr;		/* Save system buffer	*/
	*st_sysr = st_myiorec;		/* Set my io buffer	*/
}

closeaux()		/* restore system i/o buffer */
{
	*st_sysr = st_savr;
}

int un_kb = '\0';		/* ungotten keyboard character */

#define	ungetkb(c)	un_kb=(c)

kbready()
{
	return(un_kb || Bconstat(CON));
}

getkb()
{
	register long n;
	register int c;

	if(un_kb) {
		c = un_kb;
		un_kb = '\0';
		return(c);
	}
	n = Bconin(CON);
	return((0x00FF & n) | (0xFF00 & (n>>8)));
}

checkabort()			/* check for abort conditions */
{
	return(!rs232cd() || (Bconstat(CON) && (getkb() == KEY_UNDO)));
}

#define	RDLNMAX	128

char *getln()			/* Get input line from console thru OS */
{
	static char rdlnbuf[RDLNMAX+4];
	register char *cp=rdlnbuf;
	register int c, i=0;

	for(;;) {
		c = getkb();
		if(c == KEY_UNDO) {
			*(cp = rdlnbuf) = '\0';
			i=0;
			break;
		}
		else if((c == KEY_CR) || (c == KEY_LF)) {
			*cp = '\0';
			break;
		}
		else if((c == KEY_BS) || (c == KEY_DEL)) {
			if(cp != rdlnbuf) {
				--cp;
				Bconws(CON, "\b \b");
				--i;
			}
			else
				Bconout(CON, 0x07);
		}
		else if((c >= ' ') && (i < RDLNMAX)) {
			Bconout(CON, (*cp++ = c));
			++i;
		}
		else
			Bconout(CON, 0x07);
	}
	return(rdlnbuf);
}

char *prompt(p)			/* prompt for console input */
char *p;
{
	Bconws(CON, p);
	p = getln();
	Bconws(CON, "\r\n");
	return(p);
}

int un_aux = '\0';		/* ungotten AUX: character */
int can_flg = FALSE;		/* CAN character received */

#define	ungetaux(c)	un_aux=(c)

auxready()
{
	return(un_aux || Bconstat(AUX));
}

getaux(limit)			/* get a character from AUX: w/ timeout */
int limit;
{
	long t;
	register long tt;
	register int c;

	tt = ((long) (limit * 200));
	if(un_aux) {
		c = un_aux;
		un_aux = '\0';
		return(c);
	}
	start_timer(&t);
	while(!Bconstat(AUX)) {
		if(checkabort()) {
			cancel();
			return(ABORT);
		}
		if(time_since(&t) > tt)
			return(ERROR);
	}
	c = (0xFF & Bconin(AUX));
	if(c == CAN) {
		if((can_flg = !can_flg) == FALSE)
			return(CANCEL);
	}
	else
		can_flg = FALSE;
	return(c);
}

int getsnaux(s, n)		/* get 'n' characters from AUX: */
register char *s;
register int n;
{
	register int c;

	while(n--) {
		switch(c = getaux(1)) {
			case ERROR:
			case ABORT:
				return(c);
			case CANCEL:
				c = CAN;	/* FALL THRU! */
			default:
				*s++ = c;
		}
	}
	*s = '\0';
	return(TRUE);
}

putaux(c)
int c;
{
	Bconout(AUX, (c & 0xFF));
}

putsnaux(s, n)			/* send 'n' characters to AUX: */
register char *s;
register int n;
{
	register int c;

	while(n--) {
		if(auxready()) {
			c = getaux(0);
			if((c < 0) || (c == NAK))
				return(ungetaux(c));
		}
		putaux(*s++);
	}
}

purge()
{
	register int c;

	while((c = getaux(1)) >= 0)	/* wait for timeout or cancel */
		;
	return(c);
}

cancel()
{
	register int i;

	purge();
	for(i=0;i<3;++i)
		putaux(CAN);
	for(i=0;i<3;++i)
		putaux('\b');
	if(debug) Bconws(CON, "<cancel>\r\n");
	return(CANCEL);
}

/*
 * This function calculates the CRC used by the XMODEM/CRC Protocol
 * The first argument is a pointer to the message block.
 * The second argument is the number of bytes in the message block.
 * The function returns an integer which contains the CRC.
 * The low order 16 bits are the coefficients of the CRC.
 */
int calcrc(ptr, count)
register char *ptr;
register int count;
{
	register int crc = 0, i;

	while(--count >= 0) {
		crc = crc ^ (int)*ptr++ << 8;
		for(i=0; i<8; ++i)
			if(crc & 0x8000)
				crc = crc << 1 ^ 0x1021;
			else
				crc = crc << 1;
		}
	return(crc & 0xFFFF);
}

int calchk(ptr, count)
register char *ptr;
register int count;
{
	register int sum = 0;

	while(--count >= 0)
		sum += *ptr++;
	return(sum & 0x00FF);
}

mode_init()			/* initialize xfer mode settings from config */
{
	crcmode = (xfer >= XMODEMC);
	kmode = (xfer >= YMODEM);
	ybatch = (xfer == YMODEM) && (FALSE);	/* <-- check for wildcards */
}

pkt_sync(limit)
int limit;
{
	register int c;

 	kmode = FALSE;
	while(rs232cd()) {
		switch(c = getaux(limit)) {
			case ERROR:
			case CANCEL:
			case ABORT:
				return(c);
			case STX:
				kmode = TRUE;		/* FALL THRU... */
			case SOH:
				return(TRUE);
			case EOT:
				return(FALSE);
		}
	}
	return(ERROR);
}

rcv_init()
{
	register int i, j;

	pktnum = 1;
	mode_init();
	if(crcmode) {
		i = 3;
		while(i--) {
			putaux(CEE);
			Bconout(CON, 'c');
			j = pkt_sync(5);
			if(j != ERROR)
				return(j);
		}
		crcmode = FALSE;
	}
	i = 9;
	while(i--) {
		putaux(NAK);
		Bconout(CON, '.');
		j = pkt_sync(5);
		if(j != ERROR)
			return(j);
	}
	if(debug) Bconws(CON, "<init failed>\r\n");
	return(ERROR);
}

rcv_pkt()
{
	register int i, j, k = 0;

	while(++k < 6) {
		if((j = pkt_sync(10)) != TRUE)
			return(j);
		i = (kmode ? 1024 : 128);
		j = getsnaux(data, (i+3+crcmode));
		if(j == ERROR) {		/* receiver timeout */
			if(debug) Bconws(CON, "[timeout]");
			goto RETRY;
		}
		if((pktnum==1) && (data[0]==0) && (data[1]==0xFF)) {
			/* Ymodem-batch header */
			crcmode = TRUE;
			ybatch = TRUE;
			Bconout(CON, '*');
			putaux(ACK);
			putaux(CEE);
			return(rcv_pkt());	/* recurse... */
		}
		if(data[0] != ~data[1]) {	/* parity error */
			if(debug) Bconws(CON, "[parity error]");
			goto RETRY;
		}
		if(data[0] == (pktnum - 1)) {	/* duplicate packet */
			if(debug) Bconws(CON, "[duplicate packet]");
			putaux(ACK);
			return(rcv_pkt());	/* recurse... */
		}
		if(data[0] != pktnum) {		/* sequence error, CANCEL! */
			if(debug) Bconws(CON, "[sequence error]");
			return(cancel());
		}
		if(crcmode) {
			j = calcrc(data+2, i);
			i = *((int *) (data+i+2));
		}
		else {
			j = calchk(data+2, i);
			i = ((int) (0xFF & data[i+2]));
		}
		if(i == j) {
			if(++pktnum > 255)
				pktnum = 0;
			Bconout(CON, '+');
			putaux(ACK);
			return(TRUE);
		}
		if(debug) Bconws(CON, crcmode?"[crc error]":"[chksum error]");
RETRY:		purge();
		Bconout(CON, '-');
		putaux(NAK);
	}
	if(debug) Bconws(CON, "[retry error]");
	return(cancel());
}

rcv_file()
{
	register int f, j;
	register char *p;

	if(!rs232cd()) {
		Bconws(CON, "You're not on-line.\r\n");
		return(FALSE);
	}
	if(xfer == ASCII) {
		Bconws(CON, "Use text capture for ASCII receive.\r\n");
		return(TRUE);
	}
	p = prompt("\r\nReceive file: ");	/* prompt for file name */
	if(!(*p))
		return(FALSE);
	if((f = Fcreate(p, 0)) < 0) {
		Bconws(CON, "\007<file error>\r\n");
		return(ERROR);
	}
	if((j = rcv_init()) != TRUE)
		return(j);
	ungetaux(kmode ? STX : SOH);
	if(debug) Bconws(CON, (crcmode?"<crc mode>":"<chksum mode>"));
	while((j = rcv_pkt()) == TRUE) {
		if(Fwrite(f, (kmode ? 1024L : 128L), data+2) < 0)
			return(cancel());
	}
	Fclose(f);
	if(ybatch) {		/* clean up after ymodem batch xfer */
		msleep(50);
		cancel();
	}
	if(j == FALSE) {
		putaux(ACK);
		if(debug) Bconws(CON, "<file rcvd>");
		Bconws(CON, "\r\n");
		return(TRUE);
	}
	else
		return(j);
}

snd_init()
{
	register int c;

	if(xfer == ASCII)
		return(TRUE);
	pktnum = 1;
	noise = 7;
	mode_init();
	while(Bconstat(AUX))
		Bconin(AUX);
	for(;;) {
		switch(c = getaux(30)) {
			case ERROR:
			case CANCEL:
			case ABORT:
				return(c);
			case CEE:
			case NAK:
				Bconout(CON, ((c == CEE) ? 'c' : '.'));
				if(!(crcmode = (c == CEE)))
					kmode = FALSE;
				if(debug) Bconws(CON, 
						(crcmode?"<crc mode>":
							 "<chksum mode>"));
				return(TRUE);
		}
	}
}

snd_pkt()
{
	register int n, i, j, k = 0;
	register char *p;

	if(xfer == ASCII) {
		switch(i = Fgetc()) {
			case EOF:
				return(FALSE);
			case '\r':
				putaux('\r');
				if(crtocrlf)
					putaux('\n');
				msleep(10);		/* FALL THRU.. */
			case '\n':
				break;
			default:
				putaux(i);
		}
		while((i = getaux(0)) >= 0)
			Bconout(CON, (i & 0x7F));
		return((i == ERROR) ? TRUE : i);
	}
	while(++k < 6) {
		data[0] = (pktnum & 0xFF);
		data[1] = ((~pktnum) & 0xFF);
		i = (kmode ? 1024 : 128);
		p = data+2;
		if((n = Fgetn(p, i)) == 0)	/* empty packet == EOF */
			return(FALSE);
		if(kmode && (n < 1024)) {	/* send last packets as 128 */
			Fungetn(p, n);
			kmode = FALSE;
			n = Fgetn(p, (i = 128));
		}
		if(crcmode) {
			j = calcrc(p, i);
			*((int *) (p+i)) = j;
		}
		else
			*(p+i) = calchk(p, i);
		putaux(kmode ? STX : SOH);
		putsnaux(data, i+3+crcmode);
		do {
			switch(j = getaux(15)) {
				case CANCEL:
				case ABORT:
					return(j);
				case ACK:
					++noise;
					if(++pktnum > 255)
						pktnum = 0;
					Bconout(CON, '+');
					return(n == i);	/* short pkt == EOF */
				case ERROR:
					j = NAK;
					break;
			}
		} while(j != NAK);
		Fungetn(p, n);
		Bconout(CON, '-');
		if(kmode && ((noise -= 8) < 0)) {	/* if too noisy.. */
			if(debug) Bconws(CON, "<downshift>");
			kmode = FALSE;
		}
	}
	return(cancel());
}

snd_file()
{
	register int i, j;
	register char *p;

	if(!rs232cd()) {
		Bconws(CON, "You're not on-line.\r\n");
		return(FALSE);
	}
	p = prompt("\r\nSend file: ");		/* prompt for file name */
	if(!(*p))
		return(FALSE);
	if(Finit(p) < 0) {
		Bconws(CON, "\007<file error>\r\n");
		return(ERROR);
	}
	if((j = snd_init()) != TRUE)
		return(j);
	while((j = snd_pkt()) == TRUE)
		;
	Fclose(Fh);
	if(xfer == ASCII)
		return(j == FALSE);
	if(j == FALSE) {
		i = NAK;
		do {
			if(i == NAK)
				putaux(EOT);
			i = getaux(10);
			if(i < 0)
				return(i);
		} while(i != ACK);
		if(debug) Bconws(CON, "<file sent>");
		Bconws(CON, "\r\n");
		return(TRUE);
	}
	else
		return(j);
}

start_capture()
{
	register char *p;

	if(*(p = prompt("\r\nCapture file: ")) == '\0')
		return(FALSE);
	if((capture = Fcreate(p, 0)) < 0) {
		capture = 0;
		Bconws(CON, "\007<file error>\r\n");
		return(FALSE);
	}
}

putcap(c)
char c;
{
	if(Fwrite(capture, 1L, &c) != 1L) {
		capture = 0;
		Bconws(CON, "\007<write error>\r\n");
		return(FALSE);
	}
}

do_grab()
{
	register int h, i, j;

	if((h = Fopen("GRAB.TXT", 2)) < 0) {
		if((h = Fcreate("GRAB.TXT", 0)) < 0) {
			Bconws(CON, "\007<can't create grab file>\r\n");
			return(FALSE);
		}
	}
	else
		Fseek(h, 0L, 2);
	if(Fwrite(h, 8L, "\r\n----\r\n") != 8L) {
		Bconws(CON, "\007<write error>\r\n");
		return(FALSE);
	}
	i = HSIZ - hp;
	j = hp;
	if(i > 0)
		if(Fwrite(h, ((long) i), &hist[j]) <= 0) {
			Bconws(CON, "\007<write error>\r\n");
			return(FALSE);
		}
	if(j > 0)
		if(Fwrite(h, ((long) j), hist) <= 0) {
			Bconws(CON, "\007<write error>\r\n");
			return(FALSE);
		}
	Fclose(h);
	return(TRUE);
}

do_help()
{
	Bconws(CON, "\r\n");
	Bconws(CON, "[Alt][C]	Shell Command	[F1]\r\n");
	Bconws(CON, "[Alt][A]	Ascii Capture	[F2]\r\n");
	Bconws(CON, "[Alt][-]	Debug mode	[F3]\r\n");
	Bconws(CON, "[Alt][V]	Literal mode	[F4]\r\n");
	Bconws(CON, "[Alt][T]	CR -> CR+LF	[F5]\r\n");
	Bconws(CON, "[Alt][E]	Echo mode	[F6]\r\n");
	Bconws(CON, "[Alt][F]	File Xfer Mode	[F7]\r\n");
	Bconws(CON, "[Alt][R]	Receive File	[F8]\r\n");
	Bconws(CON, "[Alt][S]	Send File	[F9]\r\n");
	Bconws(CON, "[Alt][B]	Baud Rate	[F10]\r\n");
	Bconws(CON, "[Alt][0]	Send BREAK	[INS]\r\n");
	Bconws(CON, "[Alt][H]	Drop Carrier	[CLR]\r\n");
	Bconws(CON, "[Alt][X]	Exit TTY	[UNDO]\r\n");
	Bconws(CON, "[Alt][G]	Grab history	\r\n");
}

char *hexchr(c)
{
	static char hexout[5];
	static char hexdigit[] = "0123456789ABCDEF";

	hexout[4] = '\0';
	hexout[3] = hexdigit[c&0xF];
	c >>= 4;
	hexout[2] = hexdigit[c&0xF];
	hexout[1] = 'x';
	hexout[0] = '\\';
	return(hexout);
}

extern	long	_base;		/* our base-page pointer */

do_shell()
{
	register char *p, *ssp, (*shell)();
	register long rv = 0L;

	/* get _shell_p value */
	ssp = Super(0L);
	shell = *((long *) 0x4F6L);
	Super(ssp);

	/* validate _shell_p */
	if(shell == 0L) {
		Bconws(CON, "\r\nNo shell available.\r\n");
		return(ERROR);
	}
	if((((long) shell) > ((long) _base)) || !(strncmp(shell, "PATH", 4))) {
		Bconws(CON, "\r\nBad shell pointer.\r\n");
		return(ERROR);
	}

	/* execute the command */
	if(*(p = prompt("\r\nShell Command: ")))
		rv = (*shell)(p);
	return((int) rv);
}

main()
{
	register int c;

	Bconws(CON, ident);
	openaux();
	for(c = 0; c < HSIZ; hist[c++] = '\0')		/* zero history */
		;
#ifdef NEVER
	cd = !rs232cd();
#endif
	for(;;) {
		if(kbready()) {
			switch(c = getkb()) {
				case KEY_UNDO:		/* exit */
				case ALT_X:
					Bconws(CON, "\r\nExit TTY? ");
					c = (getkb() & 0xFF);
					Bconout(CON, c);
					Bconws(CON, "\r\n");
					if((c == 'y') || (c == 'Y'))
						punt(0);
					break;
				case KEY_INS:		/* break */
				case ALT_0:
					snd_brk();
					break;
				case KEY_CLR:		/* hangup */
				case ALT_H:
					hangup();
					break;
				case KEY_F1:		/* shell escape */
				case ALT_C:
					do_shell();
					break;
				case KEY_F2:		/* ascii capture */
				case ALT_A:
					if(capture) {
						Fclose(capture);
						capture = 0;
						Bconws(CON,
							"[capture off]\r\n");
					}
					else if(start_capture()) {
						Bconws(CON,
							"[capture on]\r\n");
					}
					break;
				case KEY_F3:		/* debug toggle */
				case ALT_DASH:
					Bconws(CON, ((debug = !debug) ?
						"\r\n[DEBUG ON]\r\n" :
						"\r\n[DEBUG OFF]\r\n"));
					break;
				case KEY_F4:		/* literal toggle */
				case ALT_V:
					Bconws(CON, ((literal = !literal) ?
						"\r\n[literal on]\r\n" :
						"\r\n[literal off]\r\n"));
					break;
				case KEY_F5:		/* cr -> cr+lf */
				case ALT_T:
					crtocrlf = !crtocrlf;
					Bconws(CON, crtocrlf?
						"[cr->cr+lf]" : "[cr->cr]");
					Bconws(CON, "\r\n");
					break;
				case KEY_F6:		/* duplex/echo modes */
				case ALT_E:
					echo = ((echo + 1) % ECODES);
					Bconout(CON, '[');
					Bconws(CON, ename[echo]);
					Bconws(CON, "]\r\n");
					break;
				case KEY_F7:		/* file xfer modes */
				case ALT_F:
					xfer = ((xfer + 1) % XCODES);
					Bconout(CON, '[');
					Bconws(CON, xname[xfer]);
					Bconws(CON, "]\r\n");
					break;
				case KEY_F8:		/* receive file */
				case ALT_R:
					if(rcv_file() != TRUE)
					 Bconws(CON, "\r\n<rcv failed>\r\n");
					break;
				case KEY_F9:		/* send file */
				case ALT_S:
					if(snd_file() != TRUE)
					 Bconws(CON, "\r\n<send failed>\r\n");
					break;
				case KEY_F10:		/* change baud rate */
				case ALT_B:
					bcode = ((bcode + 1) % BCODES);
					Rsconf(brate[bcode],0,-1,-1,-1,-1);
					Bconout(CON, '[');
					Bconws(CON, bname[bcode]);
					Bconws(CON, "]\r\n");
					break;
				case ALT_G:		/* grab history */
					if(do_grab())
						Bconws(CON, "<grabbed>\r\n");
					break;
				case KEY_HELP:		/* help info */
					do_help();
					break;
				default:
					c &= 0xFF;
					putaux(c);
					if(echo >= HALFDUP) {
						Bconout(CON, c);
						hist[hp++] = c;
						hp &= HMASK;
						if(capture)
							putcap(c);
					}
					if(crtocrlf && (c == '\r'))
						ungetkb('\n');
					break;
			}
		}
		if(auxready()) {
			if(un_aux) {
				c = (un_aux & 0xFF);
				un_aux = '\0';
			}
			else
				c = (Bconin(AUX) & 0x7F);
			if(literal && (c < ' '))
				Bconws(CON, hexchr(c));
			else
				Bconout(CON, c);
			hist[hp++] = c;
			hp &= HMASK;
			if(capture)
				putcap(c);
			if(echo == HOSTMOD)
				putaux(c);
			if(crtocrlf && (c == '\r'))
				ungetaux('\n');
		}
#ifdef NEVER
		if (cd != rs232cd())
			Bconws(CON, ((cd = rs232cd()) ?
				"\r\n[CARRIER]\r\n" :
				"\r\n[NO CARRIER]\r\n"));
#endif
	}
}

Bconws(dev, s)
register int dev;
register char *s;
{
	while(*s)
		Bconout(dev, *s++);
}

punt(errcode)
int errcode;
{
	closeaux();
	exit(errcode);
}

#ifdef dLibs
_initargs()		/* dummy function to avoid command line processing */
{
}
#endif
