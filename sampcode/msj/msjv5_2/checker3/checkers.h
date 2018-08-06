/*--------------------------------------
   CHECKERS.H header file, Version 0.30
  --------------------------------------*/

          // Constants and structures

#define BLACK                       0
#define WHITE                       1
#define NORM                        0
#define KING                        1

typedef struct
     {
     ULONG ulBlack ;
     ULONG ulWhite ;
     ULONG ulKing ;
     }
     BOARD ;

typedef BOARD FAR *PBOARD ;

          // Menu ID

#define ID_RESOURCE                 1

          // Pointer ID's

#define IDP_UPHAND                  2
#define IDP_DNHAND                  3

          // Menu ID's

#define IDM_NEWGAME                 1
#define IDM_BOTTOM                  2
#define IDM_ABOUT                   3

#define IDM_COLOR_BACKGROUND       11
#define IDM_COLOR_BLACK_SQUARE     12
#define IDM_COLOR_WHITE_SQUARE     13
#define IDM_COLOR_BLACK_PIECE      14
#define IDM_COLOR_WHITE_PIECE      15
#define IDM_COLOR_STANDARD         16

          // Dialog Box ID's

#define IDD_ABOUT_DLG               1
#define IDD_COLOR_DLG               2

#define IDD_HEADING                10
#define IDD_COLOR                  20     // 11 --> 20   EJM  1/10/90

          // User-Defined Messages

               // Messages from ClientWndProc to JudgeWndProc

#define WM_TELL_JUDGE_BOARD_HANDLE      (WM_USER + 1)
#define WM_TELL_JUDGE_NEW_GAME          (WM_USER + 2)

               // Messages from ClientWndProc to BoardWndProc

#define WM_TELL_BOARD_JUDGE_HANDLE      (WM_USER + 10)
#define WM_TELL_BOARD_NEW_ORIENTATION   (WM_USER + 11)
#define WM_TELL_BOARD_COLOR_DIALOG      (WM_USER + 12)
#define WM_TELL_BOARD_STANDARD_COLORS   (WM_USER + 13)

               // Messages from Judge to Board

#define WM_JUDGE_SAYS_RESET_BOARD       (WM_USER + 20)
#define WM_JUDGE_SAYS_MOVE_BLACK        (WM_USER + 21)
#define WM_JUDGE_SAYS_MOVE_WHITE        (WM_USER + 22)
#define WM_JUDGE_SAYS_REMOVE_PIECE      (WM_USER + 23)

               // Messages from Board to Judge

#define WM_QUERY_JUDGE_CURRENT_BOARD    (WM_USER + 30)
#define WM_QUERY_JUDGE_PICKUP_PIECE     (WM_USER + 31)
#define WM_QUERY_JUDGE_IF_KING          (WM_USER + 32)
#define WM_QUERY_JUDGE_PUTDOWN_PIECE    (WM_USER + 33)
#define WM_QUERY_JUDGE_CONTINUE_MOVE    (WM_USER + 34)
#define WM_TELL_JUDGE_BOARD_MOVE_ENDED  (WM_USER + 35)
