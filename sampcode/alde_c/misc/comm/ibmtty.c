/*                          *** ibmtty.c ***                         */
/*                                                                   */
/* IBM-PC microsoft "C" under PC-DOS                                 */
/*                                                                   */
/* Terminal emulation program with file upload and download capabili-*/
/* ties.  Optimized to communicate with a DEC VAX 11/780.            */
/*                                                                   */
/* Written by L. Cuthbertson, April 1984.                            */
/*                                                                   */
/*********************************************************************/
/*                                                                   */
 
#include <stdio.h>
 
#define TRUE 1
#define FALSE 0
#define XON '\021'
#define XOFF '\023'
#define ESC '\033'
#define CONZ '\032'
#define DEL '\177'
 
#define LCR 0x3FB	/* 8250 line control register */
#define DLL 0x3F8	/* 8250 least significant divisor latch */
#define DLM 0x3F9	/* 8250 most significant divisor latch */
#define LSR 0x3FD	/* 8250 line status register */
#define IIR 0x3FA	/* 8250 interrupt identification register */
#define IER 0x3F9	/* 8250 interrupt enable register */
#define MCR 0x3FC	/* 8250 modem control register */
#define MSR 0x3FE	/* 8250 modem status register */
#define RBR 0x3F8	/* 8250 receiver buffer register */
#define THR 0x3F8	/* 8250 transmitter holding register */

char *lcr=(char*)LCR, *dll=(char*)DLL, *dlm=(char*)DLM;
char *lsr=(char*)LSR, *iir=(char*)IIR, *ier=(char*)IER;
char *mcr=(char*)MCR, *msr=(char*)MSR, *rbr=(char*)RBR;
char *thr=(char*)THR;
 
struct buf {
	char *fbuf;
	char *wbuf,*rbuf;
	char *lbuf;
};
 
struct buf mem;			/* file receiption/transmission buff */

char combuf[64];		/* transmission buffer */
struct buf com;

char crtbuf[512];		/* receiption buffer */
struct buf crt;
 
char outfil[13];		/* transmit file name */
FILE *outchan;
 
char infil[13];			/* receive file name */
FILE *inchan;
 
char lecho = FALSE;
char gotxoff = FALSE;
char sendxoff = FALSE;
char sentxoff = FALSE;
 
/*********************************************************************/
/*                                                                   */
main(argc,argv)
int argc;
char *argv[];
{
	char *malloc();
	int iret,port;
	unsigned u;
 
	/* initialize file receiption/transmission */
	inchan = FALSE;
	outchan = FALSE;

	/* set up buffer structures */
	com.fbuf = com.wbuf = com.rbuf = &combuf[0];
	com.lbuf = &combuf[0] + sizeof(combuf) - 1;
	crt.fbuf = crt.wbuf = crt.rbuf = &crtbuf[0];
	crt.lbuf = &crtbuf[0] + sizeof(crtbuf) - 1;

	/* allocate as much memory as possible to file buffer */
	for (u=65023;;u -= 512) {
		if (u < (512+(5*_BUFSIZ))) {
			writes("\r\n\007*** not enough memory available ***");
			exit();
		}
		mem.fbuf = malloc(u);
		if (mem.fbuf != 0) {
			free(mem.fbuf);
			u -= 5*_BUFSIZ;
			mem.fbuf = malloc(u);
			break;
		}
	}

	mem.wbuf = mem.rbuf = mem.fbuf;
	mem.lbuf = mem.fbuf + u - 1;

	/* determine communications port to use */
	if (argc > 1) {
		sscanf(argv[1],"%d",&port);
		port--;
		if ((port < 0) || (port > 1)) {
			writes("\r\n\007*** invalid communications port - enter 1 or 2 ***");
			exit();
		}
	} else {
		port = 0;
	}

	/* adjust port addresses if neccessary */
	if (port == 1) {
		lcr -= 0x100;
		dll -= 0x100;
		dlm -= 0x100;
		lsr -= 0x100;
		iir -= 0x100;
		ier -= 0x100;
		mcr -= 0x100;
		msr -= 0x100;
		rbr -= 0x100;
		thr -= 0x100;
	}
 
	/* initialize 8250  - use DOS "MODE" command */
	outp(mcr,3);		/* send DTR and RTS */
	pause(1.);

	/* check for carrier */
	if ((inp(msr)&48) != 48) {
		writes("\r\n\007*** carrier not detected - please check your connections ***");
		exit();
	}

	/* courtesy message */
	cursor(25,1);
	writes("\r\n*** you are now connected ***\r\n");
 
	/* polling loop */
	for (;;) {

		/* check communications port for reception of data */
		if ((readcomm()) != 0)
			;

		/* check if screen output needed */
		if ((wrtscr()) != 0)
			;

		/* check communications port for receiption of data */
		if ((readcomm()) != 0)
			;

		/* write characters to comm port for transmission */
		if ((wrtcomm()) != 0)
			;

		/* check communications port for receiption of data */
		if ((readcomm()) != 0)
			;

		/* check for screen output */
		if ((wrtscr()) != 0)
			;

		/* check communications port for receiption of data */
		if ((readcomm()) != 0)
			;

		/* check for keyboard entry */
		if ((readkey()) != 0)
			;
 
	}
}

/*********************************************************************/
/* Check the comm port for receiption of data and puts recieved data */
/* into buffers.  Returns a -1 if error detected, 0 if no data, or   */
/* the character received.                                           */
/*                                                                   */
int readcomm()
{
	static char oldc = NULL;
	static char c;
	static int iret;

	/* check comm port for data receiption */
	iret = inp(lsr);
	if ((iret&1) != 0) {

		/* ignore data overrun errors */
		/* expect to loose padd characters at high baud rate */
		if ((iret&2) != 0) {
			;

		/* framing or parity error */
		} else if ((iret&12) != 0) {
			writes("\007*** data reception error ***");
			return(-1);

		/* break currenly being sent */
		} else if ((iret&16) != 0) {
			return(0);	/* break detect */
		}

		/* read character */
		c = inp(rbr)&127;

		/* process character */
		if (c == XOFF) {
			gotxoff = TRUE;
		} else if (c == XON) {
			gotxoff = FALSE;
		} else if (c != NULL) {

			/* prevent CR CR LF line endings */
			if ((c != '\r') || (oldc != '\r')) {

				/* store character in receiption buffer */
				if ((putbuf(&crt,c)) == (-1)) {
					writes("\007*** screen buffer overflow ***");
					return(-1);
				}

				/* store character if file receiption buffer */
				if (inchan) {
					if ((putbuf(&mem,c)) == (-1)) {
						writes("\007*** file receiption buffer overflow ***");
						inchan = FALSE;
						return(-1);
					}
				}
			}
			oldc = c;
		}
		return(c);
	}
 
	/* no character found */
	return(0);
}

/*********************************************************************/
/* write characters from the screen buffer to the screen.  Returns a */
/* -1 if error occured, 0 if no characters in buffer, or character   */
/* written.                                                          */
/*                                                                   */
int wrtscr()
{
	static char c;
	static int i,iret;

	/* get character from screen buffer */
	if ((iret = getbuf(&crt)) != (-1)) {
		c = iret&127;	/* strip parity */

		/* if line feed then scroll screen */
		if (c == '\n') {
			if ((iret = doscr()) == (-1)) {
				return(-1);
			}

		/* handle tabs seperatly */
		} else if (c == '\t') {
			if ((iret = dotab()) == (-1)) {
				return(-1);
			}

		/* output other characters */
		} else {
			biostty(c);
		}
		return(c);
	}

	/* no character in buffer */
	return(0);
}

/*********************************************************************/
/* scroll the screen up one line at a time while checking the comm   */
/* port for data receiption.  Returns a -1 if an error occured.      */
/*                                                                   */
int doscr()
{
	static char fillchar;
	static int trow,tlcol,brow,brcol;
	static int iret;

	/* initialize */
	fillchar = 0x20;	/* fill opened line with blanks */
	tlcol = 0;		/* top left column of window */
	brcol = 79;		/* bottom right column of window */

	/* begin check and scroll loop */
	for(trow=0;trow<24;trow++) {

		/* check comm port for data receiption */
		if ((iret = readcomm()) == (-1)) {
			return(-1);
		}

		/* scroll 1 line up */
		brow = trow + 1;
		biosup(1,trow,tlcol,brow,brcol,fillchar);
	}

	return(0);
}

/*********************************************************************/
/* Output a horizontal tab to screen while checking communications   */
/* port for transmission.  Returns a -1 if an error occured.         */
/*                                                                   */
int dotab()
{
	static int iret,irow,icol;

	/* check comm port for receiption of data */
	if ((iret = readcomm()) == (-1)) {
		return(-1);
	}

	/* find cursor position */
	iret = biospos();
	icol = iret & 255;	/* column returned in low order bits */
	irow = iret >> 8;	/* row returned in high order bits */

	/* check comm port for receiption of data */
	if ((iret = readcomm()) == (-1)) {
		return(-1);
	}

	/* calculate tab position */
	icol++;
	while ((icol % 8) != 0) {
		icol++;
	}

	/* set cursor to that position */
	biosset(irow,icol);

	return(0);
}

/*********************************************************************/
/* write characters to communications port for transmission.  Return */
/* a -1 if error, 0 if no characters written, or character written.  */
/*                                                                   */
int wrtcomm()
{
	static char c,pushback;
	static int iret,lstat;

	/* write characters to communications port for transmission */
	lstat=inp(lsr);
	if (((lstat&32) != 0) && (!gotxoff)) {

		/* write characters received from keyboard */
		if ((iret=getbuf(&com)) != (-1)) {
			c = iret&127;
			outp(thr,c);
			if (lecho) putbuf(&crt,c);
			return(c);

		/* write characters from file */
		} else if (outchan) {
			if (pushback != NULL) {
				outp(thr,pushback);
				pushback = NULL;
				if (lecho) putbuf(&crt,pushback);
				return(pushback);
			}

			if ((iret=getbuf(&mem)) == (-1)) {
				outp(thr,CONZ);
				fclose(outchan);
				outchan = FALSE;
				mem.wbuf = mem.rbuf = mem.fbuf;
				return(0);
			}

			c = iret&127;
			if (c == '\n') {
				outp(thr,'\r');
				if (lecho) {
					putbuf(&crt,'\r');
					putbuf(&crt,'\n');
				} else {
					dowait();
				}
			} else {
				outp(thr,c);
				if (lecho) putbuf(&crt,c);
			}
			return(c);
		}
	}

	return(0);
}

/*********************************************************************/
/* Function to wait until the receiption of a line feed after the    */
/* transmission of a carriage return.                                */
/*                                                                   */
int dowait()
{
	int iret;

	/* loop */
	while ((iret=readcomm()) != '\n') {
		if (iret == -1) break;
	}

	return(0);
}

/*********************************************************************/
/* Check the keyboard for receipt of a character and buffer it.      */
/* Returns a -1 if error occured, 0 if no character at keyboard, or  */
/* character typed.                                                  */
/*                                                                   */
/* kbhit() is a special IBM-PC microsoft "C" function.               */
/*                                                                   */
int readkey()
{
	static char c;

	/* check for keyboard entry */
	if (kbhit()) {
		c = readc();

		/* display menu if ESCAPE entered */
		if (c == ESC) {
			doesc();
			return(c);
		}

		/* character mapping */
		if (c == '\b') {
			c = DEL;
		} else if (c == '\n') {
			c = '\r';
		}

		/* store character in keyboard buffer */
		if (putbuf(&com,c) == (-1)) {
			writes("\007");		/* buffer full */
			return(-1);
		}
		return(c);
	}
 
	return(0);
}

/*********************************************************************/
/* Write a character into a buffer.  Returns a -1 if overflow.       */
/*                                                                   */
int putbuf(p,c)
struct buf *p;
char c;
{
	*p->wbuf = c;
 
	/* increment (possible wrap-around) pointer */
	if (++p->wbuf > p->lbuf) p->wbuf = p->fbuf;
 
	/* if overflow, indicate */
	if (p->wbuf == p->rbuf) {
		if (--p->wbuf < p->fbuf) p->wbuf = p->lbuf;
		return(-1);
	}
 
	return(0);
}
 
/*********************************************************************/
/* Read a character from a buffer.  Returns a -1 if buffer is empty. */
/*                                                                   */
int getbuf(p)
struct buf *p;
{
	static int c;
 
	/* empty */
	if (p->rbuf == p->wbuf) return(-1);
 
	/* get char, increment (possible wrap-around) pointer */
	c = *p->rbuf;
	if (++p->rbuf > p->lbuf) p->rbuf = p->fbuf;
 
	return(c);
}
 
/*********************************************************************/
/* Function to do special features.                                  */
/*                                                                   */
int doesc()
{
	char ans[3],memsiz[6];
	char c;
	int iret,ians;

	/* output special information */
	loop:
	escreen(2);		/* erase screen */

	writes("\r\n memory buffer is : ");
	sprintf(memsiz,"%u",(mem.wbuf-mem.fbuf));
	writes(memsiz);
	writes("/");
	sprintf(memsiz,"%u",(mem.lbuf-mem.fbuf));
	writes(memsiz);

	writes("\r\n receive file is  : ");
	writes((inchan)?infil:"<OFF>");
 
	writes("\r\n transmit file is : ");
	writes((outchan)?outfil:"<OFF>");
 
	writes("\r\n local echo is    : ");
	writes((lecho)?"<ON>":"<OFF>");

	/* output menu and accept choice */
	if ((ians=chosit("comm",ans,sizeof(ans))) == (-1)) {
		writes("\r\n\007*** error displaying menu ***");
		ians = 4;
	}

	/* execute command */
	switch (ians) {
 
		case 5:
			if (inchan) {
				wrtbuf();
				fclose(inchan);
			}
			if (outchan) fclose(outchan);
			outp(mcr,0);	/* drop DTR and RTS */
			writes("\r\n");
			exit();
			break;

		case 4:
			cursor(25,1);
			writes("\r\n*** you are now back as a terminal ***\r\n");
			return(0);
			break;
 
		case 3:
			lecho = !lecho;
			break;
 
		case 2:
			if (outchan) {
				fclose(outchan);
				outchan = FALSE;
				mem.wbuf = mem.rbuf = mem.fbuf;
			} else {
				if (inchan) {
					wrtbuf();
					fclose(inchan);
					inchan = FALSE;
				}
				sendfile:
				writes("\r\n\nFilename? ");
				reads(outfil,sizeof(outfil));
				if (outfil[0] == NULL) goto done;

				if ((outchan=fopen(&outfil[0],"r")) == NULL) {
					writes("\007  *** can't open file ***");
					goto sendfile;
				}

				readbuf();
			}
			break;
 
		case 1:
			if (inchan) {
				wrtbuf();
				fclose(inchan);
				inchan = FALSE;
			} else {
				if (outchan) {
					fclose(outchan);
					outchan = FALSE;
					mem.wbuf = mem.rbuf = mem.fbuf;
				}
				savefile:
				writes("\r\n\nFilename? ");
				reads(infil,sizeof(infil));
				if (infil[0] == NULL) goto done;

				if ((inchan=fopen(&infil[0],"w")) == NULL) {
					writes("\007  *** can't open file ***");
					goto savefile;
				}
			}
			break;
 
	}
	done:
	goto loop;
}
 
/*********************************************************************/
/* Function to write the data receiption buffer to output file.      */
/*                                                                   */
int wrtbuf()
{
	for (mem.rbuf=mem.fbuf;mem.rbuf<mem.wbuf;mem.rbuf++) {
		fputc(*mem.rbuf,inchan);
	}
 
	mem.wbuf = mem.rbuf = mem.fbuf;
	return(0);
}

/*********************************************************************/
/* Function to read a file into the file transmission buffer.        */
/* Returns a 0 if successful, or a -1 if buffer overflow.            */
/*                                                                   */
int readbuf()
{
	static char c;
	static int iret;

	/* loop until end-of-file is found */
	while ((iret=getc(outchan)) != EOF) {
		c = iret&127;
		if ((putbuf(&mem,c)) == (-1)) {
			writes("\r\n\007*** file transmission buffer overflow ***");
			return(-1);
		}
	}

	return(0);
}
