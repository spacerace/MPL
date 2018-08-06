/*------------------------------------------------------------------------
   CHECKERS.C -- OS/2 Presentation Manager Checkers Program, Version 0.40
                 (c) 1990, Charles Petzold
                 Microsoft Systems Journal, September 1990
  ------------------------------------------------------------------------*/

#define INCL_WIN
#include <os2.h>
#include <stdio.h>
#include "checkers.h"

MRESULT EXPENTRY ClientWndProc  (HWND, USHORT, MPARAM, MPARAM) ;
MRESULT EXPENTRY JudgeWndProc   (HWND, USHORT, MPARAM, MPARAM) ;
MRESULT EXPENTRY BoardWndProc   (HWND, USHORT, MPARAM, MPARAM) ;
MRESULT EXPENTRY StratWndProc   (HWND, USHORT, MPARAM, MPARAM) ;
MRESULT EXPENTRY NewGameDlgProc (HWND, USHORT, MPARAM, MPARAM) ;
MRESULT EXPENTRY AboutDlgProc   (HWND, USHORT, MPARAM, MPARAM) ;
MRESULT EXPENTRY SetupDlgProc   (HWND, USHORT, MPARAM, MPARAM) ;
MRESULT EXPENTRY SetupCntlProc  (HWND, USHORT, MPARAM, MPARAM) ;

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
                                     0L, 0, ID_RESOURCE, &hwndClient) ;

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static CHAR    szJudgeClass   [] = "Checkers.Judge",
                    szStratClass   [] = "Checkers.Strat",
                    szBoardClass   [] = "Checkers.Board",
                    szSetupClass   [] = "Checkers.Setup",
                    *apchColor    [2] = { "Black", "White" },
                    *apchCongrats [3] = { "Congratulations!", "Sorry!", ""},
                    szBuffer     [80] ;
     static GAMEDLG gamedlg ;
     static HWND    hwndJudge, hwndStrat, hwndBoard, hwndMenu ;
     static NEWGAME newgame ;
     SHORT          sWinColor, sCongrats ;

     switch (msg)
          {
          case WM_CREATE:
                                   // Register Judge class & create window

               WinRegisterClass (hab, szJudgeClass, JudgeWndProc,
                                 CS_SIZEREDRAW, 0) ;

               hwndJudge = WinCreateWindow (HWND_OBJECT, szJudgeClass, NULL,
                                            0L, 0, 0, 0, 0,
                                            hwnd, HWND_BOTTOM, 1, NULL, NULL) ;

                                   // Register Strat class & create window

               WinRegisterClass (hab, szStratClass, StratWndProc,
                                 CS_SIZEREDRAW, 0) ;

               hwndStrat = WinCreateWindow (HWND_OBJECT, szStratClass, NULL,
                                            0L, 0, 0, 0, 0,
                                            hwnd, HWND_BOTTOM, 1, NULL, NULL) ;

                                   // Register Board class & create window

               WinRegisterClass (hab, szBoardClass, BoardWndProc,
                                 CS_SIZEREDRAW, 0) ;

               hwndBoard = WinCreateWindow (hwnd, szBoardClass, NULL,
                                            WS_VISIBLE, 0, 0, 0, 0,
                                            hwnd, HWND_BOTTOM, 2, NULL, NULL) ;

                                   // Register Setup class

               WinRegisterClass (hab, szSetupClass, SetupCntlProc,
                                 CS_SIZEREDRAW, sizeof (PSETUPDLG)) ;

                                   // Begin a new game

               gamedlg.sColor      = IDD_BLACK ;
               gamedlg.sLevel      = IDD_INTERMEDIATE ;

               newgame.brd.ulBlack = 0x00000FFF ;
               newgame.brd.ulWhite = 0xFFF00000 ;
               newgame.brd.ulKing  = 0x00000000 ;

               newgame.hwndBlack   = hwndBoard ;
               newgame.hwndWhite   = hwndStrat ;
               newgame.hwndBoard   = hwndBoard ;
               newgame.hwndJudge   = hwndJudge ;
               newgame.hwndClient  = hwnd ;
               newgame.sBottom     = BLACK ;
               newgame.sLevel      = LEVEL_INTERMEDIATE ;

               WinSendMsg (hwndJudge, WM_NEW_GAME,
                           &newgame, NULL) ;

                                   // Obtain handle of menu window

               hwndMenu = WinWindowFromID (
                              WinQueryWindow (hwnd, QW_PARENT, FALSE),
                              FID_MENU) ;
               return 0 ;

          case WM_TELL_CLIENT_GAME_OVER:
               sWinColor = SHORT1FROMMP (mp1) ;

               if (newgame.hwndBlack == hwndBoard)
                    {
                    if (sWinColor == BLACK)
                         sCongrats = 0 ;
                    else
                         sCongrats = 1 ;
                    }
               else if (newgame.hwndWhite == hwndBoard)
                    {
                    if (sWinColor == WHITE)
                         sCongrats = 0 ;
                    else
                         sCongrats = 1 ;
                    }
               else
                    sCongrats = 2 ;

               sprintf (szBuffer, "Game over!  %s wins.  %s",
                        apchColor [sWinColor], apchCongrats [sCongrats]) ;

               WinMessageBox (HWND_DESKTOP, hwnd, szBuffer, "CHECKERS", 0,
                              MB_ICONEXCLAMATION | MB_OK) ;

               WinSendMsg (hwnd, WM_COMMAND, MPFROMSHORT (IDM_NEWGAME), NULL) ;

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

                         gamedlg.brd.ulBlack = 0 ;
                         gamedlg.brd.ulWhite = 0 ;
                         gamedlg.brd.ulKing  = 0 ;

                         if (WinDlgBox (HWND_DESKTOP, hwnd, NewGameDlgProc,
                                        0, IDD_NEWGAME_DLG, &gamedlg))
                              {
                              newgame.brd = gamedlg.brd ;

                              if (newgame.brd.ulBlack == 0 &&
                                  newgame.brd.ulWhite == 0)
                                   {
                                   newgame.brd.ulBlack = 0x00000FFF ;
                                   newgame.brd.ulWhite = 0xFFF00000 ;
                                   newgame.brd.ulKing  = 0x00000000 ;
                                   }

                              switch (gamedlg.sColor)
                                   {
                                   case IDD_BLACK:
                                        newgame.hwndBlack = hwndBoard ;
                                        newgame.hwndWhite = hwndStrat ;
                                        newgame.sBottom   = BLACK ;
                                        break ;

                                   case IDD_WHITE:
                                        newgame.hwndBlack = hwndStrat ;
                                        newgame.hwndWhite = hwndBoard ;
                                        newgame.sBottom   = WHITE ;
                                        break ;
                                   }

                              newgame.sLevel = LEVEL_SIMPLE + gamedlg.sLevel -
                                               IDD_SIMPLE ;

                              WinSendMsg (hwndJudge, WM_NEW_GAME,
                                          &newgame, NULL) ;
                              }

                         return 0 ;

                    case IDM_ABOUT:
                         WinDlgBox (HWND_DESKTOP, hwnd, AboutDlgProc,
                                    0, IDD_ABOUT_DLG, NULL) ;
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

                    case IDM_COLOR_STANDARD:
                         WinSendMsg (hwndBoard, WM_TELL_BOARD_STANDARD_COLORS,
                                     NULL, NULL) ;
                         return 0 ;
                    }
               break ;

          case WM_TELL_CLIENT_NO_MEMORY:
               WinMessageBox (HWND_DESKTOP, hwnd,
                              "Cannot allocate enough memory for analyzing "
                              "best move.  The program will be terminated.",
                              "CHECKERS", 0, MB_OK | MB_CRITICAL) ;

               WinPostMsg (hwnd, WM_QUIT, NULL, NULL) ;
               return 0 ;
          }

     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }

MRESULT EXPENTRY NewGameDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static GAMEDLG  gamedlgLocal ;
     static PGAMEDLG pgamedlg ;
     static SETUPDLG setupdlg ;

     switch (msg)
          {
          case WM_INITDLG:
               pgamedlg = PVOIDFROMMP (mp2) ;

               gamedlgLocal = *pgamedlg ;

               WinSendDlgItemMsg (hwnd, gamedlgLocal.sColor, BM_SETCHECK,
                                  MPFROM2SHORT (TRUE, 0), NULL) ;

               WinSendDlgItemMsg (hwnd, gamedlgLocal.sLevel, BM_SETCHECK,
                                  MPFROM2SHORT (TRUE, 0), NULL) ;

               WinSetFocus (HWND_DESKTOP,
                            WinWindowFromID (hwnd, gamedlgLocal.sColor)) ;

               return MRFROMSHORT (1) ;

          case WM_CONTROL:
               switch (SHORT1FROMMP (mp1))
                    {
                    case IDD_BLACK:
                    case IDD_WHITE:
                         gamedlgLocal.sColor = SHORT1FROMMP (mp1) ;
                         return 0 ;

                    case IDD_SIMPLE:
                    case IDD_BEGINNER:
                    case IDD_INTERMEDIATE:
                    case IDD_ADVANCED:
                         gamedlgLocal.sLevel = SHORT1FROMMP (mp1) ;
                         return 0 ;
                    }
               break ;

          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case DID_OK:
                         pgamedlg->brd    = gamedlgLocal.brd ;
                         pgamedlg->sColor = gamedlgLocal.sColor ;
                         pgamedlg->sLevel = gamedlgLocal.sLevel ;

                         WinDismissDlg (hwnd, TRUE) ;
                         return 0 ;

                    case DID_CANCEL:
                         WinDismissDlg (hwnd, FALSE) ;
                         return 0 ;

                    case IDD_SETUP:
                         setupdlg.brd = gamedlgLocal.brd ;

                         if (gamedlgLocal.sColor == IDD_BLACK)
                              setupdlg.sBottom = BLACK ;
                         else
                              setupdlg.sBottom = WHITE ;

                         if (WinDlgBox (HWND_DESKTOP, hwnd, SetupDlgProc,
                                        0, IDD_SETUP_DLG, &setupdlg))
                              {
                              gamedlgLocal.brd = setupdlg.brd ;
                              }
                         return 0 ;
                    }
               break ;
          }
     return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
     }

MRESULT EXPENTRY SetupDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static CHAR      *pachColor[] = { "[Black]", "[White]" } ;
     static HWND      hwndSetupCntl ;
     static PSETUPDLG psetupdlg ;
     static SETUPDLG  setupdlgLocal ;

     switch (msg)
          {
          case WM_INITDLG:
               hwndSetupCntl = WinWindowFromID (hwnd, IDD_SETUP) ;
               psetupdlg     = PVOIDFROMMP (mp2) ;
               setupdlgLocal = *psetupdlg ;

               WinSetWindowPtr (hwndSetupCntl, 0, &setupdlgLocal) ;

               WinSetDlgItemText (hwnd, IDD_TOP,
                         pachColor [psetupdlg->sBottom == BLACK ? 1 : 0]) ;

               WinSetDlgItemText (hwnd, IDD_BOTTOM,
                         pachColor [psetupdlg->sBottom == BLACK ? 0 : 1]) ;

               return 0 ;

          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case DID_OK:
                         psetupdlg->brd = setupdlgLocal.brd ;

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
