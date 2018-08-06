/*
>>:yam5.c 02-Jun-83
*
* This version implements parameterized #define in Sendline() routine
*
* Modem related functions. If your(modem supports baudrate setting,
* and hangup, write your own routines here.
* If your modem supports an autocall, put that routine here also
* define MODEMSTUFF to suppress default baud rate related functions.
* define AUTOCALL in your autocall routine to suppress the default
* which merely prints the phone number and recommended baud rate.
*
* LOOP & TEST are used for loopback testing of the communications
*	       line.
*
* LOOP simply loop comm input to comm output as each character
*	is processed, (or a timeout occurs) a '.' is printed to
*	console.  This is simply to let you monitor what is
*	going on. Slow dots = timeouts = nothing doing. 
*	Speedy dots = chars echoed.
*
*   usage 'LOOPn' where n is a number of characters (or timeouts)
*		   to stay in the loop . Keyboard bashing exits
*		   (n ommitted = 65535).
*
* TEST performs loopback tests (sending a char & reading the next
*	received char to see if the same) on the line that is
*	terminated in a loop, either physically (at modem) or
*	by other end being in 'LOOP'.
*	Successfully received chars output a '.' to console,
*	while errors echo an '*', and timeouts say so.
*
*  usage 'TESTn' where n is number of characters to send to
*		  line, keyboard bashing also exits.  Final
*		  message is number of errors (not timeouts)
*		  received.
*/

#include "yam.h"

#ifdef SORCERER
#define MODEMSTUFF
/*
* set baud rate for modem driven by Sorcerer + bitrate chip
*/

setbaud(nbaud)
unsigned nbaud;
{
	char baudcmd, *i;
	switch(nbaud)
	{
	case  300: baudcmd=0x80; break;
	case 1200: baudcmd=0xc0; break;
	default:
		return ERROR;
	}

	i = mwa() + 0x45;

	Sport = 0xfd;
	Dport = 0xfc;
	Somask = 0x01;
	Simask = 0x02;

	outp(Sport,0xfb);	/* 8 bits, 1 stop, no parity */

	*i = (baudcmd);
	Baudrate=nbaud;
	return 0;
}

readbaud()
{
	Baudrate=DEFBAUD;
}

/* Bye disconnects the line and allows another call */

bye()
{
	sleep(40/CLKMHZ);	/* wait two seconds */
	setbaud(Baudrate);
}

/* onhook disconnects the line for good */

onhook()

{}	/* no way to go on hook with this setup */

/* online connects the line    */

online()
{}	/* not implemented	*/

#endif	/* Sorcerer	*/

#ifdef MONROE
#define MODEMSTUFF
/*
* set baud rate for modem driven from MONROE
*/
setbaud (nbaud)
unsigned nbaud;
{
	char command;
	unsigned baud,count;

	count=command=0;	    /* to detect an error  */
	baud=nbaud;		    /* save for later	   */



	/* Select required SIO range factor
	and also test for legal baudrates */

	switch(baud)
	{
	case(110)	:
	case(134)	:
	case(150)	:
	case(300)	:
		command=0xce;
		count=23437;
		break;

	case(0600)	:
	case(1200)	:
	case(1800)	:
	case(2000)	:
	case(2400)	:
	case(3600)	:
	case(4800)	:
	case(7200)	:
	case(9600)	:
		command=0x4e;
		count=9375;	/* prescaling because can't handle */
		baud/=10;	/* numbers like 1.5 million */
		break;

	case(19200) :
		command=0x0e;
		count=15000;
		baud/=100;
		break;

	default	:
		return ERROR;
	}

	count=(count/baud) + (((count%baud) > (baud/2)) ? 1 : 0);
			/* divide with rounding */

	outp(Sport,04);			 /* select SIO divisor	*/
	outp(Sport,command);

	outp(CTCPRT,0x45);
	outp(CTCPRT,count);		 /* set CTC to required count	*/

	Baudrate=nbaud;
	return OK;
}

readbaud ()
{
	Baudrate=DEFBAUD;
}

/* Bye disconnects the line and allows another call */
bye ()
{
	outp(Dport, 03);	 /* send a ctrl C    */
	sleep(40/CLKMHZ);	/* wait two seconds */
	setbaud(Baudrate);
}

/* onhook disconnects the line for good */
onhook ()
{
	/* no way to go on hook with this setup */
}

/* online connects modem to the line */
online ()
{
	/* no way to go online with this setup */
}

#endif /* MONROE */


#ifdef SANYO
#define MODEMSTUFF
/*
* set baud rate for modem driven from SANYO
* Requires MBC-1000 to be jumpered for 4800 baud
* Other software baud-rate pairs may be programmed
* here instead of 4800 & 1200 baud. e.g. if MBC-1000
* is programmed for 1200 baud these rates become
* 1200 & 300 baud respectively.
*/
setbaud (nbaud)
unsigned nbaud;
{

unsigned command;

	switch (nbaud)
	{
	case(4800) : command = 0x4e; break;  /* U8251 div 16 */
	case(1200) : command = 0x4f; break;  /* U8251 div 64 */
	default	   : return ERROR;
	}

	outp(Sport,0x80);	/* mode, set synch, one synch */
	outp(Sport,0x00);	/* synch */
	outp(Sport,0x00);	/* control */
	outp(Sport,0x00);	/* control */

	outp(Sport,0x40);	/* reset U8251 control */
	outp(Sport,command); /* send required command */

	outp(Sport,MI_ERROR_RESET);	    /* reset all errors control */


	Baudrate=nbaud;
	return OK;
}

readbaud ()
{
	Baudrate=DEFBAUD;
}

/* Bye disconnects the line and allows another call */
bye ()
{
	outp(Dport, 03);	 /* send a ctrl C    */
	sleep(40/CLKMHZ);	/* wait two seconds */
	setbaud(Baudrate);
}

/* onhook disconnects the line for good */
onhook ()
{
	/* no way to go on hook with this setup */
}

/* online connects modem to the line */
online ()
{
	/* no way to go online with this setup */
}

#endif /* SANYO */


#ifdef OKI
#define MODEMSTUFF
/*
* set baud rate for modem driven from Oki-if800
*/
setbaud (nbaud)
unsigned nbaud;
{

unsigned command;

	switch (nbaud)
	{
	case(110)  : command = 0; break;
	case(150)  : command = 1; break;
	case(300)  : command = 2; break;
	case(600)  : command = 3; break;
	case(1200) : command = 4; break;
	case(2400) : command = 5; break;
	case(4800) : command = 6; break;
	case(9600) : command = 7; break;
	default	   : return ERROR;
	}

	outp(0x22,command);		/* Boy did I have fun finding this
					port number. Documents superb!!??##  */

/*  Initialize the U8251 chip	*/

	outp(Sport,0x80);	/* mode, set synch, one synch */
	outp(Sport,0x00);	/* synch */
	outp(Sport,0x00);	/* control */
	outp(Sport,0x00);	/* control */

	outp(Sport,0x40);	/* reset U8251 control */
	outp(Sport,0x4e);	/* send /16 8d, 1s, np */

	outp(Sport,MI_ERROR_RESET);	    /* reset all errors control */


	Baudrate=nbaud;
	return OK;
}

readbaud ()
{
	Baudrate=DEFBAUD;
}

/* Bye disconnects the line and allows another call */
bye ()
{
	outp(Dport, 03);	 /* send a ctrl C    */
	sleep(40/CLKMHZ);	/* wait two seconds */
	setbaud(Baudrate);
}

/* onhook disconnects the line for good */
onhook ()
{
	/* no way to go on hook with this setup */
}

/* online connects modem to the line */
online ()
{
	/* no way to go online with this setup */
}

#endif /* OKI */


#ifdef U2651
#define MODEMSTUFF
/*
* set baud rate for modem driven by 2651 UART
*/
setbaud(nbaud)
unsigned nbaud;
{
	char command;
	char baudcmd;

	command=0;

	switch(nbaud) {
	case    50: baudcmd=  0; break;
	case    75: baudcmd=  1; break;
	case   110: baudcmd=  2; break;
	case   134: baudcmd=  3; break;
	case   150: baudcmd=  4; break;
	case   300: baudcmd=  5; break;
	case   600: baudcmd=  6; break;
	case  1200: baudcmd=  7; break;
	case  1800: baudcmd=  8; break;
	case  2000: baudcmd=  9; break;
	case  2400: baudcmd= 10; break;
	case  3600: baudcmd= 11; break;
	case  4800: baudcmd= 12; break;
	case  7200: baudcmd= 13; break;
	case  9600: baudcmd= 14; break;
	case 19200: baudcmd= 15; break; 

	default:
		return ERROR;
	}

	MODEMON
	outp(MPORT, MODE1);
	outp(MPORT, MODE2 + baudcmd);
	outp(CPORT, Commbyte);		/* just to be safe */
	Baudrate = nbaud;
	return OK;
}

readbaud()
{
	char nbaud;

	MODEMON

	nbaud = inp(MPORT);		/* throw away first mode byte */
	nbaud = inp(MPORT) & 0X0F;	/* low nibble has baud rate */
	Commbyte = inp(CPORT);		/* get current command byte */

	switch(nbaud) {
	case  0: Baudrate=   50; break;
	case  1: Baudrate=   75; break;
	case  2: Baudrate=  110; break;
	case  3: Baudrate=  134; break;
	case  4: Baudrate=  150; break;
	case  5: Baudrate=  300; break;
	case  6: Baudrate=  600; break;
	case  7: Baudrate= 1200; break;
	case  8: Baudrate= 1800; break;
	case  9: Baudrate= 2000; break;
	case 10: Baudrate= 2400; break;
	case 11: Baudrate= 3600; break;
	case 12: Baudrate= 4800; break;
	case 13: Baudrate= 7200; break;
	case 14: Baudrate= 9600; break;
	case 15: Baudrate=19200; break;
	}
}

/* Bye disconnects the line and allows another call */
bye()
{
	onhook();			/* turn of DTR */
	sleep(20/CLKMHZ);		/* wait one second */
	online();			/* turn on DTR */
}

/* onhook disconnects the line for good */
onhook()
{
	if(Commbyte & 0x02){
		MODEMON
		outp(CPORT, Commbyte -= 0x02);
	}
}

/* online connects the modem to line */
online()
{
	if(!(Commbyte & 0x02)){
		MODEMON
		outp(CPORT, Commbyte += 0x02);
	}
}
#endif  /* U2651 */

#ifdef TUART
#define MODEMSTUFF
/*
* set baud rate for modem driven by Cromenco TUART
*/
setbaud(nbaud)
unsigned nbaud;
{
	char command, baudcmd;
	command=0;
	switch(nbaud) {
	case   110: baudcmd=0001; break;
	case   150: baudcmd=0202; break;
	case   300: baudcmd=0204; break;
	case  1200: baudcmd=0210; break;
	case  2400: baudcmd=0220; break;
	case  4800: baudcmd=0240; break;
	case  9600: baudcmd=0300; break;
	case 19200: baudcmd=0220; command=020; break;
	case 38400: baudcmd=0240; command=020; break;
	default:
		return ERROR;
	}
	outp(Sport, baudcmd);
	outp(Sport+2, command);
	Baudrate=nbaud;
	return 0;
}
readbaud()
{
	Baudrate=DEFBAUD;
}

/* Bye disconnects the line and allows another call */
bye()
{
	outp(Sport+2, 03);	/* turn on break */
	sleep(40/CLKMHZ);	/* wait two seconds */
	setbaud(Baudrate);
}

/* onhook disconnects the line for good */
onhook()
{}	/* no way to go on hook with this setup */
#endif /* TUART */

#ifdef Z89
#define MODEMSTUFF
/*
* Routine to set baud rate for 8250 driven modem port
*/
setbaud(nbaud)
unsigned nbaud;
{
	unsigned bcmd;

	if(nbaud==0)
		return ERROR;
	bcmd= 57600;		/* 1.8432 Mhz clock */
	bcmd /= (nbaud/2);	/* this must be done unsigned! */
	outp(Dport+3, 0203);    /* enable divisor latch */
	outp(Dport, bcmd);
	outp(Dport+1, (bcmd>>8));

	/* 8 data bits, 1 stop bit (2 if 110 baud), disable divisor latch */

	outp(Dport+3, nbaud==110? 07:03);

	/* turn on dtr and rts, also output2 is baudrate is 1200 or more */

	outp(Dport+4, nbaud>=1200?017:03);
	Baudrate=nbaud;
/*
	printf("bcmd=%d bcmdms=0%o bcmdls=0%o\n",bcmd,(bcmd>>8),(bcmd&0377));
*/
	return OK;
}
/* fetch the baudrate from the modem port */
readbaud()
{
	char dp3;
	unsigned div;

	dp3=inp(Dport+3);
	outp(Dport+3, 0200|dp3);
	div= inp(Dport) | (inp(Dport+1)<<8);	/* fetch divisor */
	outp(Dport+3, dp3);			/* restore uart modes */
	Baudrate=57600;			/* be sure all this remains unsigned */
	Baudrate /= div;
	Baudrate <<= 1;
}

/* Bye hangs up the line and then resets for another call */
bye()
{
	onhook();
	sleep(40/CLKMHZ);
	setbaud(Baudrate);
}

/* onhook goes off line for good */
onhook()
{
	outp(Dport+4, 0);
}

#endif /* Z89 (8250) */

#ifdef TRSII
#define MODEMSTUFF
/*
 * set baud rate for modem driven by TRS 80 MOD II with Pickles & Trout CP/M
 */
setbaud(nbaud)
unsigned nbaud;
{
	char calla(), command, baudcmd, regB, regC, regD, regE, regH, regL;
	command=0;
	switch(nbaud) {
	case   110: baudcmd=0; break;
	case   150: baudcmd=2; break;
	case   300: baudcmd=3; break;
	case   600: baudcmd=4; break;
	case  1200: baudcmd=5; break;
	case  2400: baudcmd=6; break;
	case  4800: baudcmd=7; break;
	case  9600: baudcmd=8; break;
	default:
		return ERROR;
	}

	regB = 0;		/* setup  serial ports */
	regC = 0;		/* port a, parity odd and disabled */
	regD = 0xe6;		/* DTR,RTS high, 8-bit wds, 1 stop */
	regE = baudcmd;		/* baud rate */
	regH = 19;		/* Xmit off char */
	regL = 17;		/* Xmit on-off handshaking; X-on char */
	calla(0x40,0,(regH << 8 | regL),(regB << 8 | regC),
			(regD << 8 | regE));   /* call P&T special function */
	Baudrate=nbaud;
	return 0;
}

readbaud()
{
	Baudrate=DEFBAUD;
}

/* Bye disconnects the line and allows another call */
bye()
{
	onhook();
	sleep(40/CLKMHZ);	/* wait two seconds */
	setbaud(Baudrate);
}

/* onhook disconnects the line for good */
onhook()
{
	char calla();
	calla(0x40,0,0,0x40,0);	/* set DTR, RTS low */
}
#endif /* TRSII */

#ifdef 8251
#define MODEMSTUFF

setbaud()
{}		/* Baud rate set in hardware on many 8251 implemenetations */

onhook()
{}		/* No way to set onhook on many 8251 implementations */

online()
{}		/* No way to set off hook on many 8251 implementations */

bye()
{}		/* No way to drop line on many 8251 implementtations */

readbaud()
{
	Baudrate=DEFBAUD;	/* Set to default baudrate */
}

#endif /* 8251 */

#ifndef MODEMSTUFF
setbaud()
{}	/* INSERT ROUTINE HERE IF AVAILABLE */
onhook()
{}	/* INSERT ROUTINE HERE IF AVAILABLE */
online()
{}	/* INSERT ROUTINE HERE IF AVAILABLE */
bye()
{}	/* INSERT ROUTINE HERE IF AVAILABLE */
readbaud()
{
	Baudrate=DEFBAUD;
}
#endif /* !MODEMSTUFF */

/*
 * Readline from MODEM13.C rewritten to allow much higher
 * baud rates.
 * Timeout is in deciseconds (1/10th's)
 * For top speed, character ready is checked in many places.
 * returns TIMEOUT if kbd character is ready.
 *
 * There are three versions of readline, the first is used if
 * there is a separate register for error conditions. The second
 * is used if error condx are in the same register asrx data ready.
 * The last, and quickest, does not check error conditions.
 */
#ifdef MIERROR
/* version for separate error register. NOT TESTED */
readline(decisecs)

int decissecs;

{
	if(MIREADY) {
		if(MIERROR)
			goto fubar;
		else
			return MICHAR;
	}
	while(--decisecs>=0) {
		if(MIREADY) {
			if(MIERROR)
				goto fubar;
			else
				return MICHAR;
		}

#ifdef CDO
		if(CDO)
			return TIMEOUT;
#endif
		if(CIREADY) {
			CICHAR;		/* dismiss character */
			return TIMEOUT;
		}
		for(Timeout=T1pause; --Timeout; )
			if(MIREADY) {
				if(MIERROR)
					goto fubar;
				else
					return MICHAR;
			}
	}
	return TIMEOUT;
fubar:
	MICHAR;		/* throw the turkey away */
	MI_ERROR_RESET;
	return ERROR;
}

#define READLINE
#endif /* MIERROR */

#ifdef MIREADYERROR
/* Version for 8250, 8251, 2651, etc. with all bits in one register */
readline(decisecs)

int decisecs;

{
	MODEMON
	if((Mstatus=inp(Sport))&MIREADYMASK)
		goto getit;
	while(--decisecs>=0) {
		if((Mstatus=inp(Sport))&MIREADYMASK)
			goto getit;
#ifdef CDO
		if(CDO)
			return TIMEOUT;
#endif /* CDO */
		if((Mstatus=inp(Sport))&MIREADYMASK)
			goto getit;
		CONSOLON
		if(CIREADY) {
			CICHAR;		/* dismiss character */
			return TIMEOUT;
		}
		MODEMON
		if((Mstatus=inp(Sport))&MIREADYMASK)
			goto getit;
		for(Timeout=T1pause; --Timeout; )
			if((Mstatus=inp(Sport))&MIREADYMASK) {
getit:
				if(Mstatus&MIERRORMASK) {
					modreset();	/* reset error bits */
					Mstatus &= MIERRORMASK;
					return ERROR;
				}
				else
					return MICHAR;
			}
	}
	return TIMEOUT;
}

modreset()

{
#ifdef CPORT

	if(!(Commbyte & MI_ERROR_RESET)){
		outp(CPORT, (Commbyte += MI_ERROR_RESET)); /* reset error */
	}
	else{
		outp(CPORT, Commbyte);
	}
	outp(CPORT, (Commbyte -= MI_ERROR_RESET));	/* clear reset */

#else /* CPORT */

#ifdef MI_ERROR_RESET

	outp(Sport,MI_ERROR_RESET);		/* clear error on U8251	 */

#endif /* MI_ERROR_RESET */

#endif /* CPORT */

	MICHAR;					/* throw away bad char */
}

#define READLINE

#endif /* MIREADYERROR */

#ifndef READLINE
readline(decisecs)

int decisecs;

{
	MODEMON
	if(MIREADY)
		return MICHAR;
	while(--decisecs>=0) {
		if(MIREADY)
			return MICHAR;
		CONSOLON
		if(CIREADY) {
			CICHAR;		/* dismiss character */
			return TIMEOUT;
		}

		MODEMON
		if(MIREADY)
			return MICHAR;
		for(Timeout=T1pause; --Timeout; )
			if(MIREADY)
				return MICHAR;
	}
	return TIMEOUT;
}
#endif /* !READLINE */

sendline(data)
char data;

{
	int	n;
	n = Tpause;	/* don't wait all day, 0.5 sec will do */

	MODEMON
	while(!MOREADY && n--)
		;
	MODATA(data);
}

purgeline()

{
	MODEMON
	while(MIREADY)
		MICHAR;
}

/* default "autodial" routine */
#ifndef AUTODIAL
dial(name)
char *name;

{
	char *s;
	if((s=cisubstr(name, "\tb")))
		if(!setbaud(atoi(s+2)))
			printf("Baudrate set to %u: ", Baudrate);
	printf("%s\n", name);
	return OK;
}
#endif /* !AUTODIAL */


linetest (n)
int n;

{
	int x,e;
	int c;
	char lstc;

	srand(n);
	printf(" Line test at %d Baud\n",Baudrate);
	CONSOLON
	while (CIREADY)
		CICHAR;
	purgeline();
	for (e=0;--n;) {
		if (moment(KTIME))
			break;
		lstc = rand();
		sendline(lstc);
		c = readline(10);

		if (c == TIMEOUT)
			printf("\nTimeout error");
		else {
			if (c==lstc)
				putchar('.');
			else {
				e++;
				putchar('*');
				readline(10);
			}

			if (!(n&63))
			putchar('\n');
		}
	}
	printf("\n %d Errors detected\n",e);
}

loop (n)    /* loop back mode for doing line tests */
int n;

{
	CONSOLON
	while (CIREADY)
		CICHAR;
	printf("Line looped, Hit any key to exit\n");
	while (!moment(KTIME) && --n) {
		sendline( readline(10));
		(n&63) ? putchar('.') : putchar('\n');
	}
	putch('\n');
}

/* end of YAM5.C  */
