/* kmstatus.c RHS 4/1/89
 *
 * Uses	OS/2 keyboard and mouse	routines.
 */

#define	INCL_SUB
#define	INCL_DOSERRORS
#define	INCL_DOS
#include<stdio.h>
#include<string.h>
#include<assert.h>
#include<stdlib.h>
#include<process.h>
#include<malloc.h>
#include<conio.h>
#include<dos.h>
#include<os2.h>

#define	VIOHDL				0
#define	THREADSTACK			800					/* size	of stack each thread*/

#define	MOU_NBUTTON_MOVE	0x0001
#define	MOU_B1_MOVE			0x0002
#define	MOU_B1_NMOVE		0x0004
#define	MOU_B2_MOVE			0x0008
#define	MOU_B2_NMOVE		0x0010
#define	MOU_B3_MOVE			0x0020
#define	MOU_B3_NMOVE		0x0040

#define	MOU_B1				(MOU_B1_MOVE | MOU_B1_NMOVE)
#define	MOU_B2				(MOU_B2_MOVE | MOU_B2_NMOVE)
#define	MOU_B3				(MOU_B3_MOVE | MOU_B3_NMOVE)

#define	MOUNOMOVE  (MOU_B1_NMOVE | MOU_B2_NMOVE	| MOU_B3_NMOVE)
#define	MOUMOVED   (MOU_NBUTTON_MOVE | MOU_B1_MOVE | MOU_B2_MOVE | MOU_B3_MOVE)

#define	STATUS_ROW		0
#define	BUTTON_ROW		7
#define	PRESSED_COL		3
#define	RELEASED_COL	42
#define	KBD_ROW			20

#define	MouNoMove(event)				(MOUNOMOVE & event.fs)
#define	MouMoved(event)					(MOUMOVED &	event.fs)
#define	MouButtonPressed(event,button)	(event.fs &	button)
#define	MouB1Pressed(event)				(event & MOU_B1)
#define	MouB2Pressed(event)				(event & MOU_B2)
#define	MouB3Pressed(event)				(event & MOU_B3)
#define	MouButtons(event)				(event.fs &	(MOU_B1	| MOU_B2 | MOU_B3))

#define	LEFTSHIFT				0x0002
#define	LEFTCTRL				0x0100
#define	LEFTALT					0x0200
#define	CAPS					0x0040
#define	NUMLOCK					0x0020
#define	SCROLLLOCK				0x0010
#define	INSERT					0x0080
#define	SYSREQ					0x8000
#define	RIGHTALT				0x0800
#define	RIGHTCTRL				0x0400
#define	RIGHTSHIFT				0x0001
#define	ECHO					0x0001
#define	RAW						0x0004
#define	COOKED					0x0008

#define	KbdLeftShift(info)		(info.fsState &	LEFTSHIFT)
#define	KbdLeftCtrl(info)		(info.fsState &	LEFTCTRL)
#define	KbdLeftAlt(info)		(info.fsState &	LEFTALT)
#define	KbdCapsL(info)			(info.fsState &	CAPS)
#define	KbdNumL(info)			(info.fsState &	NUMLOCK)
#define	KbdScrLock(info)		(info.fsState &	SCROLLLOCK)
#define	KbdInsert(info)			(info.fsState &	INSERT)
#define	KbdSysReq(info)			(info.fsState &	SYSREQ)
#define	KbdRightAlt(info)		(info.fsState &	RIGHTALT)
#define	KbdRightCtrl(info)		(info.fsState &	RIGHTCTRL)
#define	KbdRightShift(info)		(info.fsState &	RIGHTSHIFT)

#define	KbdEcho(info)			(info.fsMask & ECHO)
#define	KbdMode(info)			(info.fsMask & RAW)

typedef	struct _button
	{
	int	startrow;
	int	startcol;
	int	endrow;
	int	endcol;
	char **text;
	} BUTTON;

#define	QUIT_LENGTH	5
char *quit_button_text[QUIT_LENGTH+1] =
	{
	"ีออออออออธ",
	"ณ        ณ",
	"ณ  QUIT  ณ",
	"ณ        ณ",
	"ิออออออออพ",
	"\0"
	};

BUTTON quit_button = {12,68,12+QUIT_LENGTH-1,77,quit_button_text};

#define	CA_LENGTH 5
char *collision_area_text[CA_LENGTH+1] =
	{
	"ีออออออออออออออออออออออออออออออออออธ",
	"ณ                                  ณ",
	"ณ You can't get here from there... ณ",
	"ณ                                  ณ",
	"ิออออออออออออออออออออออออออออออออออพ",
	"\0"
	};

BUTTON collision_area =	{12,3,12+CA_LENGTH-1,38,collision_area_text};

#define	HSCF_LENGTH	5
char *hscf_button_text[HSCF_LENGTH+1] =
	{
	"ีออออออออออธ",
	"ณHORIZONTALณ",
	"ณ  SCALING ณ",
	"ณ  FACTOR  ณ",
	"ิออออออออออพ",
	"\0"
	};

BUTTON hscf_button = {12,42,12+HSCF_LENGTH-1,53,hscf_button_text};

#define	VSCF_LENGTH	5
char *vscf_button_text[VSCF_LENGTH+1] =
	{
	"ีออออออออธ",
	"ณVERTICALณ",
	"ณ SCALINGณ",
	"ณ FACTOR ณ",
	"ิออออออออพ",
	"\0"
	};

BUTTON vscf_button = {12,55,12+VSCF_LENGTH-1,64,vscf_button_text};

#define	MODE_LENGTH	3
char *mode_button_text[MODE_LENGTH+1] =
	{
	"ีออออออออออธ",
	"ณINPUT MODEณ",
	"ิออออออออออพ",
	"\0"
	};

BUTTON mode_button = {19,60,19+MODE_LENGTH-1,71,mode_button_text};

long modeSem = 0L;
unsigned InputMode = 0;

unsigned char MouseThreadStack[THREADSTACK];
unsigned char KeyboardThreadStack[THREADSTACK];

void main(void);
void display_button(BUTTON *button,unsigned	char attr);
unsigned ButtonPressed(BUTTON *b,unsigned mask,MOUEVENTINFO	*event);
void MouseThread(void);
void KeyboardThread(void);


void main(void)
	{
												/* start mouse thread		*/
	if(_beginthread(MouseThread,MouseThreadStack,THREADSTACK,NULL) == -1)
		DosExit(EXIT_PROCESS,-1);

	if(_beginthread(KeyboardThread,KeyboardThreadStack,THREADSTACK,NULL) ==	-1)
		DosExit(EXIT_PROCESS,-1);

	while(TRUE)
		DosSleep(-1L);
	}

void display_button(BUTTON *b, unsigned	char attr)
	{
	int	i;

	for(i =	0; *b->text[i];	i++)
		VioWrtCharStrAtt(b->text[i],strlen(b->text[i]),b->startrow+i,
				b->startcol,&attr,VIOHDL);
	}

unsigned ButtonPressed(BUTTON *b,unsigned mask,MOUEVENTINFO	*event)
	{
	unsigned ev	= event->fs, row = event->row, col = event->col, retval	= 0;
	unsigned char attr = 0x4f;

	ev &= 0xff7e;								/* turn	off	the	low	bit		*/

	if(!(ev	& mask))							/* if button not pressed	*/
		return 0;
												/* if pointer on button		*/
	if((row	>= b->startrow)	&& (row	<= b->endrow) && (col >= b->startcol) &&
			(col <=	b->endcol))
		{
		display_button(b,attr);					/* redisplay highlighted	*/
		DosSleep(100L);							/* wait	a sec...			*/
		attr = 0x17;
		display_button(b,attr);					/* redisplay normal			*/
		return ev;
		}
	return 0;									/* pointer not on button	*/
	}


void KeyboardThread(void)
	{
	KBDINFO	kbdinfo;
	KBDKEYINFO kbdkeyinfo;
	HKBD KbdHandle = 0;

	char Outstr[85];
	unsigned inputmode = 0;

	strcpy(Outstr,"Ascii Scan");
	VioWrtCharStr(Outstr,strlen(Outstr),KBD_ROW-1,15,VIOHDL);
	KbdFlushBuffer(KbdHandle);					/* flush keyboard buffer	*/

	while(TRUE)
		{
		DosSemRequest(&modeSem,-1L);
		if(inputmode !=	InputMode)				/* if mode has changed		*/
			{
			if(InputMode)						/* if COOKED mode			*/
				{
				kbdinfo.fsMask &= ~RAW;			/* turn	off	RAW	bit			*/
				kbdinfo.fsMask |= COOKED;		/* turn	on COOKED bit		*/
				}
			else
				{
				kbdinfo.fsMask &= ~COOKED;		/* turn	off	COOKED bit		*/
				kbdinfo.fsMask |= RAW;			/* turn	on RAW bit			*/
				}
			KbdSetStatus(&kbdinfo,KbdHandle);
			inputmode =	InputMode;
			}
		DosSemClear(&modeSem);
		KbdGetStatus(&kbdinfo,KbdHandle);
		sprintf(Outstr,"States: Echo is %3s  Input Mode is %6s",
			KbdEcho(kbdinfo) ? "ON"	: "OFF",
			KbdMode(kbdinfo) ? "RAW" : "COOKED");
		VioWrtCharStr(Outstr,strlen(Outstr),KBD_ROW,0,VIOHDL);

		KbdCharIn(&kbdkeyinfo,IO_WAIT,KbdHandle);
		sprintf(Outstr,
				"%5s %4s %3s [%3d] [%3d] %4s %7s %11s %6s %6s %3s %4s %5s",
				KbdLeftShift(kbdkeyinfo) ? "SHIFT" : " ",
				KbdLeftCtrl(kbdkeyinfo)	? "CTRL" : " ",
				KbdLeftAlt(kbdkeyinfo) ? "ALT" : " ",
				kbdkeyinfo.chChar,
				kbdkeyinfo.chScan,
				KbdCapsL(kbdkeyinfo) ? "CAPS" :	" ",
				KbdNumL(kbdkeyinfo)	? "NUMLOCK"	: " ",
				KbdScrLock(kbdkeyinfo) ? "SCROLL-LOCK" : " ",
				KbdInsert(kbdkeyinfo) ?	"INSERT" : " ",
				KbdSysReq(kbdkeyinfo) ?	"SYSREQ" : " ",
				KbdRightAlt(kbdkeyinfo)	? "ALT"	: " ",
				KbdRightCtrl(kbdkeyinfo) ? "CTRL" :	" ",
				KbdRightShift(kbdkeyinfo) ?	"SHIFT"	: " ");

		VioWrtCharStr(Outstr,strlen(Outstr),KBD_ROW+2,0,VIOHDL);
		}
	}


void MouseThread(void)
	{
	MOUEVENTINFO MouEvent;
	MOUQUEINFO mouqueinfo;
	SCALEFACT scalefact;

	char Outstr[90];
	unsigned BlankCell = 0x1720;
	char linechar =	'อ';
	unsigned char attr = 0x17;

	HMOU MouHandle;
	unsigned Status,numbuttons,nummickeys;
	int	WaitOption = 1;							/* set to block	on input	*/
	char *p,*q;
	int	i;
	unsigned buttonmasks[3], button;
	char buttondown[3],blank = ' ';

	if((Status = MouOpen((PSZ)NULL,(PHMOU)&MouHandle)))
		{
		printf("%u error opening Mouse \n",Status);
		DosExit(EXIT_PROCESS,0);
		}

	DosSemClear(&modeSem);					   /* so keyboard thread can go	*/

	VioScrollUp(0,0,-1,-1,-1,(char *)&BlankCell,VIOHDL);
	VioWrtNChar(&linechar,80,18,0,VIOHDL);

	display_button(&collision_area,attr);		/* display collision area	*/
	display_button(&hscf_button,attr);			/* display hscf	button		*/
	display_button(&vscf_button,attr);			/* display vscf	button		*/
	display_button(&quit_button,attr);			/* display quit	button		*/
	display_button(&mode_button,attr);			/* display mode	button		*/

	buttonmasks[0] = MOU_B1;
	buttonmasks[1] = MOU_B2;
	buttonmasks[2] = MOU_B3;
	memset(buttondown,0,sizeof(buttondown));

	MouDrawPtr(MouHandle);						/* display mouse pointer	*/
												/* lock	out	collision area	*/
	MouRemovePtr((NOPTRRECT	*)&collision_area,MouHandle);

	MouGetNumMickeys(&nummickeys,MouHandle);	/* get mickey count			*/
	MouGetNumButtons(&numbuttons,MouHandle);	/* get button count			*/

	p =	"Mouse Button:";
	VioWrtCharStr(p,strlen(p),BUTTON_ROW-1,0,VIOHDL);
	q =	"1:";
	for(i =	0; i < numbuttons; i++)
		{
		*q = (char)(i+'0'+1);
		VioWrtCharStr(q,strlen(q),i+BUTTON_ROW,0,VIOHDL);
		}

	sprintf(Outstr,"Mickeys per Centimeter: %2u  Number of Buttons:%u",
			nummickeys,numbuttons);
												/* display statistics		*/
	VioWrtCharStr(Outstr,strlen(Outstr),0,0,VIOHDL);

	MouFlushQue(MouHandle);						/* flush mouse queue		*/

	while(TRUE)
		{
		MouGetDevStatus(&Status,MouHandle);		/* get status				*/
		MouGetScaleFact(&scalefact,MouHandle);	/* get scaling factor		*/
												/* read	the	queue			*/
		MouReadEventQue(&MouEvent,&WaitOption,MouHandle);
		MouGetNumQueEl(&mouqueinfo,MouHandle);	/* any events left in queue?*/

		sprintf(Outstr,
				"Time:%8ld  Event Mask:%04x  Status:%04x  Pending Events:%u ",
				MouEvent.Time, MouEvent.fs,	Status,	mouqueinfo.cEvents);
												/* display event information*/
		VioWrtCharStr(Outstr,strlen(Outstr),1,18,VIOHDL);
		sprintf(Outstr,
				"Horizontal Scaling Factor:%5u    Vertical Scaling Factor:%5u",
				scalefact.colScale,scalefact.rowScale);
												/* display scaling factor	*/
		VioWrtCharStr(Outstr,strlen(Outstr),2,0,VIOHDL);

		if(MouMoved(MouEvent))					/* if moved, display row/col*/
			{
			sprintf(Outstr,"Row:%2d Col:%2d",MouEvent.row,MouEvent.col);
			VioWrtCharStr(Outstr,strlen(Outstr),1,0,VIOHDL);
			}

		for(i =	0; i < numbuttons; i++)			/* check each button		*/
												/* if mouse	button is down	*/
			if(MouButtonPressed(MouEvent,buttonmasks[i]))
				{
				if(!buttondown[i])				/* and wasn't already down  */
					{
					sprintf(Outstr," Pressed @ Row:%2u Col:%2u Time:%8ld",
							MouEvent.row,MouEvent.col,MouEvent.Time);
					VioWrtNChar(&blank,80-PRESSED_COL,BUTTON_ROW+i,
							PRESSED_COL,VIOHDL);
					VioWrtCharStr(Outstr,strlen(Outstr),BUTTON_ROW+i,3,VIOHDL);
					buttondown[i] =	TRUE;
					}
				}
			else								/* if mouse	button is up	*/
				{
				if(buttondown[i])				/* and was previously down	*/
					{
					sprintf(Outstr,"Released @ Row:%2u Col:%2u Time:%8ld",
							MouEvent.row,MouEvent.col,MouEvent.Time);
					VioWrtCharStr(Outstr,strlen(Outstr),BUTTON_ROW+i,
							RELEASED_COL,VIOHDL);
					buttondown[i] =	FALSE;
					}
				}
												/* if Quit button pressed	*/
		if(ButtonPressed(&quit_button,0xffff,&MouEvent))
			break;
												/* if Horizontal SF	button	*/
		if(button =	ButtonPressed(&hscf_button,0xffff,&MouEvent))
			{
			if(MouB1Pressed(button))			/* if button1 (increment)	*/
				scalefact.colScale++;
			if(MouB2Pressed(button))			/* if button2 (decrement)	*/
				scalefact.colScale--;
			MouSetScaleFact(&scalefact,MouHandle);
			}
												/* if Vertical SF button	*/
		if(button =	ButtonPressed(&vscf_button,0xffff,&MouEvent))
			{
			if(MouB1Pressed(button))			/* if button1 (increment)	*/
				scalefact.rowScale++;
			if(MouB2Pressed(button))			/* if button2 (decrement)	*/
				scalefact.rowScale--;
			MouSetScaleFact(&scalefact,MouHandle);
			}
		if(ButtonPressed(&mode_button,0xffff,&MouEvent))
			{
			DosSemRequest(&modeSem,-1L);
			InputMode =	!InputMode;
			DosSemClear(&modeSem);
			}
		DosSleep(32L);
		}

	MouClose(MouHandle);

	BlankCell =	0x0720;
	VioScrollUp(0,0,-1,-1,-1,(char *)&BlankCell,VIOHDL);

	DosExit(EXIT_PROCESS,0);
	}

	/* end of kmstatus.c	*/

