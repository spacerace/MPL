/*  Journal - Dynamic Link Library Demonstrating Windows 2.0 Journal Hooks.
*
*   Programmer:
*       Eric Fogelin
*       Microsoft Technical Support
*
*   Purpose:
*       DLL entry points and hook function call-backs.
*
*   Functions:
*       StartRecord() - Entry point for initiating message recording to file.
*       StopRecord()  - Entry point for halting recording.
*       StartPlay()   - Entry point for initiating message playback from file.
*       StopPlay()    - Entry point to halt message playback.
*       Record()      - Call-back for WH_JOURNALRECORD hook function.
*       Play()        - Call-back for WH_JOURNALPLAYBACK hook function.
*       LIBINIT.ASM   - MASM initialization code.  Used to get library
*                       instance handle.
*
*   Description:
*       Windows 2.0 introduces new hook functions, WH_JOURNALRECORD and
*       WH_JOURNALPLAYBACK, which hook into the system message queue
*       allowing keyboard and mouse messages to be recorded for later
*       playback.  A natural use of these functions is for creating
*       demos and test suites, though other imaginative uses must also
*       exist.  This DLL could be easily referenced by any application
*       to quickly add a demo mode.  Note: Use extreme care when developing
*       a demo file.  Window position may change, overlapped window
*       ordering may be different, and display device may vary.  This
*       will impact on demos using the mouse to open windows, move
*       windows, manipulate dialog boxes, etc.  Note:  Journal hooks
*       are disabled by system modal dialog/message boxes.  This is a
*       design feature that must be taken into consideration.
*
*   Limits:
*       Very little error checking is used.  This should be added to any
*       "real" app.  I have indicated areas that require error checking.
*       The hook functions read and write directly to disk via low-level
*       functions, read() and write().  This impacts on performance.
*       Using a 512 or 1024 byte buffer is recommended.  Remember, do
*       not use C run-time buffered stream I/O functions.
*
*   History:
*       2/15/87    - Initial version
*       2/22/87    - Cleaned up
*       2/22/87    - Documentation
*/

#include "windows.h"
#include "memory.h"
#include "dos.h"

/* File used to record/play messages */
#define FILENAME "DIARY.BIN"

/* System queue message structure */
typedef struct sysmsg {
    unsigned message;
    WORD wParam1;
    WORD wParam2;
    DWORD lParam;
} SYSMSG;
typedef SYSMSG FAR *LPSYSMSG;

/* Static definitions for hooks and chains */
FARPROC lpprocRecord;
FARPROC lplpfnNextRecord;
FARPROC lpprocPlay;
FARPROC lplpfnNextPlay;

/* Global variables (Defined in DLL's DGROUP) */
HANDLE          hFile;
OFSTRUCT        of;
SYSMSG          bufMsg;
char far        *lpTmp;
int             nNumMsg, nMsgCtr;
BOOL            bRecord, bPlay;
DWORD           dwBaseTime;

/* Handle to the DLL instance from Libinit.asm */
extern HANDLE   LIBINST;

/* Function proto-types */
DWORD   FAR PASCAL Record( int, WORD, LPSYSMSG );
DWORD   FAR PASCAL Play( int, WORD, LPSYSMSG );
void    FAR PASCAL StartRecord();
void    FAR PASCAL StopRecord();
void    FAR PASCAL StartPlay();
void    FAR PASCAL StopPlay();

/* Undocumented Kernel Function proto-types.  See C run-time doc for
   description of related functions and their parameters. */
int     FAR PASCAL _lclose( int );
int     FAR PASCAL _llseek( int, long, int );
int     FAR PASCAL _lread( int, LPSTR, int );
int     FAR PASCAL _lwrite( int, LPSTR, int );

/*  StartRecord() - Set WH_JOURNALRECORD Hook.
*
*   Programmer:
*       Eric Fogelin
*       Microsoft Technical Support
*
*   Purpose:
*       Initialize for and set WN_JOURNALRECORD Hook.
*
*   Arguments:
*       none
*
*   Return Value:
*       void
*
*   Globals (modified):
*       hFile            - Handle to write-only file.
*       bRecord          - Boolean indicating state of Record hook.
*       dwBaseTime       - Reference system time when recording started.
*       lpprocRecord     - Procedure-instance address of call-back fcn.
*       lplpfnNextRecord - Procudure-instance address of any previous
*                          Record hook.  Note: Chaining may not be
*                          applicable.
*
*   Globals (referenced):
*       LIBINST       - Handle to instance for LIBINIT.ASM init. code.
*
*   Description:
*       Isolates the application from the actual hooking and unhooking
*       details.  Allows the application to call high-level routines
*       which take care of the dirty work.
*
*   Limits:
*       Insufficient error checking.  See code for comments.
*
*   History:
*       2/15/87    - Initial version
*       2/22/87    - Cleaned up
*       2/22/87    - Documentation
*/
void FAR PASCAL StartRecord()
{
    /* Do not allow recording while playing! DONE! */
    if (bPlay) {
       MessageBox( GetFocus(), (LPSTR)"Currently Playing!", (LPSTR)NULL, MB_OK);
       return;
    }

    /* Create write-only file, initialize OFSTRUCT, close. Add error checking */
    hFile = OpenFile( (LPSTR)FILENAME, &of, OF_CREATE | OF_WRITE | OF_EXIST );

    /* Initialize: Record on, and Base Time. */
    bRecord = TRUE;
    dwBaseTime = GetTickCount();

    /* Set the Journal Record hook.  Add error checking! */
    lpprocRecord = MakeProcInstance( (FARPROC)Record, LIBINST);
    lplpfnNextRecord = SetWindowsHook( WH_JOURNALRECORD, lpprocRecord );

    return;
}
/*
*/

/*  StopRecord() - Unhook WH_JOURNALRECORD Hook.
*
*   Programmer:
*       Eric Fogelin
*       Microsoft Technical Support
*
*   Purpose:
*       Stop further recording by unhooking recording call-back fcn.
*
*   Arguments:
*       none
*
*   Return Value:
*       void
*
*   Globals (modified):
*       bRecord          - Boolean indicating state of Record hook.
*
*   Globals (referenced):
*       lpprocRecord     - Procedure-instance address of call-back fcn.
*
*   Description:
*       Isolates the application from the actual hooking and unhooking
*       details.  Allows the application to call high-level routines
*       which take care of the dirty work.
*
*   Limits:
*       What to do if UnhookWindowsHook fails?  Add error checking!
*
*   History:
*       2/15/87    - Initial version
*       2/22/87    - Cleaned up
*       2/22/87    - Documentation
*/
void FAR PASCAL StopRecord()
{
    /* Only unhook if currently hooked. */
    if (bRecord)
       UnhookWindowsHook( WH_JOURNALRECORD, lpprocRecord );
    /* Let the world know that hook is off */
    bRecord = FALSE;
    return;
}
/*
*/

/*  StartPlay() - Set WH_JOURNALPLAYBACK Hook.
*
*   Programmer:
*       Eric Fogelin
*       Microsoft Technical Support
*
*   Purpose:
*       Initialize for and set WN_JOURNALPLAYBACK Hook.
*
*   Arguments:
*       none
*
*   Return Value:
*       void
*
*   Globals (modified):
*       hFile          - Handle to read-only file.
*       nNumMsg        - Number of messages recorded (in file).
*       nMsgCtr        - Initialize message counter to zero.
*       bPlay          - Boolean indicating state of Play hook.
*       dwBaseTime     - Reference system time when playing started.
*       lpprocPlay     - Procedure-instance address of call-back fcn.
*       lplpfnNextPlay - Procudure-instance address of any previous
*                        Play hook.  Note: Chaining may not be
*                        applicable.
*
*   Globals (referenced):
*       bRecord       - Boolean indicating state of Record hook (modified
*                       if StopRecord() called).  See code.
*       LIBINST       - Handle to instance for LIBINIT.ASM init. code.
*
*   Description:
*       Isolates the application from the actual hooking and unhooking
*       details.  Allows the application to call high-level routines
*       which take care of the dirty work.
*
*   Limits:
*       Insufficient error checking.  See code for comments.
*
*   History:
*       2/15/87    - Initial version
*       2/22/87    - Cleaned up
*       2/22/87    - Documentation
*/
void FAR PASCAL StartPlay()
{
    /* Open file so the OFSTRUCT is initialized.  Add error checking! */
    hFile = OpenFile( (LPSTR)FILENAME, &of, OF_READ );
    /* Read the first message into buffer.  Required if HC_GETNEXT is
       first call to Playback hook function.  Must have message ready! */
    _llseek( hFile, 0L, 0 );
    _lread( hFile, (LPSTR)&bufMsg, sizeof(SYSMSG) );
    /* How many messages are stored? */
    nNumMsg = _llseek( hFile, 0L, 2 ) / sizeof(SYSMSG);
    /* Don't leave files open long */
    _lclose( hFile );

    /* If no messages were recorded, none to play back. DONE! */
    if (!nNumMsg) {
       MessageBox( GetFocus(), (LPSTR)"Nothing Recorded!", (LPSTR)NULL, MB_OK );
       return;
    }

    /* Trick: Allows infinite loop to be set!  One can record the steps
       required to start playing.  This code turns off further recording. */
    if (bRecord)
       StopRecord();

    /* Initialize: First Message, Play on, and Base Time. */
    nMsgCtr = 0;
    bPlay = TRUE;
    dwBaseTime = GetTickCount();

    /* Set the Journal Playback hook.  Add error checking! */
    lpprocPlay = MakeProcInstance( (FARPROC)Play, LIBINST);
    lplpfnNextPlay = SetWindowsHook( WH_JOURNALPLAYBACK, lpprocPlay );

    return;
}
/*
*/

/*  StopPlay() - Unhook WH_JOURNALPLAYBACK Hook.
*
*   Programmer:
*       Eric Fogelin
*       Microsoft Technical Support
*
*   Purpose:
*       Stop further calls to playback function, Play(), by unhooking.
*
*   Arguments:
*       none
*
*   Return Value:
*       void
*
*   Globals (modified):
*       bPlay          - Boolean indicating state of Play hook.
*
*   Globals (referenced):
*       lpprocPlay     - Procedure-instance address of call-back fcn.
*
*   Description:
*       Isolates the application from the actual hooking and unhooking
*       details.  Allows the application to call high-level routines
*       which take care of the dirty work.
*
*   Limits:
*       What to do if UnhookWindowsHook fails?  Add error checking!
*
*   History:
*       2/15/87    - Initial version
*       2/22/87    - Cleaned up
*       2/22/87    - Documentation
*/
void FAR PASCAL StopPlay()
{
    /* Only unhook if currently hooked */
    if (bPlay)
       UnhookWindowsHook( WH_JOURNALPLAYBACK, lpprocPlay );
    /* Let the world know that hook is off */
    bPlay = FALSE;
    return;
}
/*
*/

/*  Record() - Copy system event messages to a file for later playback.
*
*   Programmer:
*       Eric Fogelin
*       Microsoft Technical Support
*
*   Purpose:
*       WH_JOURNALRECORD call-back hook function used to record messages.
*
*   Arguments:
*       nCode  - Hook code
*       wParam - Not used
*       lParam - Long pointer to a system message, LPSYSMSG.
*
*   Return Value:
*       DWORD value depends on Hook Code.
*
*   Globals (modified):
*       lpTmp            - Far pointer used to get DS for movedata().
*       bufMsg           - Temporary message storage.
*       hFile            - Handle to write-only file.
*
*   Globals (referenced):
*       dwBaseTime       - Reference system time when recording started.
*                          Used to compute elapsed time for later playback.
*                          Elapsed time is placed in bufMsg.lParam.
*
*   Description:
*       From hook function memo (modified):
*       "The hook code of interest is HC_ACTION.
*       wParam is unused.
*       lParam is a long pointer to a SYSMSG structure (LPSYSMSG)
*           with the current message.
*
*       The SYSMSG structure is identical to that used by the
*       WH_JOURNALPLAYBACK structure.  (see below).
*
*       I use this hook to record a sequence of user-initiated
*       events which I later playback, but there are other
*       uses I imagine.  Since the system time when one records
*       and when one plays back the recording will, in general,
*       be different, I keep a global variable (dwBaseTime) around
*       containing the time at the start of the sequence, and store
*       elapsed times in the lParam of the SYSMSG.  When I playback the
*       sequence, I convert those elapsed times into current
*       times by the inverse operation.
*
*       The return value from HC_ACTION is ignored.  Return 0L."
*
*   Limits:
*       Insufficient file error checking.  See code for comments.
*
*   History:
*       2/15/87    - Initial version
*       2/22/87    - Cleaned up
*       2/22/87    - Documentation
*/
DWORD FAR PASCAL Record( nCode, wParam, lParam )
int nCode;
WORD wParam;
LPSYSMSG lParam;
{
    /* If it ain't HC_ACTION, pass it along */
    if (nCode != HC_ACTION )
       return DefHookProc( nCode, wParam, (LONG)lParam, (FARPROC FAR *)&lplpfnNextRecord );

    /* Need a long pointer for FP_SEG */
    lpTmp = (char far *)&bufMsg;
    /* Get a local copy of the message */
    movedata( FP_SEG( lParam ), FP_OFF( lParam ),
              FP_SEG( lpTmp ), FP_OFF( lpTmp ),
              sizeof(SYSMSG) );
    /* Compute a delta time from beginning recording.  Message time is
       in milliseconds from system power-on.  WARNING: Wrap not handled.
       Wrapping only a problem if system on continuously for ~50 days. */
    bufMsg.lParam -= dwBaseTime;

    /* Reopen write-only file using OFSTRUCT.  Add error checking. */
    hFile = OpenFile( (LPSTR)FILENAME, &of, OF_REOPEN | OF_WRITE );
    /* Seek to the end of the file */
    _llseek( hFile, 0L, 2 );
    /* Write the message to disk */
    _lwrite( hFile, (LPSTR)&bufMsg, sizeof(SYSMSG) );
    /* Don't leave files open long */
    _lclose( hFile );

    return 0L;
}
/*
*/

/*  Play() - Copy system event messages from file to system message queue.
*
*   Programmer:
*       Eric Fogelin
*       Microsoft Technical Support
*
*   Purpose:
*       WH_JOURNALPLAYBACK call-back hook function used to replay messages.
*
*   Arguments:
*       nCode  - Hook code
*       wParam - Not used
*       lParam - See Description section.
*
*   Return Value:
*       DWORD value depends on Hook Code.
*
*   Globals (modified):
*       lpTmp            - Far pointer used to get DS for movedata().
*       bufMsg           - Temporary message storage.
*       hFile            - Handle to read-only file.
*       nMsgCtr          - Message counter specifies current message.
*
*   Globals (referenced):
*       dwBaseTime       - Reference system time when playing started.
*                          Used to compute playback time from elapsed time.
*                          Elapsed time is in bufMsg.lParam.
*       nNumMsg          - Number of total messages to playback.
*
*   Description:
*       From hook function memo (modified):
*       "There are two hook codes of interest:
*     
*       HC_GETNEXT
*       wParam is unused.
*       lParam is a long pointer to a SYSMSG structure (LPSYSMSG)
*           where the hook is to put the current messsage to playback.
*     
*       HC_SKIP
*       wParam is unused.
*       lParam is unused.
*     
*       The SYSMSG (System Queue Event) is like a MSG structure:
*       typedef struct {
*           unsigned message;
*           WORD wParam1;
*           WORD wParam2;
*           DWORD lParam;
*       } SYSMSG;
*       The message is a regular Window message (WM_*), the wParam1
*       is pretty much the same as the wParam for a  regular window
*       message, (e.g. a VK_* for a message of WM_KEYDOWN), and the
*       lParam is the current time (expressed as the number of
*       timer ticks since system generation--see GetTickCount()).
*     
*       The hook is used as follows.  Windows, after this hook
*       is installed, disables hardware input.  It will call this
*       hook in lieu of using the system queue until the hook is removed
*       via UnhookWindowsHook.  Whenever the hook gets a HC_GETNEXT,
*       it should poke a SYSMSG through the long pointer given in the
*       lParam and return the time in timer ticks that needs to
*       elapse before the event is ready to be played.  The lParam
*       field of the SYSMSG should also contain the time (not delta
*       time) to play the event.  If the time to play the event is
*       now, or has passed, this hook should return 0L.  Whenever
*       the hook gets a HC_SKIP, it should fetch the next event to
*       poke through the lParam when it next gets a HC_GETNEXT,
*       i.e. it should prepare, but not do it.
*     
*       NOTES:
*       - The hook may be called mutiple times with HC_GETNEXT
*           before getting a HC_SKIP.  The hook should keep returning
*           the same event through the lParam (with the same
*           time-to-be-played in the lParam of the SYSMSG), and a
*           decreasing elpased-time-until-event until it becomes
*           0L, whereafter it returns 0L.
*       - The first message the hook gets (of these two) can be either
*           HC_SKIP or HC_GETNEXT.  Your initialization  code should
*           be invariant.  (I do my initialization when I set the hook.)
*       - If you want to turn the hook off from within the hook, call
*           UnhookWindowsHook from the HC_SKIP case only.
*       - If you want to play events as fast as possible, you'll find
*           that simply returning 0L for every HC_GETNEXT and setting
*           the play time to the current time won't work.  (You hang).
*           I've had success using the mouse double-click speed +1 as
*           the minimum time delay between events (except where the
*           actual event times are less)."
*
*   Limits:
*       Insufficient file error checking.  See code for comments.
*
*   History:
*       2/15/87    - Initial version
*       2/22/87    - Cleaned up
*       2/22/87    - Documentation
*/
DWORD FAR PASCAL Play( nCode, wParam, lParam )
int nCode;
WORD wParam;
LPSYSMSG lParam;
{
    long delta;

    switch ( nCode ) {
        case HC_SKIP:
            /* Bump up the message counter */
            nMsgCtr++;
            /* Have all messages been played?  Stop playing, if yes */
            if (nMsgCtr >= nNumMsg)
               StopPlay();

            /* Get message ready for next HC_GETNEXT */
            /* Reopen read-only file using OFSTRUCT.  Add error checking. */
            hFile = OpenFile( (LPSTR)FILENAME, &of, OF_REOPEN | OF_READ );
            /* Seek to the next message */
            _llseek( hFile, (long)nMsgCtr*sizeof(SYSMSG), 0 );
            /* Get a copy of the message in a local buffer */
            _lread( hFile, (LPSTR)&bufMsg, sizeof(SYSMSG) );
            /* Don't leave files open long */
            _lclose( hFile );

            return 0L;
            break;

        case HC_GETNEXT:
            /* Need a long pointer for FP_SEG */
            lpTmp = (char far *)&bufMsg;
            /* Put the local copy of the message into queue.  Note: Do
               not modify local message copy; may be asked repeatedly
               for message before next message is requested! */
            movedata( FP_SEG( lpTmp ), FP_OFF( lpTmp ),
                      FP_SEG( lParam ), FP_OFF( lParam ),
                      sizeof(SYSMSG) );

            /* Compute a delta time from beginning playback.  Message time is
               in milliseconds from system power-on.  WARNING: Wrap not
               handled.  Wrapping only a problem if system on continuously
               for ~50 days. */
            lParam->lParam = bufMsg.lParam + dwBaseTime;

            /* How long from current time must message be played? */
            delta = lParam->lParam - GetTickCount();

            /* Return time in milliseconds unless it has already passed
               (delta less than 0).  If passed, return 0. */
            if (delta > 0)
                return delta;
            else
                return 0L;
            break;

        default:
            /* If it ain't HC_SKIP or HC_GETNEXT, pass it along */
            return DefHookProc( nCode, wParam, (LONG)lParam, (FARPROC FAR *)&lplpfnNextPlay);
            break;
    }
}
/*
*/
