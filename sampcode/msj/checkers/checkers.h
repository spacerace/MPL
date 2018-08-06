/*--------------------------------------
   CHECKERS.H header file, Version 0.40
  --------------------------------------*/

          // Some common constants

#define BLACK                       0
#define WHITE                       1
#define NORM                        0
#define KING                        1

#define LEVEL_SIMPLE                0
#define LEVEL_BEGINNER              1
#define LEVEL_INTERMEDIATE          2
#define LEVEL_ADVANCED              3

          // Structure for representing board

typedef struct
     {
     ULONG ulBlack ;
     ULONG ulWhite ;
     ULONG ulKing ;
     }
     BOARD ;

typedef BOARD *PBOARD ;

#define CKR_BOARD_DEFINED

          // Structure for a new game

typedef struct
     {
     BOARD brd ;
     HWND  hwndBlack ;
     HWND  hwndWhite ;
     HWND  hwndBoard ;
     HWND  hwndJudge ;
     HWND  hwndClient ;
     SHORT sBottom ;
     SHORT sLevel ;
     }
     NEWGAME ;

typedef NEWGAME *PNEWGAME ;

typedef struct
     {
     BOARD brd ;
     SHORT sColor ;
     SHORT sLevel ;
     }
     GAMEDLG ;

typedef GAMEDLG *PGAMEDLG ;

typedef struct
     {
     BOARD brd ;
     SHORT sBottom ;
     }
     SETUPDLG ;

typedef SETUPDLG *PSETUPDLG ;

          // Resource ID

#define ID_RESOURCE                 1

          // Pointer IDs

#define IDP_UPHAND                  2
#define IDP_DNHAND                  3

          // Menu IDs

#define IDM_NEWGAME                 1
#define IDM_ABOUT                   2

#define IDM_COLOR_BACKGROUND       11
#define IDM_COLOR_BLACK_SQUARE     12
#define IDM_COLOR_WHITE_SQUARE     13
#define IDM_COLOR_BLACK_PIECE      14
#define IDM_COLOR_WHITE_PIECE      15
#define IDM_COLOR_STANDARD         16

          // Dialog Box ID's

#define IDD_NEWGAME_DLG             1
#define IDD_ABOUT_DLG               2
#define IDD_COLOR_DLG               3
#define IDD_SETUP_DLG               4

#define IDD_HEADING                10
#define IDD_COLOR                  20

#define IDD_BLACK                 100
#define IDD_WHITE                 101

#define IDD_SIMPLE                110
#define IDD_BEGINNER              111
#define IDD_INTERMEDIATE          112
#define IDD_ADVANCED              113

#define IDD_SETUP                 120
#define IDD_BOTTOM                121
#define IDD_TOP                   122

          // User-Defined Messages

               // Messages from Client to Judge to Board and Players

#define WM_NEW_GAME                     (WM_USER + 1)

               // Messages from Client to Board

#define WM_TELL_BOARD_COLOR_DIALOG      (WM_USER + 10)
#define WM_TELL_BOARD_STANDARD_COLORS   (WM_USER + 11)

               // Messages from Judge to Players

#define WM_JUDGE_SAYS_MAKE_MOVE         (WM_USER + 20)

               // Messages from Judge to Board

#define WM_JUDGE_SAYS_SHOW_HOURGLASS    (WM_USER + 30)
#define WM_JUDGE_SAYS_SHOW_ARROW        (WM_USER + 31)
#define WM_JUDGE_SAYS_MOVE_PIECE        (WM_USER + 32)
#define WM_JUDGE_SAYS_KING_PIECE        (WM_USER + 33)
#define WM_JUDGE_SAYS_REMOVE_PIECE      (WM_USER + 34)

               // Messages from Board to Judge

#define WM_QUERY_JUDGE_CURRENT_BOARD    (WM_USER + 40)
#define WM_QUERY_JUDGE_PICKUP_PIECE     (WM_USER + 41)
#define WM_QUERY_JUDGE_IF_KING          (WM_USER + 42)
#define WM_QUERY_JUDGE_PUTDOWN_PIECE    (WM_USER + 43)
#define WM_QUERY_JUDGE_CONTINUE_MOVE    (WM_USER + 44)
#define WM_TELL_JUDGE_BOARD_MOVE_ENDED  (WM_USER + 45)
#define WM_TELL_JUDGE_PIECE_MOVED       (WM_USER + 46)

               // Messages from Strat to Judge

#define WM_TELL_JUDGE_STRAT_MOVE_ENDED  (WM_USER + 50)

               // Messages from Judge to Client

#define WM_TELL_CLIENT_GAME_OVER        (WM_USER + 60)

               // Messages from Strat to Client

#define WM_TELL_CLIENT_NO_MEMORY        (WM_USER + 70)
