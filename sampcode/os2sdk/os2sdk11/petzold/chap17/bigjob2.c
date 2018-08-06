/*-------------------------------------------------------
   BIGJOB2.C -- Timer approach to lengthy processing job
 --------------------------------------------------------*/

#define INCL_WIN
#include <os2.h>
#include "bigjob.h"

#define ID_TIMER 1

HAB  hab ;

int main (void)
     {
     static CHAR  szClientClass [] = "BigJob2" ;
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU  |
                                 FCF_SIZEBORDER    | FCF_MINMAX   |
                                 FCF_SHELLPOSITION | FCF_TASKLIST |
                                 FCF_MENU ;
     HMQ          hmq ;
     HWND         hwndFrame, hwndClient ;
     QMSG         qmsg ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClientClass, ClientWndProc,
                       CS_SYNCPAINT | CS_SIZEREDRAW, 0) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP, WS_VISIBLE,
                                     &flFrameFlags, szClientClass,
                                     " - The Timer",
                                     0L, NULL, ID_RESOURCE, &hwndClient) ;

     WinSendMsg (hwndFrame, WM_SETICON,
                 WinQuerySysPointer (HWND_DESKTOP, SPTR_APPICON, FALSE),
                 NULL) ;

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static double A ;
     static LONG   lRep, lCalcRep,
                   lRepAmts [] = { 10, 100, 1000, 10000, 100000 } ;
     static SHORT  sCurrentRep = IDM_10 ;
     static SHORT  sStatus = STATUS_READY ;
     static ULONG  ulElapsedTime ;

     switch (msg)
          {
          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case IDM_10:
                    case IDM_100:
                    case IDM_1000:
                    case IDM_10000:
                    case IDM_100000:
                         CheckMenuItem (hwnd, sCurrentRep, FALSE) ;
                         sCurrentRep = COMMANDMSG(&msg)->cmd ;
                         CheckMenuItem (hwnd, sCurrentRep, TRUE) ;
                         return 0 ;

                    case IDM_START:
                         if (!WinStartTimer (hab, hwnd, ID_TIMER, 0))
                              {
                              WinAlarm (HWND_DESKTOP, WA_ERROR) ;
                              return 0 ;
                              }
                         EnableMenuItem (hwnd, IDM_START, FALSE) ;
                         EnableMenuItem (hwnd, IDM_ABORT, TRUE) ;

                         sStatus = STATUS_WORKING ;
                         WinInvalidateRect (hwnd, NULL, FALSE) ;

                         lCalcRep = lRepAmts [sCurrentRep - IDM_10] ;
                         ulElapsedTime = WinGetCurrentTime (hab) ;
                         A = 1.0 ;
                         lRep = 0 ;

                         return 0 ;

                    case IDM_ABORT:
                         WinStopTimer (hab, hwnd, ID_TIMER) ;

                         sStatus = STATUS_READY ;
                         WinInvalidateRect (hwnd, NULL, FALSE) ;

                         EnableMenuItem (hwnd, IDM_START, TRUE) ;
                         EnableMenuItem (hwnd, IDM_ABORT, FALSE) ;
                         return 0 ;
                    }
               break ;

          case WM_TIMER:
               A = Savage (A) ;

               if (++lRep == lCalcRep)
                    {
                    ulElapsedTime = WinGetCurrentTime (hab) -
                                        ulElapsedTime ;

                    WinStopTimer (hab, hwnd, ID_TIMER) ;

                    sStatus = STATUS_DONE ;
                    WinInvalidateRect (hwnd, NULL, FALSE) ;

                    EnableMenuItem (hwnd, IDM_START, TRUE) ;
                    EnableMenuItem (hwnd, IDM_ABORT, FALSE) ;
                    }
               return 0 ;

          case WM_PAINT:
               PaintWindow (hwnd, sStatus, lCalcRep, ulElapsedTime) ;
               return 0 ;

          case WM_DESTROY:
               if (sStatus == STATUS_WORKING)
                    WinStopTimer (hab, hwnd, ID_TIMER) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
