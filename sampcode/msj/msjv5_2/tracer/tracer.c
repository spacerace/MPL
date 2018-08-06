//----------------------------------------------
//-TRACER ( OS/2 PM Debugging Trace Facility )--
//----------------------------------------------
//-(c) 1990 Daniel Hildebrand-------------------
//----------------------------------------------

#define INCL_PM
#define INCL_WIN
#define INCL_DOS
#define INCL_WINLISTBOXES
#define INCL_WINHOOKS
#define INCL_ERRORS

#include <os2.h>
#include <process.h>
#include <stdlib.h>
#include <string.h>
#include "tracer.h"

//----------------------------------------------
//-Module declarations--------------------------
//----------------------------------------------
int               main          ( void );
MRESULT EXPENTRY  TracerWndProc ( HWND hWnd, USHORT msg,
                                  MPARAM mp1, MPARAM mp2 );
VOID              TracerCommand ( HWND hWnd, SHORT id,
                                  SHORT source, BOOL mouse );
static VOID       ClearSelector ( char far * selector_string );
BOOL EXPENTRY     HelpHook      ( HAB hab, USHORT usMode,
                                  USHORT idTopic, USHORT idSubTopic,
                                  PRECTL prcPosition );
static VOID       HelpMessage   ( void );

//----------------------------------------------
//-External references--------------------------
//----------------------------------------------
extern
void FAR PASCAL   TracerPaint      ( HWND hWnd, USHORT msg,
                                     MPARAM mp1, MPARAM mp2 );
extern
void FAR PASCAL   ClearLB          ( HWND hLB );
extern
VOID              LogToFile        ( PSZ selector_string );
extern
MRESULT EXPENTRY  TracerAboutDlg   ( HWND hWndDlg,
                                     USHORT message,
                                     MPARAM mp1, MPARAM mp2 );
extern
MRESULT EXPENTRY  TracerLogFileDlg ( HWND hWndDlg,
                                     USHORT message,
                                     MPARAM mp1, MPARAM mp2 );


//----------------------------------------------
//-Global variables-----------------------------
//----------------------------------------------
HAB        hAB;                           /* anchor block   */
HMQ        hmqTracer;                     /* handle queue   */
HSYSSEM    hSysSem;                       /* sys semaphore  */
HFILE      pLogFile;                      /* handle to file */
HWND       hParentWnd;                    /* client hWnd    */
HWND       hParentFrm;                    /* frame hWnd     */
HWND       hLB;                           /* handle ListBox */
BOOL       bUserRequestsScroll = TRUE;    /* Scroll ON ?    */
BOOL       bUserRequestsNewLog = TRUE;    /* Truncate Log?  */
BOOL       bUserRequestsLog    = FALSE;   /* LogFile ON ?   */
BOOL       bLoadFail;                     /* TRACER Load OK?*/
char       szMessage[]     =   "           PM TRACER             ";
                                          /* Log File       */
char       szLogFile[]     =   "\\tracer.fle";
                                          /* Window Class   */
char       szParentClass[] =   "PClass";
char  far  *selector_string; /* pointer to named shared mem */
int        iNumberItems;                  /* # error strings*/
USHORT     selector;         /* selector to named shared mem*/
USHORT     pusAction;                     /*  file IO       */
USHORT     pusBytesWritten;               /*  file IO       */

//----------------------------------------------
//-main()---------------------------------------
//----------------------------------------------

int main( )
{
    QMSG      qmsg;
    ULONG     ctldata;
    SWP       swpCurrent;
    int       aiGen;

    // initialize this process
    hAB = WinInitialize( NULL );

    // create a PM message queue
    hmqTracer = WinCreateMsgQueue( hAB, 0 );

    // register window class
    if ( !WinRegisterClass( hAB,
                            (PCH)szParentClass,
                            (PFNWP)TracerWndProc,
                            CS_SIZEREDRAW,
                            0 ) )
        return( 0 );

    // file control flags
    ctldata = FCF_TITLEBAR      | FCF_SYSMENU  | FCF_BORDER |
              FCF_MINBUTTON     | FCF_MENU     | FCF_ICON   |
              FCF_SHELLPOSITION | FCF_TASKLIST | FCF_ACCELTABLE;

    // create standard window
    hParentFrm = WinCreateStdWindow( HWND_DESKTOP,
                                     WS_SYNCPAINT,
                                     &ctldata,
                                     (PCH)szParentClass,
                                     NULL,
                                     0L,
                                     (HMODULE)NULL,
                                     TRACERICON,
                                     (HWND FAR *)&hParentWnd );

    // if semaphore was obtained
    // if named shared memory segment was obtained
    // if tracer file was opened
    // ...
    if ( ! bLoadFail )
    {

        // auditory feedback to user that app coming up OK.
        for ( aiGen = 0; aiGen < 12; aiGen += 2 )
             DosBeep( ((aiGen + 1) * 100), 1 );

        WinSetWindowText( hParentFrm, "OS\\2 TRACER Version 1.0");
        WinSetWindowPos ( hParentFrm, HWND_TOP,
                          20,
                          10,
                          500,
                          125,
                          SWP_SIZE | SWP_MOVE | SWP_SHOW );
    }

    // enter message loop
    while( WinGetMsg( hAB, (PQMSG)&qmsg, (HWND)NULL, 0, 0 ) )
        WinDispatchMsg( hAB, (PQMSG)&qmsg );

    // destroy TRACER window
    WinDestroyWindow( hParentFrm );
    // destroy message queue
    WinDestroyMsgQueue( hmqTracer );
    // bye bye
    WinTerminate( hAB );
}

//----------------------------------------------
//-TracerWndProc()------------------------------
//----------------------------------------------
//----------------------------------------------

MRESULT EXPENTRY TracerWndProc( HWND hWnd, USHORT msg,
                                MPARAM mp1, MPARAM mp2 )
{
RECTL    rRect;
int      aiGen;

    switch (msg)
    {

    case WM_CREATE:

        // if semaphore already exists ...
        if ( DosCreateSem( CSEM_PUBLIC, &hSysSem,
                                          TRACER_SEMAPHORE ) )
            // ... and we cannot open it, then abort
            if ( DosOpenSem( &hSysSem, TRACER_SEMAPHORE ) )
            {
                DosBeep(300,300);
                WinMessageBox( HWND_DESKTOP, hWnd,
                               (PCH)"Failed to Load!",
                               (PCH)"OS/2 TRACER Version 1.0", NULL,
                               MB_OK| MB_ICONEXCLAMATION );
                bLoadFail = 1;
                WinPostMsg( hWnd, WM_QUIT, 0L, 0L );
                break;
            }

        // allocate an 81 byte data segment
        if ( DosAllocShrSeg( 81, TRACER_SEGMENT,
                                  &selector ) )
        {
            DosBeep(300,300);
            WinMessageBox( HWND_DESKTOP, hWnd,
                           (PCH)"Failed to Load!",
                           (PCH)"OS/2 TRACER Version 1.0", NULL,
                           MB_OK| MB_ICONEXCLAMATION );
            bLoadFail = 1;
            WinPostMsg( hWnd, WM_QUIT, 0L, 0L );
            break;
        }

        // open TRACER file
        DosOpen( (PSZ)szLogFile, &pLogFile, &pusAction, 100L,
                                            0,
                                            0x11,
                                            0x41, 0L );

        // obtain a pointer to the named segment
        selector_string =
                 (char far *)((unsigned long)selector << 16);

        // clear the segment
        ClearSelector( selector_string );

        // F1 help hook
        WinSetHook( hAB, hmqTracer, HK_HELP, (PFN)HelpHook, NULL );

        // Send to anyone who was brought up BEFORE the TRACER
        WinBroadcastMsg( hWnd, TRACER_RECIEVE_HANDLE,
                              MPFROMHWND( hWnd ), 0L,
                              BMSG_FRAMEONLY | BMSG_POSTQUEUE );

        return( WinDefWindowProc( hWnd, msg, mp1, mp2 ) );

        break;

    case WM_COMMAND:

            // handle menu commands
            TracerCommand( hWnd, SHORT1FROMMP(mp1),
                      SHORT1FROMMP(mp2), SHORT2FROMMP(mp2));
            break;

    case TRACER_REQUEST_HANDLE:

         // a client has broadcasted a request for handshake
         WinPostMsg( HWNDFROMMP(mp1), TRACER_RECIEVE_HANDLE,
                           MPFROMHWND(hWnd), 0L );

        break;

    case TRACER_REQUEST_ACTION:

         // client has issued a TRACER() request

         // is screen toggled ON ?
         if ( bUserRequestsScroll )
         {
             selector_string[80] = '\0';

             WinSendMsg( hLB, LM_INSERTITEM,
                (MPARAM)-1L, (MPARAM)(PCH)selector_string );
             WinSendMsg( hLB, LM_SELECTITEM,
                (MPARAM)(iNumberItems - 1), (MPARAM)FALSE );
             WinSendMsg( hLB, LM_SELECTITEM,
                         (MPARAM)iNumberItems++, (MPARAM)TRUE );
             WinSendMsg( hLB, LM_SETTOPINDEX,
                         (MPARAM)(iNumberItems - 1),
                         (MPARAM)TRUE );
         }

         // is file toggled ON ?
         if ( bUserRequestsLog )
         {
             selector_string[80] = '\0';
             LogToFile( selector_string );

         }

         ClearSelector( selector_string );

        break;

    case WM_CLOSE:

        // return semaphore to system
        DosCloseSem( hSysSem );

        // is semaphore available to be obtained ?
        if ( DosCreateSem( CSEM_PUBLIC, &hSysSem,
                               TRACER_SEMAPHORE ) )
        {
            // that's no good.  a client must still have it opened !
            DosBeep(75,200);
            WinMessageBox( HWND_DESKTOP, hWnd,
            (PCH)
            "Will not shut down while client(s) are still attached!",
            (PCH)"OS/2 TRACER Version 1.0", NULL,
                             MB_OK| MB_ICONEXCLAMATION );
            DosOpenSem( &hSysSem, TRACER_SEMAPHORE );
            break;
        }
        else
            DosCloseSem( hSysSem );  /* one for create */

        // continue close down
        WinPostMsg( hWnd, TRACER_MYCLOSE, 0L, 0L );

        break;

    case TRACER_MYCLOSE:

        // free the named shared data segment
        if ( DosFreeSeg( selector ) ) DosBeep( 100, 450 );

        // release the F1 help hook
        WinReleaseHook ( hAB, hmqTracer, HK_HELP,
                                 (PFN)HelpHook, NULL );

        // flush the buffer to the TRACER file / close it
        DosBufReset( pLogFile );
        DosClose( pLogFile );

        // auditory feedback to user that app coming down OK.
        for ( aiGen = 12; aiGen > 0; aiGen -= 2 )
             DosBeep( ((aiGen - 1) * 100), 1 );

        WinPostMsg( hWnd, WM_QUIT, 0L, 0L );

        break;

    case WM_PAINT:

        // paint routine for TRACER window
        TracerPaint( hWnd, msg, mp1, mp2 );

        break;

    case WM_ERASEBACKGROUND:

        return( TRUE );

        break;

    default:

        return( WinDefWindowProc( hWnd, msg, mp1, mp2 ) );

        break;
    }

    return(0L);
}

//----------------------------------------------
//--HelpHook------------------------------------
//----------------------------------------------
//----------------------------------------------

BOOL EXPENTRY HelpHook( HAB hab, USHORT usMode,
     USHORT idTopic, USHORT idSubTopic, PRECTL prcPosition )
{

WinMessageBox( HWND_DESKTOP, hParentWnd,
(PCH)
"TRACER.H has instructions on how to use TRACER in your app.",
                   (PCH)"OS/2 TRACER HelpHook", NULL,
                                  MB_OK| MB_CUANOTIFICATION );

    #ifdef HelpHook
    switch ( usMode )
    {

      case HLPM_MENU:

      // idtopic is submenu identifier
      // idsubtopic is item identifier
      // prcposition is boundary of item

           switch ( idTopic )
           {
             case ID_HELPBUTTON:
             HelpMessage();
           }
      break;
      case HLPM_FRAME:

      // idtopic is frame identifier
      // idsubtopic is focus window identifier
      // prcposition is boundary of focus window

           switch ( idTopic )
           {
             case ID_HELPBUTTON:
             HelpMessage();
           }
      break;

      case HLPM_WINDOW:

      // idtopic is parent of focus window
      // idsuptopic is focus window identifier
      // prcposition is boundary of focus window

           switch ( idTopic )
           {
             case ID_HELPBUTTON:
             HelpMessage();
           }
      break;

    }
    #endif
return TRUE;
}

//----------------------------------------------
//--TracerCommand-------------------------------
//----------------------------------------------
//----------------------------------------------

VOID TracerCommand( HWND hWnd, SHORT id, SHORT source, BOOL mouse )
{

    switch( id )
    {
        case IDMABOUT:

            WinDlgBox( HWND_DESKTOP, hWnd, (PFNWP)TracerAboutDlg,
                            NULL, IDMABOUT, NULL );
            break;

        case IDMLOG:

            WinDlgBox( HWND_DESKTOP, hWnd, (PFNWP)TracerLogFileDlg,
                            NULL, IDMLOG, NULL );
            break;

        case IDMCLEAR:

            ClearLB( hLB );
            break;

    }
}

//----------------------------------------------
//-HelpMessage()--------------------------------
//----------------------------------------------
//----------------------------------------------

static VOID       HelpMessage   ( void )
{

 WinMessageBox( HWND_DESKTOP, hParentWnd,
                (PCH)
 "TRACER.H has instructions on how to use TRACER in your app.",
                (PCH)"OS/2 TRACER HelpHook", NULL,
                                   MB_OK| MB_CUANOTIFICATION );
return;
}

//----------------------------------------------
//-ClearSelector()------------------------------
//----------------------------------------------
//----------------------------------------------

static VOID ClearSelector  ( PSZ selector_string )
{
int aiX;

    for ( aiX = 0; aiX < 80; aiX++ )
         selector_string[aiX] = ' ';
return;
}
