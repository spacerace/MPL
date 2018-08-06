/* hello0.c RHS 10/14/88
 *
 * OS/2 and 1988 version of K&R's hello.c
 * demonstrates multiple threads
 */

/*
This program provides an introduction to the use of threads and semaphores
under OS/2.  It divides the screen up into a series of logical frames.
Each frame is a portion of the screen that is managed (written to) by a
single thread.	The exact number of frames will depend on the current
screen length (25, 43 and 50 lines).  Each thread has its own data from
which it knows where the frame can be found on screen.	This includes
a semaphore which signals the thread when to proceed.  These elements can
be found in the FRAME data type.

Upon receiving a signal from its semaphore (i.e., the semphore has been
cleared), the thread either draws a message on the frame or clears the
frame, and reverses the flag that determines this.	Then it again blocks
until its semaphore has been cleared again.

The main program thread starts by setting up the frame information:
checking the screen size, determining the number and size of the frames.
It also "randomly" selects the order in which the frames will appear.

Then it sets each thread's semaphore, and initiates each thread (remember
the threads will block until their semphores are cleared.

Finally, the main program goes into an infinite loop, clearing each thread's
semaphore, sleeping for at least 1 millisecond, and then continuing to the
next thread.  Thus the threads asynchronously call the VIO subsystem to
draw or clear each frame, while the main program thread continues.

An optional parameter can be passed to set the number of milliseconds passed
to DosSleep, allowing the operator to more accurately "see" the order in
which the frames appear/erase.	This value must always be at least 1 to
allow the main program thread to give time to the CPU scheduler.

A call to _beginthread() early in main() sets up a thread to monitor key
board input.  This thread blocks until a key is pressed, then examines the
key, and if they is the Escape Key (27 decimal or 1bH), the thread calls
DosExit to kill the whole process.

 */

#define INCL_SUB
#define INCL_DOS
#include<stdio.h>
#include<string.h>
#include<assert.h>
#include<stdlib.h>
#include<process.h>
#include<os2.h>

#if !defined(TRUE)
#define TRUE	1
#endif

#if !defined(FALSE)
#define FALSE	0
#endif

#define LINES25 	4							/* height in lines of frames*/
#define LINES43 	6
#define LINES50 	7

#define RANDOMIZER	5
#define MAXFRAMES	28							/* limited to max frames
possible */
#define RAND()		(rand() % maxframes);
#define THREADSTACK 400 						/* size of stack each thread*/
#define IDCOL		15
#define ESC 		0x1b

char *blank_str =								/* string for blanking frame*/
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


char **helloptr;
int numlines;

typedef struct _frame							/* frame structure			*/
	{
	unsigned	frame_cleared;
	unsigned	row;
	unsigned	col;
	unsigned	threadid;
	long		startsem;
	char		threadstack[THREADSTACK];
	} FRAME;

FRAME far *frames[MAXFRAMES];					/* pointers to frames		*/
unsigned maxframes;
unsigned curframe;
long sleeptime = 1L;							/* minim sleep time 		*/
char keythreadstack[THREADSTACK];

	/* function prototypes ***************************/
void hello_thread(FRAME far *frameptr);
void keyboard_thread(void);
void main(int argc, char **argv);


void main(int argc, char **argv)
	{
	int row, col, maxrows, maxcols, len, i, loops = 0;
	VIOMODEINFO viomodeinfo;

	if(argc > 1)
		sleeptime = atol(argv[1]);
	if(sleeptime < 1L)
		sleeptime = 1L;

												/* start keyboard thread	*/
	if(_beginthread(keyboard_thread,keythreadstack,THREADSTACK,NULL) == -1)
		exit(-1);

	viomodeinfo.cb = sizeof(viomodeinfo);
	VioGetMode(&viomodeinfo,0); 				/* get video info			*/

	maxrows = viomodeinfo.row;
	maxcols = viomodeinfo.col;

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
		default:								/* fail if not 25,43,50 lines*/
			assert(0);
			exit(-1);
		}

	len = strlen(*helloptr);

	maxframes = (maxrows / numlines) * (maxcols / len);

	assert(maxframes <= MAXFRAMES);

	for( i = 0; i < maxframes; i++) 			/* initialize structures	*/
		{
		if(!(frames[i] = malloc(sizeof(FRAME))))
			exit(0);
		frames[i]->frame_cleared = FALSE;
		frames[i]->startsem = 0L;
		memset(frames[i]->threadstack,0xff,sizeof(frames[i]->threadstack));
		}

	i = RAND(); 								/* get first random frame	*/

												/* set up random appearance */
	for(row = col = 0; loops < maxframes ; )	/* set row/col each frame	*/
		{
		if(!frames[i]->frame_cleared)
			{
			frames[i]->frame_cleared = TRUE;	/* set for empty frame		*/
			frames[i]->row = row;				/* frame upper row			*/
			frames[i]->col = col;				/* frame left column		*/

			col += len; 						/* next column on this row	*/

			if(col >= maxcols)					/* go to next row?			*/
				{
				col = 0;						/* reset for start column	*/
				row += numlines;				/* set for next row 		*/
				}

			i = RAND(); 						/* get next random frame	*/
			}
		else
			++i;

		if(i >= maxframes)
			{
			i -= maxframes;
			loops++;							/* keep track of # of frames*/
			}
		}


	for( i = 0 ; i < maxframes; i++)			/* start a thread for each	*/
		{
		DosSemSet(&frames[i]->startsem);		/* initially set each sem.	*/

												/* start each thread		*/
		if((frames[i]->threadid = _beginthread(
				(void far *)hello_thread,
				(void far *)frames[i]->threadstack,
				THREADSTACK,
				(void far *)frames[i])) == -1)
			{
			maxframes = i;						/* reset maxframes on failure*/
			break;
			}
		}

	while(TRUE) 								/* main loop				*/
		{
								/* swing thru frames, signalling to threads */
		for( i = 0; i < maxframes; i++)
			{
			DosSemClear(&frames[i]->startsem);	/* clear: thread can go 	*/
			DosSleep(sleeptime);				/* sleep a little			*/
			}
		}
	}


void hello_thread(FRAME far *frameptr)			/* frame thread function	*/
	{
	register char **p;
	register int line;
	int len = strlen(*helloptr);
	unsigned row, col = frameptr->col;
	char idstr[20];

	while(TRUE)
		{
		DosSemRequest(&frameptr->startsem,-1L); 	/* block until cleared	*/
		itoa(frameptr->threadid,idstr,10);			/* init idstr			*/

		row = frameptr->row;						/* reset row			*/

		if(!frameptr->frame_cleared)				/* if frame in use, erase*/
			for( line = 0; line < numlines; line++, row++)
				VioWrtCharStr(blank_str,len,row,col,0);
		else										/* else frame not in use*/
			{
			p = helloptr;							/* print message		*/
			for( line = 0; **p; line++, row++, p++)
				VioWrtCharStr(*p,len,row,col,0);
													/* write id # in frame	*/
			VioWrtCharStr(idstr,3,row-(numlines/2),IDCOL+col,0);
			}
		frameptr->frame_cleared = !frameptr->frame_cleared;/*toggle use flag
*/
		}
	}

void keyboard_thread(void)
	{
	KBDKEYINFO keyinfo;

	while(TRUE)
		{
		KbdCharIn(&keyinfo,IO_WAIT,0);			/* wait for keystroke		*/
		if(keyinfo.chChar == ESC)				/* if ESC pressed, break	*/
			break;
		}
	DosExit(EXIT_PROCESS,0);					/* terminate the process	*/
	}

	/******** end of hello0.c *********/

