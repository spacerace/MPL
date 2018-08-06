/*
>>:yam11.c   01-Jun-83
 * Console primitives 
 * 'moment','pstat','lpstat','lprintf','getcty','putcty','sendcon'
 * extracted from all other YAM modules, and placed here so that
 * they may be more easily got at and altered for different
 * machines	(jdw).
 */

#include "yam.h"

/* following are BIOS calls */
#define CONST 2			/* bios console char ready */
#define CONIN 3			/* bios cons char input */
#define CONOUT 4		/* bios cons char output */

/* following are BDOS calls */
#define BDOS 5			/* address used to call bdos */
#define SELDSK 14		/* bdos select disk 0=default disk */
#define SRCH 17			/* bdos search for file pattern*/
#define SRCHNXT 18		/* search for next occurrence */
#define GETDEFDISK 25		/* get current disk  (0-15) */
#define SETDMA 26		/* set address for read, write, etc. */
#define GETALLOCP 27		/* get address of allocation vector */
#define SETATTRIB 30		/* update file attributes */
#define GETDPBP 31		/* get DPB address for disk */
#define SETGETUSER 32		/* set or get user number */
#define COMPFILSIZ 35		/* compute file size into recn and recovf */
#define UFNSIZE 15		/* a:foobar12.urk\0 is 15 chars */



#ifdef	SORCERER

		   /* a local video handler for status line */
		   /* also keyboard cause unmodded Sorcerer
		      upsets UART port */

putchar(c)
char (c);
{
    putch(c);
}


putch(c)
char c;
{
	int mwasav;

    mwasav = *mwalin;
    switch (c)
    {
	case (12) : callit(17);callit(' ');callit(17);
		    setmem(0xf080,1856,' ');
		    setmem(0xf780,64,'=');
		    break;

      case ('\t') : callit(' ');
		    while (*mwacol%8)
		       putch(' ');
		    break;

      case ('\n') : callit('\r');

	  default : callit (c);
		    if((*mwacol==63) && (mwasav==*mwalin))
			putch('\n');

		    if (*mwalin >= 1792)
		    {
			movmem(0xf0c0,0xf080,1728);
			setmem(0xf740,64,' ');
			setmem(0xf780,64,'=');
			callit(23);
		    }
		    break;
    }

}

getchar()
{
	char c;
	c = getch();
	if (c) putchar(c);
	return c;
}


getch()
{
	return (keyget());
}




/*
 * SORCERER implements 30th line. pstat starts at lpstat + 3
 * note that a call to lpstat will erase what pstat displays
 */

pstat(a)
char *a;
{
	char lin[MAXLINE];

	setmem(0xf7c0 + statpos,64 - statpos,' ');
	_spr(lin,&a);
	txtplot(lin,29,statpos,0);

}

/*
 * Sorcerer 30th line. lpstat starts at col 1
 * Rest of line is erased
 */

lpstat(a,b,c)
char *a, *b, *c;
{
	char lin[MAXLINE];

	setmem(0xf7c0,64,' ');
	_spr(lin,&a);
	txtplot(lin,29,0,0);
	statpos = strlen(lin) + 3;
}



char getcty()
{
	return keyget();
}
char putcty(c)
char c;
{
#ifdef RXNONO
	if(index(c, RXNONO))
		return 0;
#endif
	putchar(c);
	if(keyprs()) {
		if( (c=(keyget() & KBMASK)) == XOFF)
			keyget();
		else
			return c;
	}
	return FALSE;
}

/*
 * moment waits a moment and returns FALSE, unless a character is hit,
 * in which case that character is returned
 */

moment(n)
int n;
{
	int c;
	for(c=n*CLKMHZ; --c>0;)
		if(keyprs())
			return(keyget());
	return FALSE;
}



sendcon(c)	    /* direct output to console */

char c;

{
	putch(c);
}

/* Any special initialisation for console hardware goes here */

init1_extra()

{
	mwalin = mwa() + 0x68;
	mwacol = mwa() + 0x6a;
	statpos = 0;
}

init2_extra()

{
}

#endif	    /* SORCERER */

#ifndef SORCERER

/*
 * moment waits a moment and returns FALSE, unless a character is hit,
 * in which case that character is returned
 */

moment(n)
int n;

{
	int c;
	for(c=n*CLKMHZ; --c>0;)
		if(kbhit())
			return(getchar());
	return FALSE;
}

pstat(a,b,c)
char *a, *b, *c;

{
	lprintf(a,b,c);
	lprintf("\n");
}

lpstat(a,b,c)
char *a, *b, *c;

{
	lprintf(a,b,c);
	lprintf("\n");
}

char getcty()

{
	return bios(CONIN,0);
}


char putcty(c)
char c;

{
#ifdef RXNONO
	if(index(c, RXNONO))
		return 0;
#endif /* RXNONO */

	if(View)		/* view set by list command, allows ^p */
		putchar(c);
	else{
		sendcon(c);
		if(Type)
			sendline(c);	/* so remote user can see it */
	}
	if(bios(CONST,0)) {
		if((c=(bios(CONIN,0)&KBMASK))==XOFF)
			bios(CONIN,0);
		else
			return c;
	}
	return FALSE;
}

#ifdef XMODEM
/*
 * lprintf is like regular printf but uses direct output to console
 * This prevents status printouts from disrupting file transfers, etc.
 */

lprintf(a,b,c,d,e,f)
char *a, *b, *c, *d, *e, *f;

{
	char lbuf[CMDLEN];
	char *s;

	sprintf(lbuf, a,b,c,d,e,f);	 /* format data into lbuf */
	for(s=&lbuf; *s != '\0' ; ) {	/* now send lbuf to console directly */

		MODEMON
		if (CDO){
			printf("Carrier lost in lprintf function\n");
			exit();			/* quit if carrier lost */
		}
		if(*s=='\n') {
			sendcon('\r');		/* expand \n to \r\n */
		}
		sendcon(*s++);
	}
}
#endif

sendcon(c)	    /* direct output to console */
char c;

{
#ifdef XMODEM
		MODEMON
		if (CDO){
			printf("Carrier lost in sendcon function\n");
			exit();
		}
#endif /* XMODEM */

#ifdef CODATA
		CONSOLON
		while(!COREADY)
		    ;
		outp(CODATA,c);
#else
		bios(CONOUT,c);
#endif /* CODATA */
}

/* Any special initialisation for console hardware goes here */

init1_extra()

{
}

init2_extra()

{
}

#endif /* !SORCERER */

/* end of YAM11.C   */
