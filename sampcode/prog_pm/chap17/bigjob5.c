/*--------------------------------------------------
   BIGJOB5.C -- Second thread and semaphore trigger
 ---------------------------------------------------*/

#define INCL_WIN
#define INCL_DOS
#include <os2.h>
#include <mt\process.h>
#include <mt\stdlib.h>
#include "bigjob.h"

VOID _CDECL FAR CalcThread (PCALCPARAM) ;

HAB  hab ;

int main (void)
     {
     static CHAR  szClientClass [] = "BigJob5" ;
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU  |
                                 FCF_SIZEBORDER    | FCF_MINMAX   |
                                 FCF_SHELLPOSITION | FCF_TASKLIST |
                                 FCF_MENU ;
     HMQ          hmq ;
     HWND         hwndFrame, hwndClient ;
     QMSG         qmsg ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClientClass, ClientWndProc, CS_SIZEREDRAW, 0) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP, WS_VISIBLE,
                                     &flFrameFlags, szClientClass,
                                     " - Second Thread with Semaphore",
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
     static CALCPARAM cp ;
     static INT       iThreadStack [STACKSIZE / 2] ;
     static LONG      lRepAmts [] = { 10, 100, 1000, 10000, 100000 } ;
     static SHORT     sCurrentRep = IDM_10 ;
     static SHORT     sStatus = STATUS_READY ;
     static TID       tidCalc ;
     static ULONG     ulElapsedTime ;

     switch (msg)
          {
          case WM_CREATE:
               cp.hwnd = hwnd ;
               DosSemSet (&cp.ulSemTrigger) ;

               tidCalc = _beginthread (CalcThread, iThreadStack,
                                       STACKSIZE, &cp) ;
               return 0 ;

          case WM_INITMENU:
               if (tidCalc == -1 && SHORT1FROMMP (mp1) == IDM_ACTION)
                    EnableMenuItem (hwnd, IDM_START, FALSE) ;
               return 0 ;

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
                         cp.lCalcRep = lRepAmts [sCurrentRep - IDM_10] ;
                         cp.fContinueCalc = TRUE ;
                         DosSemClear (&cp.ulSemTrigger) ;

                         sStatus = STATUS_WORKING ;
                         WinInvalidateRect (hwnd, NULL, FALSE) ;
                         EnableMenuItem (hwnd, IDM_START, FALSE) ;
                         EnableMenuItem (hwnd, IDM_ABORT, TRUE) ;
                         return 0 ;

                    case IDM_ABORT:
                         cp.fContinueCalc = FALSE ;
                         EnableMenuItem (hwnd, IDM_ABORT, FALSE) ;
                         return 0 ;
                    }
               break ;

          case WM_CALC_DONE:
               sStatus = STATUS_DONE ;
               ulElapsedTime = LONGFROMMP (mp1) ;
               WinInvalidateRect (hwnd, NULL, FALSE) ;
               EnableMenuItem (hwnd, IDM_START, TRUE) ;
               EnableMenuItem (hwnd, IDM_ABORT, FALSE) ;
               return 0 ;

          case WM_CALC_ABORTED:
               sStatus = STATUS_READY ;
               WinInvalidateRect (hwnd, NULL, FALSE) ;
               EnableMenuItem (hwnd, IDM_START, TRUE) ;
               return 0 ;

          case WM_PAINT:
               PaintWindow (hwnd, sStatus, cp.lCalcRep, ulElapsedTime) ;
               return 0 ;

          case WM_DESTROY:
               if (sStatus = STATUS_WORKING)
                    DosSuspendThread (tidCalc) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }

VOID _CDECL FAR CalcThread (PCALCPARAM pcp)
     {
     double A ;
     LONG   lRep, lTime ;

     while (TRUE)
          {
          DosSemWait (&pcp->ulSemTrigger, SEM_INDEFINITE_WAIT) ;

          lTime = WinGetCurrentTime (hab) ;

          for (A = 1.0, lRep = 0 ; lRep < pcp->lCalcRep &&
                                   pcp->fContinueCalc ; lRep++)
               A = Savage (A) ;

          DosSemSet (&pcp->ulSemTrigger) ;

          if (pcp->fContinueCalc)
               {
               lTime = WinGetCurrentTime (hab) - lTime ;
               WinPostMsg (pcp->hwnd, WM_CALC_DONE, MPFROMLONG (lTime), NULL) ;
               }
          else
               WinPostMsg (pcp->hwnd, WM_CALC_ABORTED, NULL, NULL) ;
          }
     }