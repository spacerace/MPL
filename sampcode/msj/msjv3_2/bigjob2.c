/*-------------------------------------------------------
   BIGJOB2.C -- Timer approach to lengthy processing job
 --------------------------------------------------------*/

#define INCL_WIN

#include <os2.h>
#include <math.h>
#include <stdio.h>
#include "bigjob.h"

#define ID_TIMER 1

INT main (VOID)
     {
     static CHAR szClassName [] = "BigJob2" ;
     HMQ         hmq ;
     HWND        hwndFrame, hwndClient ;
     QMSG        qmsg ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClassName, ClientWndProc,
                            CS_SYNCPAINT | CS_SIZEREDRAW, 0, NULL) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP,
                    WS_VISIBLE | FS_SIZEBORDER | FS_TITLEBAR
                               | FS_SYSMENU    | FS_MINMAX   | FS_MENU,
                    szClassName, "BigJob Demo No. 2",
                    0L, NULL, ID_RESOURCE, &hwndClient) ;

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
     static double A ;
     static SHORT  i, iCalcRep, iCurrentRep = IDM_10 ;
     static SHORT  iStatus = STATUS_READY ;
     static ULONG  lElapsedTime ;

     switch (msg)
          {
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

                         if (!WinStartTimer (hab, hwnd, ID_TIMER, 1))
                              {
                              WinAlarm (HWND_DESKTOP, WA_ERROR) ;
                              break ;
                              }

                         EnableMenuItem (hwnd, IDM_START, FALSE) ;
                         EnableMenuItem (hwnd, IDM_ABORT, TRUE) ;

                         iStatus = STATUS_WORKING ;
                         WinInvalidateRect (hwnd, NULL) ;

                         iCalcRep = iCurrentRep ;
                         lElapsedTime = WinGetCurrentTime (hab) ;

                         A = 1.0 ;
                         i = 0 ;
          
                         break ;

                    case IDM_ABORT:
                         WinStopTimer (hab, hwnd, ID_TIMER) ;

                         iStatus = STATUS_READY ;
                         WinInvalidateRect (hwnd, NULL) ;

                         EnableMenuItem (hwnd, IDM_START, TRUE) ;
                         EnableMenuItem (hwnd, IDM_ABORT, FALSE) ;
                         break ;
     
                    default:
                         break ;
                    }
               break ;

          case WM_TIMER:

               A = Savage (A) ;

               if (++i == iCalcRep)
                    {
                    lElapsedTime = WinGetCurrentTime (hab) -
                                        lElapsedTime ;

                    WinStopTimer (hab, hwnd, ID_TIMER) ;

                    iStatus = STATUS_DONE ;
                    WinInvalidateRect (hwnd, NULL) ;

                    EnableMenuItem (hwnd, IDM_START, TRUE) ;
                    EnableMenuItem (hwnd, IDM_ABORT, FALSE) ;
                    }
               break ;

          case WM_PAINT:
               PaintWindow (hwnd, iStatus, iCalcRep, lElapsedTime) ;
               break ;

          default:
               return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
          }
     return 0L ;
     }
