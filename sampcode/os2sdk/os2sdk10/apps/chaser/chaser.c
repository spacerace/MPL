/*  SWARM - the idea behind this game is as follows:
 *
 *   You have a collection of objects in the center of the playing field
 *   that you are trying to protect (just one object in current version). You
 *   control your own movements with the mouse. A number of "chasers" start
 *   around the edges of the field and begin moving towards the objects
 *   you want to protect. If you move the mouse on top of a chaser and click
 *   the left button, the chaser will be killed and disappear from the screen.
 *   But as you close in on the chaser, it will detect your presence and try
 *   to dodge you. Meanwhile the other chasers will continue to go after
 *   your objects. If one of the chasers reaches an object, it will begin
 *   dragging it away to the edge of the screen (currently the game just
 *   ends when the single object is reached). When all objects are dragged
 *   away, the game ends. If a chaser is killed while dragging an object, the
 *   object is left where it is and must be protected in place - player cannot
 *   move objects. If you kill all the chasers, a new group of faster ones
 *   will be spawned (currently the speed is constant). Your score is how
 *   many chasers you can kill (no score currently kept), so there is no
 *   advantage in sitting on the object for long periods.
 *
 * Swarm demonstrates several capabilities of OS/2 and the philosphy behind
 * them.  This program is made of three components: Initialization, the
 * mouse driven thread and the attacker thread.  The attacker thread is
 * launched as many times as there are attackers in a game.  Launching
 * the attacker several times takes full advantage of the OS to schedule
 * resources.  The programmer can think of the problem as only one attacker.
 * The system handles multiple instances of the thread.
 *
 * As the main loop launched threads it puts an ID code into the threads
 * stack.  The code is used to index into the universe data.
 *
 * A ram semaphore is used to control access to global data.
 *
 * This demonstration shows the use of the following OS/2 system calls:
 *
 * Tasking:	       VIO API: 	     Mouse API:
 *
 *   DOSSEMREQUEST()	 VIOSCROLLUP()	       MOUOPEN()
 *   DOSSEMCLEAR()	 VIOWRTCELLSTR()       MOUSETPTRPOS()
 *   DOSCREATETHREAD()	 VIOSETCURTYPE()       MOUREADEVENTQUE()
 *   DOSEXIT()		 VIOSETMODE()
 *   DOSSLEEP()
 */
#include <malloc.h>
#include <stdio.h>
#include <doscalls.h>
#include <subcalls.h>

#define  STACKSIZE  200

#define  DANGERZONE  3

#define  LONGNAP     500L
#define  SHORTNAP    150L

#define  WAIT (-1L)			/* Wait for ram Semaphore */

#define  CHASER    8			/* Number of chasers */

#define  SCREEN_HEIGHT	   24		/* Default screen size */
#define  SCREEN_WIDTH	   79

#define  GOAL univ[CHASER]		/* Macros for constant stuff */
#define  ME univ[ID]
#define  MOUSE univ[CHASER+1]

#define  ALIVE 1			/* Flags for attackers/goal */
#define  DEAD 0

char   Chaser[2] = { 0xE8, 0x20 };   /* character and attribute */
char	Prize[2] = { 0x03, 0x2C };   /* for our various objects */
char	Blank[2] = { 0x20, 0x22 };
char	Blood[2] = { 0x20, 0x44 };

struct {			      /* Universe structure and array */
    int     row;			/* univ[0] = chaser     */
    int     col;			/* univ[n-1] = chaser     */
    int     state;			/* univ[n] = GOAL */
} univ[CHASER+1];			/* univ[n+1]= MOUSE */

short		ScreenHeight,		/* Screen attributes */
		ScreenWidth;

unsigned short	Mouse;			/* place for mouse handle */
unsigned long	Shortnap;		/* Sleep times for chasers */
unsigned long	Longnap;
unsigned long far Semaphore = 0;	/* Ram semaphore */

struct CursorData	NewCur;		/* struct for setting cursor type */
struct CursorData	OldCur;

struct ModeData 	modedata;	/* Data saves for VIO mode */
struct ModeData 	OldVioMode;

/*
 * Define all procedures before main.
 */
void Defender();
void CleanUp();
int InitGame();
void chaserthread();
int ParseCmdLine(int,char **);

/*
 * main(ac,av)
 *
 * Top level procedure and MOUSE thread for the GAME demo.
 */
int main(ac, av)
int ac;
char *av[];
{
    /*
     * Parse the command line and perform some initialization.
     */
    if (ParseCmdLine(ac,av)) {
	printf("usage: %s [24|43] [F|M|S]\n",av[0]);
	DOSEXIT(0,1);
    }
    if (InitGame())		/* Init game, exit if some problem */
	DOSEXIT(1,1);

    Defender();			/* Run mouse loop (defend against the swarm */

    CleanUp();
}

/*
 * Defender()
 *
 * This is the main loop of the mouse control thread.
 *
 * The semaphore is used to prevent the other threads from time slicing 
 * while this routine is examining and/or modifying the universe.  The 
 * Semaphore is grabbed after the read of the Mouse queue so we don't tie
 * up the attackers while waiting for a mouse event.
 */
void Defender()
{
    unsigned ReadType = 1,	  /* Wait for mouse events */
	     alive,
	     i;
    struct EventInfo  MouInfo;	  /* mouse event packet structure */

    alive = CHASER;

    do {
	MOUREADEVENTQUE( (struct EventInfo far *)&MouInfo,
			 (unsigned far *)&ReadType,
			 Mouse );	/* read where mouse is */

	DOSSEMREQUEST((unsigned long)&Semaphore, WAIT);

	if( MouInfo.Mask & 1) { 		/* If the mouse has moved */
	    MOUSE.row = MouInfo.Row;
	    MOUSE.col = MouInfo.Col;
	}
	if( MouInfo.Mask & 4 ) {		 /* if left button pressed, */
	    for (i = 0; i < CHASER; i++ ) {
		if( ( MOUSE.row == univ[i].row ) &&
		    ( MOUSE.col == univ[i].col ) &&  /* see if we hit one */
		    ( univ[i].state == ALIVE) ) {
		     univ[i].state = DEAD;	

		     DOSBEEP(300,75);		     /* make a dying sound */
		     DOSBEEP(600,75);
		     DOSBEEP(300,85);

		     alive--;		    /* Decrease number of alive */
		     break;		    /* Can only kill one at a time */
		}
	    }
	}
	if( MouInfo.Mask & 16 )		/* If right button pressed... */
	    break;			/* End game, clean up */

	DOSSEMCLEAR((unsigned long)&Semaphore);
    }
    while (GOAL.state == ALIVE && alive);    /* loop till all are dead */
}

/*
 * This thread manages the individule attackers.  It is spun off as
 * many times as needed for a game.
 *
 * The interaction of the mouse cursor and the chaser character is sort
 * of funny, hence the funny code, below.  The mouse cursor seems to
 * remember what was under it when it was written.  Hence we cannot erase
 * the chaser if the mouse is "sitting" on it.	If we do, then when the
 * mouse moves it will re-write the original object.  This shows up as
 * phantom chasers.
 */
void far chasethread(ID)	       /* code that controls each "chaser" */
int ID;
{
    short  row, col;	       /* Our current position */
    short  deltaX, deltaY;     /* how far from the mouse are we? */
    short  danger;	       /* flag to indicate not far enough! */
    short  m;		       /* general purpose indexes */


    /* Print out the initial chaser character */

    VIOWRTCELLSTR( (char far *)Chaser, 2, ME.row, ME.col, 0 );

    /*
     * Keep running as long as the goal and myself haven't been killed.
     */
    for (;;) {

	row = ME.row;		  /* Grab the current position */
	col = ME.col;
	/*
	 * If mouse is sitting upon the chaser, do nothing.  Allow
	 * the player some time to kill the chaser
	 */
	if ((MOUSE.row == row) && (MOUSE.col == col)) {
	    DOSSLEEP( 1L );
	    continue;
	}
	DOSSEMREQUEST((unsigned long)&Semaphore, WAIT);
	/*
	 * If either the GOAL or Myself is dead, exit loop and clean up.
	 * This wasn't tested in the for loop since we don't want to exit
	 * if the MOUSE is sitting on the chaser.
	 */
	if (ME.state != ALIVE || GOAL.state != ALIVE)
	    break;

	deltaX = MOUSE.col - col;	/* calculate how far we are */
	deltaY = MOUSE.row - row;

	if (((deltaX < -DANGERZONE) || (DANGERZONE < deltaX)) ||
	    ((deltaY < -DANGERZONE) || (DANGERZONE < deltaY))) {

	    danger = 0;

	    if(GOAL.row < row)		    /* Creep towards the GOAL */
		row--;
	    else if (GOAL.row > row)
		row++;
	    if(GOAL.col < col)
		col--;
	    else if(GOAL.col > col)
		col++;
	}
	else {
	    danger = 1; 		    /* Run away from the mouse */

	    if ((MOUSE.row > row) && (row > 0))
		row--;
	    else if ((MOUSE.row < row) && (row < ScreenHeight))
		row++;
	    if ((MOUSE.col > col) && (col < ScreenWidth))
		col--;
	    else if ((MOUSE.col < col) && (col > 0))
		col++;
	}
	/*
	 * A quick and Dirty hack to prevent chasers from merging
	 */
	for (m = 0; m < CHASER; m++ ) {
	    if (univ[m].state == ALIVE &&
		univ[m].row == row &&
		univ[m].col == col &&
		m != ID) {
	       row += 1;
	       col += 3;
	    }
	}
	/*
	 * Zap the old chaser and print the new.  Release the semaphore
	 * after this, there can be no undesirable interactions now.
	 */
	VIOWRTCELLSTR( (char far *)Blank, 2, ME.row, ME.col, 0 );
	VIOWRTCELLSTR( (char far *)Chaser, 2, row, col, 0 );

	DOSSEMCLEAR((unsigned long)&Semaphore);
	/*
	 * Update the current location
	 */
	ME.row = row;
	ME.col = col;
	/*
	 * See if we have reached the GOAL, if so eat it and exit
	 */
	if ((row == GOAL.row) && (col == GOAL.col)) {
	    VIOWRTCELLSTR( (char far *)Blank, 2, row, col, 0 );
	    DOSBEEP(600,175);
	    DOSBEEP(1200,175);	    /* if we reach the prize, let out a yell */
	    DOSBEEP(600,185);	    /* paint the screen red and end the game */
	    DOSBEEP(1200,175);
	    VIOSCROLLUP( 0, 0, -1, -1, -1, (char far *)Blood, 0 );
	    GOAL.state = DEAD;
	}
	/*
	 * Sleep an amount of time that varies depending
	 * upon the danger level
	 */
	if( danger )
	    DOSSLEEP(Shortnap);
	else
	    DOSSLEEP(Longnap);

    }
    /*
     * chaser is now dead or the game is over.
     * Erase its body and terminate the thread.  Release the semaphore.
     */
    DOSSEMCLEAR((unsigned long)&Semaphore);

    if (GOAL.state == ALIVE) {
	VIOWRTCELLSTR( (char far *)Blank, 2, ME.row, ME.col, 0 );
    }
    DOSEXIT(0,0);
}

/*
 * InitGame()
 *
 * Initialize the GOAL, MOUSE and the CHASERS, launch each chase thread.
 *
 * Returns an error if any internal processing errors
 */
int InitGame()
{
    struct PtrLoc InitMouPos;
    void far chasethread();		/* code to control chasers */
    unsigned far *Tstack;		/* stack for new threads */
    unsigned chaseID;
    int i, rc;
    /*
     * Clear the screen.
     */
    VIOSCROLLUP( 0, 0, -1, -1, -1, (char far *)Blank, 0 );
    /*
     * Draw the prize
     */
    GOAL.row = ScreenHeight/2;
    GOAL.col = ScreenWidth /2;
    GOAL.state = ALIVE;
    VIOWRTCELLSTR((char far *)Prize, 2, GOAL.row, GOAL.col, 0 );
    /*
     * Open the mouse pointer device and set it's location.
     */
    MOUOPEN( 0L, (unsigned far *)&Mouse );
    InitMouPos.RowPos = GOAL.row;
    InitMouPos.ColPos = GOAL.col;
    MOUSETPTRPOS((struct PtrLoc far *)&InitMouPos, Mouse);
    MOUDRAWPTR(Mouse);
    /*
     * A simple minded initialization for the start of each chaser.
     * Some sort of random placement (based upon system time?) would
     * be nice.
     */
    univ[0].row = 0;  univ[0].col = 0;
    univ[1].row = 0;  univ[1].col = 25;
    univ[2].row = 0;  univ[2].col = 55;
    univ[3].row = 0;  univ[3].col = 79;
    univ[4].row = ScreenHeight;  univ[4].col = 0;
    univ[5].row = ScreenHeight;  univ[5].col = 25;
    univ[6].row = ScreenHeight;  univ[6].col = 55;
    univ[7].row = ScreenHeight;  univ[7].col = 79;
    /* 
     * Grab the semaphore to prevent chaser from running until we are done.
     */
    DOSSEMREQUEST((unsigned long)&Semaphore, WAIT);

    for( i = 0; i < CHASER; i++ ) {		/* for each of our threads... */
	univ[i].state = ALIVE;			/* Set each one alive */
	Tstack = (int *)malloc(sizeof(int) * STACKSIZE);
	if (Tstack == NULL ) {			/* Create a stack */
	    printf( "thread %d stack malloc failed\n", i );
	    return(1);
	}
	Tstack += STACKSIZE;	/* set stack pointer to correct end */
	*--Tstack = i;		/* Push the ID on as a parameter */

	rc = DOSCREATETHREAD(chasethread, (unsigned far *)&chaseID,
	     (char far *)Tstack);
	if(rc) {
	    printf( "create of thread %d failed, error: %d\n", i, rc );
	    return (1);
	}
    }
    DOSSEMCLEAR((unsigned long)&Semaphore);

    return (0);
}

/*
 * CleanUp()
 *
 * Routine to reset the Video modes back to where they were.
 * (As best as possible).
 */
void CleanUp()
{
    char blank[2];

    DOSSLEEP(1L);	/* Yeild the machine so attacker can clean up */
    VIOSETMODE((struct ModeData far *)&OldVioMode, 0);
/*
    blank[0] = ' ';
    blank[1] = OldVioMode.color;
    VIOSCROLLUP( 0, 0, -1, -1, -1, (char far *)blank, 0 );
*/
    VIOSETCURTYPE(&OldCur, 0);
    DOSEXIT(1,0);		/* Exit and terminate all threads. */
}

/*
 * ParseCmdLine(ac, av)
 *
 * Parses the command line arguments and sets up the game accordingly
 *
 */
int ParseCmdLine(ac,av)
int ac;
char **av;
{
    struct ModeData modedata;
    int    VioMode;

    Longnap = LONGNAP;
    Shortnap = SHORTNAP;
    ScreenWidth = SCREEN_WIDTH;
    ScreenHeight = SCREEN_HEIGHT;
    VioMode = 25;

    while(--ac) {
	av++;
	switch(**av) {
	    case 'f':
	    case 'F':
		Longnap = LONGNAP / 2;
		Shortnap= SHORTNAP/ 2;
		break;
	    case 'm':
	    case 'M':
		Longnap = LONGNAP;
		Shortnap= SHORTNAP;
		break;
	    case 's':
	    case 'S':
		Longnap = LONGNAP * 2;
		Shortnap= SHORTNAP* 2;
		break;
	    case '4':	    /* Assume 43 line mode was wanted */
		ScreenHeight = 42;
		ScreenWidth  = 79;
		VioMode = 43;
		break;
	    case '2':
		ScreenHeight = 24;
		ScreenWidth  = 79;
		VioMode = 25;
		break;
	    default:
		return(1);
	}
    }

    VIOGETCURTYPE(&OldCur, 0);		/* Save old cursor */

    modedata.length = sizeof(modedata); /* change mode as needed */
    VIOGETMODE((struct ModeData far *)&modedata, 0);
    OldVioMode = modedata;
    modedata.row = VioMode;
    VIOSETMODE((struct ModeData far *)&modedata, 0);

    NewCur.cur_start	 = 0;
    NewCur.cur_end	 = 0;
    NewCur.cur_width	 = 1;
    NewCur.cur_attribute = -1;

    VIOSETCURTYPE(&NewCur, 0 ); 	/* make cursor go away */

    return (0);
}
