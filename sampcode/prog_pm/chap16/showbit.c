/*--------------------------------------------------------------------
   SHOWBIT.C -- Loads Bitmap Resources from BITLIB.DLL and Draws Them
  --------------------------------------------------------------------*/

#define INCL_DOS
#define INCL_WIN
#include <os2.h>

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

int main (void)
     {
     static CHAR  szClientClass [] = "ShowBit" ;
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
                                     &flFrameFlags, szClientClass,
                                     " (Space bar or mouse click for next)",
                                     0L, NULL, 0, &hwndClient) ;
     if (hwndFrame != NULL)
          {
          while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
               WinDispatchMsg (hab, &qmsg) ;

          WinDestroyWindow (hwndFrame) ;
          }
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static HMODULE hmodBitLib ;
     static USHORT  idBitmap = 1 ;
     HBITMAP        hbm ;
     HPS            hps ;
     RECTL          rcl ;

     switch (msg)
          {
          case WM_CREATE:
               if (DosLoadModule (NULL, 0, "BITLIB", &hmodBitLib))
                    {
                    WinMessageBox (HWND_DESKTOP, HWND_DESKTOP,
                                   "Cannot load BITLIB.DLL library",
                                   "ShowBit", 0, MB_OK | MB_ICONEXCLAMATION) ;
                    return 1 ;
                    }
               return 0 ;

          case WM_CHAR:
               if (  CHARMSG(&msg)->fs & KC_KEYUP ||
                   !(CHARMSG(&msg)->fs & KC_VIRTUALKEY) ||
                   !(CHARMSG(&msg)->vkey == VK_SPACE))
                         break ;

               if (++idBitmap == 10)
                    idBitmap = 1 ;

               WinInvalidateRect (hwnd, NULL, FALSE) ;
               return 0 ;

          case WM_BUTTON1DOWN:
               if (++idBitmap == 10)
                    idBitmap = 1 ;

               WinInvalidateRect (hwnd, NULL, FALSE) ;
               break ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;
               GpiErase (hps) ;

               hbm = GpiLoadBitmap (hps, hmodBitLib, idBitmap, 0L, 0L) ;

               if (hbm != NULL)
                    {
                    WinQueryWindowRect (hwnd, &rcl) ;

                    WinDrawBitmap (hps, hbm, NULL, (PPOINTL) &rcl,
                                   CLR_NEUTRAL, CLR_BACKGROUND, DBM_STRETCH) ;

                    GpiDeleteBitmap (hbm) ;               
                    }
               WinEndPaint (hps) ;
               return 0 ;

          case WM_DESTROY:
               DosFreeModule (hmodBitLib) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
