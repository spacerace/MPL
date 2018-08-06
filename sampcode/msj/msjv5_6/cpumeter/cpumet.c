//
// cpumet.c - (c) 1990 Chiverton Graphics, Inc.
//

#define INCL_WIN
#define INCL_DOS
#include <os2.h>
#include <process.h>
#include <stdio.h>

#define THREADSTACKSIZE   4096

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;
VOID    TimerThread   (void) ;
VOID    CounterThread (void) ;

TID   tidTimer ,
      tidCounter;

HWND  hwndClient,
      hwndFrame ;

HSYSSEM hSem ;

LONG  lCount = 0L;

INT   iTimerThreadStack     [THREADSTACKSIZE / 2] ,
      iCounterThreadStack   [THREADSTACKSIZE / 2] ;

int main (void)
     {
     CHAR  szClientClass [] = "CPUMET";
     ULONG flFrameFlags = FCF_TITLEBAR | FCF_SYSMENU | FCF_TASKLIST;
     HAB   hab ;
     HMQ   hmq ;
     QMSG  qmsg ;

     if ( DosCreateSem(CSEM_PUBLIC, &hSem, "\\sem\\cpumeter.sem") )
          DosExit (EXIT_PROCESS, 0);

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClientClass, ClientWndProc, CS_SIZEREDRAW, 0) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP, WS_VISIBLE,
                                     &flFrameFlags, szClientClass,
                                     NULL, 0L, NULL, NULL, &hwndClient) ;

     WinSetWindowPos (hwndFrame, HWND_TOP, 0, 0,
             WinQuerySysValue (HWND_DESKTOP, SV_CXSCREEN)/3,   // cx
             WinQuerySysValue (HWND_DESKTOP, SV_CYTITLEBAR),   // cy
             SWP_SHOW | SWP_SIZE | SWP_MOVE);

     tidTimer   = _beginthread (TimerThread,   iTimerThreadStack,   THREADSTACKSIZE, NULL) ;
     tidCounter = _beginthread (CounterThread, iCounterThreadStack, THREADSTACKSIZE, NULL) ;

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     switch (msg)
          {
          case WM_SEM1:
               {
               static char szBuffer [60];

               sprintf (szBuffer, "%ld counts/sec", LONGFROMMP (mp1));
               WinSetWindowText (hwndFrame, szBuffer) ;
               WinInvalidateRect (hwndFrame, NULL, FALSE);
               WinUpdateWindow (hwndFrame);
               return 0 ;
               }

          case WM_DESTROY:
               {
               DosCloseSem (hSem);
               return 0 ;
               }
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }

VOID TimerThread ()
     {
     DosSetPrty (PRTYS_THREAD, PRTYC_TIMECRITICAL, PRTYD_MAXIMUM, tidTimer);
     while (TRUE)
          {
          DosSleep (1000L);

          WinPostMsg (hwndClient, WM_SEM1, MPFROMLONG (lCount), NULL) ;
          lCount = 0L;
          }
     }

VOID CounterThread ()
     {
     DosSetPrty (PRTYS_THREAD, PRTYC_IDLETIME, 0, tidCounter);
     while (TRUE) lCount++;
     }
