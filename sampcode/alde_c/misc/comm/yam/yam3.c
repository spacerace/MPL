/*
>>:yam3.c 28-Sep-82 (dif 10-17-81)
 *
 * two-way conversation with remote -whatever.
 * Printer is buffered such that it needn't be as fast as the baudrate
 * as long as it eventually gets a chance to catch up.
 * This buffering also allows one to write the received data out to disk
 * after the conservation has started.
 */
#include "yam.h"

term()
{
#ifndef BDSC
	register char cfast;
#endif
	register c, cc;
	unsigned register charsent;
	sayterm();
	charsent=0;
	Waitecho=Txwait=FALSE;
	for(;;) {
		MODEMON
		if(MIREADY) {
			*bufcq++ = MICHAR;
			Timeout=0;
			if(bufcq >= bufend) {
				Wrapped=TRUE;
				bufcq=bufst;
			}
			if(--Free == Low)
				sendline(Xoffflg=XOFF);
			else if(Free==0)
				goto belch;
			continue;
		}
		CONSOLON
		if(COREADY && bufcdq != bufcq) {
			switch(cfast= (*bufcdq & 0177) ) {
			case '\b':
				 if(Ttycol)
					--Ttycol;
				break;
			case '\t':
				sendcon(' ');
				if(++Ttycol & 07)
					continue;
				else
					goto chuckit;
			case '\r':
				Ttycol=0;
			case '\n':
				break;
			case XOFF:
#ifdef STATLINE
				sayterm();
#endif
				Txgo=FALSE; break;
			case XON:
				Txgo=TRUE;
#ifdef STATLINE
				sayterm();
#endif
				break;
#ifdef XMODEM
			case CPMEOF:
/*				pstat(""); */
				return;
			case 007:
				break;
#endif
			default:
				if(Ctlview)
				{
					if(cfast & 0200) {
						cfast &= 0177;
						if(Ctlview>1) {
							putchar('~'); ++Ttycol;
						}
					}
					if(cfast<040) {
						putchar('^');
						putchar(cfast|0100);
						Ttycol += 2;
						goto chuckit;
					}
					else
						++Ttycol; break;
				}
			}
			if(cfast==GOchar && Txeoln==EOL_CRWAIT)
				Waitecho=FALSE;

#ifdef RXNONO
			if( !index(cfast, RXNONO))
#endif /* RXNONO */
				sendcon(cfast);
			if(Echo) {
				sendline(cfast);
				if(Chat && cfast== '\r') {
					sendcon('\n');
#ifndef XMODEM
					sendline('\n');
#endif
				}
			}
chuckit:
			if(++bufcdq >= bufend)
				bufcdq=bufst;
			continue;
		}
		if(Pflag && bufpcdq!=bufcq && POREADY) {
#ifdef CPM
			bios(5, (*bufpcdq++ & 0177));
#else
			foobar...
#endif
			if(bufpcdq >= bufend)
				bufpcdq=bufst;
		}
		CONSOLON
		if(CIREADY) {
			switch(cfast= CICHAR & KBMASK){
			 case ENQ:
				if(moment(KTIME2))   /* ^E ^E quick sends one */
					break;
/*				pstat(""); */
				putchar('\n');
				return;
			case XON:
				if(Tfile && !Txgo) {
					Txgo=TRUE;
					sayterm();
				}
				break;
			case XOFF:
				if(Tfile && Txgo) {
					Txgo=FALSE;
					sayterm();
				}
				break;
			case 'v'&037:
				if(moment(KTIME2))
					break;
				replot(TLENGTH<<1);
				sayterm();
				continue;
			case '\r':
				if(Hdx)
					printf("\n");
				if(Chat)
					sendline('\n');
				break;
			default:
				break;
			}
			sendline(cfast);
			if(Hdx) {
				sendcon(cfast);
			}
			continue;
		}
		MODEMON
		if(MOREADY) {
			if(Tfile && !Txwait && !Waitecho && Txgo) {
				c= getc(fin);
#ifdef CPM
				if(c==EOF || (c==CPMEOF && Txeoln != TX_BINARY))
#else
				if(c==EOF)
#endif
				{
					closetx(FALSE);
#ifdef XMODEM
/*					pstat(""); */
					return;
#else
					if(Exoneof)
						return;
					sayterm();
#endif
				}
offwithit:
				if(Waitbunch && ++charsent==Waitnum) {
					charsent=0;
					if(Waitnum>1) {
						Waitecho=TRUE;
						Timeout=0;
					} else {
						Txwait=TRUE;
						Txtimeout=Throttle;
					}
				}

				if(c=='\r') {	/* end of line processing */
					switch(Txeoln) {
					case EOL_NL:
						continue;
					case EOL_CRPROMPT:
					case EOL_CRWAIT:
						Waitecho=TRUE;
						Timeout=0;
					case EOL_CR:
						if(Tfile) {
							if((cc=getc(fin))!='\n')
								ungetc(cc, fin);
						}
						else if(*++abptr != '\n')
							--abptr;
						break;
					}
				}
				sendline(c);
				continue;
			}
			if(abptr && !Txwait && !Waitecho && Txgo) {
				if(abptr>=abend) {
					abptr=NULL;
					if(Exoneof)
						return;
				}
				else {
					c= *abptr++;
#ifdef LOOPBACKNONO
					if(index(c, LOOPBACKNONO))
						continue;
#endif
					goto offwithit;
				}
			}
		}
		if(++Timeout == Tpause) {
			Waitecho=FALSE;
belch:
			if(Xoffflg) {
				dumprxbuff();
				MODEMON
				if(MIREADY) {
					lpstat("\007OVERRUN: DATA LOST");
					if(Low<1000)
						return;
				}
				if(bufcdq != bufcq)
					continue;
				if(Pflag && bufpcdq != bufcq)
					continue;
				Free=Bufsize-1;
				Xoffflg=FALSE;
				sendline(XON);
			}
#ifdef CDO
			MODEMON
			if(CDO) {
				pstat("Carrier Lost");
				return;
			}
#endif
		}
		if(--Txtimeout==0)
			Txwait=FALSE;
	}
}

/* display the appropriate status information */
sayterm()
{
#ifdef TERMRESET
	lprintf(TERMRESET);
#endif
	if(Tfile)
		lpstat("Sending '%s' %s", Tname, Txgo?"":"Stopped by XOFF");
	if(Rfile)
		pstat("Term Receiving '%s'", Rname);
	else
		pstat("Term Function");
}

/* open a capture file and set the removal pointer to get max goods */
opencapt(name)
char *name;
{
	dumprxbuff();
	closerx(TRUE);
	if(openrx(name)==ERROR)
		return ERROR;
	if(buffcdq<bufst)
		buffcdq=bufst;
	if(Wrapped)
		buffcdq=bufcq+1;
	if(buffcdq >= bufend)
		buffcdq=bufst;
/*	dumprxbuff(); */
}

dumprxbuff()
{
	register c;

	if(!Rfile || buffcdq==NULL)
		return OK;
	while(buffcdq != bufcq) {
		c= *buffcdq++;
		if(buffcdq >= bufend)
			buffcdq=bufst;
		if(!Image) {
			switch(c &= 0177) {
			 case 0:
				 continue;
			case 032:		/* ^Z or CPMEOF */
				if(Zeof) {
					closerx(TRUE);
					return;
				}
				else
#ifdef CPM
					continue;
#else
					break;
#endif
			case 022:
				if(Squelch) {
					Dumping=TRUE;
					continue;
				}
				break;
			case 024:
				if(Squelch) {
					Dumping=FALSE;
					continue;
				}
				break;
			default:
				break;
			}
		}
		if(Dumping || Image)
			if(putc(c, fout)==ERROR) {
				printf("\nDisk Full\n");
				closerx(FALSE);
				return ERROR;
			}
	}
	Free=Bufsize-1;
	return OK;
}

rewindcb()
{
	bufcdq=buffcdq=bufpcdq=Wrapped?bufcq+1:bufst;
}

/*
 * replot redisplays the buffer contents thru putcty allowing XOFF
 * number will represent how many lines to go back first 
 */
replot(number)
{
	char *s, c;
	int count, shorts;

fromtop:
	shorts=0;
	bufmark=Wrapped?bufcq+1:bufst;

	if(number) {
		s=bufcq;
backsome:
#ifdef TERMINIT
		/* backing up is confusing unless screen is cleard first */
		lprintf(TERMINIT);
#endif
		for(;;) {
			--s;
			if(s<bufst) {
				if(Wrapped)
					s= bufend-1;
				else {
					s=bufst;
					break;
				}
			}
			if(s==bufcq)
				break;
			if((*s&0177)=='\n' && --number<=0)
				break;
		}
		bufmark=s;
	}
	s=bufmark;
nextscreen:
#ifdef T4014
	/* Do the big flash on the 4014 or 4012 */
	printf("\033\014");
	sleep(CLKMHZ * 5);
#endif
	count=TLENGTH - shorts;
	shorts =0;
nextline:
	while(s != bufcq) {
		cfast= *s++ & 0177;
		if(s >= bufend)
			s=bufst;
		if(c=putcty(cfast))
			goto choose;
		if(cfast=='\r' && --count<=0)
			break;
	}

#ifdef STATLINE
	pstat("Replot cmd?");
#endif
	c=getcty();
#ifdef STATLINE
/*	pstat(""); */
#endif
choose:
/* Treat Control chars and letters the same */
	switch((c|0140)&0177) {
	case 'b':
		number=0;
		goto fromtop;
	case 'v':	/* control-v or v */
	case 'h':	/* backspace */
		number=TLENGTH-2;
		s=bufmark;
		goto backsome;
	case 0140:		/* space bar */
		shorts=2;
		bufmark=s;
		goto nextscreen;
	case 'p':
		number=1;
		s=bufmark;
		goto backsome;
	case 'n':
	case 'j':	/* linefeed */
		bufmark=s;
		count=1;
		goto nextline;
	default:
		return;
	}
}

chat()
{
#ifdef XMODEM
	printf("Ring My Chimes, Maybe I'll Come\n");
	printf("Exit chat with ^Z\n");
#endif
	Chat=Ctlview=Hdx=Echo=TRUE;
	term();
}

/* end of YAM3.C */
