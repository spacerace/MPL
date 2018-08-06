//-----------------------------------------------
//-TRACERD.C Source code for TRACER file routines
//-----------------------------------------------

#define INCL_PM
#define INCL_WIN
#define INCL_DOS
#define INCL_WINLISTBOXES
#define INCL_WINBUTTONS
#define INCL_WINHOOKS
#define INCL_ERRORS

#include <os2.h>
#include "tracer.h"

//----------------------------------------------
//-External references--------------------------
//----------------------------------------------
extern char    szLogFile[];
extern BOOL    bUserRequestsScroll;
extern BOOL    bUserRequestsLog;
extern BOOL    bUserRequestsNewLog;
extern HFILE   pLogFile;
extern USHORT  pusAction;
extern USHORT  pusBytesWritten;

//----------------------------------------------
//--LogToFile()---------------------------------
//----------------------------------------------

VOID LogToFile ( PSZ selector_string )
{
// write text string to log file
DosWrite( pLogFile, (char far *)selector_string,
                         (USHORT)80, &pusBytesWritten );
DosWrite( pLogFile, (char far *)"\r\n",
                         (USHORT)2, &pusBytesWritten );
return;
}

//----------------------------------------------
//--TracerAboutDlg()----------------------------
//----------------------------------------------

MRESULT EXPENTRY TracerAboutDlg( HWND hWndDlg, USHORT message,
                                 MPARAM mp1, MPARAM mp2 )
{
    switch( message )
    {
      case WM_COMMAND:
        /* the user has pressed a button */
        switch( SHORT1FROMMP( mp1 ) )
        {
          case ID_OK:
            WinDismissDlg( hWndDlg, TRUE );
            break;

          default:
            return( FALSE );
        }
        break;

      default:
        return( WinDefDlgProc( hWndDlg, message, mp1, mp2 ) );
    }
    return( FALSE );
}

//----------------------------------------------
//--TracerLogFileDlg()--------------------------
//----------------------------------------------

MRESULT EXPENTRY TracerLogFileDlg( HWND hWndDlg, USHORT message,
                                 MPARAM mp1, MPARAM mp2 )
{
    switch( message )
    {
      case WM_INITDLG:

        // options controls init
        WinSendDlgItemMsg( hWndDlg, ID_LOGFILESCROLL, BM_SETCHECK,
            MPFROM2SHORT( (bUserRequestsLog) ? 1 : 0, 0), 0L );
        WinSendDlgItemMsg( hWndDlg, ID_SCREENSCROLL, BM_SETCHECK,
            MPFROM2SHORT( (bUserRequestsScroll) ? 1 : 0, 0), 0L );

      case WM_COMMAND:
        switch( SHORT1FROMMP( mp1 ) )
        {
          case ID_CANCEL:
            WinDismissDlg( hWndDlg, TRUE );
            break;

          case ID_OK:

            // if user wishes to truncate log file, do it
            if (
            bUserRequestsNewLog =
             (SHORT)WinSendDlgItemMsg( hWndDlg,
                     ID_SCREENSCROLL, BM_QUERYCHECK, 0L, 0L ) )
                 DosNewSize( pLogFile, 0L );

            // does user wish to see messages on screen ?
            bUserRequestsScroll =
             (SHORT)WinSendDlgItemMsg( hWndDlg,
                     ID_SCREENSCROLL, BM_QUERYCHECK, 0L, 0L );

            // does user wish to log messages to file ?
            bUserRequestsLog =
             (SHORT)WinSendDlgItemMsg( hWndDlg,
                     ID_LOGFILESCROLL, BM_QUERYCHECK, 0L, 0L );

            WinDismissDlg( hWndDlg, TRUE );

            break;

          default:
            return( FALSE );
        }
        break;

      default:
        return( WinDefDlgProc( hWndDlg, message, mp1, mp2 ) );
    }
    return( FALSE );
}
