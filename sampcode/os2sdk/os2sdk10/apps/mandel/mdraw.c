/*	mdraw.c - display Mandelbrot set on EGA
 *	R. A. Garmoe  86/12/15
 */



/**	Mdraw displays the elements of the Mandelbrot set
 *
 *		z = z**2 + c
 *
 *	where z and c are in the complex plane and z = 0 + 0i for the first
 *	iteration.  The elements of the set are displayed in black and
 *	elements not in the set are displayed in color.  The data to be
 *	displayed is computed by the program mandel.
 */


/**	Call
 *
 *	mdraw file file ...
 *
 *	where
 *		file	a .cnt file created by the computation program mandel
 *
 *	All of the files are read and displayed in order.  After all of
 *	the images are displayed, keystrokes have the following meaning.
 *		e		   edit palette registers
 *					up/down arrow - select register
 *					right/left arrow - select color
 *		i		   reinitialize palette registers
 *		n		   advance to next display
 *		p		   backup to previous display
 *		q		   cleanup and exit
 *		r		   decrement palette register ripple
 *		R		   increment palette register ripple
 *		z		   continous forward zoom through images
 *		Z		   continous backward zoom through images
 *
 *	All other characters are ignored.
 */




/*
 *	The .cnt file has the format
 *		int	number of points along the real axis
 *		int	number of points along the imaginary axis
 *		int	maximum iteration point for each point
 *		double	real coordinate of upper left
 *		double	imaginary coordinate of upper left
 *		double	real coordinate of lower right
 *		double	imaginary coordinate lower rightft
 *		double	increment between points on real axis
 *		double	increment between points on imaginary axis
 *		long	(loop + 1) counters for histogram values
 *
 *	The remainder of the file is the run length encoded scan
 *	lines encoded as:
 *		int	number of words in scan line encoded as:
 *			+int	    actual count value for pixel
 *			-int int    The first value is the run length and
 *				    second value is the run value
 */



#include <stdio.h>
#include <subcalls.h>
#include <doscalls.h>
#include <dos.h>
#include "mdraw.h"

/*	The routines that are in the IOPL segment must be declared
 *	as pascal routines.  When the IOPL routine is called through
 *	the intersegment call gate, the parameters are copied from the
 *	ring 3 user's stack to the ring 2 IOPL stack.  The parameters
 *	must be popped from the IOPL stack (and the ring 3 stack) by the
 *	ret n instruction of the IOPL routine.	Otherwise, the ring 2
 *	stack is left in an invalid state.  For more information, refer
 *	to the 80286 Programmer's Manual for
 *		intersegment calls
 *		parameter passing through call gates
 *		CALLs to and RETurns from inner protection rings
 *
 *	Note also that the number of words of passed parameters defined in
 *	each of these functions must exactly match the word counts defined
 *	in the EXPORTS statements in the mdraw.def linker definitions file.
 */

extern void far pascal SetDVideo ();
extern void far pascal SetEVideo ();
extern void far pascal SetScanClear ();
extern void far pascal SetScan (int, int, char *);
extern void far pascal SetScanSave (unsigned far *);
extern void far pascal SetScanSFont (unsigned);
extern void far pascal SetScanRFont (unsigned);
extern void far pascal SetScanRestore (unsigned far *);
extern void far pascal SetCursor (char far *);


void far mode_wait (void);	/* starting address for a thread */
void far redraw_wait (void);	/* starting address for another thread */



/*	Structures for VIO calls */

struct	ConfigData config;	/* Display configuration data */
struct	ModeData mode;		/* Display mode data */
struct	ModeData grmode;	/* graphics display mode data */
struct	PhysBufData physbuf;	/* Physical buffer data */
struct	CursorData cursor;	/* Cursor data structure */
struct	KeyData key;		/* Keystroke data */
struct	VIOSTATE vstate;	/* Set vio state */




char	pmand[60] = "mandel.cnt";
FILE	*fmand;

char	chvalid = FALSE;	/* key valid if true */
char	cinit[15] = {WHITE, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, IWHITE,
		     DGRAY, LBLUE, LGREEN, LCYAN, LRED, LMAGENTA, YELLOW};
char	firstread = FALSE;	/* first character read from keyboard if true */
char	flip = FALSE;		/* flip display pages if true */
char	graphics = FALSE;	/* display is in graphics mode if true */
char	mapped[MAXREAL];	/* character array holding mapped interation counters */
char	initpal[16] = {
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
	0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F
};
char	palette[16] = {
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
	0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F
};

char   *screenbuf;		/* pointer to buffer to hold text screen */

int    *bp;			/* pointer to next iteration counter in buf */
int	buf[BUFFER];		/* run length encoded iteration counters */
int	count = 0;		/* number of integers remaining in buf */

long	delay = 0;		/* ripple delay in milliseconds */
long	drawsem = 0;		/* RAM semaphore to control drawing */
long	hist[MAXLOOP + 1] = {0}; /* histogram counters */

struct image timage = {0};	/* local image descriptor structure */
struct ilist *head = NULL;	/* pointer to head of ilist */
struct ilist *curr = NULL;	/* pointer to current ilist */
struct ilist *tail = NULL;	/* pointer to tail of ilist */

unsigned curcol;		/* cursor column position */
unsigned currow;		/* cursor row position */
unsigned descsel;		/* selector for image structure */
unsigned fontsel;		/* selectors for font saves */
unsigned dpoffset = 0;		/* display page offset 0x0000 or 0x8000 */
unsigned *psel; 		/* display memory selector */
unsigned screenlen;		/* length of screen buffer */

main (argc, argv)
int	argc;
char	**argv;
{
	int	c;
	int	i;
	int	temp;
	int	pal;
	unsigned char	*stack1,      /* stack for mode_wait thread  */
			*stack2;      /* stack for redraw_wait thread  */
	unsigned	ThreadID;

	/*  Validate the display configuration.  It must be an EGA on an
	 *  EGA  adapter with at least 128k of memory.
	 */

	config.length = 10;
	if ((temp = VIOGETCONFIG (0, (struct ConfigData far *)&config, 0)) != 0) {
		printf ("Unable to get display configuration data - %d\n", temp);
		exit (1);
	}
	if ((config.adapter_type != 2) || (config.display_type != 2) ||
	    config.memory_size < 0x20000) {
		printf ("Display is not EGA on 128k EGA adapter %d %d %ld\n",
		    config.adapter_type, config.display_type, config.memory_size);
		exit (1);
	}

	if (config.memory_size == 0x40000)
		flip = TRUE;

	/*  Save information about the current display mode.  This includes
	 *  the current mode as known by VIO, the cursor type data, the
	 *  current cursor position and the text screen data.
	 */

	mode.length = sizeof (struct ModeData);
	if ((temp = VIOGETMODE ((struct ModeData far *)&mode, 0)) != 0) {
		printf ("Unable to get display mode data - %d\n", temp);
		exit (1);
	}
	if (VIOGETCURTYPE ((struct CursorData *)&cursor, 0) != 0) {
		printf ("Unable to get current cursor data\n");
		exit (1);
	}
	if (VIOGETCURPOS ((unsigned far *)&currow, (unsigned far *)&curcol, 0) != 0) {
		printf ("Unable to get current cursor position\n");
		exit (1);
	}

	/*  Allocate buffer to hold text screen data.  Note that the buffer
	 *  size is doubled because of the attribute bytes.
	 */

	screenlen = mode.col * mode.row * 2;
	if ((screenbuf = (char *)malloc (screenlen)) == NULL) {
		printf ("Unable to allocate memory for text screen buffer\n");
		exit (1);
	}
	if (VIOREADCELLSTR ((char far *)screenbuf, (unsigned far *)&screenlen,
	    0, 0, 0) != 0) {
		printf ("Unable to read text screen data\n");
		exit (1);
	}

	/*  Obtain selectors to the display memory */

	physbuf.buf_start = 0xa0000;
	physbuf.buf_length = 0x20000;
	if ((temp = VIOGETPHYSBUF ((struct PhysBufData far *)&physbuf, 0)) != 0) {
		printf ("Unable to get physical buffer mapping - %d\n", temp);
		exit (1);
	}
	if (DOSALLOCSEG (0,(unsigned far *)&fontsel, 0) != 0) {
		printf ("Unable to allocate text font save segment\n");
		exit (1);
	}
	/* Allocate memory for separate thread execution */

	stack1 = (unsigned char *)malloc (STACKSIZE);
	stack2 = (unsigned char *)malloc (STACKSIZE);
	if (!stack1 || !stack2) {
		printf ("Out of memory\n");
		exit (1);
	}
	stack1 += STACKSIZE;
	stack2 += STACKSIZE;

	/* create thread to execute the mode wait */

	DOSCREATETHREAD (mode_wait, &ThreadID, stack1);

	/* create thread that will execute the redraw_wait () */

	DOSCREATETHREAD (redraw_wait, &ThreadID, stack2);


	/* Pass pointer to the selectors for display memory and the
	 * selector to the font save segment to the ring 2 IOPL code.
	 */

	psel = physbuf.selectors;

	/* Save the font tables */

	SetScanSFont (fontsel);

	/* Set the EGA to 640 x 350 graphics mode */

	grmode = mode;
	grmode.type = grmode.type | 0x02;
	grmode.hres = 640;
	grmode.vres = 350;
	VIOSETMODE ((struct ModeData far *)&grmode, 0);
	DOSSEMREQUEST ((unsigned long)(long far *)&drawsem, (long)-1);
	setpalette (palette);
	DOSSEMCLEAR ((unsigned long)(long far *)&drawsem);

	/* Set the file name if specified and open the file for reading */

	argc--;
	argv++;
	graphics = TRUE;
	printf ("Clearing screen\n");
	SetScanClear ();
	printf ("Screen cleared\n");
	while (argc > 0) {
		/* set file name */
		strcpy (pmand, *argv);
		strcat (pmand, ".cnt");
		if ((fmand = fopen (pmand, "rb")) == NULL) {
			cleandisplay ();
			printf ("Unable to open count file %s\n", pmand);
			exit (3);
		}
		readimage ();
		fclose (fmand);
		argc--;
		argv++;
	}

	/* Wait for keyboard input to switch display or terminate.  This
	 * thread must delay between checks for keyboard input.  Otherwise,
	 * the code will execute a CPU bound loop and all other threads
	 * and processes will execute at a reduced rate.
	 */

	while (TRUE) {
		if (nextchar (&c, 1)) {
			switch (c) {
				case 'e':
					/* edit palette registers */
					paledit ();
					break;

				case 'i':
					/* reinitialize palette registers */
					for (i = 0; i < 16; i++)
						palette[i] = initpal[i];
					DOSSEMREQUEST ((unsigned long)(long far *)&drawsem, (long)-1);
					setpalette (palette);
					DOSSEMCLEAR ((unsigned long)(long far *)&drawsem);
					break;

				case 'n':
					/* advance to next display */
					nextimage ();
					break;

				case 'p':
					/* backup to previous display */
					previmage ();
					break;

				case 'q':
					/* cleanup and exit */
					cleandisplay ();
					exit ();

				case 'r':
					/* decrement palette register ripple */
					delay += SCANTIME;
					break;

				case 'R':
					/* increment palette register ripple */
					delay -= SCANTIME;
					break;

				case 'z':
					/* continous forward zoom through images */
					while (!nextchar (&c, 1))
						nextimage ();
					ungetchar ();
					break;

				case 'Z':
					/* continous backward zoom through images */
					while (!nextchar (&c, 1))
						previmage ();
					ungetchar ();
					break;

				default:
					/* ignore character */
					break;
			}
		}

		/* delay to prevent CPU bound loop */

		if (delay == 0)
			DOSSLEEP ((long)100);
		else if (delay > 0) {
			ripple (RIPPLE_UP);
			DOSSLEEP (delay);
		}
		else {
			ripple (RIPPLE_DN);
			DOSSLEEP ((long)(-delay));
		}
	}
}




/**	mdisp - display Mandelbrot set
 *
 *	mdisp (im);
 *
 */



mdisp (im)
struct image far *im;
{
	int	i;
	int	ni;			/* scan row counter */
	int	temp;
	int	len;
	char	*pmapped;

	VIOSCRLOCK (1, (char far *)&temp, 0);
	DOSSEMREQUEST ((unsigned long)(long far *)&drawsem, (long)-1);
	SetScanClear ();
	DOSSEMCLEAR ((unsigned long)(long far *)&drawsem);

	for (ni = 0; ni < im->lni; ni++) {
		pmapped = mapped;
		if (!nextvalue (&len))
			break;
		while ((--len >= 0) && (pmapped - mapped < im->lnr) && nextvalue (&i)) {
			if (i >= 0)
				*pmapped++ = im->cmap[i];
			else {
				if (!nextvalue (&temp))
					break;
				len--;
				temp = im->cmap[temp];
				for (; (i < 0) && (pmapped - mapped < im->lnr); i++)
					*pmapped++ = temp;
			}
		}

		/*  The scan line drawing is locked with a RAM semaphore so
		 *  the redraw-wait routine can save and restore the screen
		 */

		DOSSEMREQUEST ((unsigned long)(long far *)&drawsem, (long)-1);
		SetScan (ni, im->lnr, mapped);
		DOSSEMCLEAR ((unsigned long)(long far *)&drawsem);
	}

	/*  Save screen image and unlock the screen */

	DOSSEMREQUEST ((unsigned long)(long far *)&drawsem, (long)-1);
	SetScanSave (curr->desc->savesel);
	DOSSEMCLEAR ((unsigned long)(long far *)&drawsem);

	VIOSCRUNLOCK (0);
}




/**	nextvalue - return next value from file
 *
 *	flag = nextvalue (ptr);
 *
 *	Entry	ptr = pointer to integer to receive value
 *	Exit	*ptr = next value from file
 *	Return	TRUE if next value returned
 *		FALSE if end of file
 */


int nextvalue (p)
int *p;
{
	if (count == 0) {
		count = fread ((char *)buf, sizeof (int), BUFFER, fmand);
		bp = buf;
	}
	if (count == EOF)
		return (FALSE);
	*p = *bp++;
	count--;
	return (TRUE);
}




/**	mode_wait - wait for mode reset request
 *
 *	This routine is executed by another thread that is started
 *	by the main routine.
 *
 *	This routine calls VIOmode_wait requesting to be notified
 *	after the completion of an application or hard error popup.
 *	On such a notification, it puts the screen in the desired
 *	graphics mode.
 */

void mode_wait ()
{
	unsigned NotifyType;

	while (TRUE) {
		/* wait for notification to restore mode */
		VIOMODEWAIT (0, &NotifyType, 0);
		if (NotifyType == 0 && graphics)
			DOSSEMREQUEST ((unsigned long)(long far *)&drawsem, (long)-1);
			VIOSETMODE ((struct ModeData far *)&grmode, 0);
			SetScanClear ();
			DOSSEMCLEAR ((unsigned long)(long far *)&drawsem);
	}
}




/**	redraw_wait - wait for mode reset request
 *
 *	This routine is executed by another thread that is started
 *	by the main routine.
 *
 *	This routine calls VioSavRedrawWait requesting to be notified
 *	for both save and redraw.  On save notification, it copies the
 *	EGA adapter memory to allocated memory segments.  On redraw
 *	notification, it puts the screen in the desired graphics mode
 *	and then redraws the Mandelbrot set.
 */


void redraw_wait ()
{
	unsigned NotifyType;

	while (TRUE) {
		VIOSAVREDRAWWAIT (0, &NotifyType, 0);
		if (graphics) {
			if (NotifyType == 0) {
				/* save graphics screen */
				DOSSEMREQUEST ((unsigned long)(long far *)&drawsem, (long)-1);
				SetScanSave (curr->desc->savesel);
				SetScanClear ();
				continue;
			}
			else if (NotifyType == 1) {
				/* restore graphics screen */
				dpoffset = 0;
				VIOSETMODE ((struct ModeData far *)&grmode, 0);
				SetScanClear ();
				SetDVideo ();
				SetScanRestore (curr->desc->savesel);
				SetEVideo ();
				DOSSEMCLEAR ((unsigned long)(long far *)&drawsem);
			}
		}
	}
}




/**	readimage - read and display next image
 *
 *	readimage ();
 *
 */


readimage ()
{
	int	i;
	struct ilist *tlist;
	int	nr;

	/*  Allocate memory for image structure and link to chain of images */

	if ((tlist = (struct ilist *)malloc (sizeof (struct ilist))) == NULL) {
		cleandisplay ();
		printf ("Unable to allocate memory for ilist structure\n");
		exit (1);
	}
	if (DOSALLOCSEG (sizeof (struct image),(unsigned far *)&descsel, 0) != 0) {
		cleandisplay ();
		printf ("Unable to allocate image descriptor segment of %d bytes\n", sizeof (struct image));
		exit (1);
	}

	FP_SEG (tlist->desc) = descsel;
	FP_OFF (tlist->desc) = 0;

	/*  Read description of the computed Mandelbrot set from the file
	 *  and perform simple validation checks.  Limit the display rows and
	 *  columns to the display maximums.
	 */

	if (fread ((char *)&timage.nreal, sizeof (int), 1, fmand) != 1) {
		cleandisplay ();
		printf ("Error reading maximum number of reals %d\n", timage.nreal);
		exit (2);
	}
	timage.lnr = (timage.nreal > 640)? 640: timage.nreal;
	if (fread ((char *)&timage.nimag, sizeof (int), 1, fmand) != 1) {
		cleandisplay ();
		printf ("Error reading maximum number of imaginaries %d\n", timage.nimag);
		exit (2);
	}
	timage.lni = (timage.nimag > 350)? 350: timage.nimag;
	if (fread ((char *)&timage.mloop, sizeof (int), 1, fmand) != 1) {
		cleandisplay ();
		printf ("Error reading maximum loop count %d\n", timage.mloop);
		exit (2);
	}
	if (fread ((char *)&timage.ul, sizeof (timage.ul), 1, fmand) != 1) {
		cleandisplay ();
		printf ("Error reading upper left coordinates\n");
		exit (2);
	}
	if (fread ((char *)&timage.lr, sizeof (timage.lr), 1, fmand) != 1) {
		cleandisplay ();
		printf ("Error reading lower right coordinates\n");
		exit (2);
	}

	if (fread ((char *)&timage.rinc, sizeof (timage.rinc), 1, fmand) != 1) {
		cleandisplay ();
		printf ("Error reading real increment\n");
		exit (2);
	}
	if (fread ((char *)&timage.iinc, sizeof (timage.iinc), 1, fmand) != 1) {
		cleandisplay ();
		printf ("Error reading imaginary increment\n");
		exit (2);
	}
	if (fread ((char *)&timage.aspect, sizeof (timage.aspect), 1, fmand) != 1) {
		cleandisplay ();
		printf ("Error reading aspect ratio\n");
		exit (2);
	}
	if (fread ((char *)timage.hist, sizeof (long), timage.mloop + 1, fmand) != timage.mloop + 1) {
		cleandisplay ();
		printf ("Error reading histogram\n");
		exit (2);
	}

	/*  Assign colors to each iteration count.  Black is reserved for
	 *  points within the Mandelbrot set.
	 */

	for (nr = 0; nr < timage.mloop; nr++) {
		timage.cmap[nr] = (nr % 15) + 1;
	}
	timage.cmap[timage.mloop] = 0;

	/*  Allocate memory for segments to save graphics screen and
	 *  EGA font tables.
	 */

	for ( i = 0; i < 2; i++) {
		if (DOSALLOCSEG (56020, (unsigned far *)&timage.savesel[i], 0) != 0) {
			cleandisplay ();
			printf ("Unable to allocate screen save segments\n");
			exit (1);
		}
	}

	/* Link image to chain of images */

	DOSSEMREQUEST ((unsigned long)(long far *)&drawsem, (long)-1);
	/* copy structure to far segment */
	*(tlist->desc) = timage;
	tlist->next = NULL;
	tlist->prev = tail;
	if (head == NULL)
		head = tlist;
	else
		tail->next = tlist;
	tail = tlist;
	curr = tlist;

	DOSSEMCLEAR ((unsigned long)(long far *)&drawsem);

	/*  Lock the screen, set the display to graphics mode
	 *  and draw the Mandelbrot set
	 */

	mdisp (curr->desc);
}




/**
 *
 *
 */

cleandisplay ()
{
	/* cancel mode wait and redraw wait threads */

	VIOMODEUNDO (GIVEUPOWNER, KILLTHREAD, RESERVED);
	VIOSAVREDRAWUNDO (GIVEUPOWNER, KILLTHREAD, RESERVED);

	/*  Return the EGA adapter to a default mode that hopefully
	 *  matches the mode that was present when we went into graphics
	 *  mode.  Then restore mode data, text screen data, cursor
	 *  position and cursor type.
	 */

	SetDVideo ();
	if (graphics) {
		/* clear display memory */
		dpoffset = 0;
		SetScanClear ();
		if (flip)
			dpoffset = PAGESIZE;
			SetScanClear ();
		SetScanRFont (fontsel);
	}
	SetEVideo ();
	VIOSETMODE ((struct ModeData far *)&mode, 0);
	if (VIOWRTCELLSTR ((char far *)screenbuf, screenlen, 0, 0, 0) != 0) {
		printf ("Unable to write text screen data\n");
		exit (1);
	}
	if (VIOSETCURPOS (currow, curcol, 0) != 0) {
		printf ("Unable to set current cursor position\n");
		exit (1);
	}
	if (VIOSETCURTYPE ((struct CursorData *)&cursor, 0) != 0) {
		printf ("Unable to set current cursor data\n");
		exit (1);
	}
}



/**	nextimage
 *
 */


nextimage ()
{
	int	temp;

	if (head != tail) {
		DOSSEMREQUEST ((unsigned long)(long far *)&drawsem, (long)-1);
		if (curr->next == NULL)
			curr = head;
		else
			curr = curr->next;
		if (flip)
			dpoffset ^= PAGESIZE;
		SetScanRestore (curr->desc->savesel);
		DOSSEMCLEAR ((unsigned long)(long far *)&drawsem);
	}
}



/**	previmage
 *
 */


previmage ()
{
	int	temp;

	if (head != tail) {
		DOSSEMREQUEST ((unsigned long)(long far *)&drawsem, (long)-1);
		VIOSCRLOCK (1, (char far *)&temp, 0);
		if (curr->prev == NULL)
			curr = tail;
		else
			curr = curr->prev;
		if (flip)
			dpoffset ^= PAGESIZE;
		SetScanRestore (curr->desc->savesel);
		VIOSCRUNLOCK (0);
		DOSSEMCLEAR ((unsigned long)(long far *)&drawsem);
	}
}




/**	ripple - ripple palette registers
 *
 *	ripple (direction);
 *
 *	Entry	direction = 0 if rotate palette registers up
 *		direction = 1 if rotate palette registers down
 *	Exit	palette registers rotated by one
 *	Returns none
 */


ripple (dir)
int dir;
{
	int temp;
	int i;

	switch (dir) {
		case RIPPLE_UP:
			temp = palette[0x0f];
			for (i = 0x0f; i > 1; i--)
				palette[i] = palette[i - 1];
			palette[1] = temp;
			break;

		case RIPPLE_DN:
			temp = palette[1];
			for (i = 1; i < 0x0f; i++)
				palette[i] = palette[i + 1];
			palette[0x0f] = temp;
			break;
	}
	DOSSEMREQUEST ((unsigned long)(long far *)&drawsem, (long)-1);
	setpalette (palette);
	DOSSEMCLEAR ((unsigned long)(long far *)&drawsem);
}




/**	flicker - flicker palette value
 *
 *	flicker (pal);
 *
 *	Entry	pal = index of register to flicker
 *	Exit	palette register toggled between value and black
 *		until keyboard input
 *	Returns none
 */


flicker (pal)
int pal;
{
	int temp;
	int c;

	if ((pal > 0) && (pal < 0x10)) {
		temp = palette[pal];
		while (!nextchar (&c, 1)) {
			palette[pal] = BLACK;
			DOSSEMREQUEST ((unsigned long)(long far *)&drawsem, (long)-1);
			setpalette (palette);
			DOSSEMCLEAR ((unsigned long)(long far *)&drawsem);
			DOSSLEEP ((long)100);
			palette[pal] = temp;
			DOSSEMREQUEST ((unsigned long)(long far *)&drawsem, (long)-1);
			setpalette (palette);
			DOSSEMCLEAR ((unsigned long)(long far *)&drawsem);
			DOSSLEEP ((long)100);
		}
		ungetchar ();
	}
}





/**	paledit - edit palette registers
 *
 *	paledit ();
 *
 *	Entry	none
 */


paledit ()
{
	int	pal;
	int	c;

	/* select palette register */

	pal = 1;
	flicker (pal);
	while (TRUE) {
		nextchar (&c, 0);
		switch (-c) {
			case UP:
				if (pal++ > 0x0f)
					pal = 1;
				flicker (pal);
				break;

			case DOWN:
				if (pal-- < 1)
					pal = 0x0f;
				flicker (pal);
				break;

			case RIGHT:
				if (++palette[pal] > 0x3f)
					palette[pal] = 1;
				DOSSEMREQUEST ((unsigned long)(long far *)&drawsem, (long)-1);
				setpalette (palette);
				DOSSEMCLEAR ((unsigned long)(long far *)&drawsem);
				break;

			case LEFT:
				if (--palette[pal] == 0)
					palette[pal] = 0x3f;
				DOSSEMREQUEST ((unsigned long)(long far *)&drawsem, (long)-1);
				setpalette (palette);
				DOSSEMCLEAR ((unsigned long)(long far *)&drawsem);
				break;

			default:
				ungetchar ();
				return;
		}
	}
}




/**	nextchar - get next character from keyboard
 *
 *	flag = nextchar (pchar, wait);
 *
 *	Entry	char = pointer to character return location
 *		wait = 0 if delay until character entered
 *		wait = 1 if immediate return if no character
 *	Exit	char = next character if found
 *		char = -scan code if extend character
 *	Return	flag = TRUE if character read from keyboard
 *		flag = FALSE if no character read and immediate return
 */


nextchar (pchar, wait)
int *pchar;
int wait;
{
	int	ret;

	if (!chvalid) {
		if ((ret = KBDCHARIN ((struct KeyData far *)&key, wait, 0)) != 0) {
			cleandisplay ();
			printf ("Error %d return from KBDCHARIN\n", ret);
			exit (1);
		}
		firstread = TRUE;
		if ((key.status & 0x40) != 0x40)
			return (FALSE);
	}
	chvalid = FALSE;
	if (key.char_code == 0x00)
		/* process extended ASCII character */
		*pchar = -(int)key.scan_code;
	else
		*pchar = (int)key.char_code;
	return (TRUE);
}





/**	ungetchar - unget character from keyboard
 *
 *	ungetchar ();
 *
 *	Entry	none
 *	Exit	previous character returned to keyboard
 *	Return	none
 */


ungetchar ()
{
	if (firstread)
		chvalid = TRUE;
}




/**	setpalette - set EGA palette registers
 *
 *	setpalette (palette);
 *
 *	Entry	palette = byte array of palette registers
 *	Exit	palette registers written to EGA
 *	Return	none
 */


setpalette (p)
char   *p;
{
	int    *ip;

	vstate.length = sizeof (struct VIOSTATE);
	vstate.req_type = 0;
	vstate.double_defined = 0;
	for (ip = &vstate.palette0; ip <= &vstate.palette15; )
		*ip++ = *p++;
	if (VIOSETSTATE ((struct VIOSTATE far *)&vstate, 0) != 0) {
		cleandisplay ();
		printf ("Unable to set palette registers\n");
		exit (1);
	}
}
