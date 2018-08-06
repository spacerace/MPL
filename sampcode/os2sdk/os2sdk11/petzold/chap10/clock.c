/*-------------------------
   CLOCK.C -- Analog Clock 
  -------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <stdlib.h>

#define ID_TIMER 1

typedef struct
     {
     SHORT cxClient ;
     SHORT cyClient ;
     SHORT cxPixelDiam ;
     SHORT cyPixelDiam ;
     }
     WINDOWINFO ;

typedef WINDOWINFO *PWINDOWINFO ;

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

int main (void)
     {
     static CHAR  szClientClass[] = "Clock" ;
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU |
                                 FCF_SIZEBORDER    | FCF_MINMAX  |
                                 FCF_SHELLPOSITION | FCF_TASKLIST ;
     HAB          hab ;
     HMQ          hmq ;
     HWND         hwndFrame, hwndClient ;
     QMSG         qmsg ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClientClass, ClientWndProc, CS_SIZEREDRAW, 0) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP, WS_VISIBLE,
                                     &flFrameFlags, szClientClass, NULL,
				     0L, NULL, 0, &hwndClient) ;

     if (WinStartTimer (hab, hwndClient, ID_TIMER, 1000))
          {
          while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
               WinDispatchMsg (hab, &qmsg) ;

          WinStopTimer (hab, hwndClient, ID_TIMER) ;
          }
     else
          WinMessageBox (HWND_DESKTOP, hwndClient,
                         "Too many clocks or timers",
                         szClientClass, 0, MB_OK | MB_ICONEXCLAMATION) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

VOID RotatePoint (POINTL aptl[], SHORT sNum, SHORT sAngle)
     {
     static SHORT sSin [60] =
                    {
                       0,  105,  208,  309,  407,  500,  588,  669,  743,  809,
                     866,  914,  951,  978,  995, 1000,  995,  978,  951,  914,
                     866,  809,  743,  669,  588,  500,  407,  309,  208,  105,
                       0, -104, -207, -308, -406, -499, -587, -668, -742, -808,
                    -865, -913, -950, -977, -994, -999, -994, -977, -950, -913,
                    -865, -808, -742, -668, -587, -499, -406, -308, -207, -104
                    } ;
     POINTL       ptlTemp ;
     SHORT        sIndex ;

     for (sIndex = 0 ; sIndex < sNum ; sIndex++)
          {
          ptlTemp.x = (aptl[sIndex].x * sSin [(sAngle + 15) % 60] +
                       aptl[sIndex].y * sSin [sAngle]) / 1000 ;

          ptlTemp.y = (aptl[sIndex].y * sSin [(sAngle + 15) % 60] -
                       aptl[sIndex].x * sSin [sAngle]) / 1000 ;

          aptl[sIndex] = ptlTemp ;
          }
     }

VOID ScalePoint (POINTL aptl[], SHORT sNum, PWINDOWINFO pwi)
     {
     SHORT sIndex ;

     for (sIndex = 0 ; sIndex < sNum ; sIndex++)
          {
          aptl[sIndex].x = aptl[sIndex].x * pwi->cxPixelDiam / 200 ;
          aptl[sIndex].y = aptl[sIndex].y * pwi->cyPixelDiam / 200 ;
          }
     }

VOID TranslatePoint (POINTL aptl[], SHORT sNum, PWINDOWINFO pwi)
     {
     SHORT sIndex ;

     for (sIndex = 0 ; sIndex < sNum ; sIndex++)
          {
          aptl[sIndex].x += pwi->cxClient / 2 ;
          aptl[sIndex].y += pwi->cyClient / 2 ;
          }
     }

VOID DrawHand (HPS hps, POINTL aptlIn[], SHORT sNum, SHORT sAngle,
               PWINDOWINFO pwi)
     {
     POINTL aptl [5] ;
     SHORT  sIndex ;

     for (sIndex = 0 ; sIndex < sNum ; sIndex++)
          aptl [sIndex] = aptlIn [sIndex] ;

     RotatePoint    (aptl, sNum, sAngle) ;
     ScalePoint     (aptl, sNum, pwi) ;
     TranslatePoint (aptl, sNum, pwi) ;

     GpiMove (hps, aptl) ;
     GpiPolyLine (hps, sNum - 1L, aptl + 1) ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static DATETIME   dtPrevious ;
     static HDC        hdc ;
     static LONG       xPixelsPerMeter, yPixelsPerMeter ;
     static POINTL     aptlHour   [5] = { 0,-15, 10,0, 0,60, -10,0, 0,-15 },
                       aptlMinute [5] = { 0,-20,  5,0, 0,80,  -5,0, 0,-20 },
                       aptlSecond [2] = { 0,  0,  0,80 } ;
     static WINDOWINFO wi ;
     DATETIME          dt ;
     HPS               hps ;
     POINTL            aptl [3] ;
     SHORT             sDiamMM, sAngle ;

     switch (msg)
          {
          case WM_CREATE:
               hdc = WinOpenWindowDC (hwnd) ;

               DevQueryCaps (hdc, CAPS_VERTICAL_RESOLUTION,
                                  1L, &yPixelsPerMeter) ;
               DevQueryCaps (hdc, CAPS_HORIZONTAL_RESOLUTION,
                                  1L, &xPixelsPerMeter) ;

               DosGetDateTime (&dtPrevious) ;
               dtPrevious.hours = (dtPrevious.hours * 5) % 60 +
                                   dtPrevious.minutes / 12 ;
               return 0 ;

          case WM_SIZE:
               wi.cxClient = SHORT1FROMMP (mp2) ;
               wi.cyClient = SHORT2FROMMP (mp2) ;

               sDiamMM = (SHORT) min (wi.cxClient * 1000L / xPixelsPerMeter,
                                      wi.cyClient * 1000L / yPixelsPerMeter) ;

               wi.cxPixelDiam = (SHORT) (xPixelsPerMeter * sDiamMM / 1000) ;
               wi.cyPixelDiam = (SHORT) (yPixelsPerMeter * sDiamMM / 1000) ;
               return 0 ;

          case WM_TIMER:
               DosGetDateTime (&dt) ;
               dt.hours = (dt.hours * 5) % 60 + dt.minutes / 12 ;

               hps = WinGetPS (hwnd) ;
               GpiSetColor (hps, CLR_BACKGROUND) ;

               DrawHand (hps, aptlSecond, 2, dtPrevious.seconds, &wi) ;

               if (dt.hours   != dtPrevious.hours ||
                   dt.minutes != dtPrevious.minutes)
                    {
                    DrawHand (hps, aptlHour,   5, dtPrevious.hours,   &wi) ;
                    DrawHand (hps, aptlMinute, 5, dtPrevious.minutes, &wi) ;
                    }

               GpiSetColor (hps, CLR_NEUTRAL) ;

               DrawHand (hps, aptlHour,   5, dt.hours,   &wi) ;
               DrawHand (hps, aptlMinute, 5, dt.minutes, &wi) ;
               DrawHand (hps, aptlSecond, 2, dt.seconds, &wi) ;

               WinReleasePS (hps) ;
               dtPrevious = dt ;
               return 0 ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;
               GpiErase (hps) ;

               for (sAngle = 0 ; sAngle < 60 ; sAngle++)
                    {
                    aptl[0].x = 0 ;
                    aptl[0].y = 90 ;

                    RotatePoint    (aptl, 1, sAngle) ;
                    ScalePoint     (aptl, 1, &wi) ;
                    TranslatePoint (aptl, 1, &wi) ;

                    aptl[2].x = aptl[2].y = sAngle % 5 ? 2 : 10 ;

                    ScalePoint (aptl + 2, 1, &wi) ;

                    aptl[0].x -= aptl[2].x / 2 ;
                    aptl[0].y -= aptl[2].y / 2 ;

                    aptl[1].x = aptl[0].x + aptl[2].x ;
                    aptl[1].y = aptl[0].y + aptl[2].y ;

                    GpiMove (hps, aptl) ;
                    GpiBox (hps, DRO_OUTLINEFILL, aptl + 1,
                                 aptl[2].x, aptl[2].y) ;
                    }
               DrawHand (hps, aptlHour,   5, dtPrevious.hours,   &wi) ;
               DrawHand (hps, aptlMinute, 5, dtPrevious.minutes, &wi) ;
               DrawHand (hps, aptlSecond, 2, dtPrevious.seconds, &wi) ;

               WinEndPaint (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
