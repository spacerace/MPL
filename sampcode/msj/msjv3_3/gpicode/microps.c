/*
Microsoft Systems Journal
Volume 3; Issue 3; May, 1988

	pp. 9-18

Author(s): Charles Petzold
Title:     The Graphics Programming Interface: A Guide to OS/2 
           Presentation Spaces

*/

/*------------------------------------
   MICROPS.C -- Demonstrates Micro-PS
  ------------------------------------*/

#define INCL_WIN
#define INCL_GPI

#include <os2.h>

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

HAB  hab ;

main ()
     {
     static CHAR szClientClass [] = "MicroPS" ;
     HMQ    hmq ;
     HWND   hwndFrame, hwndClient ;
     QMSG   qmsg ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClientClass, ClientWndProc,
                                           CS_SIZEREDRAW, 0) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP,
                    WS_VISIBLE | FS_SIZEBORDER | FS_TITLEBAR
                               | FS_SYSMENU    | FS_MINMAX,
                    szClientClass, "Micro-PS",
                    0L, NULL, 0, &hwndClient) ;

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;

     return 0 ;
     }

MRESULT EXPENTRY ClientWndProc (hwnd, msg, mp1, mp2)
     HWND          hwnd ;
     USHORT        msg ;
     MPARAM        mp1 ;
     MPARAM        mp2 ;
     {
     static CHAR   szText []      = "Graphics Programming Interface" ;
     static HPS    hps ;
     static LONG   lTextLength    = sizeof szText - 1L ;
     static LONG   alColorData [] = { CLR_BACKGROUND, RGB_WHITE,
                                      CLR_NEUTRAL,    RGB_BLACK } ;
     static POINTL ptlTextStart, aptlLineStart [4],
                   aptlTextBox [TXTBOX_COUNT] ;
     static SHORT  cxClient, cyClient ;
     HDC           hdc ;
     POINTL        ptl ;
     SHORT         sIndex ;
     SIZEL         sizl ;

     switch (msg)
          {
          case WM_CREATE:
               hdc = WinOpenWindowDC (hwnd) ;

               sizl.cx = 0 ;
               sizl.cy = 0 ;

               hps = GpiCreatePS (hab, hdc, &sizl, PU_PELS |
                                   GPIF_DEFAULT | GPIT_MICRO |
                                   GPIM_NORMAL  | GPIA_ASSOC) ;

               GpiCreateLogColorTable (hps, LCOL_RESET,
                    LCOLF_INDRGB, 0L, 4L, alColorData) ; /* temp fix */

               GpiQueryTextBox (hps, lTextLength, szText,
                                TXTBOX_COUNT, aptlTextBox) ;

               GpiSetColor (hps, CLR_RED) ;
               GpiSetLineType (hps, LINETYPE_DOT) ;
               break ;

          case WM_SIZE:
               cxClient = LOUSHORT (mp2) ;
               cyClient = HIUSHORT (mp2) ;

               ptlTextStart.x = (cxClient - 
                                aptlTextBox [TXTBOX_BOTTOMRIGHT].x -
                                aptlTextBox [TXTBOX_BOTTOMLEFT].x) / 2 ;

               ptlTextStart.y = (cyClient -
                                aptlTextBox [TXTBOX_TOPLEFT].y -
                                aptlTextBox [TXTBOX_BOTTOMLEFT].y) / 2 ;

               for (sIndex = 0 ; sIndex < 4 ; sIndex ++)
                    {
                    aptlLineStart [sIndex] = aptlTextBox [sIndex] ;
                    aptlLineStart [sIndex].x += ptlTextStart.x ;
                    aptlLineStart [sIndex].y += ptlTextStart.y ;
                    }

               break ;

          case WM_PAINT:
               /* BeginPaint (hwnd, hps, NULL) ; */

               GpiErase (hps) ;

               GpiCharStringAt (hps, &ptlTextStart,
                                   lTextLength, szText) ;

               GpiMove (hps, aptlLineStart + TXTBOX_BOTTOMLEFT) ;
               ptl.x = 0 ;
               ptl.y = 0 ;
               GpiLine (hps, &ptl) ;

               GpiMove (hps, aptlLineStart + TXTBOX_BOTTOMRIGHT) ;
               ptl.x = cxClient ;
               GpiLine (hps, &ptl) ;

               GpiMove (hps, aptlLineStart + TXTBOX_TOPRIGHT) ;
               ptl.y = cyClient ;
               GpiLine (hps, &ptl) ;

               GpiMove (hps, aptlLineStart + TXTBOX_TOPLEFT) ;
               ptl.x = 0 ;
               GpiLine (hps, &ptl) ;

               /* EndPaint (hps) ; */

               WinValidateRect (hwnd, NULL, FALSE) ;	/* temp fix */
               break ;

          case WM_DESTROY:
               GpiDestroyPS (hps) ;
               break ;

          default:
               return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
          }
     return FALSE ;
     }
