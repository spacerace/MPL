/*--------------------------------------
   CHECKERS.H header file, version 0.10
  --------------------------------------*/

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

#define ID_RESOURCE                 1

#define IDM_BOTTOM                  1
#define IDM_ABOUT                   2

#define IDM_COLOR_BACKGROUND       11
#define IDM_COLOR_BLACK_SQUARE     12
#define IDM_COLOR_WHITE_SQUARE     13
#define IDM_COLOR_BLACK_PIECE      14
#define IDM_COLOR_WHITE_PIECE      15
#define IDM_COLOR_STANDARD         16

#define IDD_ABOUT_DLG               1
#define IDD_COLOR_DLG               2

#define IDD_HEADING                10
#define IDD_COLOR                  20
