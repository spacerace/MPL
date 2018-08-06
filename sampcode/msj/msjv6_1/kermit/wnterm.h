/* 
 * WNTERM header file
 *
 * Written by
 * William S. Hall
 * 3665 Benton Street, #66
 * Santa Clara, CA 95051
*/

/* used to define global variables in only one place */
#ifndef EXTERN
#define EXTERN extern
#endif

/* declarations for shared functions */
/* from wnterm.c */
LONG FAR PASCAL MainWndProc(HWND,unsigned,WORD,LONG);
LONG FAR PASCAL MainWndSubProc(HWND,unsigned,WORD,LONG);

/* from wntint.c */
BOOL FAR InitProgram(HANDLE,HANDLE,LPSTR,int);
void FAR WndCreate(HWND hWnd, LPCREATESTRUCT pCS);

/* from wntfns.c */
int NEAR ProcessComm(void);
void NEAR WndCommand(HWND, WORD, LONG);
void NEAR MainWndPaint(HWND, LPPAINTSTRUCT);
void FAR _cdecl dbs(const char *fmt, ...);

/* from wntmsc.c */
void ShowMessage(HWND, int);
BOOL FAR PASCAL AboutBoxProc(HWND,unsigned,WORD,LONG);
BOOL FAR PASCAL SetCommParams(HWND,unsigned,WORD,LONG);

/* virtual key value of scroll key */
#define VK_SCROLL 0x91

/* string manifests */
#define IDS_APPNAME		100
#define IDS_ABOUT		101
#define IDS_WINTITLE		102
#define IDS_ICONSTRING		103
#define IDS_OFFLINE		105
#define IDS_ONLINE		106
#define IDS_PORTSECTION		109
#define IDS_CANNOTOPENFIRSTPORT	111
#define IDS_CANNOTOPENANYPORT	112
#define IDS_COMMSETERROR	113
#define IDS_SMALLFONT		114
#define IDS_FONTFACE		115
#define IDS_CANNOTOPENPORT	116
#define IDS_COM1		117
#define IDS_COM2		118
#define IDS_COM3		119
#define IDS_COM4		120

/* menu manifests */
#define IDM_ABOUT		200
#define IDM_ONLINE		201
#define IDM_OFFLINE		202
#define IDM_COMM		203
#define IDM_CLEAR		204
#define IDM_LOCAL		205

/* comm port manifests */
#define MAXCOMMPORTS	4
#define INQUESIZE	256
#define OUTQUESIZE	256
#define BUFSIZE INQUESIZE

/* variables for the windows */
EXTERN struct TTYWND MWnd;	/* private data for window */

/* strings */
EXTERN char szAppName[10];	/* class name of main window */
EXTERN char szIconTitle[5];	/* icon string */
EXTERN char szOffLine[10];	/* menu strings */
EXTERN char szOnLine[10];

/* comm port variables */
EXTERN int cid;			/* comm port id */
EXTERN DCB CommData;		/* comm data structure */

/* misc variables */
EXTERN HANDLE hInst;		/* instance handle */
EXTERN FARPROC fpMainWndProc;	/* subclass window procedure pointer */
EXTERN int LineState;		/* on/off line state flag */
EXTERN BOOL ScrollLock;		/* Scroll Lock key state */
EXTERN BYTE Buffer[BUFSIZE + 1];  /* communications buffer */
EXTERN int Buflen;		/* length of buffer */
EXTERN BOOL smallfont;		/* create and use a tiny font */
EXTERN HFONT hfont;		/* handle to the font */

