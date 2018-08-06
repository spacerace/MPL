/*----------------------------------------
   VECTFONT.C -- Vector Font Demo Program
  ----------------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include "vectfont.h"

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

HAB hab ;

int main (void)
     {
     static CHAR  szClientClass [] = "VectFont" ;
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
                            CS_SIZEREDRAW, 0) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP, WS_VISIBLE,
                                     &flFrameFlags, szClientClass,
                                     " - Vector Font Demo", 0L,
                                     (HMODULE) NULL, ID_RESOURCE, &hwndClient) ;

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

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1,
                                                       MPARAM mp2)
     {
     static struct {
                   SHORT idCmd ;
                   VOID (*fn) (HPS, LONG, LONG) ;
                   }
                   vectfont [] = {
                                 IDM_NOTHING,     NULL,
                                 IDM_24POINT,     Display_24Point,
                                 IDM_MIRROR,      Display_Mirror,
                                 IDM_STRETCH,     Display_Stretch,
                                 IDM_CHARANGLE,   Display_CharAngle,
                                 IDM_ROTATE,      Display_Rotate,
                                 IDM_CHARSHEAR,   Display_CharShear,
                                 IDM_SHADOW,      Display_Shadow,
                                 IDM_HOLLOW,      Display_Hollow,
                                 IDM_DROPSHADOW,  Display_DropShadow,
                                 IDM_BLOCK,       Display_Block,
                                 IDM_NEON,        Display_Neon,
                                 IDM_FADE,        Display_Fade,
                                 IDM_SPOKES,      Display_Spokes,
                                 IDM_WAVY,        Display_Wavy,
                                 IDM_MODSPOKES,   Display_ModSpokes
                                 } ;
     static HDC    hdc ;
     static HPS    hps ;
     static HWND   hwndMenu ;
     static POINTL ptlClient ;
     static SHORT  sNumRoutines = sizeof vectfont / sizeof vectfont[0],
                   sDisplay = IDM_NOTHING ;
     INT           i ;
     RECTL         rcl ;
     SIZEL         sizl ;

     switch (msg)
          {
          case WM_CREATE:
               hdc = WinOpenWindowDC (hwnd) ;

                                   // Create PS use Twips page units
               sizl.cx = 0 ;
               sizl.cy = 0 ;
               hps = GpiCreatePS (hab, hdc, &sizl,
                                       PU_TWIPS     | GPIF_DEFAULT |
                                       GPIT_MICRO   | GPIA_ASSOC) ;

                                   // Adjust Page Viewport for points

               GpiQueryPageViewport (hps, &rcl) ;
               rcl.xRight *= 20 ;
               rcl.yTop   *= 20 ;
               GpiSetPageViewport (hps, &rcl) ;

               hwndMenu = WinWindowFromID (
                               WinQueryWindow (hwnd, QW_PARENT, FALSE),
                               FID_MENU) ;
               return 0 ;

          case WM_SIZE:
               ptlClient.x = SHORT1FROMMP (mp2) ;      // client width
               ptlClient.y = SHORT2FROMMP (mp2) ;      // client height

               GpiConvert (hps, CVTC_DEVICE, CVTC_PAGE, 1L, &ptlClient);
               return 0 ;

          case WM_COMMAND:
               for (i = 0 ; i < sNumRoutines ; i++)
                    if (COMMANDMSG(&msg)->cmd == (USHORT) vectfont[i].idCmd)
                         {
                         if ((USHORT) sDisplay == COMMANDMSG(&msg)->cmd)
                              return 0 ;

                         WinSendMsg (hwndMenu, MM_SETITEMATTR,
                                     MPFROM2SHORT (sDisplay, TRUE),
                                     MPFROM2SHORT (MIA_CHECKED, 0)) ;

                         sDisplay = COMMANDMSG(&msg)->cmd ;

                         WinSendMsg (hwndMenu, MM_SETITEMATTR,
                                     MPFROM2SHORT (sDisplay, TRUE),
                                     MPFROM2SHORT (MIA_CHECKED,
                                                   MIA_CHECKED)) ;

                         WinInvalidateRect (hwnd, NULL, FALSE) ;
                         return 0 ;
                         }
               break ;

          case WM_PAINT:
               WinBeginPaint (hwnd, hps, NULL) ;
               GpiErase (hps) ;
                                   // Display hourglass pointer

               WinSetPointer (HWND_DESKTOP,
                    WinQuerySysPointer (HWND_DESKTOP,SPTR_WAIT,FALSE));

               if (!WinQuerySysValue (HWND_DESKTOP, SV_MOUSEPRESENT))
                    WinShowPointer (HWND_DESKTOP, TRUE) ;

                                   // Execute font routine

               for (i = 0 ; i < sNumRoutines ; i++)
                    if (sDisplay == vectfont[i].idCmd)
                         {
                         if (vectfont[i].fn != NULL)
                              {
                              GpiSavePS (hps) ;
                              vectfont[i].fn (hps, ptlClient.x,
                                                   ptlClient.y) ;
                              GpiRestorePS (hps, -1L) ;
                              }
                         break ;
                         }
                                        // Display arrow pointer

               if (!WinQuerySysValue (HWND_DESKTOP, SV_MOUSEPRESENT))
                    WinShowPointer (HWND_DESKTOP, FALSE) ;

               WinSetPointer (HWND_DESKTOP,
                    WinQuerySysPointer (HWND_DESKTOP,SPTR_ARROW,FALSE));

               WinEndPaint (hps) ;
               return 0 ;

          case WM_DESTROY:
               GpiDestroyPS (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
