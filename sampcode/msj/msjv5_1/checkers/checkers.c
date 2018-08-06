/*--------------------------------------------------------------------
   CHECKERS.C source code file, version 0.10 (draws board and pieces)
              (c) 1990, Charles Petzold
  --------------------------------------------------------------------*/

#define INCL_WIN
#include <os2.h>
#include "checkers.h"
#include "ckrdraw.h"

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2) ;
MRESULT EXPENTRY AboutDlgProc  (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2) ;

HAB  hab ;

int main (void)
     {
     static CHAR  szClientClass[] = "Checkers" ;
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
                                     &flFrameFlags, szClientClass, NULL,
                                     0L, NULL, ID_RESOURCE, &hwndClient) ;

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static BOARD brd = { 0x00000FFF, 0xFFF00000, 0x00000000 } ;
     static HPS   hps ;
     static HWND  hwndMenu ;
     static SHORT sBottom = BLACK ;

     switch (msg)
          {
          case WM_CREATE:
               hwndMenu = WinWindowFromID (
                              WinQueryWindow (hwnd, QW_PARENT, FALSE),
                              FID_MENU) ;

               hps = CkdCreatePS (hwnd) ;
               return 0 ;

          case WM_SIZE:
               CkdResizePS (hps, hwnd) ;
               CkdDestroyPieces () ;
               CkdCreatePieces (hps) ;
               return 0 ;

          case WM_PAINT:
               WinBeginPaint (hwnd, hps, NULL) ;

               CkdDrawWindowBackground (hps, hwnd) ;
               CkdDrawWholeBoard (hps) ;
               CkdDrawAllPieces (hps, &brd, sBottom) ;

               WinEndPaint (hps) ;
               return 0 ;

          case WM_COMMAND:
               switch (COMMANDMSG (&msg)->cmd)
                    {
                    case IDM_BOTTOM:
                         WinSendMsg (hwndMenu, MM_SETITEMATTR,
                                     MPFROM2SHORT (IDM_BOTTOM, TRUE),
                                     MPFROM2SHORT (MIA_CHECKED,
                                        sBottom ? MIA_CHECKED : 0)) ;
                         sBottom ^= 1 ;
                         WinInvalidateRect (hwnd, NULL, FALSE) ;
                         return 0 ;

                    case IDM_ABOUT:
                         WinDlgBox (HWND_DESKTOP, hwnd, AboutDlgProc,
                                    NULL, IDD_ABOUT_DLG, NULL) ;
                         return 0 ;

                    case IDM_COLOR_BACKGROUND:
                    case IDM_COLOR_BLACK_SQUARE:
                    case IDM_COLOR_WHITE_SQUARE:
                    case IDM_COLOR_BLACK_PIECE:
                    case IDM_COLOR_WHITE_PIECE:
                         if (!WinDlgBox (HWND_DESKTOP, hwnd, ColorDlgProc,
                                         NULL, IDD_COLOR_DLG,
                                         &(COMMANDMSG (&msg)->cmd)))
                              return 0 ;

                         if (COMMANDMSG (&msg)->cmd == IDM_COLOR_BLACK_PIECE ||
                             COMMANDMSG (&msg)->cmd == IDM_COLOR_WHITE_PIECE)
                              {
                              CkdDestroyPieces () ;
                              CkdCreatePieces (hps) ;
                              }
                         WinInvalidateRect (hwnd, NULL, FALSE) ;
                         return 0 ;

                    case IDM_COLOR_STANDARD:
                         CkdSetStandardColors () ;
                         CkdDestroyPieces () ;
                         CkdCreatePieces (hps) ;
                         WinInvalidateRect (hwnd, NULL, FALSE) ;
                         return 0 ;
                    }
               break ;

          case WM_DESTROY:
               CkdDestroyPieces () ;
               CkdDestroyPS (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
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
          }
     return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
     }
