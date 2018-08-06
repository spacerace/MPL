/*---------------------------------------------------------------
   BIGJOB5.C -- Object window approach to lengthy processing job
 ----------------------------------------------------------------*/

#define INCL_WIN
#define INCL_DOS

#include <os2.h>
#include <math.h>
#include <stdio.h>
#include "bigjob.h"

#define WM_OBJECT_CREATED     (WM_USER + 0)
#define WM_START_CALC         (WM_USER + 1)
#define WM_ABORT_CALC         (WM_USER + 2)
#define WM_CALC_DONE          (WM_USER + 3)
#define WM_CALC_ABORTED       (WM_USER + 4)
#define WM_OBJECT_DESTROYED   (WM_USER + 5)

VOID  FAR SecondThread (VOID) ;
ULONG EXPENTRY ObjectWndProc (HWND, USHORT, ULONG, ULONG) ;

HWND  hwndClient, hwndObject ;
UCHAR cThreadStack [8192] ;

INT main (VOID)
     {
     static CHAR szClassName [] = "BigJob5" ;
     HMQ         hmq ;
     HWND        hwndFrame ;
     QMSG        qmsg ;
     TID         idThread ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClassName, ClientWndProc,
                            CS_SIZEREDRAW, 0, NULL) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP,
                    WS_VISIBLE | FS_SIZEBORDER | FS_TITLEBAR
                               | FS_SYSMENU    | FS_MINMAX   | FS_MENU,
                    szClassName, "BigJob Demo No. 5",
                    0L, NULL, ID_RESOURCE, &hwndClient) ;

     EnableMenuItem (hwndClient, IDM_START, FALSE) ;

     if (DosCreateThread (SecondThread, &idThread,
                          cThreadStack + sizeof cThreadStack))

          WinAlarm (HWND_DESKTOP, WA_ERROR) ;

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;

     return 0 ;
     }

ULONG EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, ULONG mp1,
                                                     ULONG mp2)
     {
     static SHORT iCalcRep, iCurrentRep = IDM_10 ;
     static SHORT iStatus = STATUS_READY ;
     static ULONG lElapsedTime ;

     switch (msg)
          {
          case WM_OBJECT_CREATED:

               EnableMenuItem (hwnd, IDM_START, TRUE) ;
               break ;

          case WM_COMMAND:

               switch (LOUSHORT (mp1))
                    {
                    case IDM_10:
                    case IDM_100:
                    case IDM_1000:
                    case IDM_10000:
                         CheckMenuItem (hwnd, iCurrentRep, FALSE) ;
                         iCurrentRep = LOUSHORT (mp1) ;
                         CheckMenuItem (hwnd, iCurrentRep, TRUE) ;
                         break ;

                    case IDM_START:
                         EnableMenuItem (hwnd, IDM_START, FALSE) ;
                         EnableMenuItem (hwnd, IDM_ABORT, TRUE) ;

                         iStatus = STATUS_WORKING ;
                         WinInvalidateRect (hwnd, NULL) ;

                         iCalcRep = iCurrentRep ;
                         WinPostMsg (hwndObject, WM_START_CALC,
                                   MAKEULONG (iCalcRep, 0), 0L) ;
                         break ;

                    case IDM_ABORT:
                         WinPostMsg (hwndObject, WM_ABORT_CALC,
                                                 0L, 0L) ;

                         EnableMenuItem (hwnd, IDM_ABORT, FALSE) ;
                         break ;
     
                    default:
                         break ;
                    }
               break ;

          case WM_CALC_DONE:

               iStatus = STATUS_DONE ;
               lElapsedTime = mp1 ;
               WinInvalidateRect (hwnd, NULL) ;

               EnableMenuItem (hwnd, IDM_START, TRUE) ;
               EnableMenuItem (hwnd, IDM_ABORT, FALSE) ;
               break ;

          case WM_CALC_ABORTED:

               iStatus = STATUS_READY ;
               WinInvalidateRect (hwnd, NULL) ;

               EnableMenuItem (hwnd, IDM_START, TRUE) ;
               break ;

          case WM_PAINT:

               PaintWindow (hwnd, iStatus, iCalcRep, lElapsedTime) ;
               break ;

          case WM_CLOSE:

               if (hwndObject)
                    WinPostMsg (hwndObject, WM_QUIT, 0L, 0L) ;
               else
                    WinPostMsg (hwnd, WM_QUIT, 0L, 0L) ;
               break ;

          case WM_OBJECT_DESTROYED:

               WinPostMsg (hwnd, WM_QUIT, 0L, 0L) ;
               break ;

          default:
               return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
          }
     return 0L ;
     }

VOID FAR SecondThread ()
     {
     static CHAR szClassName [] = "BigJob5.Object" ;
     HMQ         hmq ;
     QMSG        qmsg ;

     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClassName, ObjectWndProc, 0L, 0, NULL) ;

     hwndObject = WinCreateWindow (HWND_OBJECT, szClassName,
                    NULL, 0L, 0, 0, 0, 0, NULL, NULL, 0, NULL, NULL) ;

     WinPostMsg (hwndClient, WM_OBJECT_CREATED, 0L, 0L) ;

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndObject) ;
     WinDestroyMsgQueue (hmq) ;

     WinPostMsg (hwndClient, WM_OBJECT_DESTROYED, 0L, 0L) ;

     DosExit (0, 0) ;
     }

ULONG EXPENTRY ObjectWndProc (HWND hwnd, USHORT msg, ULONG mp1,
                                                     ULONG mp2)
     {
     double A ;
     SHORT  i, iCalcRep ;
     LONG   lQueueStatus, lTime ;

     switch (msg)
          {
          case WM_START_CALC:

               iCalcRep = LOUSHORT (mp1) ;
               lTime = WinGetCurrentTime (hab) ;

               for (A = 1.0, i = 0 ; i < iCalcRep ; i++)
                    {
                    lQueueStatus = WinQueryQueueStatus (HWND_DESKTOP) ;

                    if (lQueueStatus & QS_POSTMSG)
                         break ;

                    A = Savage (A) ;
                    }

               if (lQueueStatus & QS_POSTMSG)
                    break ;

               lTime = WinGetCurrentTime (hab) - lTime ;

               WinPostMsg (hwndClient, WM_CALC_DONE, lTime, 0L) ;
               break ;

          case WM_ABORT_CALC:

               WinPostMsg (hwndClient, WM_CALC_ABORTED, 0L, 0L) ;
               break ;

          default:
               return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
          }
     return 0L ;
     }
