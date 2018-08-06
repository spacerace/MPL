/* Snap - An OS/2 screen capture utility
 *
 * Snap starts a background process containing a keyboard monitor.
 * The monitor checks for a hot key (ALT-*). If found, a thread is
 * launched to write the screen to a file. Various command line options
 * allow you to specify capture behavior or to deinstall the program.
 *
 * To compile, use the following command line:
 *
 *   cl /MT /G2s snap.c
 */

/* Function prototypes */
int  Monitor( void );
void Snap( unsigned long _far *arg );
void BackError( char *msgErr );
void Syntax( void );
void EvalOptions( int argc, char **argv );

/* Define constants to enable function groups in OS2 include files */
#define INCL_NOCOMMON
#define INCL_NOPM
#define INCL_KBD            // KBDKEYINFO
#define INCL_VIO            // Vio functions
#define INCL_DOSMEMMGR      // DosGetShrSeg, DosAllocShrSeg,
#define INCL_DOSMONITORS    // DosMon functions
#define INCL_DOSMISC        // DosGetEnv
#define INCL_DOSSEMAPHORES  // DosSem functions
#define INCL_DOSPROCESS     // DosBeep, DosSetPrty
#define INCL_DOSINFOSEG     // DosGetInfoSeg
#include <os2.h>

#include <malloc.h>         // malloc, free
#include <process.h>        // _beginthread, _endthread, exit, spawnl
#include <string.h>         // strcpy, strcat
#include <stdlib.h>         // atoi, itoa, _MAX_PATH
#include <stddef.h>         // _threadid variable
#include <stdio.h>          // puts, fopen, fwrite, etc.
#include <conio.h>          // kbhit

#define CON  0              // Handle for the console device
#define FAIL -1             // Fail to start thread

#define STAR    0x37        // Scan code for * on numeric keypad
#define RELEASE 0x40        // Bit mask for key release

/* Name and structure for shared memory data */
char szShrSeg[] = { "\\SHAREMEM\\SNAP.DAT" };
struct SHARED
{
    BOOL  fSound;               // Sound flag
    BOOL  fAppend;              // Append flag
    BOOL  fInstall;             // Install flag
    SHORT cScreen;              // Count of screens
    LONG  lfWait;               // Wait semaphore
    CHAR  achSnap[_MAX_PATH];   // Snap file name
} _far *pshrSnap = 0;           // Initialize offset to 0. Segment will
                                // be initialized by system call.

/* Count in bytes of shared segment */
#define C_SHARESEG sizeof( struct SHARED )

void main( int argc, char **argv )
{
    USHORT offCmd;              // Dummy for DosGetEnv
    CHAR   *pchSnapExe = 0;     // Pointer to name of executable file
                                //   (offset initialized to 0)

    /* Try to get shared segment (note how selector value is placed
     * directly in the segment word of the pointer address). There are
     * three possibilities:
     *
     *   - We can't get memory. This means SNAP is not installed,
     *     so we must allocate the memory and exec ourself in the
     *     background to install the monitor.
     *   - We can get memory and we are not installed. This means
     *     we have been execed by previous process to install monitor.
     *   - We can get memory and we are already installed. This means
     *     we were just called to modify options.
     */
    if( DosGetShrSeg( szShrSeg, (PSEL)&pshrSnap + 1 ) )
    {
        /* Segment doesn't exist, so try to allocate it. */
        if( DosAllocShrSeg( C_SHARESEG, szShrSeg, (PSEL)&pshrSnap + 1 ) )
        {
            puts( "Can't allocate shared memory" );
            exit( 1 );
        }
        else
        {
            /* This is the first time through, so we must execute
             * ourself to do installation. First set defaults, then
             * modify for options.
             */
            pshrSnap->fSound = TRUE;
            pshrSnap->fAppend = TRUE;
            pshrSnap->cScreen = 0;
            pshrSnap->fInstall = FALSE;
            strcpy( pshrSnap->achSnap, "SNAP.IMG" );
            DosSemSet( &pshrSnap->lfWait );
            EvalOptions( argc, argv );

            /* Get our own path name from the end of the environment
             * segment. This is the most reliable way to get the full
             * path name of the current file, since the extension is
             * ommitted from argv[0].
             */
            DosGetEnv( (PUSHORT)&pchSnapExe + 1, &offCmd );

            /* Adjust forward until we point to full path of .EXE file. */
            while( *pchSnapExe++ || *pchSnapExe )
                ;
            ++pchSnapExe;

            /* Spawn ourself as a background process. Can't install
             * monitor now because we are in foreground. A background
             * process needs to install the monitor.
             */
            if( spawnl( P_DETACH, pchSnapExe, pchSnapExe, NULL ) == -1 )
            {
                puts( "Can't start background process" );
                exit( 1 );
            }
            puts( "Snap installed" );
            Syntax();

            /* Wait for background child process to report receiving
             * shared data.
             */
            DosSemWait( &pshrSnap->lfWait, SEM_INDEFINITE_WAIT );
        }
    }
    else
    {
        /* Already installed. We are being run to evaluate options and
         * modify behavior accordingly.
         */
        if( pshrSnap->fInstall )
            if( argc == 1 )
                puts( "Snap already installed" );
            else
                EvalOptions( argc, argv );
        else
        {
            /* Not installed, so we were execed by original SNAP to
             * install monitor. Tell parent we have received data, set
             * install flag, and install monitor.
             */
            DosSemClear( &pshrSnap->lfWait );

            /* Set installed flag and start monitor. */
            pshrSnap->fInstall = TRUE;

            exit( Monitor() );
        }
    }
}

/* Monitor routine checks keystrokes as they occur and calls
 * the Snap thread if the hot key is pressed.
 *
 * Params: None
 *
 * Return: 1 if error, 0 if deinstalled
 *
 * Uses:   pshrSnap - Shared memory structure
 */
int Monitor()
{
    #define BUFSIZE 128             // Size for monitor buffers:
                                    //   64 minimum, 128 recommended
    #define STACKSIZE 2048          // 2K minimum for any system call

    PMONIN pmnin;
    PMONOUT pmnout;

    struct KEYPACKET                // KBD monitor data record
    {
        USHORT fMon;
        KBDKEYINFO kki;
        USHORT fDD;
    } keyBuff;
    USHORT ckeyBuff = sizeof( keyBuff );

    HMONITOR hKeyMon;               // Keyboard handle from monitor open
    PGINFOSEG pGIS = 0, pLIS = 0;   // Information segment structures
    LONG  lfSnap = FALSE;           // Semaphore for each Snap thread

    /* Allocate space for monitor read/write buffers and mark size. */
    pmnin = (PMONIN)malloc( BUFSIZE );
    pmnin->cb = BUFSIZE;
    pmnout = (PMONOUT)malloc( BUFSIZE );
    pmnout->cb = BUFSIZE;

    /* Register monitor to the keyboard device (KBD$). */
    if( DosMonOpen( "KBD$", &hKeyMon ) )
    {
        BackError( "Can't open monitor" );
        return 1;
    }

    /* Get information segments (all we really need is ID of current
     * screen group from Global Information Segment).
     */
    DosGetInfoSeg( (PSEL)&pGIS + 1, (PSEL)&pLIS + 1 );

    /* Register the monitor buffers to the current screen group */
    if( DosMonReg( hKeyMon, (PBYTE)pmnin, (PBYTE)pmnout,
                   MONITOR_DEFAULT, pGIS->sgCurrent ) )
    {
        BackError( "Can't register monitor" );
        return 1;
    }

    /* Make process time critical so keys are interpreted without delay. */
    DosSetPrty( PRTYS_PROCESS, PRTYC_TIMECRITICAL, 0, 0 );

    /* Monitor loop - read into monitor buffer and examine. Take action
     * if hot key, otherwise pass on to device driver.
     */
    while( pshrSnap->fInstall )
    {
        DosMonRead( (PBYTE)pmnin, IO_WAIT, (PBYTE)&keyBuff, &ckeyBuff );

        /* Snap if ALT+STAR is down. */
        if( ((keyBuff.kki.chScan == STAR) || (keyBuff.kki.chScan == 0x2a)) &&
            (keyBuff.kki.fsState & ALT) &&
            (!(keyBuff.fDD & RELEASE)) )
        {
            /* Make sure last thread is finished */
            DosSemWait( &lfSnap, SEM_INDEFINITE_WAIT );
            if( (_beginthread( Snap, NULL, STACKSIZE,
                               (PVOID)&lfSnap )) == FAIL )
                BackError( "Can't start screen capture thread" );
            else
                DosSemSet( &lfSnap );
        }
        else
            /* Pass the key through if it is not the hot key */
            DosMonWrite( (PBYTE)pmnout, (PBYTE)&keyBuff, ckeyBuff );
    }

    /* Close monitor */
    free( pmnin );
    free( pmnout );
    DosMonClose( hKeyMon );
    return 0;
}

/* Screen capture routine (run as a thread). Does a pop-up to get access
 * to the current screen. Reads characters from the screen into a buffer.
 * Then filters trailing spaces as it writes buffer to a file.
 *
 * Params: plfSnap - pointer to flag indicated snap status
 *
 * Return: none
 *
 * Uses:   pshrSnap - Shared memory structure
 */
void Snap( ULONG _far *plfSnap )
{
    enum { NOTE_B = 494, NOTE_C = 523, NOTE_F = 698 };
    CHAR   *pchScreen;              // Buffer for captured screen
    USHORT cbScreen;                // Count of bytes in buffer
    FILE   *sFile;                  // File stream
    USHORT usLine, usPos, usWidth;
    CHAR   ach[5];
    USHORT fWait = VP_WAIT | VP_TRANSPARENT;
    VIOMODEINFO vmi = { sizeof( vmi ) };

    if( pshrSnap->fSound )
        DosBeep( NOTE_F, NOTE_C );

    /* Pop up to current screen and check its size. */
    VioPopUp( &fWait, CON );
    VioGetMode( &vmi, CON );

    /* Allocate memory for a full screen plus one byte */
    cbScreen = vmi.col * vmi.row;
    pchScreen = malloc( cbScreen + 1 );

    /* Read screen and end popup */
    VioReadCharStr( pchScreen, &cbScreen, 0, 0, CON );
    VioEndPopUp( 0 );

    /* Increment screen count (4 digits or less) and convert to string. */
    pshrSnap->cScreen = (pshrSnap->cScreen + 1) % 9999;
    itoa( pshrSnap->cScreen, ach, 10 );

    /* Make numbered file name if appropriate. */
    if( !pshrSnap->fAppend )
        strcat( strcat( strcpy( pshrSnap->achSnap, "SNAP" ), ach ), ".IMG" );

    /* Open file and write buffer to it a line at a time */
    if( (sFile = fopen( pshrSnap->achSnap, "at" )) == NULL )
    {
        BackError( "Can't open file" );
        --pshrSnap->cScreen;
    }
    else
    {
        if( pshrSnap->fAppend )
        {
            /* Not using fprintf reduces overhead. */
            fputs( "**** Screen ", sFile );
            fputs( ach, sFile );
            fputs( " ****\n", sFile );
        }

        for( usLine = 0, usPos = 0; usLine < vmi.row; usLine++ )
        {
            /* Throw away trailing spaces */
            for( usWidth = vmi.col;
                 (pchScreen[usPos + usWidth - 1] == ' ' ) && usWidth;
                 usWidth-- )
                ;
            /* Write line and newline */
            fwrite( pchScreen + usPos, 1, usWidth, sFile );
            fputc( '\n', sFile );
            usPos += vmi.col;
        }
        fclose( sFile );
    }
    if( pshrSnap->fSound )
        DosBeep( NOTE_C, NOTE_B );

    /* Free memory and let parent know we are done */
    free( pchScreen );
    DosSemClear( plfSnap );
}

/* Displays an error message from within a background process or thread.
 * The monitor is in the background and has no screen group, so it must
 * use VioPopUp to get access to the screen.
 *
 * Params: msgErr - error message string
 *
 * Return: None
 */
void BackError( char *pchErr )
{
    USHORT fWait = VP_WAIT | VP_TRANSPARENT;

    VioPopUp( &fWait, CON );
    puts( pchErr );
    puts( "Press any key to continue . . ." );
    while( !kbhit() )
        ;
    VioEndPopUp( CON );
}

/* Displays syntax.
 *
 * Params: None
 *
 * Return: None
 */
void Syntax()
{
    puts( "\nOptions: " );
    puts( "\t/H\t  Display help." );
    puts( "\t/S\t  Turn sound on (default)." );
    puts( "\t/Q\t  Turn sound off." );
    puts( "\t/D\t  Deinstall." );
    puts( "\t/A [path] Append each screen to a file (complete path allowed)." );
    puts( "\t\t  If no file given, default is SNAP.IMG in current directory" );
    puts( "\t\t  Resets screen number to 1." );
    puts( "\t/N [num]  Create numbered file for each screen." );
    puts( "\t\t  Example: SNAP1.IMG, SNAP2.IMG in current directory." );
    puts( "\t\t  Resets screen number to 1 or to num if given." );
}

/* Evaluate command-line options.
 *
 * Params: argc - Number of arguments
 *         argv - Pointer to argument list
 *
 * Return: none
 *
 * Uses: Shared memory structure - pshrSnap
 */
void EvalOptions( int argc, char **argv )
{
    SHORT i;

    /* Look for and handle arguments */
    for(  i = 1; i < argc; i++ )
    {
        if( argv[i][0] == '/' || argv[i][0] == '-' )
        {
            switch( argv[i][1] )
            {
                case 'A':
                case 'a':
                    pshrSnap->fAppend = TRUE;
                    pshrSnap->cScreen = 0;
                    if( (argv[++i]) &&
                        (argv[i][0] != '/') &&
                        (argv[i][0] != '-') )

                    {
                        strcpy( pshrSnap->achSnap, argv[i] );
                        puts( "Append mode - name set" );
                    }
                    else
                        puts( "Append mode" );
                    break;

                case 'N':
                case 'n':
                    pshrSnap->fAppend = FALSE;
                    puts( "Numbered file mode" );
                    if( (argv[++i]) &&
                        (argv[i][0] != '/') &&
                        (argv[i][0] != '-') )
                    {
                        pshrSnap->cScreen = (atoi( argv[i] ) % 9999) - 1;
                    }
                    else
                        pshrSnap->cScreen = 0;
                    break;

                case 'Q':
                case 'q':
                    pshrSnap->fSound = FALSE;
                    puts( "Sound off" );
                    break;

                case 'S'  :
                case 's'  :
                    pshrSnap->fSound = TRUE;
                    puts( "Sound on" );
                    break;

                case 'D':
                case 'd':
                    if( pshrSnap->fInstall )
                    {
                        pshrSnap->fInstall = FALSE;
                        puts( "Deinstalling" );
                    }
                    else
                        exit( 0 );
                    break;

                case 'H'  :
                case 'h'  :
                    if( pshrSnap->fInstall )
                        Syntax();
                    break;
            }
        }
    }
}
