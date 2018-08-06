/* Snap	- An OS/2 screen capture utility
 *
 * Snap	starts a background process containing a keyboard monitor. The
 * monitor checks for a	hot key	(ALT-PRTSCR). If found,	a thread is
 * launched to write the screen	to a file. Various command line	options
 * allow you to	specify	capture	behavior or to deinstall the program.
 *
 * To make a statically	linked version,	use these command lines:
 *
 *  cl /c /AL /G2s /Zp /X /I\include\mt	snap.c
 *  link /NOD /NOI snap,,,llibcmt doscalls;
 *
 * To creat a dynamically linked version, create the dynamic linked
 * library CRTLIB.DLL (as explained in MTDYNA.DOC file)	and place it in
 * your	LIBPATH	directory. Create the imports library CRTLIB.LIB in your
 * LIB directory. Make sure you	have the objec file CRTEXE.OBJ in your
 * LIB directory. Then use these command lines:
 *
 *  cl /c /AL /G2s /Zp /X /I\include\mt	snap.c
 *  link /NOD /NOI snap	\lib\crtexe,,,crtlib doscalls;
 */

/* Function prototypes */
int main (int argc, char * *argv);
int Monitor (void);
void Snap (unsigned long far *arg);
void BackError (char *msgErr);
void Syntax (void);
void OptionEval	(int argc, char	* *argv);

/* Define constants to enable function groups in OS2 include files */
#define	INCL_NOCOMMON
#define	INCL_SUB
#define	INCL_DOSMEMMGR
#define	INCL_DOSMONITORS
#define	INCL_DOSMISC
#define	INCL_DOSSEMAPHORES
#define	INCL_DOSPROCESS
#define	INCL_DOSINFOSEG
#include <os2.h>

#include <malloc.h>
#include <process.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define	FALSE	0
#define	TRUE	1

#define	CON	0		/* Handle for the console device    */
#define	FAIL	-1		/* Fail	to start thread		    */

#define	NOCARE	0		/* DosMonReg constants specifying   */
#define	FRONT	1		/* where to install in the monitor  */
#define	BACK	2		/* chain */

#define	PRTSCR	0x37		/* Scan	code for PtrScr		    */
#define	ALT	0x8		/* Shift mask for either ALT	    */

#define	RELEASE	0x40		/* Bit mask for	key release	    */

#define	BUFSIZE	128		/* Size	for monitor buffers:
				 * 64 minimum, 128 recommended	    */

#define	STACKSIZE 2048		/* 2K minimum for any system call   */

#define	MAXSPEC	 (2 + 66 + 12)	/* Drive - 2, directory	- 66, file name	- 12 */

static	char namShrSeg[] = { "\\SHAREMEM\\SNAP.DAT" };

struct SNAPDATA	{
    int	sound;
    int	append;
    int	count;
    unsigned long wait;
    int	install;
    char name[MAXSPEC];
} far *pdatShrSnap = 0;		/* Initialize offset of	pointer	to 0. Segment
				 * will	be initialized by system call.	     */

#define	SHRSEGSIZE  sizeof(struct SNAPDATA)

main (int argc,	char * *argv)
{
    char namFail[MAXSPEC];	/* Dummies for DosExecPgm  */
    RESULTCODES	resChild;

    char *namSnapExe = 0;	/* Name	of executable file */
    unsigned offCmd;		/* Dummy used by DosGetEnv */

    /* Try to get shared segment (note how selector value is placed
     * directly	in the segment word of the pointer address).
     */
    if (DosGetShrSeg (namShrSeg, (PSEL)&pdatShrSnap + 1)) {

	/* Segment doesn't exist, so try to allocate it. */
	if (DosAllocShrSeg (SHRSEGSIZE,	namShrSeg, (PSEL)&pdatShrSnap +	1)) {

	    puts ("Can't allocate shared memory");
	    exit (1);

	} else {

	    /* This is the first time through, so we must execute
	     * ourself to do installation.
	     *
	     * First set defaults, then	modify for options.
	     */
	    pdatShrSnap->sound = TRUE;
	    pdatShrSnap->append	= TRUE;
	    pdatShrSnap->count = 0;
	    pdatShrSnap->install = FALSE;
	    strcpy (pdatShrSnap->name, "SNAP.IMG");
	    DosSemSet (&pdatShrSnap->wait);
	    OptionEval (argc, argv);

	    /* Get our own path	name from the end of the environment
	     * segment.	This is	the most reliable way to get the full
	     * path name of the	current	file, since the	extension is
	     * ommitted	from argv[0].
	     */
	    DosGetEnv((PUSHORT)&namSnapExe + 1,	&offCmd);
	    while (*namSnapExe++ || *namSnapExe)    /* Adjust forward until  */
		;				    /* we point	to full	path */
	    ++namSnapExe;			    /* name of .EXE file.    */

	    if (DosExecPgm (namFail,
			    MAXSPEC,
			    EXEC_BACKGROUND,
			    NULL,
			    0,
			    &resChild,
			    namSnapExe)) {

		puts ("Can't start background process");
		exit (1);
	    }

	    puts ("Snap installed");
	    Syntax ();

	    /* Wait for	child to report	receiving shared data */
	    DosSemWait (&pdatShrSnap->wait, -1L);
	}

    } else {

	/* Shared segment already existed.
	 *
	 * If installed, just evaluate options.
	 */
	if (pdatShrSnap->install)

	    if (argc ==	1)
		puts ("Snap already installed");
	    else
		OptionEval (argc, argv);

	else {

	    /* Not installed, so install now.
	     *
	     * Tell parent we have received data
	     */
	    DosSemClear	(&pdatShrSnap->wait);

	    /* Flag as installed and start monitor */
	    pdatShrSnap->install = TRUE;
	    return (Monitor());

	}
    }
}

/* Monitor routine checks keystrokes as	they occur and calls
 * the Snap thread if the hot key is pressed.
 *
 * Input: none
 * Return: 1 if	error, 0 if deinstalled
 * Uses: Shared	memory structure - pdatShrSnap
 */

int Monitor ()
{
    /* The first 16 bytes of a monitor buffer are devoted to the
     * header, as shown	in the MONBUFF structure below.	Prior to
     * registering, the	first word (lBuff) must	be set to the size
     * of the buffer.
     */
    struct MONBUFF {		    /* Generic monitor buffer header	   */
	unsigned    lBuff;
	unsigned    lBuffDD;
	unsigned char dispatch[12];
    } *inBuff, *outBuff;

    struct KEYPACKET {		    /* KBD monitor data	record		   */
	unsigned    flgMn;
	KBDKEYINFO  k;
	unsigned    flgDD;
    } keyBuff;

    HMONITOR	hKeybrd;	    /* Keyboard	handle from monitor open   */
    unsigned	cntMonChar;	    /* Chars in	monitor	read/write buffer  */

    PGINFOSEG pGIS = 0,	pLIS = 0;   /* Information segment structures	   */

    char stkSnap[STACKSIZE];	    /* Stack and semaphore for Snap thread */
    ULONG flgSnap = FALSE;

    int	ret;

    /* Allocate	space for monitor read/write buffers. */
    inBuff = (struct MONBUFF *)malloc(BUFSIZE);
    outBuff = (struct MONBUFF *)malloc(BUFSIZE);

    /* Prepare buffer headers for registration.	*/
    inBuff->lBuff = BUFSIZE;
    outBuff->lBuff = BUFSIZE;

    /* Register	monitor	to the keyboard	device (KBD$). */
    if (DosMonOpen ("KBD$", &hKeybrd)) {
	BackError ("Can't open monitor");
	return (1);
    }

    /* Get information segments	(all we	really need is ID of current
     * screen group from Global	Information Segment).
     */
    DosGetInfoSeg((PSEL)&pGIS +1, (PSEL)&pLIS +	1);

    /* Register	the monitor buffers to the current screen group	*/
    if (DosMonReg (hKeybrd,
		   (PBYTE)inBuff,
		   (PBYTE)outBuff,
		   NOCARE,
		   pGIS->sgCurrent)) {
	BackError ("Can't register monitor");
	return (1);
    }

    /* Make process time critical so keys are interpreted without delay. */
    DosSetPrty (PRTYS_PROCESS, PRTYC_TIMECRITICAL, 0, 0);

    /* Monitor loop - read into	monitor	buffer and examine. Take action
     * if hot key, otherwise pass on to	device driver.
     */
    while (pdatShrSnap->install) {
	cntMonChar = sizeof(keyBuff);
	DosMonRead ((PBYTE)inBuff, IO_WAIT, (PBYTE)&keyBuff, &cntMonChar);
	if ((keyBuff.k.chScan == PRTSCR) &&
	    (keyBuff.k.fsState & ALT) &&
	    (!(keyBuff.flgDD & RELEASE))) {

	    /* Make sure last thread is	finished */
	    DosSemWait (&flgSnap, -1L);
	    ret	= _beginthread (Snap,
				(void far *)stkSnap,
				STACKSIZE,
				(void far *)&flgSnap);
	    if (ret == FAIL)
		BackError ("Can't start screen capture thread");
	    else
		DosSemSet (&flgSnap);
	} else

	    /* Pass the	key through if it is not the hot key */
	    DosMonWrite	((PBYTE)outBuff, (PBYTE)&keyBuff, cntMonChar);
    }

    /* Close monitor */
    DosMonClose	(hKeybrd);
    return (0);
}

/* Screen capture routine (run as a thread). Does a pop-up to get access
 * to the current screen. Reads	characters from	the screen into	a buffer.
 * Then	filters	trailing spaces	as it writes buffer to a file.
 *
 * Input: pointer to flag indicated snap status	- pflgSnap
 * Return: none
 * Uses: Shared	memory structure - pdatShrSnap
 */

void Snap (unsigned long far *pflgSnap)
{
    VIOMODEINFO	modScreen;	/* Variables for VioGetMode and	VioPopUp */
    unsigned flgPopUp =	VP_WAIT	| VP_TRANSPARENT;

    char    *bufScreen;		/* Buffer for captured screen		 */
    unsigned lbufScreen;

    FILE    *sFile;		/* File	stream				 */

    int	    line, pos, width;
    char    num[5];

    if (pdatShrSnap->sound)
	DosBeep	(698, 500);

    /* Pop up to current screen	*/
    VioPopUp (&flgPopUp, CON);

    /* Check screen */
    modScreen.cb = sizeof(modScreen);
    VioGetMode (&modScreen, CON);

    /* Allocate	memory for a full screen plus one byte */
    lbufScreen = (modScreen.col	* modScreen.row);
    bufScreen =	malloc (lbufScreen + 1);

    /* Read screen and end popup */
    VioReadCharStr (bufScreen, &lbufScreen, 0, 0, CON);
    VioEndPopUp	(0);

    /* Increment screen	count (four digits or less) and	convert	to string */
    pdatShrSnap->count = (pdatShrSnap->count + 1) % 9999;
    itoa (pdatShrSnap->count, num, 10);

    if (!pdatShrSnap->append)
	/* Make	numbered file name */
	strcat (strcat (strcpy (pdatShrSnap->name, "SNAP"), num), ".IMG");

    /* Open file and write buffer to it	a line at a time */
    if ((sFile = fopen (pdatShrSnap->name, "at")) == NULL ) {

	BackError ("Can't open file");
	--pdatShrSnap->count;

    } else {

	if (pdatShrSnap->append) {
	    /* Not using fprintf saves a few K */
	    fputs ("**** Screen ", sFile);
	    fputs (num,	sFile);
	    fputs (" ****\n", sFile);
	}

	for (line = 0, pos = 0;	line < modScreen.row; line++) {

	    /* Throw away trailing spaces */
	    for	(width = modScreen.col;
		 (bufScreen[pos	+ width	- 1] == ' ') &&	width;
		 width--)
		 ;
	    /* Write line and newline */
	    fwrite (bufScreen +	pos, 1,	width, sFile);
	    fputc ('\n', sFile);
	    pos	+= modScreen.col;
	}

	fclose (sFile);
    }
    if (pdatShrSnap->sound)
	DosBeep	(523, 500);

    /* Free memory and let parent know we are done */
    free (bufScreen);
    DosSemClear	(pflgSnap);
}

/* Displays an error message from within a background process or thread.
 * The monitor is in the background and	has no screen group, so	it must
 * use VioPopUp	to get access to the screen.
 *
 * Input: error	message	string - msgErr
 * Return: none
 */
void BackError (char *msgErr)
{
    unsigned flgPopUp =	VP_WAIT	| VP_TRANSPARENT;

    VioPopUp (&flgPopUp, CON);
    puts (msgErr);
    puts ("Press any key to continue . . .");
    while (!kbhit ())
	;
    VioEndPopUp	(CON);
}

/* Displays syntax.
 *
 * Input: none
 * Return: none
 */
void Syntax ()
{
    puts ("\nOptions: ");
    puts ("\t/H\t  Display help.");
    puts ("\t/S\t  Turn sound on (default).");
    puts ("\t/Q\t  Turn sound off.");
    puts ("\t/D\t  Deinstall.");
    puts ("\t/A [path] Append each screen to a file (complete path allowed).");
    puts ("\t\t  If no file given, default is SNAP.IMG in current directory");
    puts ("\t\t  Resets screen number to 1.");
    puts ("\t/N [num]  Create numbered file for each screen.");
    puts ("\t\t  Example: SNAP1.IMG, SNAP2.IMG in current directory.");
    puts ("\t\t  Resets screen number to 1 or to num if given.");
}

/* Evaluated command-line options.
 *
 * Input: Number of arguments -	argv
 *	  Pointer to argument list - argv
 * Return: none
 * Uses: Shared	memory structure - pdatShrSnap
 */
void OptionEval	(int argc, char	* *argv)
{
    int	i;

    /* Look for	and handle arguments */
    for	(i = 1;	i < argc; i++) {
	if (argv[i][0] == '/' || argv[i][0] == '-') {
	    switch (toupper (argv[i][1])) {
		case 'A' :
		    pdatShrSnap->append	= TRUE;
		    pdatShrSnap->count = 0;
		    if ((argv[++i]) && (argv[i][0] != '/') &&
				       (argv[i][0] != '-')) {
			strcpy (pdatShrSnap->name, argv[i]);
			puts ("Append mode - name set");
		    } else
			puts ("Append mode");
		    break;
		case 'N' :
		    pdatShrSnap->append	= FALSE;
		    puts ("Numbered file mode");
		    if ((argv[++i]) && (argv[i][0] != '/') &&
				       (argv[i][0] != '-'))
			pdatShrSnap->count = ((atoi (argv[i])) % 9999) - 1;
		    else
			pdatShrSnap->count = 0;
		    break;
		case 'Q' :
		    pdatShrSnap->sound = FALSE;
		    puts ("Sound off");
		    break;
		case 'S' :
		    pdatShrSnap->sound = TRUE;
		    puts ("Sound on");
		    break;
		case 'D' :
		    if (pdatShrSnap->install) {
			pdatShrSnap->install = FALSE;
			puts ("Deinstalling");
		    } else
			exit (0);
		    break;
		case 'H' :
		    if (pdatShrSnap->install)
			Syntax ();
		    break;
	    }
	}
    }
}
