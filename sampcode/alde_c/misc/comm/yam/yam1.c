/*
>>:yam1.c
 * Mainline for yam 18-Nov-82 (diff 10-25-81)
 * 'go', 'exit' and 'off' modifications by Bill Bolton, March 15, 1982
 * 'loop' and 'test' added for loopback testing of line, John Woolner,
 * August 28, 1982
 */
#include "yam.h"
#define MAXARGS 20

main(argc, argv)
char **argv;

{
	int c;
	char *args[MAXARGS], **argp, **argq, *nextcmd, *p;

	init1_extra();	/* special initialisation in YAM11.C */

	nextcmd=NULL;
	printf("Yet Another Modem by Chuck Forsberg\n");
	printf("%s %s\n", FLAVOR, VERSION);
	init();
	if(argc>MAXARGS)
		goto usage;
	/* copy given arguments to our own array */
	for(argp=argv,argq=args,c=argc; --c>=0;)
		*argq++ = *argp++;

#ifdef TERMRESET
	lprintf(TERMRESET);
#endif /* TERMRESRET */

	for(;;) {
		if(argc < 2) {
			if(nextcmd)
				cp=nextcmd;
			else {
#ifdef XMODEM
				printf("xYAM(%c%d)", defdisk+'A', user);
#else
				printf(">>>%c%d:", defdisk+'a', user);
#endif /* XMODEM*/
				gets(cmdbuf);
#ifdef TERMRESET
				lprintf(TERMRESET);
#endif /* TERMRESET */
				init2_extra();	/* in YAM11 */
				cp=cmdbuf;
/*
 * Check for *text... which sends the rest of the line to
 * the modem
 */
				if((*cp)=='*') {
					abptr= ++cp;
					abend= cp + strlen(cp);
/*  Add proper end of line stuff according to Txeoln */
					switch(Txeoln) {  
					case EOL_CR:
					case EOL_CRWAIT:
					case EOL_CRPROMPT:
						*abend++ = '\r';
						break;
					case EOL_NOTHING:
					case TX_BINARY:
						*abend++ = '\r';
					case EOL_NL:
						*abend++ = '\n';
						break;
					}
					Exoneof=TRUE;
					term();
					continue;
				}
			}
			if(nextcmd=index(';', cp))
				*nextcmd++ =0;	/*  ; separates commands */
			else if(nextcmd=index('\\', cp))
				*nextcmd++ =0;	/*  \ separates commands */

			argp= &args[1]; argc=1;
			for(;;) {
				if(isgraphic(*cp)) {
					*argp++ = cp;
					argc++;
					while(isgraphic(*cp))
						cp++;
				}
				while(*cp==' ' || *cp=='\t')
					*cp++ =0;
				if(*cp > ' ')
					continue;
				*cp=0;
				break;
			}
		}
		for(argp= &args[1]; --argc>0; ) {
			uncaps(*argp);
			cp= *argp++;

			Gototerm=Batch=Crcflg=Creamfile=Echo=Ctlview=
			  Chat=View=Type=Quiet=FALSE;
#ifdef CPM
			if(index(':', cp)) {
				chdir(cp);
				continue;
			}
#endif /* CPM */
			switch(*cp++) {
			case 'b':
				if(cmdeq(cp, "ye")) {
#ifdef BYEPROG
					cp=4; *cp=0;	/* reset user/driv */
					exec(BYEPROG);
#else
					bye();
					continue;
#endif /* BYEPROG */
				}
				else if(setbaud(atoi(cp)))
					goto usage;
				continue;
			case 'c':
#ifndef RESTRICTED
				if(cmdeq(cp, "all") && --argc ==1) {
					uncaps(*argp);	/* make name lcase */
					if(getphone(*argp++,Phone)==ERROR)
						goto usage;
					else if(dial(Phone)==ERROR)
						goto usage;
					continue;
				}
#endif /* RESTRICTED */
				if(cmdeq(cp, "hat")) {
					chat();
					continue;
				}
				if(cmdeq(cp, "lose")) {
					dumprxbuff();
					closerx(TRUE); closetx(TRUE);
					continue;
				}
				if(cmdeq(cp, "rck")) {
					docrck( --argc, argp);
					argc=0; continue;
				}
				goto usage;
			case 'd':
				if(cmdeq(cp, "ir")) {
#ifndef CDOS
					if(cp[2])
						docomp(--argc, argp);
					else
#endif /* CDOS */
						dodir(--argc, argp);
					argc=0;
				}
				else if(setval(0)==ERROR)
					goto usage;
				continue;
			case 'e':
#ifndef RESTRICTED
				if(cmdeq(cp, "ra")) {
					doerase( --argc, argp); argc=0;
					continue;
				}
#endif /* RESTRICTED */
				if(cmdeq(cp, "xit")) {
					dumprxbuff();
					closerx(TRUE); closetx(TRUE);
					printf("Exiting to DOS, files closed");
#ifndef XMODEM
					onhook();
					printf(", modem disconnected.");
#endif /* XMODEM */
					printf("\n");
					exit(0);
				}					
				if(setval(~0)==ERROR)
					goto usage;
				continue;
			case 'f':
				closetx(TRUE);
				if(setval(~0)==ERROR)
					goto usage;
				if(--argc<=0 || opentx(*argp++)==ERROR)
					goto usage;
				if(Squelch) {
					/* frame file with ^R T if Squelch */
					sendline(022);
				}
				term(0);
				if(Squelch) {
					sendline(024);
				}
				continue;
			case 'g':
				if(cmdeq(cp, "o"))
					exit();
				else
					goto usage; 
			case 'i':
				if(cmdeq(cp, "nit")) {
					init(); argc=nextcmd=0; continue;
				}
				else
					goto usage;
			case '?':
			case 'h':
#ifdef XMODEM
				Type++; /* send to remote user */
#endif /* XMODEM */
				listfile(HELPFILE);
				continue;
			case 'k':
				if(cmdeq(cp, "ill")) {
					clearbuf();
					continue;
				}
				else
					goto usage;
			case 'l':
				if(cmdeq(cp, "ist")) {
					View++;
listit:
					dolist( --argc, argp);
					argc=0;
					continue;
				}
#ifndef RESTRICTED
				if (cmdeq(cp,"oop")) {
					loop(atoi(cp += 3));
					continue;
				}
#endif /* RESTRICTED */
				else
					goto usage;
#ifndef RESTRICTED
			case 'm':
				Sport=(Dport=atoi(cp))+(SPORT-DPORT);
				readbaud(); continue;
#endif /* RESTRICTED */
			case 'o':
				if(cmdeq(cp, "ff")) {
#ifdef TERMRESET
					lprintf(TERMRESET);
#endif /*TERMRESET */
#ifdef BYEPROG
					cp=4; *cp=0;	/* reset user/driv */
					exec(BYEPROG);
#else
					onhook();
#endif /* BYEPROG */
					printf("Off Line\n"); Continue;
				}
				if(cmdeq(cp, "n")) {
					online();
					printf("On line\n");
					continue;
				}
				else
					goto usage;
			case 'p':
				if(setparm(cp)==ERROR)
					goto usage;
				continue;
			case 'q':
				if(cmdeq(cp, "uit")) {
					printf("Quitting YAM\n");
					exit();
				}
				goto usage;
			case 'r':
				if(cmdeq(cp, "eset")) {
#ifdef TERMINIT
					lprintf(TERMINIT);
#endif /* TERMINIT */
					closetx(TRUE);
					dumprxbuff();
					closerx(TRUE);
#ifdef CPM
					bdos(13,0);
					bdos(14,defdisk);
#endif /* CPM */
					/* change buffer size for debugging */
					if(--argc>0) {
						bufend= bufst + atoi(*argp++);
						clearbuf();
						dostat();
					}
					continue;
				}
				if(cmdeq(cp, "ep")) {
					replot(argc>1 ? (--argc,atoi(*argp++)) : 0);
					continue;
				}
				if(cmdeq(cp, "ew")) {
					rewindcb(); continue;
				}
				if(chkbvq()==ERROR)
					goto usage;
				if(argc<2)
					Batch=TRUE;
				psxfer(wcreceive(--argc, argp));
				if(Gototerm)
					term();
				argc=0; continue;
			case 's':
				if(*cp == 0 && argc==1) {
					dostat();
					continue;
				}
				if(argc<2 || chkbvq()==ERROR)
					goto usage;
				if( argc > 2
				  || index('?',*argp) || index('*',*argp))
					Batch=TRUE;
#ifdef XMODEM
				/* tell 'em how long it will take! */
				if(Batch)
					docomp(argc-1, argp);
#endif /* XMODEM */
				psxfer(wcsend(--argc, argp));
				if(Gototerm)
					term();
				argc=0;
				continue;
#ifndef XMODEM
			case 'u':
				if(argc>1)
					goto usage;	/* trap "user 0" */
				abptr=Wrapped?bufcq+1:bufst;
				abend=bufcq;
				/* fall through to T */
#endif /* XMODEM */
			case 't':
				if(cmdeq(cp, "ype")){
#ifdef XMODEM
					Type++;	/* send to remote user */
#endif /* XMODEM */
					goto listit;
				}
#ifndef RESTRICTED
				if (cmdeq(cp,"est")) {
					linetest(atoi(cp += 3));
					continue;
				}
#endif /* RESTRICTED */
#ifndef XMODEM 
				if(--argc > 0) {
					dumprxbuff(); closerx(TRUE);
					if(opencapt(*argp++)==ERROR)
						goto usage;
				}
				switch(setval(~0)) {
				case ERROR:
					goto usage;
				case OK:
					term();
				case TRUE:
					/* remain in command mode if t..c  */
					break;
				}
				continue;
			/* note: case u precedes T */
			case 'w':
				dumprxbuff(); continue;
#endif /* XMODEM */
			case 'x':
				continue;	/* in case you said "XMODEM" */
			case 0:
			default:
				goto usage;
			}
			continue;

		}
		continue;
usage:
		printf("Type HELP for instructions\n");
		nextcmd=argc=0; continue;

	}
}

chkbvq()
{
	while(*cp)
		switch(*cp++) {
		case 'b':
			Batch=TRUE; break;
		case 'c':
			Crcflg=TRUE; break;
		case 'q':
			Quiet=TRUE; break;
#ifndef RESTRICTED
		case 't':
			Gototerm=TRUE; break;
		case 'v':
			View=TRUE; break;
		case 'y':
			Creamfile=TRUE; break;
#endif /* RESTRICTED */
		default:
			return ERROR;
		}
	return FALSE;
}
setval(value)
unsigned value;
{
	FLAG dumped;
	dumped=FALSE;
	while(*cp)
		switch(*cp++) {
		case 'a':
			Chat = value;
			break;
		case 'b':
			Txeoln= value&TX_BINARY;
			Txmoname="BINARY";
			break;
		case 'c':
			dumprxbuff();
			closerx(dumped=TRUE);
			break;
		case 'd':
			Dumping = !Squelch || value;
			break;
		case 'e':
			Echo=value;
			break;
		case 'f':
			Hdx= !value;
			break;
		case 'g':
			Txgo= value;
			break;
		case 'h':
			Hdx=value;
			break;
		case 'i':
			Image=value;
			break;
		case 'l':
			Pflag=value;
			break;
		case 'n':
			Txeoln =value&EOL_NL;
			Txmoname="NL ONLY";
			break;
		case 'o':
			Pflag= !value;
			break;
		case 'p':
			Txeoln =value&EOL_CRPROMPT;
			Txmoname="WAIT FOR PROMPT";
			break;
		case 'r':
			Txeoln =value&EOL_CR;
			Txmoname="CR ONLY";
			break;
		case 's':
			Squelch=value;
			break;
		case 't':
			Waitbunch=value;
			break;
		case 'v':
			Ctlview++;
			break;
		case 'w':
			Txeoln= value&EOL_CRWAIT;
			Txmoname="CR WAIT FOR NL";
			break;
		case 'x':
			Exoneof=value;
			break;
		case 'z':
			Zeof=value;
			break;
		default:
			return ERROR;
		}
	return dumped;
}

init()
{
#ifdef BDSC
	char *endext(), *topofmem(), *codend(), *externs();
	if(codend() > externs()) {	/* check for bad -e value! */
		printf("urk"); exit();
	}
#endif /* BDSC */
#ifdef USERINIT
	userinit();	/* allows extra user externs' to be initialized */
#endif /* USERINIT */
	initdd();		/* fetch default disk and user number */
	bufst=endext();
	bufend=topofmem()-1024;	/* fudge so we don't crash ... */
	Dport=DPORT; Sport=SPORT;
#ifndef DEFBAUD
	readbaud();
#else
	Baudrate=DEFBAUD;
#endif /* DEFBAUD */
#ifdef INITBAUD
	setbaud(Baudrate);
#endif /* INITBAUD */
	Mstatus=0;	
	Rfile=Tfile=Pflag=FALSE;
	Image=Waitbunch=Exoneof=Hdx=Zeof=Squelch=FALSE;
	Txgo=TRUE;
	Parity= NORMAL;
	Originate= TRUE;
	Txeoln= EOL_NOTHING;
	Low= 400;
	Tpause=1500*CLKMHZ;
	Throttle=80*CLKMHZ;
	T1pause=311*CLKMHZ;
	Waitnum=1;
	GOchar= '\n';
	clearbuf();
}

clearbuf()
{
	abptr=NULL;
	Xoffflg=Wrapped= FALSE;
	buffcdq=bufcq=bufcdq=bufpcdq=bufmark= bufst;
	Bufsize=Free= bufend-bufst-1;
}


cmdeq(s,p)
char *s, *p;
{
	while(*p)
		if(*s++ != *p++)
			return 0;
	return 1;
}

dostat()
{

	printf("Capture %s Receiving %s ",
	  Dumping?"ON":"SQUELCHED", Rfile?Rname:"<nil>");
	if(Image)
		printf("Transparency mode ");
	if(Squelch)
		printf("^R ^T Squelch ");
	if(Zeof)
		printf("EOF on ^Z");
	printf("\n");

	if(Txeoln==EOL_NOTHING)
		Txmoname="IMAGE";

	printf("%sSending %s in %s mode\n",
	  Txgo? "" : "Pausing in ", Tfile?Tname:"<nil>", Txmoname);
	printf("%sWaiting %d loops every %u chars  Pause=%u GOchar=0%o\n",
	  Waitbunch?"":"NOT ",Throttle, Waitnum, Tpause, GOchar);

	printf("Printer %s  ", Pflag?"ON":"OFF");
	if(Hdx)
		printf("Half Duplex ");
	printf("At %u baud data port %u\n", Baudrate, Dport);
	printf("%u of %u chars used %u free%s\n",
	  Bufsize-Free, Bufsize, Free, Wrapped?" POINTERS WRAPPED":"");
	printf("bufst=%x bufcq=%x bufcdq=%x\nbuffcdq=%x bufpcdq=%x bufend=%x\n",
	  bufst, bufcq, bufcdq, buffcdq, bufpcdq, bufend);
#ifndef CDOS
	printdfr();
#endif /* CDOS */
}

isgraphic(c)
{
	if(c>' ' && c<0177)
		return TRUE;
	else
		return FALSE;
}

/*
 * index returns a pointer to the first occurrence of c in s,
 * NULL otherwise.
 */
char *index(c, s)
char c,*s;
{
	for(; *s; s++)
		if(c== *s)
			return s;
	return NULL;
}
psxfer(status)
{
	lpstat("\007File transfer(s) %s\007",
	 status==ERROR?"ABORTED BY ERROR" : "successful");
#ifndef CDOS
	printdfr();
#endif /* CDOS */
}

setparm(p)
char *p;
{
	if(*p)
		switch(*p++) {
		case 'g':
			if((GOchar= *p) == '^')
				GOchar= (*++p & 037);
			break;
		case 'l':
			Low=atoi(p);
			break;
		case 'p':
			Tpause=atoi(p);
			break;
		case 't':
			Throttle=atoi(p);
			break;
		case 'w':
			Waitnum=atoi(p);
			break;
		default:
			return ERROR;
 	}
	return 0;
}
/* make strings lower case */
uncaps(s)
char *s;
{
	while(*s)
		*s = tolower(*s++);
}

/* end of YAM1.C */
