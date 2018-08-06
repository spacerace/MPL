/***
 *  Title:
 *
 *	LIFE - An OS/2 Game of Life
 *
 *
 *  Author:
 *
 *	Brian J. Smith
 *	(c) Microsoft Corporation
 *	1987
 *
 *
 *  Description:
 *
 *	This is a OS/2 implementation of the game of Life.  It is designed
 *	to be bound to allow it to operate in both the protect mode of OS/2
 *	and in earlier MS-DOS versions.  The program uses a mouse if one
 *	is installed, but one is not needed.
 *
 *	Keyboard commands:   B(lank) - redraw screen
 *			     G(o)    - step through generations until a key or
 *				       button is hit
 *			     H(alt)  - freeze at current generation
 *			     Q(uit)  - exit program
 *			     R(ead)  - read a game board from the disk
 *			     S(step) - advance a single generation
 *			     W(rite) - write the current board to disk
 *			     D(own-speed) - slow down GOs
 *			     U(p-speed) - speed up GOs
 *
 */


#include <doscalls.h>			/* definitions for DOS calls and */
#include <subcalls.h>			/*    associated data structures */

/* defines */
#define SCR_WID 80			/* screen buffer row width in bytes */
#define SIGNATURE 0x5342		/* 1st word of all files from this */

/* these defines are all coordinates to put various messsages */
#define COM_ROW 23			/* alpha row for command line */
#define COM_COL 0			/* column for same */
#define PROMPT_ROW 24			/* alpha row for prompts */
#define PROMPT_COL 0			/* column for same */
#define GEN_COL 13			/* column to put generation number on*/
#define FILE_COL 37			/* column to put filespec */

/* misc. defines */
#define FONT_ROW 8			/* pixel rows in the font */
#define BIT 8				/* number of bits in a byte */
#define BYTEINCOLS 8			/* no. of interal columns in a byte */

/****	all functions in this file ******/
int main();		 /* setup and master loop for life game*/
void blank();		 /* blank internal and screen grid*/
void go();		 /* steps through generations until a key is pressed*/
void halt();		 /* does nothing */
void quit();		 /* exit after prompting for certainty*/
void diskread();	 /* read new grid from disk*/
int step();		 /* advance one generation on screen and internally*/
void diskwrite();	 /* write internal grid to disk*/
void up();		 /* speeds up GO if possible */
void down();		 /* slows down GO */
void beep();		 /* beep the speaker for bad commands*/
void showingrid();	 /* display internal grid on screen*/
int getfilespec();	 /* get a file name from the user*/
void putgen();		 /* puts the current generation counter on prompt line*/
void fill();		 /*  fill in a grid cell on the screen and internally*/
void remove();		 /* clear a grid cell on the screen and internal grids*/
int kbdreadeventque();	 /* simulates MouReadEventQue with the keyboard*/
void xorptr();		 /* xors the mouse ptr on the screen*/
void gputs();		 /* put a character string on the graphics screen*/
void gputchar();	 /* put a character on the graphics screen*/
void anerror(); 	 /* error handler*/
char gethit();		 /* waits for a key hit or mouse button hit*/
void wait4release();	 /* wait until mouse buttons are up before returning*/
void far pascal exitlife();/* exit program, resetting original screen mode*/


/* global data */
/* global variables for the internal and screen grids */
int InRow = 45; 			/* rows & columns of cells w/ default*/
int InCol = 80; 			/*   in internal grid. Col must be /8*/
int ScrRow = 45;			/* rows & column on screen grid */
int ScrCol = 79;
int SizeScrRow = 4;			/* pixel rows per screen grid row */
int SizeScrCol = 8;			/* pixels cols per screen grid column*/
char far *InGrid;			/* pointer to internal grid, a simple
					 * bit map of the space and cells */
char far *InGrid2;			/* pointer for secondary map used
					 *   to calculate next generation */

/* screen related global data */
static struct ModeData Highres = {12, 3, 1, 80, 25, 640, 200}; /* 640x200 b/w*/
struct ModeData Savemode = {12};	/* place to save old screen mode */
unsigned ScrSeg;			/* screen buffer segment address */
char Cell[]={32,7};			/* blank for clear screen */
unsigned OddPage=0x2000;		/* offset on Cga of odd row bit plane*/
					/*   should be 0 on non-Cga modes */
unsigned Cga=2; 			/* if using a Cga, screen adresses on
					 *   must be divided by 2 because of
					 *   the odd and even row bit planes
					 * if using non-Cga mode, should be 1*/
/* mouse related global data */
unsigned Mouse = 0;			/* handle if mouse is present, else */
int MouBoundRow = COM_ROW*FONT_ROW+6;	/* last row mouse is allowed on */
int MouBoundCol = 631;			/* last column mouse is allowed on */

/* misc. global data */
char Filespec[79-FILE_COL];		/* file name holder (allows default)*/
unsigned Generation;			/* current generation count */
char Logo[] = "                                                             \
    Microsoft LIFE";                    /* logo used to clear prompt line */
int Slow=0;				/* number of slow-down loops for Go */

/* Data for commands.  Column of command on COM_ROW for printed name,
 *	name for printing on command line, flag if not 0 then the command
 *	can be executed within a Go command without stopping execution,
 *	and the function that does the command. */
struct Commands {
	int Col;
	char *Name;
	char GoAble;
	void (*Fun)();
} ComLine[] = { 0, "Command:", 0, beep,
		10, "Blank", 0, blank,
		16, "Go", 0, go,
		19, "Halt", 0, halt,
		24, "Quit", 0, quit,
		29, "Read", 0, diskread,
		34, "Step", 0, step,
		39, "Write", 0, diskwrite,
		46, "Down-speed", 1, down,
		57, "Up-speed", 1, 0,
		67, 0, 0
	      };


/***	main - setup and master loop for life game
 *
 *	First this sets the screen mode, gets the address of the screen buffer,
 *	sets the ctrl-C handle to a routine to reset the screen mode on exit,
 *	and allocates the data structures for the internal representation of
 *	the life grid.	The main loop of the program continuously reads from
 *	the keyboard and the mouse and translates the key or location of the
 *	mouse with the ComLine structure into a command to execute.
 */
main () {
    struct KeyData kbd; 			/* return for KBD call */
    struct PhysBufData get_phys;		/* return for GetPhysBufData */
    unsigned long throwaway;			/* for returns I don't use */
    int i;					/* just a counter */
    unsigned far *ptr;				/*used to clear internal grid*/

    /* data for the mouse (or keyboard emulator) */
    int ptrrow = 86;		/* current pointer position */
    int ptrcol = 316;
    unsigned status = 0x100;	/* for MouSetDevStatus */
    int type=0; 		/* for no waits on mouse read */
    struct EventInfo event;	/* return for mouse reads */
    struct PtrLoc loc;		/* data for MouSetPtrPos */


    /* try to open and initialize mouse.  If none installed, Mouse will
     *	    stay 0 to show keyboard emulation must be used */
    if (!MOUOPEN (0L, (unsigned far *) &Mouse))   /* get handle */
	    /* mouse is here and well, so initialize it */
	    MOUSETDEVSTATUS ((unsigned far *) &status, Mouse);

    /* get current screen mode and save it for restoring on exit */
    if (VIOGETMODE ((struct ModeData far *) &Savemode, 0)) {
	    printf ("Error setting screen mode, exitting\n");
	    DOSEXIT (1, 0);	   /* exit if error */
    }
    /* set screen mode to 640x200 b/w (Cga high resolution */
    if (VIOSETMODE ((struct ModeData far *) &Highres, 0)) {
	    printf ("Error setting screen mode, exitting\n");
	    DOSEXIT (1, 0);	   /* exit if error */
    }

    /* set mouse pointer to middle of screen */
    if (Mouse) {
	loc.RowPos=ptrrow;
	loc.ColPos=ptrcol;
	MOUSETPTRPOS ((struct PtrLoc far *) &loc, Mouse);
    }

    /* get screen buffer segment */
    get_phys.buf_start=0xb8000L;
    get_phys.buf_length=16*1024L;
    if (VIOGETPHYSBUF ((struct PhysBufData far *) &get_phys, 0)) {
	    /* if error here, restore screen and exit */
	    VIOSETMODE ((struct ModeData far *) &Savemode, 0);
	    printf ("Error accessing screen memory, exitting\n");
	    DOSEXIT (1, 0);
    }
    ScrSeg = get_phys.selectors[0];	    /* store in global */

    /* set ctrl-C to quit() to reset screen at even a break */
    DOSSETSIGHANDLER (exitlife, &throwaway, (unsigned far *)&throwaway, 2, 1);

    /* allocate a segment to hold internal grid representation */
    if (DOSALLOCSEG (InRow*InCol/BYTEINCOLS, (unsigned far *) &InGrid, 0))
	    /* from here on use anerror() to report errors */
	    anerror ("Error allocating memory", 1);
    (long) InGrid *= 0x10000L;		    /* move selector to high word*/

    /* allocate a segment to hold second buffer used in stepping */
    if (DOSALLOCSEG(InRow*InCol/BYTEINCOLS, (unsigned far *) &InGrid2, 0))
	    anerror ("Error allocating memory", 1);
    (long) InGrid2 *= 0x10000L; 	    /* move selector to high word*/

    /* blank internal grid and draw screen */
    for (i=InRow*InCol/16, ptr=(unsigned far *) InGrid ; i--;)
	    *ptr++ = 0;
    showingrid ();
    xorptr (ptrrow, ptrcol);		    /* show cursor */


    /* main input loop, polling mouse and keyboard */
    while (1) {
	/* try to get mouse event from keyboard or mouse */
	kbd.char_code=0;			/* clear key buffer residue */
	kbdreadeventque (&event, ptrrow, ptrcol);
	if (Mouse)
	    MOUREADEVENTQUE ((struct EventInfo far *) &event,
				 (unsigned far *) &type, Mouse);

	/* if any mouse-like events, do this */
	if (event.Mask) {
	    xorptr (ptrrow, ptrcol);	    /* hide cursor */
	    if (event.Mask & 1+2+8) {	    /* if any motion */
		    /* update position, if off screen grid,ptr=max or min*/
		    ptrrow = event.Row;
		    ptrcol = event.Col;
		    if (ptrrow > MouBoundRow)
			ptrrow = MouBoundRow;
		    if (ptrcol > MouBoundCol)
			ptrcol = MouBoundCol;
	    }
	    /* if a button was hit */
	    if (event.Mask & (2|4|8|16)) {
		if (ptrrow < COM_ROW*FONT_ROW-SizeScrRow) { /* if on grid */
		    Generation=0;		       /* reset Gen if	 */
		    putgen();			       /*   grid modified*/
		    if (event.Mask & (2 | 4))	       /* if left down*/
			fill (ptrcol/SizeScrCol, ptrrow/SizeScrRow);
		    else if (event.Mask & (8 | 16))    /* if right */
			remove (ptrcol/SizeScrCol, ptrrow/SizeScrRow);
		}
		else {				       /* on command line*/
		    /* if pointing at command word, execute it, highlighting */
		    for(i=0; ComLine[i].Fun != 0; i++)
			if (ptrcol/BIT >= ComLine[i].Col &&
				ptrcol/BIT < ComLine[i+1].Col-1) {
			    gputs(ComLine[i].Name,COM_ROW,ComLine[i].Col,0xff);
			    wait4release ();		/* execute on release*/
			    (*(ComLine[i].Fun))();
			    if (ComLine[i].Fun) /*restore name if not removed*/
			       gputs(ComLine[i].Name,COM_ROW,ComLine[i].Col,0);
			}
		}
	    }
	    xorptr (ptrrow, ptrcol);		   /* show cursor */
	}

	/* check keys, no wait */
	KBDPEEK ((struct KeyData far *) &kbd, 0);
	if (kbd.char_code) {		/* get key only if not regular char */
		KBDCHARIN ((struct KeyData far *) &kbd, 1, 0);	/*get command*/
		xorptr (ptrrow, ptrcol);		/* avoid overwrites */
		/* walk command structure and execute function, highlighting */
		for(i=0; ComLine[i].Fun != 0; i++)
		    if ((kbd.char_code & 0xdf) == *(ComLine[i].Name)) {
			gputs(ComLine[i].Name,COM_ROW,ComLine[i].Col,0xff);
			(*(ComLine[i].Fun))();
			if (ComLine[i].Fun) /*restore name if not removed*/
			    gputs(ComLine[i].Name,COM_ROW,ComLine[i].Col,0);
		    }
		xorptr (ptrrow, ptrcol);		/* restore pointer */
	}
    }
}


/***	blank - blank internal and screen grid
 *
 *	Sets the internal grid to all 0's and calls draw_grid() to
 *	clear the screen and put up a screen grid.  blank() then draws
 *	the command line and the program logo.
 *
 *	Entry:	ScrSeg = current screen segment
 *		InGrid points to internal grid defined by InCol and InRow
 *		Logo points to program logo that also clears the prompt line
 *
 *	Exit:	Generation = 0
 *
 *	Calls:	gputs(), putgen(), draw_grid [assembler routine]
 */
void
blank (){
	unsigned far *ptr;
	int i;

	gputs (Logo, PROMPT_ROW, PROMPT_COL, 0);
	gputs ("Are you sure?", PROMPT_ROW, PROMPT_COL, 0);
	if (gethit () == 'Y') {
		/* if yes, blank screen and internal grid */
		draw_grid ();			/* draw blank grid on screen */
		for (i=InRow*InCol/16, ptr=(unsigned far *) InGrid ; i--;)
			*ptr++ = 0;		/* blank internal grid */
		for (i=0; ComLine[i].Fun != 0; i++) /* print command line */
			gputs (ComLine[i].Name, COM_ROW, ComLine[i].Col, 0);
		Generation=0;			/* reset generation count */
	}
	gputs (Logo, PROMPT_ROW, PROMPT_COL, 0);   /* put on logo */
	putgen ();				/* initial generation message*/
}


/***	go - steps through generations until a key is pressed
 *
 *	Calls step() until a key is hit, which terminates the loop
 *	and the key is left on the buffer for main() to process
 *	as a command.
 *	step()'s return is checked to see if this program has just
 *	moved from background to foreground, in which case the
 *	the screen will need manual updating with showingrid().
 *	If the D or U keys are hit, down() or up() is executed.
 *
 *	Entry:	none
 *
 *	Exit:	None
 *
 *	Calls:	step(), showingrid()
 */
void
go () {
    struct KeyData kbd; 			/* return from KBD call */
    int background=0;				/* 0 if forground, 1 backgrd */
    struct EventInfo event;			/* return from mouse read */
    int type=0; 				/* for no waits on mouse read*/
    struct QueInfo num; 			/* return for GetNumQueEl */
    unsigned x,y;				/* for slowing down loops */

    /* step until key or button is hit, executing GoAble commands */
    kbd.scan_code=0;
    event.Mask=0;
    while (!kbd.scan_code) {
	if (step ())
		background=1;
	else if (background==1) {
		background=0;
		showingrid();
	}
	if (Mouse) {
		/* read all events on que */
		MOUGETNUMQUEEL ((struct QueInfo far *) &num, Mouse);
		while (num.Events--) {
			MOUREADEVENTQUE ((struct EventInfo far *) &event,
				 (unsigned far *) &type, Mouse);
			if (event.Mask & (2|4|8|16)) {	/* leave if mouse hit*/
				wait4release();
				return;
			}
		}
	}

	/* check keys, no wait.  Execute if GoAble command */
	KBDPEEK ((struct KeyData far *) &kbd, 0);
	if (kbd.scan_code) {
		/* walk command structure and execute function, highlighting */
		for(x=0; ComLine[x].Fun != 0; x++)
		    if ((kbd.char_code & 0xdf) == *(ComLine[x].Name)
			       && ComLine[x].GoAble) {
			gputs(ComLine[x].Name,COM_ROW,ComLine[x].Col,0xff);
			KBDCHARIN ((struct KeyData far *) &kbd, 1, 0);
			kbd.scan_code=0;
			(*(ComLine[x].Fun))();
			if (ComLine[x].Fun) /*restore name if not removed*/
			    gputs(ComLine[x].Name,COM_ROW,ComLine[x].Col,0);
		    }
	}
	if (Slow)		   /* slow down if requested */
		for (x=Slow; x--;)
			for (y=50000; y--;);
    }
}


/***	halt - does nothing
 */
void
halt () {
	int i;

	for (i=30000; i--;);		/* short pause */
}


/***	quit - exit after prompting for certainty
 *
 *	Entry:	None
 *
 *	Exit:	None
 */
void
quit () {
	gputs (Logo, PROMPT_ROW, PROMPT_COL, 0);
	gputs ("Are you sure?", PROMPT_ROW, PROMPT_COL, 0);
	if (gethit () == 'Y')
		exitlife ();			/* if yes, do quit routine */
	gputs (Logo, PROMPT_ROW, PROMPT_COL, 0);   /* else, continue */
	putgen();				/* put back gen count*/
}


/***	diskread - read new grid from disk
 *
 *	getfilespec() is called to get a file name from the user which
 *	is put in Filespec.
 *	Then an internal grid is read from the disk which is in the form:
 *				WORD SIGNATURE ; Life file signature
 *				WORD rows
 *				WORD columns
 *				WORD generation
 *    (rows*columns/BYTEINCOLS) BYTES of the bit mapped grid
 *
 *	InGrid is ReAlloced to the size of the newly read grid and
 *	InCol, InRow, and Generation are all set the values contained
 *	in the file read.
 *
 *	Entry:	InGrid points to internal grid defined by InCol and InRow
 *
 *	Exit:	InGrid is ReAlloced to size of newly read grid.
 *		InCol, InRow, and Generation are all set the values
 *			contained in the file read.
 *						 d
 *	Calls:	getfilespec ();
 */
void
diskread () {
	unsigned handle;			/* file handle */
	unsigned action;			/* return for file calls */
	unsigned signature;			/* life file signature word */

	/* get file name into Filespec */
	if (getfilespec ())
		return; 			/* return if user hit ESC */

	/* open file, fail if it doesn't exist */
	if (DOSOPEN ((char far *) Filespec, (unsigned far *) &handle,
	    (unsigned far *) &action, 0L, 0, 0x01, 0x42, 0L)) {
		anerror ("Can't open file", 0);
		Filespec[0]=0;	/* clear bad file name */
		return;
	}

	/* read appropriate from file */
	if (DOSREAD (handle, (char far *) &signature, sizeof (signature),
	    (unsigned far *) &action) || action != sizeof (signature))
		anerror ("Error writing to file", 0);

	else if (signature != SIGNATURE) {
		anerror ("Not a life file", 0);
		if (DOSCLOSE (handle))
			anerror ("Error closing file", 0);
		return;
	}
	else if (DOSREAD (handle, (char far *) &InRow, sizeof (InRow),
	    (unsigned far *) &action) || action != sizeof (InRow))
		anerror ("Error reading from file", 0);

	else if (DOSREAD (handle, (char far *) &InCol, sizeof (InCol),
	    (unsigned far *) &action) || action != sizeof (InCol))
		anerror ("Error reading from file", 0);

	else if (DOSREAD (handle, (char far *) &Generation,
	    sizeof (Generation), (unsigned far *) &action) ||
	    action != sizeof (Generation))
		anerror ("Error reading from file", 0);

	/* change size of *InGrid to match saved pattern */
	else if (DOSREALLOCSEG (InRow*InCol/BYTEINCOLS, (unsigned)
	    ((long) InGrid/0x10000L)))
		anerror ("Error allocating memory", 0);

	else if (DOSREAD (handle, InGrid, InRow*InCol/BYTEINCOLS,
	    (unsigned far *) &action) || action != InRow*InCol/BYTEINCOLS)
		anerror ("Error reading from file", 0);

	/* show the newly loaded pattern on the screen */
	showingrid ();

	/* close file */
	if (DOSCLOSE (handle))
		anerror ("Error closing file", 0);
}


/***	step - advance one generation on screen and internally
 *
 *	Uses dostep to advance the current internal and screen grid
 *	to the next generation.  dostep() returns 1 if the screen
 *	was not available and thus no update was made, this allows
 *	the program to continue to execute in the background.
 *	The return from dostep() is passed back to step()'s caller.
 *
 *	Entry:	None
 *
 *	Exit:	Returns 1 if executing in background and there was thus
 *		      no screen update.
 *		Else, returns 0 if in forground
 *		Generation is incremented.
 *
 *	Calls:	putgen(), dostep [assembler routine]
 */
int
step() {
	int	rc;				/* return code from dostep */

	/* do the stepping using an assembler routine for speed */
	rc=dostep(InGrid, InGrid2, InRow, InCol);  /* do the step */

	Generation++;				/* advance the count */
	putgen();				/* and display gen */
	return(rc);
}


/***	diskwrite - write internal grid to disk
 *
 *	getfilespec() is called to get a file name from the user which
 *	is put in Filespec.
 *	Then the internal grid is saved to disk in the form:
 *				WORD SIGNATURE ; Life file signature
 *				WORD rows
 *				WORD columns
 *				WORD generation
 *    (rows*columns/BYTEINCOLS) BYTES of the bit mapped grid
 *
 *	Entry:	InGrid points to internal grid defined by InCol and InRow
 *
 *	Exit:	None
 *
 *	Calls:	getfilespec ();
 */
void
diskwrite () {
	unsigned handle;			/* file handle */
	unsigned action;			/* return for file calls */
	unsigned signature=SIGNATURE;		/* life file signature word */

	/* get file name into Filespec */
	if (getfilespec ())
		return; 			/* return if user hit ESC */

	/* open file and truncate or create it if it doesn't exist */
	if (DOSOPEN ((char far *) Filespec, (unsigned far *) &handle,
	    (unsigned far *) &action, 0L, 0, 0x12, 0x42, 0L)) {
		anerror ("Can't open file", 0);
		Filespec[0]=0;	/* clear bad file name */
		return;
	}

	/* write appropriate info to file */
	if (DOSWRITE (handle, (char far *) &signature, sizeof (signature),
	    (unsigned far *) &action)	|| action != sizeof (signature))
		anerror ("Error writing to file", 0);

	else if (DOSWRITE (handle, (char far *) &InRow, sizeof (InRow),
	    (unsigned far *) &action) || action != sizeof (InRow))
		anerror ("Error writing to file", 0);

	else if (DOSWRITE (handle, (char far *) &InCol, sizeof (InCol),
	    (unsigned far *) &action) || action != sizeof (InCol))
		anerror ("Error writing to file", 0);

	else if (DOSWRITE (handle, (char far *) &Generation,
	    sizeof (Generation), (unsigned far *) &action) ||
	    action != sizeof (Generation))
		anerror ("Error writing to file", 0);

	else if (DOSWRITE (handle, InGrid, InRow*InCol/BYTEINCOLS,
	    (unsigned far *) &action) || action != InRow*InCol/BYTEINCOLS)
		anerror ("Error writing to file", 0);

	/* close file */
	if (DOSCLOSE (handle))
		anerror ("Error closing file", 0);
}


/***	up - speeds up GOs (decrement Slow)
 */
void
up() {
	int i;

	if (Slow)
		Slow--;
	if (!Slow) {
		/* if at top speed take out message */
		for(i=0; *(ComLine[i].Name) != 'U'; i++);
		ComLine[i].Fun=0;
		gputs ("        ", COM_ROW, ComLine[i].Col, 0);
	}
	for (i=30000; i--;);		/* short pause */
}


/***	down - slows down GOs (increment Slow)
 */
void
down() {
	int i;

	Slow++;
	for (i=30000; i--;);		/* short pause */
	if (Slow){		    /* if at top speed, display UP */
		for(i=0; *(ComLine[i].Name) != 'U'; i++);
		ComLine[i].Fun=up;
		for (i=0; ComLine[i].Fun != 0; i++) /* print command line */
			gputs (ComLine[i].Name, COM_ROW, ComLine[i].Col, 0);
	}
}


/***	beep - beep the speaker for bad commands
 */
void
beep () {
	DOSBEEP (500, 50);
}


/***	showingrid - display internal grid on screen
 *
 *	Clears the screen using the draw_grid routine and then
 *	puts the internal grid on the screen using fill() so as to
 *	not be resolution dependent.
 *
 *	Entry:	InGrid points to internal grid defined by InCol and InRow.
 *		ScrSeg points to the screen buffer.
 *		ScrRow and ScrCol describe the dimensions of the screen grid.
 *		Logo points to program logo that also clears the prompt line
 *
 *	Exit:	None
 *
 *	Calls:	fill(), putgen(), gputs(), draw_grid [assembler routine]
 */
void
showingrid () {
	int x, y;				/* cell coordinates for loop */
	char retcode;				/* for VIOSCRLOCK */

	/* prepare blank screen so we only have to fill in cells that are on */
	draw_grid ();				/* draw blank grid on screen */
	/* print command line */
	for (x=0; ComLine[x].Fun != 0; x++)
		gputs (ComLine[x].Name, COM_ROW, ComLine[x].Col, 0);
	gputs (Logo, PROMPT_ROW, PROMPT_COL, 0);
	putgen ();

	/* loop through the screen sized area of the grid */
	VIOSCRLOCK (1, (char far *) &retcode, 0);   /* get screen access */
	for (y=ScrRow; y--;)
		for (x=ScrCol; x--;)
		    /* if the internal cell is on, turn on the screen one*/
		    if (*(InGrid + (x + y*InCol)/BYTEINCOLS) & (0x80 >> (x&7)))
			    fill (x, y);
	VIOSCRUNLOCK (0);
}


/***	getfilespec - get a file name from the user
 *
 *	Prompts user for a file name for use with diskread() or diskwrite().
 *	The last file name used is shown on the screen as a default
 *	and the user can hit enter (or left mouse button) to specify the
 *	default name.  If any other key is pressed, this routine takes the
 *	input until an enter and returns in Filespec.  If ESC is hit any time
 *	during typeing, the input will be aborted and the buffer cleared.
 *
 *	Entry:	Filespec points to default file name
 *		Logo points to program logo that also clears the prompt line
 *
 *	Exit:	Filespec points to new file name
 *		returns 0x1b if ESC was hit during entry, else 0
 *
 *	Calls:	gputs(), putgen()
 */
int
getfilespec () {
	int c=0;				/* index for Filespec[] */
	struct KeyData kbd;			/* return for KBD call */
	unsigned space=0x0020;			/* space character for print */
	struct EventInfo event; 		/* return for mouse */
	int type=0;				/* mouse reads w/no wait */

	kbd.scan_code=0;
	event.Mask=0;
	/* put up default file name if one exists */
	if (Filespec[0]) {
	    gputs ("Type file name or enter for default:  ", PROMPT_ROW,
							     PROMPT_COL,0);
	    gputs (Filespec, PROMPT_ROW, FILE_COL, 0);
	    /* wait for first key or button to see if they accept default */
	    while (!kbd.scan_code && !(event.Mask & (2|4|8|16))) {
		    if (Mouse)
			    MOUREADEVENTQUE ((struct EventInfo far *) &event,
					     (unsigned far *) &type, Mouse);
		    KBDPEEK ((struct KeyData far *) &kbd, 0);
	    }
	}
	if (event.Mask & (8 | 16)) {	/* if right button hit, same as ESC */
		kbd.char_code = 0x1b;
		wait4release();
	}
	if (event.Mask & (2 | 4)) {	/* if left button hit, same as \r */
		wait4release();
		gputs (Logo, PROMPT_ROW, PROMPT_COL, 0);
		putgen();
		return (0);
	}
	if (kbd.char_code == 0x1b) {	/* if ESC, return */
		gputs (Logo, PROMPT_ROW, PROMPT_COL, 0);
		putgen();
		return (0x1b);			   /*	return escape */
	}

	gputs(Logo, PROMPT_ROW, PROMPT_COL, 0);
	gputs("Type file name,  followed by enter:  ",PROMPT_ROW,PROMPT_COL,0);

	/* read chars and print them until \r */
	while (!(KBDCHARIN ((struct KeyData far *) &kbd, 0, 0)) &&
	   kbd.char_code != '\r') {
	     if (kbd.char_code == '\b') { /* if backspace, backspace */
		     if (c > 0) {			/* prevent underflow*/
			     Filespec[--c]=0;
			     /* erase backed-out character*/
			     gputs (&space, PROMPT_ROW, FILE_COL+c,0);
		     }
		     else				/* if overflow, yell */
			     beep();
	     }
	     else if (kbd.char_code == 0x1b) {		/* if escape was hit */
		     Filespec[0]=0;			/*   blank name */
		     gputs (Logo, PROMPT_ROW, PROMPT_COL, 0);
		     putgen();
		     return (0x1b);			/*   return escape */
	     }
	     else if (kbd.char_code <46 || kbd.char_code > 122)
		     beep();				/* if not char, beep */
	     else
		     if (c < sizeof (Filespec)) {    /* prevent overflow */
			     Filespec[c]=kbd.char_code;  /* add to string */
			     Filespec[++c]=0;		 /* null terminal */
		     }
		     else				/* if overflow, yell */
			     beep();

	     gputs (Filespec, PROMPT_ROW, FILE_COL, 0);   /* print new name */
	}
	/* return generation count to screen */
	gputs (Logo, PROMPT_ROW, PROMPT_COL, 0);
	putgen();
	return (0);
}


/***	putgen - puts the current generation counter on prompt line
 *
 *	Entry:	Generation = current generation number
 *
 *	Exit:	None
 *
 *	Calls:	gputs()
 */
void
putgen ()
{
	char num[8];				/* number string to print */
	int i, g;

	gputs ("Generation:         ", PROMPT_ROW, PROMPT_COL,0);/* gen label*/

	if (Generation == 0)
		gputs ("0", PROMPT_ROW,GEN_COL,0);/*if special case need this*/
	else {					/* not 0, do this: */
		for (g=Generation, i=sizeof(num)-1; i-- >= 0 && g; g/=10)
			num[i]=(g%10)+'0';      /* convert int to string */
		num[sizeof(num)-1]= 0;	     /* zero terminated for gputs */
		gputs (&num[i+1], PROMPT_ROW, GEN_COL,0); /* print number */
	}
}


/***	fill -	fill in a grid cell on the screen and internally
 *
 *	fill (x, y)
 *
 *	Entry:	x = grid horizontal coordinate
 *		y = grid vertical coordinate
 *		ScrSeg = screen buffer segment
 *		VioScrLock is active
 *		InGrid points to internal grid defined by InCol and InRow
 *		ScrRow and ScrCol define limits of screen grid
 *		Currently, must use 79x25 grid drawn by draw_grid()
 *		Currently, screen must be in 640x200 b/w mode
 *
 *	Exit:	None
 */
void
fill (x, y)
int x,y;
{
	char far *gridptr;

	/* only do if within screen grid limits */
	if (x < ScrCol && y < ScrRow) {
	    /* set gridptr to & in screen buff of cell to fill */
	    (long) gridptr = ScrSeg*0x10000;	  /* fill in address */
	    (unsigned) gridptr = (x*SizeScrCol/BIT)+(y*SizeScrRow/Cga*SCR_WID);
	    /* fill in screen cell */
	    *(gridptr+OddPage)=0x7f;
	    *(gridptr+SCR_WID)=0x7f;
	    *(gridptr+OddPage+SCR_WID)=0x7f;
	}
	/* fill in internal cell*/
	/*InGrid= &internal grid of x,y*/
	gridptr = InGrid + x/BYTEINCOLS + y*InCol/BYTEINCOLS;
	*gridptr = *gridptr | (0x80 >> (x & 7));   /* fill bit */
}


/***	remove - clear a grid cell on the 79x45 grid screen and internally
 *
 *	remove (x, y)
 *
 *	Entry:	x = grid horizontal coordinate
 *		y = grid vertical coordinate
 *		ScrSeg = screen buffer segment
 *		VioScrLock is active
 *		InGrid points to internal grid defined by InCol and InRow
 *		ScrRow and ScrCol define limits of screen grid
 *		Currently, must use 79x25 grid drawn by draw_grid()
 *		Currently, screen must be in 640x200 b/w mode
 *
 *	Exit:	None
 */
void
remove (x, y)
int x,y;
{
	char far *gridptr;

	/* only do if within screen grid limits */
	if (x < ScrCol && y < ScrRow) {
	    /* set gridptr to & in screen buff of cell to fill */
	    (long) gridptr = ScrSeg*0x10000;	  /* fill in address */
	    (unsigned) gridptr = (x*SizeScrCol/BIT)+(y*SizeScrRow/Cga*SCR_WID);
	    /* fill in screen cell */
	    *(gridptr+OddPage)=0x80;
	    *(gridptr+SCR_WID)=0x80;
	    *(gridptr+OddPage+SCR_WID)=0x80;
	}

	/* remove internal cell*/
	/*InGrid= &internal grid of x,y*/
	gridptr = InGrid + x/BYTEINCOLS + y*InCol/BYTEINCOLS;
	*gridptr = *gridptr & ~((0x80 >> (x & 7)));   /* remove bit */
}


/***	kbdreadeventque - simulates MouReadEventQue with the keyboard
 *
 *	F9 key is responded to as left button, F10 key as right.
 *	Pointer location is advanced by SizeScrCol or SizeScrRow according
 *	to direction of direction key hits.
 *	Does not wait for input, instead returns 0 event mask.
 *	Does not check MouDevStatus, instead always returns pixel coordinates
 *	Event buffer is filled according to MouReadEventQue
 *
 *	Entry:	event  - structure to return data
 *		ptrrow - current point row
 *		ptrcol - current pointer column
 *
 *	Exit:	None
 *		*event contains any event that may have occurred
 */
int
kbdreadeventque (event, ptrrow, ptrcol)
struct EventInfo *event;
int ptrrow, ptrcol;
{
	struct KeyData kbd;			/* return for KBD call */

	kbd.char_code=0;			/* clear data struct residue*/
	kbd.scan_code=0;
	/* peek at key buffer */
	KBDPEEK ((struct KeyData far *) &kbd, 0);

	/* if it is an extended key code, process it */
	if (kbd.char_code == 0 && kbd.scan_code) {
		KBDCHARIN ((struct KeyData far *) &kbd, 1, 0); /* take char */
		event->Row=ptrrow;		/* initial position */
		event->Col=ptrcol;
		switch (kbd.scan_code) {	/* check second byte of code */
		    case 0x48:			/* cursor up */
			event->Row -= SizeScrRow;
			event->Mask=1;
			break;
		    case 0x50:			/* cursor down */
			event->Row += SizeScrRow;
			event->Mask=1;
			break;
		    case 0x4b:			/* cursor left */
			event->Col -= SizeScrCol;
			event->Mask=1;
			break;
		    case 0x4d:			/* cursor right */
			event->Col += SizeScrCol;
			event->Mask=1;
			break;
		    case 0x43:			/* left button (F9) */
			event->Mask=4;
			break;
		    case 0x52:			/* left button (INS) */
			event->Mask=4;
			break;
		    case 0x44:			 /* right button (F10) */
			event->Mask=16;
			break;
		    case 0x53:			 /* right button (DEL) */
			event->Mask=16;
			break;
		    default:			/* anything else is no good */
			event->Mask=0;
			break;
		}
	}
	else
		event->Mask=0;			/* if no extended key */
}


/***	xorptr - xors the mouse ptr on the screen
 *
 *	xorptr (ptrrow, ptrcol)
 *
 *	Entry:	ptrrow = pixel row on screen
 *		ptrcol = pixel column on screen
 *		ScrSeg = screen buffer segment
 *
 *	Exit:	None
 */
void
xorptr (ptrrow, ptrcol)
unsigned ptrrow, ptrcol;
{
	char far *gridptr;
	unsigned mask=0;			/* bit mask for pointer shape*/
	int x=8;				/* count for rows of pointer */
	char retcode;				/* return from VioScrLock */

	/* set gridptr to & in screen buff to xor */
	ptrrow++;				/* 1st move ptr to next row*/
	(long) gridptr = ScrSeg*0x10000;	/* fill in segment address */
	(unsigned) gridptr = ptrcol/x+(ptrrow/Cga*SCR_WID);/* fill offset*/

	/* fill in screen pointer */
	VIOSCRLOCK (1, (char far *) &retcode, 0);	/* get screen */
	while (x--) {
		mask = mask >> 1;			/* make mask thicker */
		mask |= 0x8000; 			/*    at bottom */
		if (ptrrow & 1 && Cga == 2)		/* if odd bit plane */
			gridptr += OddPage;
		/* xor mask to scr, being sure to reverse bytes in word */
		*(gridptr+1) ^= (char) (mask >> (ptrcol & 7));
		*(gridptr) ^= (char) ((mask >> (ptrcol & 7)) / 256);
		if (ptrrow & 1 && Cga == 2)		/* if odd bit plane */
			gridptr -= OddPage;		/*    plane, next row*/
		ptrrow++;				/* next row */
		if (!(ptrrow & 1) && Cga == 2)		/*only advance on odd*/
			gridptr += SCR_WID;		/*   row w/o CGA */
	 }
	VIOSCRUNLOCK (0);				/* give back screen */
}


/***	gputs - put a character string on the graphics screen
 *
 *	gputs (string, row, col)
 *
 *	Draws the passed asciiz string on the screen unless the screen
 *	is not available (i.e., if running in background screen group)
 *	in which case this routine does nothing so as to not hold up
 *	the program.
 *
 *	Entry:	string - address of asciiz string to print
 *		row    - alpha row to print at
 *		col    - alpha col to print at
 *		mask   - xor'ed with bytes of character as printed
 *
 *	Exit:	None
 *
 *	Calls:	gputchar()
 *
 *	Warning: Prints graphics characters for ascii control codes (<32).
 *		 Does not wrap-around at line end.
 */
void
gputs (string, row, col, mask)
char *string;
unsigned col, row;
char mask;
{
	static struct VIOFONT fontdata = {14,1,FONT_ROW,8,0L,0};/*for GetFont*/
	char c;
	char retcode;				/* for VIOSCRLOCK */
	char far *scr_addr;			/* address in screen buffer
						   to print at */

	/* try lock without waiting for availability */
	VIOSCRLOCK (0, (char far *) &retcode, 0);	/* get screen access */

	/* do display only if screen available now */
	if (retcode == 0) {
		/* get address of font table if haven't yet */
	       if (!fontdata.font_data)
		       VIOGETFONT ((struct VIOFONT far *) &fontdata, 0);

	       (long) scr_addr = ScrSeg*0x10000;   /* fill in segment address*/
	       (unsigned)scr_addr = col+(row*SCR_WID*(FONT_ROW/Cga));/*offset*/
	       while ((c=*(string++)) != 0)	       /* print each char */
		      gputchar(scr_addr++, fontdata.font_data+c*FONT_ROW,mask);
	       VIOSCRUNLOCK (0);		       /* give up screen */
	}
}


/***	gputchar - put a character on the graphics screen
 *
 *	gputchar (scr_addr, font_addr, mask)
 *
 *	Entry:	scr_addr  - far address in screen buffer to place character
 *			    must be on even Cga bit plane
 *		font_addr - far address in font table bit pattern
 *		mask	  - xor'ed with each byte put on screen
 *		VIOSCRLOCK is active
 *
 *	Exit:	None
 */
void
gputchar(scr_addr, font_addr, mask)
char far *scr_addr;
char far *font_addr;
char mask;
{
	int i=FONT_ROW; 		/* count to draw whole character */

	while (i--) {
		if (i & 1 && Cga == 2)		/* if odd bit plane */
			scr_addr += OddPage-SCR_WID;
		*scr_addr = (*(font_addr++) ^ mask);/* put byte on even plane*/
		if (i & 1 && Cga == 2)		/* if odd bit plane */
			scr_addr -= OddPage;
		scr_addr+=SCR_WID;		/* move down a line */
	}
}


/***	anerror - error handler
 *
 *	anerror (message, fatal);
 *
 *	Prints passed error message and waits for a key hit to allow
 *	user to read it.  If fatal flag is 0, the routine then returns,
 *	otherwise the routine restores the original screen mode, clears
 *	the screen, and exits.
 *
 *	Entry:	message - string to print describing error
 *		fatal - flag, if == 0, this procedure prompts for any key hit
 *			then returns.  If != 0, error is fatal & program exits.
 *		Logo points to program logo that also clears the prompt line
 *
 *	Exit:	None
 *
 *	Calls:	gputs(), putgen()
 */
void
anerror (message, fatal)
char message[];
int fatal;
{
	struct KeyData kbd;			/* return for KBD call */

	gputs (Logo, PROMPT_ROW, PROMPT_COL,0);
	gputs (message, PROMPT_ROW, PROMPT_COL, 0);/* print passed message */

	if (fatal) {
		gputs(":  Press any key to exit",PROMPT_ROW,strlen(message),0);
		gethit ();
		exitlife();
	}
	else  {
		gputs (":  Press any key to continue",PROMPT_ROW,
						      strlen(message), 0);
		gethit ();
		gputs (Logo, PROMPT_ROW, PROMPT_COL, 0);
		putgen();
	}
}


/***	gethit - waits for a key hit or mouse button hit
 *
 *	Entry:	None
 *
 *	Exit:	Returns character hit (forced upper case) or 'Y' for left
 *		mouse button and 'N' for right mouse
 */
char
gethit () {
	struct KeyData kbd;			/* for keyboard */
	struct EventInfo event; 		/* for mouse */
	int type=0;
	char rc;				/* return code */

	/* loop till a hit */
	kbd.scan_code=0;
	event.Mask=0;
	while (!kbd.scan_code && !(event.Mask & (2|4|8|16))) {
		if (Mouse)
			MOUREADEVENTQUE ((struct EventInfo far *) &event,
					 (unsigned far *) &type, Mouse);
		KBDCHARIN ((struct KeyData far *) &kbd, 1, 0);
	}
	if (kbd.scan_code)			/* if key, return char */
		rc=kbd.char_code & 0xdf;
	else if (event.Mask & (8 | 16)) {	/* if right */
		rc='N';
		wait4release ();
	}
	else if (event.Mask & (2 | 4))	{	/* if left down*/
		rc='Y';
		wait4release ();
	}
	return (rc);
}


/***	wait4release - wait until mouse buttons are up before returning
 *
 *	Entry:	Mouse = mouse handle or 0 if no mouse
 */
void
wait4release () {
	struct EventInfo event;
	unsigned type = 0;		/* if OS/2, wait for events */

	event.Mask=2;			 /* force 1st read */
	if (Mouse)			/* loop till no button events */
		while ((event.Mask & (2|4|8|16)) || event.Time==0)
			MOUREADEVENTQUE ((struct EventInfo far *) &event,
					 (unsigned far *) &type, Mouse);
}


/***	exitlife - exit program, resetting original screen mode
 *
 *	Called by a Ctrl-C
 *
 *	Entry:	Savemode contains original screen mode info
 *
 *	Exit:	None
 */
void far pascal
exitlife() {
		VIOSETMODE ((struct ModeData far *) &Savemode, 0); /* restore*/
		VIOSCROLLUP (0,0,-1,-1,-1, (char far *) Cell,0);   /* cls */
		DOSEXIT (1, 0); 	/* exit all threads */
}

