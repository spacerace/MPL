//
// cpumeter.c - (c) 1990 Chiverton Graphics, Inc.
//

#define INCL_WIN
#define INCL_DOS
#include <os2.h>
#include <process.h>
#include <stdlib.h>
#include <stdio.h>
#include "cpumeter.h"
#include "histgram.h"

#define THREADSTACKSIZE   4096

VOID TimerThread (void) ;
VOID CounterThread (void) ;
VOID CalibrationThread (void) ;
INT  nearest_10_percent (LONG lAmount);
INT  nearest_1_percent  (LONG lAmount);

TID   tidTimer ,
      tidCounter,
      tidCalibration;

HWND  hwndClient,
      hwndFrame ;

HAB   hab ;

HSYSSEM hSem ;

LONG  lCount     = 0L,
      lCountMax  = 0L,
      lTenth     = 0L,
      lTwentieth = 0L;

INT   iTimerThreadStack       [THREADSTACKSIZE / 2] ,
      iCalibrationThreadStack [THREADSTACKSIZE / 2] ,
      iCounterThreadStack     [THREADSTACKSIZE / 2] ;

HPOINTER hIcon [11];

INT main (void)
     {
     static CHAR  szClientClass [] = "CPUMETER";
     static ULONG flFrameFlags = FCF_TITLEBAR | FCF_SYSMENU  | FCF_SIZEBORDER |
                                 FCF_MINMAX   | FCF_TASKLIST |
                                 FCF_ICON     | FCF_SHELLPOSITION ;
     HMQ  hmq  ;
     QMSG qmsg ;

     if ( DosCreateSem(CSEM_PUBLIC, &hSem, "\\sem\\cpumeter.sem") )
          DosExit (EXIT_PROCESS, 0);

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClientClass, ClientWndProc, CS_SIZEREDRAW, 0) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP, WS_VISIBLE,
                                     &flFrameFlags, szClientClass,
                                     NULL, 0L, NULL, ID_ICON00, &hwndClient) ;

     DosSetPrty (PRTYS_THREAD, PRTYC_TIMECRITICAL, 31, 1);
     tidCalibration = _beginthread (CalibrationThread, iCalibrationThreadStack, THREADSTACKSIZE, NULL) ;
     DosSleep (1000L);
     DosSuspendThread (tidCalibration);
     DosSetPrty (PRTYS_THREAD, PRTYC_REGULAR, 0, 1);

     lTenth     = lCountMax / 10;
     lTwentieth = lCountMax / 20;

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
     INT i;
     switch (msg)
          {
          case WM_CREATE:
               {
               for (i=0; i<=10; i++)  hIcon [i] = WinLoadPointer (HWND_DESKTOP, NULL, (100 + i));
               create_histogram (hab, hwnd) ;
               return 0 ;
               }

          case WM_SIZE:
               {
               size_histogram (SHORT1FROMMP (mp2), SHORT2FROMMP (mp2));
               return 0 ;
               }

          case WM_PAINT:
               {
               paint_histogram ();
               return 0 ;
               }

          case WM_SEM1:
               {
               static char szBuffer [60];
               static INT iOld = -1;

               i = nearest_1_percent (LONGFROMMP (mp1));
               sprintf (szBuffer, "%d%% CPU Load", i);
               WinSetWindowText (hwndFrame, szBuffer) ;
               update_histogram (i) ;

               i = nearest_10_percent (LONGFROMMP (mp1));
               if (i != iOld)
                    {
                    iOld = i;
                    WinSendMsg (hwndFrame, WM_SETICON, hIcon [i], NULL) ;
                    WinInvalidateRect (hwndFrame, NULL, FALSE);
                    WinUpdateWindow (hwndFrame);
                    }
               return 0 ;
               }

          case WM_DESTROY:
               {
               for (i=0; i<=10; i++)  WinDestroyPointer (hIcon [i]);
               destroy_histogram ();
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

VOID CalibrationThread ()
     {
     DosSetPrty (PRTYS_THREAD, PRTYC_TIMECRITICAL, 30, tidCalibration);
     while (TRUE) lCountMax++;
     }

INT nearest_10_percent (LONG lAmount)
     {
     ldiv_t struct_ldiv;

     if (lAmount == 0) return 10;
     if (lAmount >= lCountMax) return 0;

     struct_ldiv = ldiv (lAmount, lTenth);
     if (struct_ldiv.rem >= lTwentieth) struct_ldiv.quot++;
     return (INT) (10 - struct_ldiv.quot);
     }

INT nearest_1_percent (LONG lAmount)
     {
     if (lAmount == 0) return 100;
     if (lAmount >= lCountMax) return 0;

     return (INT) (100. * ((lCountMax - lAmount)/(double)lCountMax));
     }
