/*-------------------------------------------
   TRANSFRM.C -- Demonstrates GPI Transforms
  -------------------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "transfrm.h"

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

HAB  hab ;

int main (void)
     {
     static CHAR szClientClass [] = "Transfrm" ;
     HMQ         hmq ;
     HWND        hwndFrame, hwndClient ;
     QMSG        qmsg ;
     ULONG       flFrameFlags = FCF_STANDARD ;
     ULONG       flFrameStyle = WS_VISIBLE ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClientClass, ClientWndProc, CS_SIZEREDRAW, 0) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP, flFrameStyle,
                                     &flFrameFlags, szClientClass,
                                     "GPI Transform Demo",
                                     0L, NULL, ID_RESOURCE, &hwndClient) ;

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }
          /*-------------------------------------
             Functions for managing dialog boxes
            -------------------------------------*/

VOID SetFixedItem (HWND hwnd, USHORT idItem, FIXED fxValue)
     {
     CHAR szBuffer [20] ;

     sprintf (szBuffer, "%.3f", fxValue / 65536.0) ;
     WinSetDlgItemText (hwnd, idItem, szBuffer) ;
     }

FIXED GetFixedItem (HWND hwnd, USHORT idItem)
     {
     CHAR szBuffer [20] ;

     WinQueryDlgItemText (hwnd, idItem, sizeof szBuffer, szBuffer) ;
     return (FIXED) (atof (szBuffer) * 65536) ;
     }

VOID SetLongItem (HWND hwnd, USHORT idItem, LONG lValue)
     {
     CHAR szBuffer [20] ;

     ltoa (lValue, szBuffer, 10) ;
     WinSetDlgItemText (hwnd, idItem, szBuffer) ;
     }

LONG GetLongItem (HWND hwnd, USHORT idItem)
     {
     CHAR   szBuffer [20] ;

     WinQueryDlgItemText (hwnd, idItem, sizeof szBuffer, szBuffer) ;
     return atol (szBuffer) ;
     }
          /*------------------------------
             Dialog box window procedures
            ------------------------------*/

MRESULT EXPENTRY AngleDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static PMATRIXLF pmatlf ;
     FIXED            fxAngle, fxScale ;

     switch (msg)
          {
          case WM_INITDLG:
               pmatlf = (PMATRIXLF) mp2 ;
               return 0 ;

          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case DID_OK:
                         fxAngle = GetFixedItem (hwnd, IDD_ANGLE) ;
                         fxScale = GetFixedItem (hwnd, IDD_SCALE) ;

                         pmatlf->fxM11 = (FIXED) (fxScale * cos (6.283 *
                                            fxAngle / 65536 / 360)) ;

                         pmatlf->fxM22 = pmatlf->fxM11 ;

                         pmatlf->fxM12 = (FIXED) (fxScale * sin (6.283 *
                                             fxAngle / 65536 / 360)) ;

                         pmatlf->fxM21 = - pmatlf->fxM12 ;

                         WinDismissDlg (hwnd, TRUE) ;
                         return 0 ;

                    case DID_CANCEL:
                         WinDismissDlg (hwnd, FALSE) ;
                         return 0 ;
                    }
               break ;
          }
     return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
     }

MRESULT EXPENTRY MatrixDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static PMATRIXLF pmatlf ;

     switch (msg)
          {
          case WM_INITDLG:
               pmatlf = (PMATRIXLF) mp2 ;

               SetFixedItem (hwnd, IDD_FXM11, pmatlf->fxM11) ;
               SetFixedItem (hwnd, IDD_FXM12, pmatlf->fxM12) ;
               SetFixedItem (hwnd, IDD_FXM21, pmatlf->fxM21) ;
               SetFixedItem (hwnd, IDD_FXM22, pmatlf->fxM22) ;
               
               SetLongItem (hwnd, IDD_LM31, pmatlf->lM31) ;
               SetLongItem (hwnd, IDD_LM32, pmatlf->lM32) ;
               return 0 ;

          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case IDD_ANGLEPOP:
                         if (!WinDlgBox (HWND_DESKTOP, hwnd,
                                        AngleDlgProc, NULL,
                                        IDD_ANGLEBOX, pmatlf))
                              return 0 ;

                         SetFixedItem (hwnd, IDD_FXM11, pmatlf->fxM11) ;
                         SetFixedItem (hwnd, IDD_FXM12, pmatlf->fxM12) ;
                         SetFixedItem (hwnd, IDD_FXM21, pmatlf->fxM21) ;
                         SetFixedItem (hwnd, IDD_FXM22, pmatlf->fxM22) ;
                         return 0 ;

                    case DID_OK:
                         pmatlf->fxM11 = GetFixedItem (hwnd, IDD_FXM11);
                         pmatlf->fxM12 = GetFixedItem (hwnd, IDD_FXM12);
                         pmatlf->fxM21 = GetFixedItem (hwnd, IDD_FXM21);
                         pmatlf->fxM22 = GetFixedItem (hwnd, IDD_FXM22);

                         pmatlf->lM31 = GetLongItem (hwnd, IDD_LM31) ;
                         pmatlf->lM32 = GetLongItem (hwnd, IDD_LM32) ;

                         WinDismissDlg (hwnd, TRUE) ;
                         return 0 ;

                    case DID_CANCEL:
                         WinDismissDlg (hwnd, FALSE) ;
                         return 0 ;
                    }
               break ;
          }
     return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
     }

MRESULT EXPENTRY AboutDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     switch (msg)
          {
          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case DID_OK:
                    case DID_CANCEL:
                         WinDismissDlg (hwnd, TRUE) ;
                         return 0 ;
                    }
               break ;
          }
     return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
     }
          /*-------------------
             Drawing functions
            -------------------*/
 
VOID DrawLine (HPS hps, SHORT x1, SHORT y1, SHORT x2, SHORT y2)
     {
     POINTL ptl ;

     ptl.x = x1 ; ptl.y = y1 ; GpiMove (hps, &ptl) ;
     ptl.x = x2 ; ptl.y = y2 ; GpiLine (hps, &ptl) ;
     }

VOID DrawRuler (HPS hps)
     {
     SHORT sIndex ;

     DrawLine (hps, -350,    0, 350,   0) ;
     DrawLine (hps,    0, -350,   0, 350) ;

     for (sIndex = -300 ; sIndex <= 300 ; sIndex += 100)
          {
          DrawLine (hps, sIndex,     -5, sIndex,      5) ;
          DrawLine (hps,     -5, sIndex,      5, sIndex) ;
          }
     }

VOID DrawFigure (HPS hps)
     {
     static POINTL aptlFigure [] = 
               {
                -20,  100,  20, 100,  20,   90,  10,   90,   10,   85,
                 20,   85,  20,  75,  35,   75,  35,   70,   20,   70,
                 20,   65,   5,  65,   5,   60,  20,   60,   10,   50,
                 10,   40,  25,  40,  85,  100, 100,  100,  100,   85,
                 25,   10,  25, -25,  50,  -85,  75,  -85,   75, -100,
                 35, -100,   0, -15, -35, -100, -75, -100,  -75,  -85,
                -50,  -85, -25, -25, -25,   10, -85,  -50, -100,  -50,
               -100,  -35, -25,  40, -10,   40, -10,   50,  -20,   60,
                -20,  100
               } ;
     static POINTL aptlBox [] =
               {
               -100, -100, 100, -100, 100, 100, -100, 100
               } ;

     GpiMove (hps, aptlFigure) ;
     GpiPolyLine (hps, sizeof aptlFigure / sizeof aptlFigure [0] - 1L,
                  aptlFigure + 1) ;

     GpiSetLineType (hps, LINETYPE_DOT) ;

     GpiMove (hps, aptlBox + 3) ;
     GpiPolyLine (hps, 4L, aptlBox) ;

     GpiSetLineType (hps, LINETYPE_DEFAULT) ;
     }
          /*-------------------------
             Client window procedure
            -------------------------*/

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static HPS      hps ;
     static MATRIXLF matlfModel ;
     static POINTL   aptl [3] = { -100, -100, 100, -100, -100,  100 } ;
     HDC             hdc ;
     MATRIXLF        matlf ;
     POINTL          ptl ;
     SIZEL           sizl ;
     enum            { LowerLeft, LowerRight, UpperLeft } ;

     switch (msg)
          {
          case WM_CREATE:
               hdc = WinOpenWindowDC (hwnd) ;

               sizl.cx = sizl.cy = 0 ;
               hps = GpiCreatePS (hab, hdc, &sizl,
                                  PU_LOENGLISH | GPIF_DEFAULT |
                                  GPIT_MICRO   | GPIA_ASSOC) ;

               GpiQueryModelTransformMatrix (hps, 9L, &matlfModel) ;
               return 0 ;

          case WM_SIZE:
               ptl.x = SHORT1FROMMP (mp2) / 2 ; /* Half window width  */
               ptl.y = SHORT2FROMMP (mp2) / 2 ; /* Half window height */

               GpiConvert (hps, CVTC_DEVICE, CVTC_PAGE, 1L, &ptl) ;

               GpiQueryDefaultViewMatrix (hps, 9L, &matlf) ;
               
               matlf.lM31 = ptl.x ;
               matlf.lM32 = ptl.y ;

               GpiSetDefaultViewMatrix (hps, 9L, &matlf,
                                        TRANSFORM_REPLACE) ; 
               return 0 ;

          case WM_BUTTON1DOWN:
               ptl.x = MOUSEMSG(&msg)->x ;
               ptl.y = MOUSEMSG(&msg)->y ;

               GpiConvert (hps, CVTC_DEVICE, CVTC_MODEL, 1L, &ptl) ;

               if (WinGetKeyState (HWND_DESKTOP, VK_CTRL) < 0)
                    aptl [UpperLeft] = ptl ;

               else if (WinGetKeyState (HWND_DESKTOP, VK_SHIFT) < 0)
                    aptl [LowerRight] = ptl ;

               else                          // neither shift nor control
                    aptl [LowerLeft] = ptl ;

               matlfModel.fxM11 = 65536 * (aptl [LowerRight].x -
                                           aptl [LowerLeft].x) / 200 ;
               matlfModel.fxM12 = 65536 * (aptl [LowerRight].y -
                                           aptl [LowerLeft].y) / 200 ;
               matlfModel.fxM21 = 65536 * (aptl [UpperLeft].x -
                                           aptl [LowerLeft].x) / 200 ;
               matlfModel.fxM22 = 65536 * (aptl [UpperLeft].y -
                                           aptl [LowerLeft].y) / 200 ;
               matlfModel.lM31 = (aptl [LowerRight].x +
                                  aptl [UpperLeft].x) / 2 ;
               matlfModel.lM32 = (aptl [LowerRight].y +
                                  aptl [UpperLeft].y) / 2 ;

               WinInvalidateRect (hwnd, NULL, FALSE) ;
               return 1 ;

          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case IDM_SELECT:
                         if (WinDlgBox (HWND_DESKTOP, hwnd,
                                   MatrixDlgProc, NULL,
                                   IDD_MATRIXBOX, &matlfModel))
                              {
                              GpiSetModelTransformMatrix (hps, 9L,
                                   &matlfModel, TRANSFORM_REPLACE) ;

                              aptl [LowerLeft].x  = -100 ;
                              aptl [LowerLeft].y  = -100 ;
                              aptl [LowerRight].x =  100 ;
                              aptl [LowerRight].y = -100 ;
                              aptl [UpperLeft].x  = -100 ;
                              aptl [UpperLeft].y  =  100 ;

                              GpiConvert (hps, CVTC_WORLD, CVTC_MODEL,
                                             3L, aptl) ;

                              WinInvalidateRect (hwnd, NULL, FALSE) ;
                              }
                         return 0 ;

                    case IDM_ABOUT:
                         WinDlgBox (HWND_DESKTOP, hwnd, AboutDlgProc,
                                    NULL, IDD_ABOUTBOX, NULL) ;
                         return 0 ;
                    }
               break ;

          case WM_PAINT:
               WinBeginPaint (hwnd, hps, NULL) ;
               GpiErase (hps) ;

               GpiSetModelTransformMatrix (hps, 0L, NULL, TRANSFORM_REPLACE) ;
               DrawRuler (hps) ;
     
               GpiSetModelTransformMatrix (hps, 9L, &matlfModel,
                                           TRANSFORM_REPLACE) ;
               DrawFigure (hps) ;

               WinEndPaint (hps) ;
               return 0 ;

          case WM_DESTROY:
               GpiDestroyPS (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
