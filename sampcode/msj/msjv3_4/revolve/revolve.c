/*------------------------------------------
   REVOLVE.C -- Demonstrates GPI Transforms
  ------------------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <math.h>

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

HAB  hab ;

int main (void)
     {
     static CHAR szClientClass [] = "Revolve" ;
     HMQ         hmq ;
     HWND        hwndFrame, hwndClient ;
     QMSG        qmsg ;
     ULONG       flFrameFlags = FCF_STANDARD & ~FCF_MENU ;
     ULONG       flFrameStyle = WS_VISIBLE ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClientClass, ClientWndProc,
                       CS_SIZEREDRAW | CS_SYNCPAINT, 0) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP, flFrameStyle,
                                     &flFrameFlags, szClientClass,
                                     "GPI Transform Demo",
                                     0L, NULL, 0, &hwndClient) ;

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

VOID SetModelTransform (HPS hps, FIXED fxYScale, FIXED fxAngle, FIXED fxScale)
     {
     MATRIXLF matlf ;

     GpiSetModelTransformMatrix (hps, 0L, NULL, TRANSFORM_REPLACE) ;
     GpiQueryModelTransformMatrix (hps, 9L, &matlf) ;

     matlf.fxM22 = fxYScale ;

     GpiSetModelTransformMatrix (hps, 9L, &matlf, TRANSFORM_REPLACE) ;

     matlf.fxM11 = (FIXED) (cos (6.283*fxAngle/65536/360) * 65536) ;
     matlf.fxM22 = matlf.fxM11 ;
     matlf.fxM12 = (FIXED) (sin (6.283*fxAngle/65536/360) * 65536) ;
     matlf.fxM21 = -matlf.fxM12 ;

     GpiSetModelTransformMatrix (hps, 9L, &matlf, TRANSFORM_ADD) ;

     matlf.fxM11 = matlf.fxM22 = fxScale ;
     matlf.fxM12 = matlf.fxM21 = 0 ;

     GpiSetModelTransformMatrix (hps, 9L, &matlf, TRANSFORM_ADD) ;
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

     GpiMove (hps, aptlFigure) ;

     GpiPolyLine (hps, sizeof aptlFigure / sizeof aptlFigure [0] - 1L,
                  aptlFigure + 1) ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static double dScaler = 1.1, dYScaleAngle = 0.0,
                   dYScaleAngleInc = 0.25 ;
     static FIXED  fxAngle,  fxAngleInc = 0xA0000L,
                   fxYScale, fxScale = 0x2000L ;
     static HPS    hps ;
     HDC           hdc ;
     MATRIXLF      matlf ;
     POINTL        ptl ;
     SIZEL         sizl ;

     switch (msg)
          {
          case WM_CREATE:
               hdc = WinOpenWindowDC (hwnd) ;

               sizl.cx = sizl.cy = 0 ;
               hps = GpiCreatePS (hab, hdc, &sizl,
                                  PU_LOENGLISH | GPIF_DEFAULT |
                                  GPIT_MICRO   | GPIA_ASSOC) ;

               GpiSetMix (hps, FM_INVERT) ;

               if (!WinStartTimer (hab, hwnd, 1, 100L))
                    WinAlarm (HWND_DESKTOP, WA_ERROR) ;

               return 0 ;

          case WM_SIZE:
               ptl.x = LOUSHORT (mp2) / 2 ;  // Half of window width
               ptl.y = HIUSHORT (mp2) / 2 ;  // Half of window height

               GpiConvert (hps, CVTC_DEVICE, CVTC_PAGE, 1L, &ptl) ;

               GpiQueryDefaultViewMatrix (hps, 9L, &matlf) ;
               matlf.lM31 = ptl.x ;
               matlf.lM32 = ptl.y ;
               GpiSetDefaultViewMatrix (hps, 9L, &matlf, TRANSFORM_REPLACE) ;
               return 0 ;

          case WM_TIMER:
               SetModelTransform (hps, fxYScale, fxAngle, fxScale) ;
               DrawFigure (hps) ;

               fxYScale = (FIXED) (65535 * cos (dYScaleAngle)) ;

               if ((dYScaleAngle += dYScaleAngleInc) > 6.283)
                    dYScaleAngle -= 6.283 ;

               if ((fxAngle += fxAngleInc) > (FIXED) 0x1680000L)
                    fxAngle -= (FIXED) 0x1680000L ;

               if ((FIXED) (fxScale *= dScaler) >= (FIXED) 0x30000L ||
                         fxScale <= (FIXED) 0x2000L)
                    dScaler = 1 / dScaler ;

               SetModelTransform (hps, fxYScale, fxAngle, fxScale) ;
               DrawFigure (hps) ;
               return 0 ;

          case WM_PAINT:
               WinBeginPaint (hwnd, hps, NULL) ;
               GpiErase (hps) ;

               SetModelTransform (hps, fxYScale, fxAngle, fxScale) ;
               DrawFigure (hps) ;

               WinEndPaint (hps) ;
               return 0 ;

          case WM_DESTROY:
               WinStopTimer (hab, hwnd, 1) ;
               GpiDestroyPS (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
