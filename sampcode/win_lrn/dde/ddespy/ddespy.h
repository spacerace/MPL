/* 'About' dialog box resource id */
#define ABOUTBOX    1
#define DLGSPY      2

#define MAXFOOLEN 200  /* Maximum string length for DebugPrintf */
#define MAXLINES   50  /* Number of lines in window */
#define DEBUG          /* Define debuging symbol */

#define FIRST(pTxt) ((pTxt)->iFirst)
#define TOP(pTxt)   (((pTxt)->iFirst + (pTxt)->iTop) % MAXLINES)
#define LAST(pTxt)  (((pTxt)->iFirst + (pTxt)->iCount-1) % MAXLINES)
#define INC(x) ((x) = ++(x) % MAXLINES)
#define DEC(x) ((x) = --(x) % MAXLINES)
#define PAGE 10
#define OFFSETX (Tdx/2)
#define OFFSETY 1

#define wprintf   DebugPrintf
#define vwprintf  vDebugPrintf


long FAR PASCAL DebugWndProc(HWND, unsigned, WORD, LONG);

/*
  HWND CreateDebugWindow (HWND,char *,BOOL);
  HWND CreateDebugWin (HWND,char *,DWORD,WORD,WORD,WORD,WORD);
*/

#define NEW

#ifdef OLD
int  DebugPrintf  (HWND,char *,...);
int  vDebugPrintf (HWND,char *, int *);
#else
int  DebugPrintf  (HWND,char *, int);
int  vDebugPrintf (HWND,char *, int);
#endif

/* 'About' command id number */
#define CMDABOUT    1

/* ID's for the SPY dialog box */
#define ID_NULL    0x0000
#define ID_PARENT  0x0009
#define ID_CAPTION 0x0007
#define ID_MODULE  0x0008
#define ID_OK      0x0005
#define ID_CANCEL  0x0006
#define ID_MOUSE   0x0001
#define ID_NC      0x0002
#define ID_ALL     0x000A

/* Menu items */
#define SPYON      0x0000
#define SPYOFF     0x0002
#define SPYSET     0x0003

/* reversed MSG minus time and pt */
typedef struct {
    LONG lParam;
    WORD wParam;
    WORD message;
    HWND hwnd;
} MSGR;
typedef MSGR *PMSGR;
typedef MSGR FAR *LPMSGR;

typedef struct {
        MSG msg;
        ATOM atom1;     /* either app or item atom */
        ATOM atom2;     /* topic item if applicable */
        union {
                WORD cf;        /* clipboard format */
                HANDLE hData;   /* local copy of data */
                WORD type;      /* ack type 0=normal 1=initiate 2=execute */
        } un;
} QEL;

