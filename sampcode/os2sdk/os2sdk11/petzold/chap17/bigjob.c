/*--------------------------------------------------------------------
   BIGJOB.C -- Common functions used in BIGJOB1, BIGJOB2, and BIGJOB3
  --------------------------------------------------------------------*/

#define INCL_WIN
#include <os2.h>
#include <math.h>
#include <stdio.h>

double Savage (double A)
     {
     return tan (atan (exp (log (sqrt (A * A))))) + 1.0 ;
     }

VOID CheckMenuItem (HWND hwnd, SHORT sMenuItem, BOOL fCheck)
     {
     HWND  hwndParent = WinQueryWindow (hwnd, QW_PARENT, FALSE) ;
     HWND  hwndMenu   = WinWindowFromID (hwndParent, FID_MENU) ;

     WinSendMsg (hwndMenu, MM_SETITEMATTR,
                 MPFROM2SHORT (sMenuItem, TRUE),
                 MPFROM2SHORT (MIA_CHECKED, fCheck ? MIA_CHECKED : 0)) ;
     }

VOID EnableMenuItem (HWND hwnd, SHORT sMenuItem, BOOL fEnable)
     {
     HWND  hwndParent = WinQueryWindow (hwnd, QW_PARENT, FALSE) ;
     HWND  hwndMenu   = WinWindowFromID (hwndParent, FID_MENU) ;

     WinSendMsg (hwndMenu, MM_SETITEMATTR,
                 MPFROM2SHORT (sMenuItem, TRUE),
                 MPFROM2SHORT (MIA_DISABLED, fEnable ? 0 : MIA_DISABLED)) ;
     }

VOID PaintWindow (HWND hwnd, SHORT sStatus, LONG lCalcRep, ULONG ulTime)
     {
     static CHAR *szMessage [3] = { "Ready", "Working...",
                                    "%ld repetitions in %lu msec." } ;
     CHAR        szBuffer [60] ;
     HPS         hps ;
     RECTL       rcl ;

     hps = WinBeginPaint (hwnd, NULL, NULL) ;
     WinQueryWindowRect (hwnd, &rcl) ;

     sprintf (szBuffer, szMessage [sStatus], lCalcRep, ulTime) ;
     WinDrawText (hps, -1, szBuffer, &rcl, CLR_NEUTRAL, CLR_BACKGROUND,
                  DT_CENTER | DT_VCENTER | DT_ERASERECT) ;

     WinEndPaint (hps) ;
     }
