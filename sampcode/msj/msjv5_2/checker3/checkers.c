/*------------------------------------------------------------------------
   CHECKERS.C -- OS/2 Presentation Manager Checkers Program, Version 0.30
                 (c) 1990, Charles Petzold
  ------------------------------------------------------------------------*/

#define INCL_WIN
#include <os2.h>
#include "checkers.h"

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY JudgeWndProc  (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY BoardWndProc  (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY AboutDlgProc  (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);

HAB hab ;

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
     static CHAR  szJudgeClass[] = "Checkers.Judge",
                  szBoardClass[] = "Checkers.Board" ;
     static HWND  hwndJudge, hwndBoard, hwndMenu ;
     static SHORT sBottom = BLACK ;

     switch (msg)
          {
          case WM_CREATE:
                                   // Register Judge class & create window

               WinRegisterClass (hab, szJudgeClass, JudgeWndProc,
                                 CS_SIZEREDRAW, 0) ;

               hwndJudge = WinCreateWindow (HWND_OBJECT, szJudgeClass, NULL,
                                            0L, 0, 0, 0, 0,
                                            hwnd, HWND_BOTTOM, 1, NULL, NULL) ;

                                   // Register Board class & create window

               WinRegisterClass (hab, szBoardClass, BoardWndProc,
                                 CS_SIZEREDRAW, 0) ;

               hwndBoard = WinCreateWindow (hwnd, szBoardClass, NULL,
                                            WS_VISIBLE, 0, 0, 0, 0,
                                            hwnd, HWND_BOTTOM, 2, NULL, NULL) ;

                                   // Inform windows of each other's handles

               WinSendMsg (hwndJudge, WM_TELL_JUDGE_BOARD_HANDLE,
                           MPFROMHWND (hwndBoard), NULL) ;

               WinSendMsg (hwndBoard, WM_TELL_BOARD_JUDGE_HANDLE,
                           MPFROMHWND (hwndJudge), NULL) ;

                                   // Begin a new game

               WinSendMsg (hwndJudge, WM_TELL_JUDGE_NEW_GAME,
                           NULL, NULL) ;

                                   // Obtain handle of menu window

               hwndMenu = WinWindowFromID (
                              WinQueryWindow (hwnd, QW_PARENT, FALSE),
                              FID_MENU) ;
               return 0 ;

          case WM_SIZE:
                                   // Resize Board window

               WinSetWindowPos (hwndBoard, NULL, 0, 0,
                                SHORT1FROMMP (mp2), SHORT2FROMMP (mp2),
                                SWP_MOVE | SWP_SIZE) ;
               return 0 ;

          case WM_CHAR:
                                   // Send keystrokes to Board window

               return WinSendMsg (hwndBoard, WM_CHAR, mp1, mp2) ;

          case WM_COMMAND:
                                   // Process menu commands

               switch (COMMANDMSG (&msg)->cmd)
                    {
                    case IDM_NEWGAME:
                         WinSendMsg (hwndJudge, WM_TELL_JUDGE_NEW_GAME,
                                     NULL, NULL) ;
                         return 0 ;

                    case IDM_BOTTOM:
                         WinSendMsg (hwndMenu, MM_SETITEMATTR,
                                     MPFROM2SHORT (IDM_BOTTOM, TRUE),
                                     MPFROM2SHORT (MIA_CHECKED,
                                        sBottom ? MIA_CHECKED : 0)) ;
                         sBottom ^= 1 ;
                         WinSendMsg (hwndBoard, WM_TELL_BOARD_NEW_ORIENTATION,
                                     MPFROMSHORT (sBottom), NULL) ;
                         return 0 ;

                    case IDM_ABOUT:
                         WinDlgBox (HWND_DESKTOP, hwnd, AboutDlgProc,
                                    NULL, IDD_ABOUT_DLG, NULL) ;
                         return 0 ;

                    case IDM_COLOR_BACKGROUND:
                    case IDM_COLOR_BLACK_SQUARE:
                    case IDM_COLOR_WHITE_SQUARE:
                         WinSendMsg (hwndBoard, WM_TELL_BOARD_COLOR_DIALOG,
                                     MPFROMP (&COMMANDMSG (&msg)->cmd),
                                     MPFROMSHORT (FALSE)) ;
                         return 0 ;

                    case IDM_COLOR_BLACK_PIECE:
                    case IDM_COLOR_WHITE_PIECE:
                         WinSendMsg (hwndBoard, WM_TELL_BOARD_COLOR_DIALOG,
                                     MPFROMP (&COMMANDMSG (&msg)->cmd),
                                     MPFROMSHORT (TRUE)) ;
                         return 0 ;
                         return 0 ;

                    case IDM_COLOR_STANDARD:
                         WinSendMsg (hwndBoard, WM_TELL_BOARD_STANDARD_COLORS,
                                     NULL, NULL) ;
                         return 0 ;
                    }
               break ;
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
