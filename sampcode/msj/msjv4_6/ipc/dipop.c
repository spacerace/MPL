/* dipop.c RHS 7/14/89
 *
 * directory utility that allows use of keyboard and mouse to display
 * directory information. To run it:

	DIPOP

 *
 */

#define	INCL_SUB
#define	INCL_DOSERRORS
#define	INCL_DOS
#include<os2.h>
#include<mt\stdio.h>
#include<mt\string.h>
#include<mt\stdlib.h>
#include<mt\process.h>
#include<mt\dos.h>
#include"moudefs.h"
#include"kbddefs.h"
#include"msgq.h"
#include"button.h"
#include"errexit.h"
#include"di.h"

#define	MAINMSGQUEUE	"\\QUEUES\\DIPOPMSG.QUE"
#define	PRINTQUEUE		"\\QUEUES\\DIPOPPRT.QUE"

#define	VIOHDL				0
#define	PRINTTHREADSTACK	1000				/* mou/key thread stack size*/
#define	THREADSTACK			500					/* print thread stack size	*/

#define	BORDERCOL	60
#define	DIRROW		17
#define	STARTROW	2
#define	ENDROW		16

#define	BUTTON_ON				0x4f
#define	BUTTON_OFF				0x1f

#define	SCAN						0x0100
#define	EVENT						0x0200
#define	MOUSE						0x1000

#define	ASCIICODE(val)			(val)
#define	SCANCODE(val) 			(val | SCAN)
#define	EVENTCODE(val)			(val | EVENT)

#define	MOUSECODE(val)			(val | MOUSE)
#define	ButtonEvent(event)	(event & MOUSE)
#define	MOUSECODEOFF(val)		(val &= ~MOUSE)

#define	REMAINING_CHARS	(sizeof(filespecbuffer)-(currchar-filespecbuffer))

#define	PAUSE_EVENT				EVENTCODE(1)
#define	DIRECTORY_EVENT			EVENTCODE(2)
#define	READONLY_EVENT			EVENTCODE(3)
#define	HIDDEN_EVENT			EVENTCODE(4)
#define	SYSTEM_EVENT			EVENTCODE(5)
#define	ARCHIVE_EVENT			EVENTCODE(6)
#define	DR_DEC_EVENT			EVENTCODE(7)
#define	DR_INC_EVENT			EVENTCODE(8)
#define	DIR_DEC_EVENT			EVENTCODE(9)
#define	DIR_INC_EVENT			EVENTCODE(10)
#define	ROOT_EVENT				EVENTCODE(11)
#define	CD_PARENT_EVENT			EVENTCODE(12)
#define	CD_CHILD_EVENT			EVENTCODE(13)
#define	CLR_EVENT				EVENTCODE(14)
#define	ESCAPE_EVENT			EVENTCODE(15)
#define	POPUP_EVENT				EVENTCODE(16)
#define	TERMINATE_EVENT			EVENTCODE(17)

#define	LEFT_ARROW_EVENT		SCANCODE(75)
#define	RIGHT_ARROW_EVENT		SCANCODE(77)
#define	DEL_EVENT				SCANCODE(83)
#define	HOME_EVENT				SCANCODE(71)
#define	END_EVENT				SCANCODE(79)

#define	ALT_R					SCANCODE(19)
#define	ALT_D					SCANCODE(32)
#define	ALT_H					SCANCODE(35)
#define	ALT_S					SCANCODE(31)
#define	ALT_A					SCANCODE(30)
#define	ALT_C					SCANCODE(46)
#define ALT_P					SCANCODE(25)

#define	ENTER					13
#define	ESC					27
#define	BACKSPACE_EVENT	8


#define FILE_UNUSED0			0x0040
#define FILE_UNUSED1			0x0080
#define FILE_VOLLABEL			0x0008


#define	beep()					DosBeep(440,200)
#define	Lastchar(str)			(str[strlen(str)-1])
#define DIROFFSET				3


	/********************* Buttons ****************************/

		
BUTTON buttonlist[18] =
	{
/* text			row,col,row,col,attr,		type	left val, 		right val, accelerator	*/
{" Pause ",		22,	27,	0,	0,	BUTTON_OFF,	BTOGGLE,PAUSE_EVENT,	PAUSE_EVENT,   	ALT_P, 	0},
{" Enter ",		22,	17,	0,	0,	BUTTON_OFF,	BPRESS,	ENTER, 			ENTER,			ENTER,	0},
{" Esc ",		22,	37,	0,	0,	BUTTON_OFF,	BPRESS,	ESCAPE_EVENT,	ESCAPE_EVENT,  	ESC,	0},
{" Directory ",0,		64,	0,	0,	BUTTON_OFF,	BTOGGLE,	DIRECTORY_EVENT,DIRECTORY_EVENT,	ALT_D,	0},
{" Read-Only ",4,		64,	0,	0,	BUTTON_OFF,	BTOGGLE,	READONLY_EVENT,	READONLY_EVENT,	ALT_R,	0},
{"  Hidden   ",8,		64,	0,	0,	BUTTON_OFF,	BTOGGLE,	HIDDEN_EVENT,	HIDDEN_EVENT,  	ALT_H,	0},
{"  System   ",12,	64,	0,	0,	BUTTON_OFF,	BTOGGLE,	SYSTEM_EVENT,	SYSTEM_EVENT,  	ALT_S,	0},
{"  Archive  ",16,	64,	0,	0,	BUTTON_OFF,	BTOGGLE,	ARCHIVE_EVENT,	ARCHIVE_EVENT, 	ALT_A,	0},
{"-C-",			19,	1,		0,	0,	BUTTON_OFF,	BPRESS,	DR_DEC_EVENT,	DR_INC_EVENT,  	0,		0},
{"Directory                                            ",
					19,	7,		0,	0,	BUTTON_OFF,	BPRESS,	DIR_DEC_EVENT,	DIR_INC_EVENT, 	0,		0},
{" [\\] ",		22,	0,		0,	0,	BUTTON_OFF,	BPRESS,	ROOT_EVENT,		ROOT_EVENT,	   	0,		0},
{" [..] ",		22,	8,		0,	0,	BUTTON_OFF,	BPRESS,	CD_PARENT_EVENT,CD_CHILD_EVENT,	0,		0},
{" Clear ",		22,	45,	0,	0,	BUTTON_OFF,	BPRESS,	CLR_EVENT,		CLR_EVENT,     	ALT_C,	0},
{" * ",			22,	55,	0,	0,	BUTTON_OFF,	BPRESS,	'*',           	'*',           	'*',	0},
{" \\ ",			22,	61,	0,	0,	BUTTON_OFF,	BPRESS,  '\\',          '\\',          	'\\',	0},
{" . ",			22,	67,	0,	0,	BUTTON_OFF,	BPRESS,	'.',           	'.',           	'.',	0},
{" ? ",			22,	73,	0,	0,	BUTTON_OFF,	BPRESS,	'?',           	'?',           	'?',	0},
{NULL,			0,		0,		0,	0,	0,			0,		0,				0,             	0,		0}
	};

BUTTON	*drive_button;
BUTTON	*dir_button;

BUTTON filespecfield =
	{
	"            ",		19,	63,	21,	76,	BUTTON_OFF,	INPUT,	0,				0,				0,		0
	};


long PauseSem = 0L;
unsigned InputMode = 0;
char *title = "   Filename ----- Size ---- UpDate --- Time ---- Attributes ";
char *format= "%-12s   %8ld   %2d/%02d/%02d   %2d:%02d:%02d%c   %8s";

BYTE MouseThreadStack[THREADSTACK];
BYTE KeyboardThreadStack[THREADSTACK];
BYTE PrintThreadStack[PRINTTHREADSTACK];

HMOU MouHandle;

#define	FILESPECSIZE	12
char filespecbuffer[FILESPECSIZE+1];
USHORT	attribute_mask;

ULONG			drive_map;
char			dirspecbuffer[80];
PVOID	dirheader = NULL;
USHORT			dirbuflen = 0;

#define DIRPTRS		200
PCH 			dirptrs[DIRPTRS], *dirptr;
USHORT			dirs;
unsigned BlankCell = 0x1f20;

void main(void);
void MouseThread(void);
void KeyboardThread(void);
void PrintThread(void);

void InitDriveButton(void);
void InitScreen(void);
void RefreshDirButton(void);
void InitDirButton(void);
void SendRequest(void);
void PrintRequest(PVOID header);
void PrintRequestEntry(PFILEFINDBUF f);
void WriteRequestEntry(char *buffer);
void WriteDirButton(char *result);

void display_button(BUTTON *button,unsigned	char attr);
USHORT AcceleratorPressed(unsigned char key);
USHORT ButtonPressed(MOUEVENTINFO *ev);
void ButtonPress(USHORT *eventcode);

void buffermgr(unsigned event);
void initfield(void);
void set_attribute(USHORT attribute);
void drive_event(USHORT event);
void dir_event(USHORT event);
void set_attribute_values(char *str, unsigned attribute);
void child_changedir(void);


void main(void)
	{
	USHORT event, pause = 0;
	HQUEUE 	qhandle;

#define DEBUG
#ifdef DEBUG
	USHORT	retval;

	if(retval = DosSelectDisk(5))
		error_exit(retval,"Changing drive");
	if(retval = DosChDir("\\wrk\\hello",0L))
		error_exit(retval,"Changing dir");
#endif

	MsgQCreate(&qhandle,MAINMSGQUEUE);
	DosSemClear(&PauseSem);
												/* start mouse thread		*/
	if(_beginthread(PrintThread,PrintThreadStack,sizeof(PrintThreadStack),NULL) == -1)
		error_exit(0,"_beginthread(Keyboard)");
	if(_beginthread(MouseThread,MouseThreadStack,sizeof(MouseThreadStack),NULL) == -1)
		error_exit(0,"_beginthread(Mouse)");
	if(_beginthread(KeyboardThread,KeyboardThreadStack,sizeof(KeyboardThreadStack),NULL) == -1)
		error_exit(0,"_beginthread(Keyboard)");

	DosSleep(1000L);							/* let threads get going	*/

	InitButtons();
	InitScreen();
	DisplayButtons();
	InitDriveButton();
	InitDirButton();
	initfield();

	while(TRUE)
		{
		MsgQGet(qhandle,&event);

		if(ButtonEvent(event))
			ButtonPress(&event);

		switch(event)
			{
			case PAUSE_EVENT:
				/* if text thread is active, pause text, if already paused, unpause */
				if(pause)						/* if pause is on, clear sem*/
					DosSemClear(&PauseSem);
				else
					DosSemSet(&PauseSem);
				pause = !pause;					/* toggle pause flag		*/
				break;

			case ENTER:
				/* send request to directory server	*/
				SendRequest();
				break;

			case ESCAPE_EVENT:
			case ESC:
				DosCloseQueue(qhandle);
				if(dirheader)
					DiDestroyRequest(&dirheader);
				VioScrollUp(0,0,-1,-1,-1,(char *)&BlankCell,VIOHDL);
				DosExit(EXIT_PROCESS,0);		/* and exit						*/
				break;

			case READONLY_EVENT:
				/* set the request mask to only return files with read-only attr*/
				set_attribute(FILE_READONLY);
				break;
			case HIDDEN_EVENT:
				/* set the request mask to only return files with hidden attr	*/
				set_attribute(FILE_HIDDEN);
				break;
			case SYSTEM_EVENT:
				/* set the request mask to only return files with system attr	*/
				set_attribute(FILE_SYSTEM);
				break;
			case ARCHIVE_EVENT:
				/* set the request mask to only return files with archive attr	*/
				set_attribute(FILE_ARCHIVED);
				break;

			case DIRECTORY_EVENT:
				set_attribute(FILE_DIRECTORY);
				break;

			case DR_DEC_EVENT:
			case DR_INC_EVENT:
				/* decrement/increment the current drive, get current dirs, redisplay		*/
				drive_event(event);
				break;

			case DIR_DEC_EVENT:
			case DIR_INC_EVENT:
				/* move up/down in directory list	*/
				dir_event(event);
				break;

			case ROOT_EVENT:
				/* change to root of current drive	*/
				DosChDir("\\",0L);
				RefreshDirButton();
				break;

			case CD_PARENT_EVENT:
				/* change to parent directory of current directory */
				DosChDir("..",0L);
				RefreshDirButton();
				break;
			case CD_CHILD_EVENT:
				/* change to child directory of current directory */
				child_changedir();
				RefreshDirButton();
				break;

			default:
				buffermgr(event);
				break;
			}
		}
	}

void InitScreen(void)
	{
	USHORT i;

	VioScrollUp(0,0,-1,-1,-1,(char *)&BlankCell,VIOHDL);
	VioWrtNChar("Ä",BORDERCOL,DIRROW,0,VIOHDL);
	VioWrtCharStr(title,strlen(title),0,0,VIOHDL);

	for( i = 0; i < DIRROW; i++)
		VioWrtCharStr("³",1,i,BORDERCOL,VIOHDL);

	VioWrtCharStr("Ù",1,DIRROW,BORDERCOL,VIOHDL);
	}

void child_changedir(void)
	{
	char dirspec[70];

	if(!(*dirptr))
		{
		beep();
		return;
		}

	strcpy(dirspec,dirspecbuffer);
	if(Lastchar(dirspec) != '\\')
		strcat(dirspec,"\\");
	strcat(dirspec,*dirptr);
	DosChDir(dirspec,0L);
	}

void SendRequest(void)
	{
	char requestbuffer[80];
	PVOID header = NULL;
	static HQUEUE qhandle = 0;

	if(!qhandle)
		MsgQOpen(&qhandle,PRINTQUEUE);

	if(!strlen(filespecbuffer))
		{
		beep();
		return;
		}

	strcpy(requestbuffer,dirspecbuffer);
	if(Lastchar(requestbuffer) != '\\')
		strcat(requestbuffer,"\\");
	strcat(requestbuffer,filespecbuffer);

	DiMakeRequest(&header,requestbuffer,attribute_mask);
	DiSendRequest(header);

	MsgQSend(qhandle,SELECTOROF(header));
	DosSleep(1000L);
	}


void set_attribute(USHORT attribute)
	{
	if(!(attribute_mask & attribute))
		attribute_mask |= attribute;
	else
		attribute_mask &= ~attribute;
	}


void buffermgr(unsigned event)
	{
	static char *currchar = filespecbuffer;
	static USHORT curpos = 0;

	switch(event)
		{
		case CLR_EVENT:
				/* clear the filespec input buffer	*/
			memset(filespecbuffer,0,sizeof(filespecbuffer));
			attribute_mask = 0;
			curpos = 0;
			currchar = filespecbuffer;
			ResetButtons();
			break;

		case BACKSPACE_EVENT:
			if(currchar > filespecbuffer)
				{
				currchar--;
				curpos--;
				memmove(currchar,currchar+1,REMAINING_CHARS);
				}
			break;

		case DEL_EVENT:
			if(*currchar)
				memmove(currchar,currchar+1,REMAINING_CHARS);
			break;

		case RIGHT_ARROW_EVENT:
			if(*currchar && (curpos < FILESPECSIZE-1))
				{
				currchar++;
				curpos++;
				}
			break;

		case LEFT_ARROW_EVENT:
			if(currchar > filespecbuffer)
				{
				currchar--;
				curpos--;
				}
			break;

		case HOME_EVENT:
			currchar = filespecbuffer;
			curpos = 0;
			break;

		case END_EVENT:
			for(  ; *currchar; currchar++, curpos++);
			if(curpos >= FILESPECSIZE)
				{
				curpos--;
				currchar--;
				}
			break;

		default:												/* insert charcter at cursor*/
                                                /* if cursor is at end		*/
			if(curpos == FILESPECSIZE-1 && !(*currchar))
				*currchar = (char)event;				/* insert char at cursor	*/
				/* else if cursor not at end and room for more in buffer			*/
			else if((curpos < FILESPECSIZE) && (strlen(filespecbuffer) < FILESPECSIZE))
				{
				memmove(currchar+1,currchar,strlen(currchar));
				*currchar++ = (char)event;
				curpos++;
				}
			break;
		}
	VioWrtCharStr(filespecbuffer,FILESPECSIZE,filespecfield.startrow+1,
			filespecfield.startcol+1,VIOHDL);
	VioSetCurPos(filespecfield.startrow+1,filespecfield.startcol+1+curpos,
			VIOHDL);
	}

void initfield(void)
	{
	ButtonDisplay(&filespecfield);
	VioSetCurPos(filespecfield.startrow+1,filespecfield.startcol+1,VIOHDL);
	memset(filespecbuffer,0,sizeof(filespecbuffer));
	attribute_mask = 0;
	}

void InitDriveButton(void)
	{
	DosQCurDisk((PUSHORT)dirspecbuffer,&drive_map);
	*dirspecbuffer += ('A'-1);
	strcpy(&dirspecbuffer[1],":\\");
	findbutton("-C-", &drive_button);

	VioWrtCharStr(dirspecbuffer,1,drive_button->startrow+1,drive_button->startcol+2,VIOHDL);
	}


void InitDirButton(void)
	{
	findbutton("Directory",&dir_button);
	dirbuflen = strlen(dir_button->text);

	RefreshDirButton();
	}

void RefreshDirButton(void)
	{
	USHORT numresults,numrequests,i;
	PVOID	resulthdl;
	PFILEFINDBUF	dirp;
	char 	*endbuf;
	USHORT	dirspeclength = sizeof(dirspecbuffer)-DIROFFSET;

	DosQCurDir((USHORT)(*dirspecbuffer-'A'+1),&dirspecbuffer[DIROFFSET],&dirspeclength);
	VioWrtCharStr(dirspecbuffer,strlen(dirspecbuffer),dir_button->startrow,
			dir_button->startcol+1,VIOHDL);
	VioWrtNChar("Í",dirbuflen-strlen(dirspecbuffer),dir_button->startrow,
			dir_button->startcol+1+strlen(dirspecbuffer),VIOHDL);

	WriteDirButton("<waiting....>");

	endbuf = &dirspecbuffer[strlen(dirspecbuffer)-1];
	if(*endbuf != '\\')						/* if not the root			*/
		strcat(endbuf,"\\");
	endbuf++;

	strcat(dirspecbuffer,"*.*");
	if(dirheader)
		DiDestroyRequest(&dirheader);
	DiMakeRequest(&dirheader,dirspecbuffer,0);
	DiSendRequest(dirheader);
	DiGetNumResults(dirheader,&numresults,&numrequests);
	DiGetResultHdl(dirheader,0,&numresults,&resulthdl);
	memset(dirptrs,0,sizeof(dirptrs));

	for( dirs = i = 0; i < numresults; i++)
		{
		if(!i)
			DiGetFirstResultPtr(resulthdl,&dirp);
		else
			DiGetNextResultPtr(resulthdl,&dirp);
		if((dirp->attrFile & FILE_DIRECTORY) && (*dirp->achName != '.'))
			dirptrs[dirs++] = dirp->achName;
		}

	*endbuf = '\0';

	dirptr = (dirs ? dirptrs : NULL);

	WriteDirButton(dirs ? *dirptr : "<No Sub-Directories>");
	}

void WriteDirButton(char *result)
	{
	VioWrtCharStr(result,strlen(result),dir_button->startrow+1,
			dir_button->startcol+1,VIOHDL);
	VioWrtNChar(" ",dirbuflen-strlen(result),dir_button->startrow+1,
			dir_button->startcol+1+strlen(result),VIOHDL);
	}

void dir_event(USHORT event)
	{
	if(!dirs)
		return;
	if( ((event == DIR_DEC_EVENT) && (dirptr == dirptrs)) ||
		((event == DIR_INC_EVENT) && (dirptr == &dirptrs[dirs-1])) )
		{
		beep();
		return;
		}
	if(event == DIR_DEC_EVENT)
		dirptr--;
	else
		dirptr++;

	VioWrtCharStr(*dirptr,strlen(*dirptr),dir_button->startrow+1,
			dir_button->startcol+1,VIOHDL);
	VioWrtNChar(" ",dirbuflen-strlen(*dirptr),dir_button->startrow+1,
			dir_button->startcol+1+strlen(*dirptr),VIOHDL);
	}

void drive_event(USHORT event)
	{
	USHORT curdrive;

	curdrive = *dirspecbuffer-'A';
	if( ((event == DR_DEC_EVENT) && !curdrive) ||
			((event == DR_INC_EVENT) && (*dirspecbuffer == 'Z')) )
		{
		beep();
		return;
		}
	if( event == DR_DEC_EVENT)
		curdrive--;
	else
		curdrive++;
	if( ((drive_map >> curdrive) & 0x0001) && !DosSelectDisk(++curdrive))
		*dirspecbuffer = (char)(curdrive+'A'-1);
	else
		{
		beep();
		return;
		}
	VioWrtCharStr(dirspecbuffer,1,drive_button->startrow+1,drive_button->startcol+2,VIOHDL);
	RefreshDirButton();
	}

void ButtonPress(USHORT *eventcode)
	{
	BUTTON *b = &buttonlist[0];

	MOUSECODEOFF(*eventcode);

	for( ; b->text; b++)								/* find the button			*/
		if((b->left_button_val == *eventcode) ||
				(b->right_button_val == *eventcode) ||
				(b->accelerator == *eventcode))
			{
			switch(b->type)
				{
				case BPRESS:								/* if a press button			*/
					ButtonPaint(b,BUTTON_ON);			/* turn it on					*/
					DosSleep(100L);						/* wait							*/
					ButtonPaint(b,BUTTON_OFF);			/* turn it off					*/
					break;

				case BTOGGLE:								/* if a toggle button		*/
					b->state = !b->state;				/* toggle it					*/
																/* and toggle the color		*/
					ButtonPaint(b,(b->state ? (BYTE)BUTTON_ON : b->attribute));
					break;

				default:
					break;
				}
			return;
			}
	}


	/******************* keyboard thread code *****************/
void KeyboardThread(void)
	{
	KBDINFO	kbdinfo;
	KBDKEYINFO kbdkeyinfo;
	HKBD KbdHandle = 0;
	HQUEUE	qhandle;
	USHORT	event;

	MsgQOpen(&qhandle,MAINMSGQUEUE);

	KbdFlushBuffer(KbdHandle);						/* flush keyboard buffer	*/
	KbdGetStatus(&kbdinfo,KbdHandle);				/* get keyboard status		*/
	kbdinfo.fsMask &= ~COOKED;						/* turn	off	COOKED bit	*/
	kbdinfo.fsMask |= RAW;	  						/* turn	on RAW bit			*/
	KbdSetStatus(&kbdinfo,KbdHandle);				/* set the keyboard status	*/

	while(TRUE)
		{
		KbdCharIn(&kbdkeyinfo,IO_WAIT,KbdHandle);	/* get a character			*/
		if(kbdkeyinfo.chChar) 						/* if Ascii code				*/
			MsgQSend(qhandle,(USHORT)kbdkeyinfo.chChar);	/* pass it on		*/
		else if(event = AcceleratorPressed(kbdkeyinfo.chScan))	/* Accelerator?*/
			MsgQSend(qhandle,event);		/* if so, pass it on			*/
		else
			MsgQSend(qhandle,SCANCODE(kbdkeyinfo.chScan));
		DosSleep(32L);
		}
	}


USHORT AcceleratorPressed(unsigned char key)
	{
	BUTTON *b = &buttonlist[0];

	for( ; b->text; b++)
		if(key == (unsigned char)b->accelerator)
			return MOUSECODE(b->left_button_val);
	return 0;
	}

	/********** end of keyboard thread code *******************/


	/****************** mouse thread code *********************/
void MouseThread(void)
	{
	MOUEVENTINFO MouEvent;
	USHORT WaitOption = MOU_WAIT;						/* set to block on input	*/
	HQUEUE	qhandle;
	USHORT	event,buttondown = FALSE;
	USHORT	retval;

	MsgQOpen(&qhandle,MAINMSGQUEUE);

	if((retval = MouOpen((PSZ)NULL,(PHMOU)&MouHandle)))
		error_exit(retval,"MouOpen");

	MouDrawPtr(MouHandle);								/* display mouse pointer	*/
	MouFlushQue(MouHandle);								/* flush mouse queue			*/

	while(TRUE)
		{														/* read	the	queue			*/
		MouReadEventQue(&MouEvent,&WaitOption,MouHandle);
		MouEventDropLowBit(MouEvent);				 	/* turn off the low bit		*/

		if(!buttondown && IsMouButtonPressed(MouEvent))/* if mouse button pressed	*/
			{
			if(event = ButtonPressed(&MouEvent))	/* while on screen button	*/
				{
				MsgQSend(qhandle,event);				/* notify						*/
				buttondown = TRUE;
				DosSleep(32L);
				}
			}
		else
			buttondown = FALSE;
		DosSleep(32L);
		}

	MouClose(MouHandle);
	}

USHORT ButtonPressed(MOUEVENTINFO *ev)
	{
	register USHORT row = ev->row, col = ev->col;
	BUTTON	*b = &buttonlist[0];

	for( ; b->text; b++)
		{
		if((row	>= b->startrow)	&& (row	<= b->endrow)/* if on button		*/
				&& (col >= b->startcol) && (col <= b->endcol))
			
			if(MouB1Pressed(ev->fs))
				return MOUSECODE(b->left_button_val);
			else if(MouB2Pressed(ev->fs))
				return MOUSECODE(b->right_button_val);
			else
				return 0;
		}
	return 0;
	}

	/******* end of mouse thread code *************************/


	/*************** print thread code ************************/
void PrintThread(void)
	{
	HQUEUE 			qhandle;
	USHORT			retval,event;
	PVOID	header = NULL;

	if(retval = DosCreateQueue(&qhandle,QUE_FIFO,PRINTQUEUE))
		error_exit(retval,"DosCreateQueue");

	while(TRUE)
		{
		MsgQGet(qhandle,&event);
		if(header)
			DiDestroyRequest(&header);
		header = MAKEP(event,0);
		PrintRequest(header); 
		DosSleep(32L);
		}
	}

void PrintRequest(PVOID header)
	{
	USHORT numresults,numrequests,i,len;
	PFILEFINDBUF f;
	PVOID resulthdl;
	char dirspec[80];

	DiGetNumResults(header,&numresults,&numrequests);
	DiGetResultHdl(header,0,&numresults,&resulthdl);

	WriteRequestEntry(" ");

	strcpy(dirspec,DiGetResultDir(resulthdl));
	strcat(dirspec,"\\");
	strcat(dirspec,DiGetResultFspec(resulthdl));

	VioWrtCharStr(dirspec,len = strlen(dirspec),DIRROW,1,VIOHDL);
	VioWrtNChar("Ä",BORDERCOL-len-1,DIRROW,1+len,VIOHDL);

	for( i = 0; i < numresults; i++)
		{
		DosSemWait(&PauseSem,SEM_INDEFINITE_WAIT);

		if(!i)
			DiGetFirstResultPtr(resulthdl,&f);
		else
			DiGetNextResultPtr(resulthdl,&f);
		if( (attribute_mask && (f->attrFile & attribute_mask))
				|| !attribute_mask)
			PrintRequestEntry(f);
		DosSleep(32L);
		}
	}

void PrintRequestEntry(PFILEFINDBUF f)
	{
	char formatbuffer[60];
	char m, attrstr[9];
	USHORT hours;

	if(f->ftimeLastWrite.hours > 12)
		{
		hours = (f->ftimeLastWrite.hours-12);
		m = 'p';
		}
	else
		{
		hours = f->ftimeLastWrite.hours;
		m = 'a';
		}
	set_attribute_values(attrstr,f->attrFile); 
	sprintf(formatbuffer,format,
			strlwr(f->achName),
			f->cbFile,
			f->fdateLastWrite.month,
			f->fdateLastWrite.day,
			f->fdateLastWrite.year+80,
			hours,
			f->ftimeLastWrite.minutes,
			f->ftimeLastWrite.twosecs*2,
			m,
			attrstr);

	WriteRequestEntry(formatbuffer);
	}

void WriteRequestEntry(char *buffer)
	{	
	VioScrollUp(STARTROW,0,ENDROW,BORDERCOL-1,1,(char *)&BlankCell,VIOHDL); 
	VioWrtCharStr((PCH)buffer,strlen(buffer),ENDROW,1,VIOHDL);  
	}

const char *attrs = "--advshr";

unsigned attrvals[8] =
    {
    FILE_UNUSED1,	FILE_UNUSED0,	FILE_ARCHIVED,	FILE_DIRECTORY,
    FILE_VOLLABEL,	FILE_SYSTEM,	FILE_HIDDEN,	FILE_READONLY
    };

    /* sets the string to "-----" or "drhsa", etc. */
void set_attribute_values(char *str, unsigned attribute)
    {
    unsigned i, attrvals = FILE_UNUSED1, len = strlen(attrs);

    memset(str,'-',len);      /* initialize attribute value string */
    str[len] = '\0';
    for( i = 0; i < len; i++, attrvals >>= 1)
        if(attrvals & attribute)
            str[i] = attrs[i];
    }

	/******* end of print thread code *************************/

	/********************** end of dipop.c ********************/

