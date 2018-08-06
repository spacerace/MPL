/*
>>:yam2.c 04-Jun-83 (diff 05-11-81)
 * Ward Christensen Protocol handler for sending and receiving
 * ascii and binary files.  Modified for choice of checksum or crc.
 */

#include "yam.h"
#define WCEOT (-10)

wcsend(argc, argp)
char **argp;
{
	int wcs();

	dumprxbuff();		      /* flush buffers first  */
	closerx(TRUE); closetx(TRUE);

	Crcflg=FALSE;
	firstsec=TRUE;
	if(Batch)
	{
#ifdef XMODEM
		printf("Sending in Batch Mode\n");
#endif
		if(expand(wcs, argc, argp)==ERROR)
		{
			goto fubar;
		}
		if(wctxpn("")==ERROR)
			goto fubar;
	}
	else
	{
		for(; --argc>=0;)
		{
			if(opentx(*argp++)==ERROR)
			{
				goto fubar;
			}
			if(wctx()==ERROR)
				goto fubar;
		}
	}
	return OK;
fubar:
	closetx(TRUE);
	sendline(CAN);
	sendline(CAN);
	sendline(CAN);
	return ERROR;
}

wcs(name)
char *name;
{
	if(opentx(name)==ERROR)
		return ERROR;
	if(wctxpn(name)!= ERROR)
		return wctx();
	else
	{
		return ERROR;
	}
}


wcreceive(argc, argp)

char **argp;

{
	char rpn[SECSIZ];		/* rx path name */

	dumprxbuff();		      /* flush buffers first  */
	closerx(TRUE); closetx(TRUE);

	if(Batch || argc==0)
	{
		printf("Receiving in Batch Mode\n");
		for(;;)
		{
			if(wcrxpn(rpn)== ERROR)
				goto fubar;
			if(*rpn==0)
				return OK;
			if(wcrx(rpn)==ERROR)
				goto fubar;
		}
	}
	else
		for(; --argc>=0;)
		{
#ifdef XMODEM
			printf("Receive:'%s' FILE OPEN\n", *argp);
#endif
			if(wcrx(*argp++)==ERROR)
				goto fubar;
		}
	return OK;
fubar:
	sendline(CAN);sendline(CAN);sendline(CAN);
	closerx(TRUE);
	return ERROR;
}

/*
 * Fetch a pathname from the other end as a C ctyle ASCIZ string.
 * Length is indeterminate as long as less than SECSIZ
 * a null string represents no more files
 */

wcrxpn(rpn)

char *rpn;	/* receive a pathname */

{
	char rname[SECSIZ];

	purgeline();
	firstsec = TRUE;
	sendline(Crcflg?WANTCRC:NAK);
	if(wcgetsec(rname, 100) != 0)
	{
		printf("Pathname fetch failed\n");
		return ERROR;
	}
	strcpy(rpn, rname);
	sendline(ACK);
	return OK;
}


wctxpn(name)

char *name;

{
	char *p;

	lpstat("Awaiting initial pathname NAK");
	if((firstch=readline(400))==TIMEOUT)
		return ERROR;
	if(firstch==WANTCRC)
		Crcflg=TRUE;
	/* don't send drive specification */
	if(p=index(':', name))
		name= ++p;
	if(wcputsec(name, 0)==ERROR)
	{
		printf("Can't send pathname %s\n", name);
		return ERROR;
	}
	return OK;
}

/*
 * Adapted from CMODEM13.C, written by
 * Jack M. Wierda and Roderick W. Hart
 */

wcrx(name)

char *name;

{
	int sectnum, sectcurr, sectcomp;
	char rxbuf[128], sendchar;

	if(openrx(name)==ERROR)
		return ERROR;
	firstsec=TRUE;sectnum=0;
	sendchar=Crcflg?WANTCRC:NAK;

	for(;;)
	{
		if(!Quiet
#ifdef STATLINE
		)
			lpstat("Sector %3d", sectnum+1);
#else
		&& !View)
			lprintf("%s", (sectnum+1 &63)?"*":"*\n");
#endif
		purgeline();
		sendline(sendchar);	/* send it now, we're ready! */
		sectcurr=wcgetsec(rxbuf, (sectnum&0177)?100:200);
		if(sectcurr==(sectnum+1 &0377))
		{

			sectnum++;
			for(cp=rxbuf,wcj=128; --wcj>=0; )
				if(putc(*cp++, fout)==ERROR)
				{
					printf("\nDisk Full\n");
					return ERROR;
				}
#ifndef XMODEM
			if(View)
				for(cp=rxbuf,wcj=128;--wcj>=0;)
					putchar(*cp++);
#endif
			sendchar=ACK;
		}
		else if(sectcurr==sectnum)
		{
			pstat("Received dup Sector %d",sectcurr);
			sendchar=ACK;
		}
		else if(sectcurr==WCEOT)
		{
			sendline(ACK);
			/* don't pad the file any more than it already is */
			closerx(FALSE);
			return OK;
		}
		else if(sectcurr==ERROR)
			return ERROR;
		else
		{
			printf(" Sync Error\n");
			return ERROR;
		}
	}
}

/*
 * wcgetsec fetches a Ward Christensen type sector.
 * Returns sector number encountered or ERROR if valid sector not received,
 * or CAN CAN received
 * or WCEOT if eot sector
 * time is timeout for first char, set to 5 seconds thereafter
 ***************** NO ACK IS SENT IF SECTOR IS RECEIVED OK **************
 *    (Caller must do that when he is good and ready to get next sector)
 */

wcgetsec(rxbuf, time)

char *rxbuf;
int time;

{
/* BDSC
	register checksum, wcj, firstch;
*/
	int sectcurr,errors;

	for(Lastrx=errors=0; errors<RETRYMAX; errors++) {

		firstch=readline(time);
		if(firstch==SOH)
		{
			sectcurr=readline(1);
			if((sectcurr+readline(1))==255)
			{
				oldcrc=checksum=0;
				for(cp=rxbuf,wcj=128; --wcj>=0; )
				{
					if((firstch=readline(1)) < 0)
						goto bilge;
					oldcrc=updcrc(firstch, oldcrc);
					checksum += (*cp++ = firstch);
				}
				if((firstch=readline(1)) < 0)
					goto bilge;
				if(Crcflg)
				{
					oldcrc=updcrc(firstch, oldcrc);
					if((firstch=readline(1)) < 0)
						goto bilge;
					oldcrc=updcrc(firstch, oldcrc);
					if(oldcrc)
						pstat("CRC=0%o Error #%d",
						  oldcrc, errors);
					else
					{
						firstsec=FALSE;
						return sectcurr;
					}
				}
				else if(((checksum-firstch)&0377)==0)
				{
					firstsec=FALSE;
					return sectcurr;
				}
				else
					pstat("Checksum Error #%d", errors);
			}
			else
				pstat("Sector number garbled #%d", errors);
		}

		/* make sure eot really is eot and not just mixmash */

		else if(firstch==EOT && readline(1)==TIMEOUT)
			return WCEOT;
		else if(firstch==CAN)
		{
			if(Lastrx==CAN)
			{
				printf("Sender CANcelled\n");
				return ERROR;
			} else
			{
				Lastrx=CAN;
				continue;
			}
		}
		else if(firstch==TIMEOUT)
			pstat("Timeout #%d", errors);
		else if(firstch==ERROR)
		{
bilge:
			pstat("Modem error #%d SR=%0o", errors, Mstatus);
		}
		else
			pstat("Got 0%o sector header", firstch);
		Lastrx=0;
		while(readline(1)!=TIMEOUT)
			;
		time=40;
		sendline(Crcflg&&firstsec?WANTCRC:NAK);
	}

	/* try to stop the bubble machine. */

	sendline(CAN);sendline(CAN);sendline(CAN);
	return ERROR;
}

wctx()
{
	int sectnum, attempts;
	char txbuf[SECSIZ];

	firstsec=TRUE;
	lpstat("Awaiting initial NAK");
	while((firstch=readline(400))!=NAK && firstch != WANTCRC
	  && firstch!=TIMEOUT && firstch!=CAN)
		lprintf("%c", firstch);	/* let user see it if strange char */
	if(firstch==CAN)
		return ERROR;
	if(firstch==WANTCRC)
		Crcflg=TRUE;
	sectnum=1;
	while(filbuf(txbuf, SECSIZ)) {
		if(!Quiet
#ifdef STATLINE
		)
			lpstat("Sector %3d", sectnum);
#else
		&& !View)
			lprintf("%s", (sectnum&63)?"*":"*\n");
#endif
		if(wcputsec(txbuf, sectnum)==ERROR)
		{
			return ERROR;
		} else
		{
			if(View)	/* View can't be set in xyam */
				for(cp=txbuf,wcj=128;--wcj>=0;)
					putchar(*cp++);
			sectnum++;
		}
	}
	closetx(FALSE);
	attempts=0;
	do
	{
		sendline(EOT);
		purgeline();
		attempts++;
	}
		while((firstch=(readline(100)) != ACK) && attempts < RETRYMAX);
	if(attempts == RETRYMAX)
	{
		printf("No ACK on EOT\n");
		return ERROR;
	}
	else
		return OK;
}

wcputsec(txbuf, sectnum)
char *txbuf;
{
/* BDSC
	register checksum, wcj, firstch;
*/
	int attempts;

	firstch=0;	/* part of logic to detect CAN CAN */

	for(attempts=0; attempts <= RETRYMAX; attempts++)
	{
		Lastrx= firstch;
		sendline(SOH);
		sendline(sectnum);
		sendline(-sectnum-1);
		oldcrc=checksum=0;
		for(wcj=SECSIZ,cp=txbuf; --wcj>=0; )
		{
			sendline(*cp);
			oldcrc=updcrc(*cp, oldcrc);
			checksum += *cp++;
		}
		if(Crcflg)
		{
			oldcrc=updcrc(0,updcrc(0,oldcrc));
			sendline(oldcrc>>8);sendline(oldcrc);
		}
		else
			sendline(checksum);
		purgeline();

		firstch=readline(100);
		if(firstch==CAN && Lastrx==CAN)
		{
cancan:
			printf("\nReceiver CANcelled transmission ");
			return ERROR;
		}
		else if(firstch==ACK)
		{
			firstsec=FALSE;
			return OK;
		}
		else if(firstch==TIMEOUT)
			pstat("Timeout on sector ack attempt %d", attempts);
		else
		{
			if(firstsec && firstch==WANTCRC)
				Crcflg=TRUE;
			pstat("Got 0%o for sector ACK attempt %d",
			  firstch, attempts);
			for(;;)
			{
				Lastrx=firstch;
				/* let user see it if strange char */
				lprintf("%c", firstch);
				if((firstch=readline(1))==TIMEOUT)
					break;
				if(firstch==CAN && Lastrx==CAN)
					goto cancan;
			}
		}
	}
	printf("No ACK on sector; Abort\n");
	return ERROR;

}

/* end of YAM2.C */
