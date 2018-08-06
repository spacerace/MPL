/* hello1.c	RHS	12/14/88
 *
 * OS/2	and	1988 version of	K&R's hello.c
 * demonstrates	multiple threads
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

#include"hello1.h"

	/* Data	***********************************************/

	/*	"Enter Your Name In The Field Below:"	*/
CELL cellstr[] =
	{
{'E',C0},	 {'n',C0},	  {'t',C0},	   {'e',C0},	{'r',C0},	 {' ',C7},
{'Y',C1},	 {'o',C1},	  {'u',C1},	   {'r',C1},	{' ',C7},
{'N',C2},	 {'a',C2},	  {'m',C2},	   {'e',C2},	{' ',C7},
{'I',C3},	 {'n',C3},	  {' ',C7},
{'T',C4},	 {'h',C4},	  {'e',C4},	   {' ',C7},
{'F',C5},	 {'i',C5},	  {'e',C5},	   {'l',C5},	{'d',C5},	 {' ',C7},
{'B',C6},	 {'e',C6},	  {'l',C6},	   {'o',C6},	{'w',C6},	 {':',C6}
	};

	/* Graphics	bitmap for "Hi"	***************************/
char *hi_bitmap[MAPSIZE] =
	{
	{ "111111111111000000000000111111111111000000000000000000000000" },
	{ "111111111111000000000000111111111111000000000000000000000000" },
	{ "111111111111000000000000111111111111000000000000000000000000" },
	{ "111111111111000000000000111111111111000000000000111111111111" },
	{ "111111111111000000000000111111111111000000000000111111111111" },
	{ "111111111111000000000000111111111111000000000000111111111111" },
	{ "111111111111000000000000111111111111000000000000111111111111" },
	{ "111111111111000000000000111111111111000000000000111111111111" },
	{ "111111111111000000000000111111111111000000000000111111111111" },
	{ "111111111111000000000000111111111111000000000000111111111111" },
	{ "111111111111000000000000111111111111000000000000000000000000" },
	{ "111111111111000000000000111111111111000000000000000000000000" },
	{ "111111111111111111111111111111111111000000000000000000000000" },
	{ "111111111111111111111111111111111111000000000000111111111111" },
	{ "111111111111111111111111111111111111000000000000111111111111" },
	{ "111111111111111111111111111111111111000000000000111111111111" },
	{ "111111111111111111111111111111111111000000000000111111111111" },
	{ "111111111111111111111111111111111111000000000000111111111111" },
	{ "111111111111000000000000111111111111000000000000111111111111" },
	{ "111111111111000000000000111111111111000000000000111111111111" },
	{ "111111111111000000000000111111111111000000000000111111111111" },
	{ "111111111111000000000000111111111111000000000000111111111111" },
	{ "111111111111000000000000111111111111000000000000111111111111" },
	{ "111111111111000000000000111111111111000000000000111111111111" },
	{ "111111111111000000000000111111111111000000000000111111111111" },
	{ "111111111111000000000000111111111111000000000000111111111111" },
	{ "111111111111000000000000111111111111000000000000111111111111" },
	{ "111111111111000000000000111111111111000000000000111111111111" },
	{ "111111111111000000000000111111111111000000000000111111111111" },
	{ "111111111111000000000000111111111111000000000000111111111111" },
	{ NULL }
	};


char *blank_str	=								/* string for blanking frame*/
	"                    ";

	/* frame data *************************/
char *hello_str25[LINES25+1] =
	{
	"ีออออออออออออออออออธ",
	"ณ   Hello, world!  ณ",
	"ณ from Thread #    ณ",
	"ิออออออออออออออออออพ",
	"\0"
	};

char *hello_str43[LINES43+1] =
	{
	"ีออออออออออออออออออธ",
	"ณ                  ณ",
	"ณ   Hello, world!  ณ",
	"ณ from Thread #    ณ",
	"ณ                  ณ",
	"ิออออออออออออออออออพ",
	"\0"
	};

char *hello_str50[LINES50+1] =
	{
	"ีออออออออออออออออออธ",
	"ณ                  ณ",
	"ณ   Hello, world!  ณ",
	"ณ                  ณ",
	"ณ from Thread #    ณ",
	"ณ                  ณ",
	"ิออออออออออออออออออพ",
	"\0"
	};


char *hello_str2[LINES25+1]	=
	{
	"ีออออออธ",
	"ณHello ณ",
	"ณ Worldณ",
	"ิออออออพ",
	"\0"
	};

char *adapters_displays[] =
	{
	"Monochrome",
	"Color graphics",
	"Enhanced graphics",
	"Video graphics array",
	"8503 monochrome",
	"8512-8514 color"
	};


char **helloptr;
int	numlines, maxrows, maxcols;
												/* minim sleep times		*/
long sleeptime = 1L, sleeptime2	= 1500L, sleeptime3	= 100L;

FRAME far *frames[MAXFRAMES];					/* pointers	to frames		*/
unsigned maxframes;
unsigned curframe;
char keythreadstack[THREADSTACK];
long doneSem = 0L;
int	done = FALSE;
VIOMODEINFO	viomodeinfo;
int	orgrows;
unsigned char screen[CGABUFSIZE];
unsigned char far *video_base;
unsigned char Savethreadstack[THREADSTACK];


	/* main	*****************************************************************/
void main(int argc,	char **argv)
	{
	startup(argc,argv);							/* set/save	options	modes	*/

	initialization();							/* initialize FRAME	info	*/

	flicker_frames();							/* launch flicker frames	*/

	chasing_frames();							/* launch chasing frames	*/

	displayconfig();							/* get name, and show config*/

	togglebackground();							/* toggle background color	*/

	graphics();									/* say "Hi"	with save-redraw*/

	termination();								/* kill	threads, restore	*/
	}

	/* startup **************************************************************/
void startup(int argc, char	**argv)
	{
	if(argc	> 1)								/* process any arguments	*/
		sleeptime =	atol(argv[1]);
	if(sleeptime < 1L)
		sleeptime =	1L;
	if(argc	> 2)
		sleeptime2 = atol(argv[2]);
	if(argc	> 3)
		sleeptime3 = atol(argv[3]);

												/* start keyboard thread	*/
	if(_beginthread(keyboard_thread,keythreadstack,THREADSTACK,NULL) ==	-1)
		exit(-1);

	Screen(SAVE);								/* save	the	screen			*/
	Cursor(HIDECUR | SAVECUR);					/* hide	and	save the cursor	*/

	Sizeof(viomodeinfo);
	VioGetMode(&viomodeinfo,VIOHDL);			/* get video info			*/
	orgrows	= viomodeinfo.row;					/* save	original row value	*/

	viomodeinfo.row	= 50;						/* set for 50 lines			*/
	if(VioSetMode(&viomodeinfo,VIOHDL))			/* no can do?				*/
		{
		viomodeinfo.row	= 43;					/* try 43					*/
		if(VioSetMode(&viomodeinfo,VIOHDL))		/* still bad news?			*/
			{
			viomodeinfo.row	= orgrows;			/* set original	# of rows	*/
			VioSetMode(&viomodeinfo,VIOHDL);	/* this	has	to work			*/
			}
		}

	maxrows	= viomodeinfo.row;					/* maxrows available		*/
	maxcols	= viomodeinfo.col;					/* maxcols available		*/
	}

	/* Screen ***************************************************************/
void Screen(int	save)
	{
	static void	far	*lbuffer = NULL;
	static unsigned	lbuffersize;
	static void	*oldscreenbuffer = NULL;

	if(save)									/* if save screen			*/
		{										/* get address/size	of LVB	*/
		VioGetBuf((PULONG)&lbuffer,&lbuffersize,VIOHDL);

		if(oldscreenbuffer)						/* if screen buffer	alloc'd */
			free(oldscreenbuffer);				/* free	it first			*/

												/* allocate	buffer			*/
		if(!(oldscreenbuffer = (void far *)malloc(lbuffersize)))
			{
			printf("Unable to save screen buffer\n");
			exit(-1);
			}
												/* save	the	screen			*/
		movedata(FP_SEG(lbuffer),FP_OFF(lbuffer),FP_SEG(oldscreenbuffer),
				FP_OFF(oldscreenbuffer),lbuffersize);
		}
	else										/* if restore the screen	*/
		{
		assert(lbuffer);
												/* update the logical buffer*/
		movedata(FP_SEG(oldscreenbuffer),FP_OFF(oldscreenbuffer),
				FP_SEG(lbuffer),FP_OFF(lbuffer),lbuffersize);
		free(oldscreenbuffer);					/* free	the	buffer storage	*/
		oldscreenbuffer	= NULL;
		VioShowBuf(0,lbuffersize,VIOHDL);		/* display original	screen	*/
		}
	}

	/* Cursor ***************************************************************/
void Cursor(unsigned op)
	{
	static VIOCURSORINFO viocursorinfo;
	static int orgcurattr =	0;
	static unsigned	orgcurrow =	0, orgcurcol = 0;

	if(op &	HIDECUR)							/* if hide cursor			*/
		{
		VioGetCurType(&viocursorinfo,VIOHDL);	/* get cursor type			*/
		if(orgcurattr == 0)
			orgcurattr = viocursorinfo.attr;	/* save	old	cursor attribute*/
		viocursorinfo.attr = 0xffff;			/* set to hide cursor		*/
		VioSetCurType(&viocursorinfo,VIOHDL);	/* set hidden cursor type	*/
		}
	if(op &	SAVECUR)							/* if save cursor position	*/
		VioGetCurPos(&orgcurrow,&orgcurcol,VIOHDL);

	if(op &	RESTORECUR)							/* if restore cursor		*/
		{
		VioGetCurType(&viocursorinfo,VIOHDL);	/* get cursor type			*/
		viocursorinfo.attr = orgcurattr;		/* set to restore cursor	*/
		VioSetCurType(&viocursorinfo,VIOHDL);	/* show	cursor type			*/
		VioSetCurPos(orgcurrow,orgcurcol,VIOHDL);/*	set	cursor position		*/
		}
	}

	/* initialization ********************************************************/
void initialization(void)
	{
	int	i, len,	row, col, loops;

	switch(maxrows)
		{
		case 25:
			helloptr = hello_str25;
			numlines = LINES25;
			break;
		case 43:
			helloptr = hello_str43;
			numlines = LINES43;
			break;
		case 50:
			helloptr = hello_str50;
			numlines = LINES50;
			break;
		default:								/* fail	if not 25,43,50	lines*/
			assert(0);
			exit(-1);
		}

	len	= strlen(*helloptr);

	maxframes =	(maxrows / numlines) * (maxcols	/ len);

	assert(maxframes <=	MAXFRAMES);

	for( i = 0;	i <	maxframes; i++)				/* initialize structures	*/
		{
		if(!(frames[i] = malloc(sizeof(FRAME))))
			exit(0);
		frames[i]->frame_cleared = FALSE;
		frames[i]->startSem	= 0L;
		frames[i]->attr	= (unsigned	char)((i*17) & 0x7f);
		memset(frames[i]->threadstack,0xff,sizeof(frames[i]->threadstack));
		}

	i =	RAND();									/* get first random	frame	*/

												/* set up random appearance	*/
												/* set row/col each	frame	*/
	for(loops =	row	= col =	0; loops < maxframes ; )
		{
		if(!frames[i]->frame_cleared)
			{
			frames[i]->frame_cleared = TRUE;	/* set for empty frame		*/
			frames[i]->row = row;				/* frame upper row			*/
			frames[i]->col = col;				/* frame left column		*/

			col	+= len;							/* next	column on this row	*/

			if(col >= maxcols)					/* go to next row?			*/
				{
				col	= 0;						/* reset for start column	*/
				row	+= numlines;				/* set for next	row			*/
				}

			i =	RAND();							/* get next	random frame	*/
			}
		else
			++i;

		if(i >=	maxframes)
			{
			i -= maxframes;
			loops++;							/* keep	track of # of frames*/
			}
		}
	}

	/* flicker_frames *******************************************************/
void flicker_frames(void)
	{
	int	i;

	for( i = 0 ; i < maxframes;	i++)			/* start a thread for each	*/
		{
		DosSemSet(&frames[i]->startSem);		/* initially set each sem.	*/

												/* start each thread		*/
		if((frames[i]->threadid	= _beginthread(
				(void far *)hello_thread,
				(void far *)frames[i]->threadstack,
				THREADSTACK,
				(void far *)frames[i]))	== -1)
			{
			maxframes =	i;						/* reset maxframes			*/
			break;
			}
		}

	while(!done)								/* when	keyboard thread		*/
		{										/* says	to					*/
								/* swing thru frames, signalling to	threads	*/
		for( i = 0;	i <	maxframes; i++)
			{
			frames[i]->attr	+= (unsigned char)(16+i);
			if((frames[i]->attr	& 0x07)	== (frames[i]->attr	>> 4))
				frames[i]->attr--;
			frames[i]->attr	&= NOBLINKING;
			DosSemClear(&frames[i]->startSem);	/* clear: thread can go		*/
			DosSleep(sleeptime);				/* sleep a little			*/
			}
		DosSemClear(&doneSem);					/* tell	keyboard thread	ok	*/
		DosSleep(32L);							/* sleep a little			*/
		DosSemSet(&doneSem);					/* set it again				*/
		}

	for( i = 0;	i <	maxframes; i++)				/* make	each clear and die	*/
		{
		frames[i]->frame_cleared = 0;
		DosSemClear(&frames[i]->startSem);		/* clear: thread can go		*/
		}
	WaitForThreadDeath(maxframes);				/* wait	for	threads	to die	*/
	}


	/* hello_thread	*********************************************************/
void hello_thread(FRAME	far	*frameptr)			/* frame thread	function	*/
	{
	register char **p;
	register int line;
	int	len	= strlen(*helloptr);
	unsigned row, col =	frameptr->col;
	char idstr[20];
	BYTE att = 0x07;

	while(TRUE)
		{
		if(done	&& frameptr->frame_cleared)
			break;
		DosSemRequest(&frameptr->startSem,-1L);	/* block until cleared		*/
		itoa(frameptr->threadid,idstr,10);		/* init	idstr				*/

		row	= frameptr->row;					/* reset row				*/

		if(!frameptr->frame_cleared)			/* if frame	in use,	erase	*/
			for( line =	0; line	< numlines;	line++,	row++)
				VioWrtCharStrAtt(blank_str,len,row,col,&att,VIOHDL);
		else									/* else	frame not in use	*/
			{
			p =	helloptr;						/* print message			*/
			for( line =	0; **p;	line++,	row++, p++)
				VioWrtCharStrAtt(*p,len,row,col,&frameptr->attr,VIOHDL);
												/* write id	# in frame		*/
			VioWrtCharStrAtt(idstr,3,row-(numlines/2),IDCOL+col,
					&frameptr->attr,VIOHDL);
			}
		frameptr->frame_cleared	= !frameptr->frame_cleared;/*toggle	use	flag*/
		}
	DosSemClear(&frameptr->startSem);
	_endthread();								/* kill	thread				*/
	}

	/* keyboard_thread ******************************************************/
void keyboard_thread(void)
	{
	KBDKEYINFO keyinfo;

	DosSemSet(&doneSem);						/* set done	semaphore		*/
	while(TRUE)
		{
		KbdCharIn(&keyinfo,IO_WAIT,0);			/* wait	for	keystroke		*/
		if(keyinfo.chChar == ESC)				/* if ESC pressed, break	*/
			break;
		}
	DosSemRequest(&doneSem,	-1L);				/* block until cleared		*/
	done = TRUE;

	while(TRUE)
		{
		KbdCharIn(&keyinfo,IO_WAIT,0);			/* don't wait for keystroke */
		if(keyinfo.chChar == ESC)				/* if ESC pressed, break	*/
			break;
		}
	done = TRUE;								/* set it anyway			*/
	_endthread();								/* kill	thread				*/
	}

	/* WaitForThreadDeath ***************************************************/
void WaitForThreadDeath(int	nframes)
	{
	int	i;

	DosSleep(100L);								/* time	for	first to die	*/
	for( i = 0;	i <	nframes; i++)				/* wait	for	each one to	die	*/
		DosSemRequest(&frames[i]->startSem,-1L);
	}

	/* chasing_frames *******************************************************/
void chasing_frames(void)
	{
	int	i;
	unsigned char attr = 0;

	for( i = 0 ; i < maxframes;	i++, attr += 16)/* start each new thread	*/
		{
		DosSemSet(&frames[i]->startSem);		/* initially set each sem.	*/
		frames[i]->attr	= attr;
												/* start each thread		*/
		if((frames[i]->threadid	= _beginthread(
				(void far *)box_thread,
				(void far *)frames[i]->threadstack,
				THREADSTACK,
				(void far *)frames[i]))	== -1)
			{
			maxframes =	i;						/* reset maxframes			*/
			break;
			}
		}
												/* launch chasing threads	*/
	for( i = 0,	done = FALSE; i	< maxframes	&& !done; i++ )
		{
		DosSemClear(&frames[i]->startSem);
		DosSleep(sleeptime2);
		}
	done = TRUE;
	WaitForThreadDeath(i);						/* wait	for	threads	to die	*/
	}

	/* box_thread ***********************************************************/
void box_thread(FRAME far *frameptr)
	{
	char **p;
	int	line, len =	strlen(*hello_str2);
	unsigned row, col;
	int	top, left, bottom, right, count;
	int	tborder	= 0, bborder = maxrows-1, lborder =	0, rborder = maxcols-1;
	BYTE filler[2];

	filler[0] =	' ';
	filler[1] =	frameptr->attr;

	DosSemRequest(&frameptr->startSem,-1L);		/* block until cleared		*/

	frameptr->attr += 0x07;
	if((frameptr->attr & 0x07) == (frameptr->attr >> 4))
		frameptr->attr--;

	col	= (maxcols-len);
												/* write the box			*/
	for(row	= line = 0,	p =	hello_str2;	**p; line++, row++,	p++)
		VioWrtCharStrAtt(*p, len, row, col,	&frameptr->attr, VIOHDL);

	left = col;
	bottom = LINES25-1;
	right =	col+len-1;

	while(!done)								/* main	chasing	frame loop	*/
		{
		count =	0;								/* move	to upper left		*/
		for(left--,	top	= tborder; left	> lborder-1; left--, right--, count++)
			{
			VioScrollLf(top, left, bottom, right, 1, filler, VIOHDL);
			DosSleep(!done ? 1L	: 0L);
			}
												/* move	to lower left		*/
		for(left = lborder,	bottom++; bottom < bborder+1;
				bottom++, top++, count++)
			{
			VioScrollDn(top, left, bottom, right, 1, filler, VIOHDL);
			DosSleep(!done ? 1L	: 0L);
			}
												/* move	to lower right		*/
		for(bottom = bborder, right++; right < rborder+1;
				right++, left++, count++)
			{
			VioScrollRt(top, left, bottom, right, 1, filler, VIOHDL);
			DosSleep(!done ? 1L	: 0L);
			}

		if((bborder	- tborder) > LINES25)		/* adjust top border		*/
			tborder	+= LINES25;
												/* move	to upper right		*/
		for(right =	rborder, top--;	top	> tborder-1; top--,	bottom--, count++)
			{
			VioScrollUp(top, left, bottom, right, 1, filler, VIOHDL);
			DosSleep(!done ? 1L	: 0L);
			}
		rborder	-= len;							/* adjust right	border		*/
		lborder	+= len;							/* adjust left border		*/
		if((bborder	- tborder) > LINES25)		/* adjust bottom border		*/
			bborder	-= LINES25;
		if((rborder-lborder) < len)				/* reached the center?		*/
			break;								/* get out					*/
		if(!count)								/* made	no revolutions?		*/
			break;								/* get out					*/
		DosSleep(sleeptime3);					/* wait	a second...			*/
		}
	DosSemClear(&frameptr->startSem);			/* tell	main thread	we dead	*/
	_endthread();								/* kill	thread				*/
	}

	/* displayconfig ********************************************************/
void displayconfig(void)
	{
	char username[MAXUSERNAME+1];
	unsigned len = sizeof(username), bytesread;
	VIOCONFIGINFO vioconfiginfo;

	bytesread =	get_user_name();				/* get user	name			*/
												/* read	name from screen	*/
	VioReadCharStr(username,&len,FIELDROW,FIELDCOL,VIOHDL);
	username[bytesread]	= '\0';

	Sizeof(vioconfiginfo);
	VioGetConfig(0,	&vioconfiginfo,	VIOHDL);	/* get configuration		*/
	VioSetCurPos(FIELDROW+2, 0,	VIOHDL);		/* set cursor position		*/
	printf("%s, VioGetConfig reports:\n"
			"You\'re using a%s %s adapter with %ld bytes of video memory\n"
			"and a%s %s display\n",
			username,
			(vioconfiginfo.adapter == 2	? "n" :	""),
			adapter(vioconfiginfo.adapter),
			vioconfiginfo.cbMemory,
			(vioconfiginfo.display == 2	? "n" :	""),
			display(vioconfiginfo.display));
	}

	/* get_user_name ********************************************************/
int	get_user_name(void)
	{
	char username[MAXUSERNAME+1];
	unsigned char cell[2];
	STRINGINBUF	kb;

	memset(username,' ',sizeof(username));
												/* print "Enter your name.."*/
	VioWrtCellStr((PCH)cellstr,sizeof(cellstr),FIELDROW-2,
			(maxcols-(sizeof(cellstr)/2))/2,VIOHDL);

	cell[0]	= LEFTMARKER;
	cell[1]	= COLORS(WHITE,BLACK) |	BLINKING;	/* print left marker		*/
	VioWrtNCell(cell,1,FIELDROW,FIELDCOL-1,VIOHDL);

	cell[0]	= BLANK;
	cell[1]	= COLORS(BLACK,CYAN);				/* print field				*/
	VioWrtNCell(cell,sizeof(username),FIELDROW,FIELDCOL,VIOHDL);

	cell[0]	= RIGHTMARKER;
	cell[1]	= COLORS(WHITE,BLACK) |	BLINKING;	/* print right marker		*/
	VioWrtNCell(cell,1,FIELDROW,FIELDCOL+MAXUSERNAME,VIOHDL);

	Cursor(RESTORECUR);							/* restore cursor			*/
	VioSetCurPos(FIELDROW,FIELDCOL,VIOHDL);		/* set cursor position		*/
	kb.cb =	sizeof(username);
	KbdStringIn(username,&kb,IO_WAIT,KBDHDL);	/* get user	name			*/
	Cursor(HIDECUR);							/* hide	cursor				*/
	return kb.cchIn;							/* return chars	read		*/
	}

	/* adapter **************************************************************/
char *adapter(unsigned n)
	{
	return adapters_displays[n];
	}

	/* display **************************************************************/
char *display(unsigned n)
	{
	if(n > 2)
		n++;
	return adapters_displays[n];
	}

	/* togglebackground	*****************************************************/
void togglebackground(void)
	{
	KBDKEYINFO keyinfo;

	printf("Use the Up/Down arrow keys to increment/decrement "
			"the background color\n"
			"\n\nPress Esc to enter graphics mode..."
			"pressing Esc again will terminate the program\n\n"
			"Don\'t press anything but Esc the 2nd time.");

	while(TRUE)
		{
		KbdCharIn(&keyinfo,IO_WAIT,0);			/* wait	for	keystroke		*/
		if(keyinfo.chChar == ESC)				/* if Esc key, break out	*/
			break;
		if(keyinfo.chChar == 0)					/* otherwise...				*/
			{
			if(keyinfo.chScan == UPARROW)		/* Uparrow?	increment bkgrnd*/
				Background(1);
			else if(keyinfo.chScan == DOWNARROW)/* Dnarrow?	decrement bkgrnd*/
				Background(-1);
			}
		DosSleep(1L);							/* wait	a sec				*/
		}
	}

	/* Background ***********************************************************/
void Background(int	n)
	{
	unsigned char cell[2],backgroundattr, attr;
	unsigned len = sizeof(cell);

	VioReadCellStr(cell,&len,0,0,VIOHDL);		/* read	corner cell			*/

	backgroundattr = ((cell[1] >> 4) & 0x7);
	backgroundattr += (unsigned	char)n;
	if(backgroundattr &	0x08)
		backgroundattr = (unsigned char)((n	> 0) ? 0 : 7);
	attr = ((cell[1] & ~BACKGROUNDMASK)	| (backgroundattr << 4));
	attr &=	NOBLINKING;							/* set new attribute		*/
												/* flood screen	with color	*/
	VioWrtNAttr(&attr,maxrows*maxcols,0,0,VIOHDL);
	}

	/* graphics	*************************************************************/
void graphics(void)
	{
	VIOPHYSBUF viophysbuf;
	VIOMODEINFO	graphics_modeinfo;
	KBDKEYINFO keyinfo;
	char status;
	int	x,y;
	unsigned result, wait =	(VP_WAIT | VP_OPAQUE);
	char *bitmap, **bitmapptr;

	viophysbuf.pBuf	= (unsigned	char far *)0xb8000;	/* address of buffer	*/
	viophysbuf.cb =	CGABUFSIZE;					/* size	of buffer			*/

	Sizeof(graphics_modeinfo);
												/* get the video mode		*/
	if(result =	VioGetMode(&graphics_modeinfo,VIOHDL))
		{
		printf("%u error getting video mode\n",result);
		exit(1);
		}

	graphics_modeinfo.fbType = 3;				/* set for non-mono	graphics*/
	graphics_modeinfo.color	= 2;				/* set for 2 color bits		*/
	graphics_modeinfo.col =	40;					/* set for 40 columns		*/
	graphics_modeinfo.row =	25;					/* set for 25 lines			*/
	graphics_modeinfo.hres = 320;				/* set for 320 hor.	pels	*/
	graphics_modeinfo.vres = 200;				/* set for 200 ver.	pels	*/

												/* set the new video mode	*/
	if(result =	VioSetMode(&graphics_modeinfo,VIOHDL))
		{
		printf("%u error setting video mode\n",result);
		exit(1);
		}
	if(result =	VioGetPhysBuf(&viophysbuf, 0))	/* get the buffer address	*/
		{
		printf("%u error getting physical buffer selector\n",result);
		exit(1);
		}

	video_base = MAKEP(viophysbuf.asel[0],0);	/* convert to a	pointer		*/
												/* lock	the	screen			*/
	if(result =	VioScrLock(LOCKIO_WAIT,	&status, VIOHDL))
		{
		printf("%u error locking screen\n",result);
		exit(1);
		}
												/* create save/restore thread*/
	if(_beginthread((void far *)SaveRestoreScreen, (void far *)Savethreadstack,
			sizeof(Savethreadstack),(void far *)video_base)	== -1)
		{
		printf("Unable to start Save/Redraw thread\n");
		exit(1);
		}

	clear_graphics_screen(video_base);			/* clear the screen			*/

												/* write "Hi"				*/
	for( x = UPPER_START_PIXEL,	bitmapptr =	hi_bitmap; *bitmapptr; x++,	bitmapptr++)
		for( y = LEFT_START_PIXEL, bitmap =	(char *)*bitmapptr;	*bitmap; bitmap++, y++)
			if(*bitmap == '1')
				putpixel(x,y,CGA_MAGENTA);

	if(result =	VioScrUnLock(VIOHDL))			/* unlock the screen		*/
		{
		printf("%u error unlocking screen\n",result);
		exit(1);
		}

	while(TRUE)
		{
		KbdCharIn(&keyinfo,IO_WAIT,0);			/* wait	for	keystroke		*/
		if(keyinfo.chChar == ESC)				/* if ESC pressed, break	*/
			break;
		else									/* else	something else pressed*/
			{
			VioPopUp(&wait,VIOHDL);				/* Pop-up new screen		*/
			printf("\n\n\n\nYou did not press the Esc key"
					"...now press a key to try it again\n"
					"\n(Note:\n"
					"As long as the graphics \'HI\' is on screen,\n"
					"the SaveRestoreScreen thread will be activated\n"
					"when you switch screens or this Pop-up appears)\n");
			getch();							/* wait	for	key				*/
			VioEndPopUp(VIOHDL);				/* end the pop-up			*/
			}
		}
	}

	/* SaveRestoreScreen ****************************************************/
void SaveRestoreScreen(void)
	{
	unsigned result,notify;
	unsigned char far *sbank1, *bank2, *sbank2,	*bank1;
	unsigned count;
	VIOMODEINFO	viomodeinfo;

	Sizeof(viomodeinfo);

												/* save	current	video mode	*/
	if(result =	VioGetMode(&viomodeinfo,VIOHDL))
		{
		printf("%u error getting video mode\n",result);
		exit(1);
		}

	while(1)
		{
		sbank1 = screen;						/* set pointers	for	copy	*/
		sbank2 = &screen[BANK2OFFSET];
		bank1  = video_base;
		bank2  = &video_base[BANK2OFFSET];
		count  = 8000;
												/* wait	for	redraw			*/
		if(result =	VioSavRedrawWait(VSRWI_SAVEANDREDRAW,&notify,0))
			{
			printf("%u error on SaveReDrawWait\n",result);
			exit(1);
			}

		if(notify == VSRWN_SAVE)				/* if save screen			*/
			{
			for( ; count; count--)				/* copy	screen to buffer	*/
				{
				*sbank1++ =	*bank1++;
				*sbank2++ =	*bank2++;
				}
			}
		if(notify == VSRWN_REDRAW)				/* if redraw screen			*/
			{									/* restore the video mode	*/
			if(result =	VioSetMode(&viomodeinfo,VIOHDL))
				{
				printf("%u error setting video mode\n",result);
				exit(1);
				}
			for( ; count; count--)				/* copy	buffer to screen	*/
				{
				*bank1++ = *sbank1++;
				*bank2++ = *sbank2++;
				}
			}
		}
	}

	/* clear_graphics_screen ************************************************/
void clear_graphics_screen(register	unsigned char far *bank1)
	{
	register unsigned char far *bank2;
	unsigned count;

	for(bank2 =	(bank1+BANK2OFFSET), count = 0;
			count <	8000; count++, bank1++,	bank2++)
		{
		*(bank1) = 0xff;
		*(bank2) = 0xff;
		}
	}

	/* putpixel	*************************************************************/
void putpixel(int row, int col,	unsigned color)
	{
	unsigned mask =	0xff3f;
	unsigned char far *off;

		/* pixel is	at video_base +	(# of rows * bytes per row)				*/
		/* plus	# of columns/2 (since half are in next bank)				*/
		/* add BANK2OFFSET-BYTES_PER_ROW for 2nd bank if odd-numbered row		   */
	off	= (video_base +	(row * BYTES_PER_ROW) +	(col >>	2) +
			((row &	1) ? (BANK2OFFSET-BYTES_PER_ROW) : 0));

		/* put color bits into top pair, leave on bottom 2 bits	of col	*/
		/* col now has bit offset into byte	of the bits	to set			*/
	for( color <<= 6, col &= 0x03; col;	col--)
		{
		mask >>= 2;								/* shift mask and color	bits*/
		color >>= 2;
		}
	*off = ((*off &	(char)mask)	| color);		/* set byte	to color		*/
	}

	/* termination **********************************************************/
void termination(void)
	{
	if(orgrows != viomodeinfo.row)				/* if screen mode changed	*/
		{
		viomodeinfo.row	= orgrows;
		VioSetMode(&viomodeinfo,VIOHDL);		/* reset orig. screen mode	*/
		}

	Screen(RESTORE);							/* restore screen			*/
	Cursor(RESTORECUR);							/* restore cursor			*/

	DosExit(EXIT_PROCESS,0);					/* terminate the process	*/
	}

	/******** end of hello1.c *********/


