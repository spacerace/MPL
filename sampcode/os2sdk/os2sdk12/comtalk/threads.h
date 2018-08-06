/*
    threads.h -- Thread parameters, synchronization entry points
    Created by Microsoft Corporation, 1989
*/
/*
    Additions for second thread
*/
#define	BUFSIZE		2056		/* Size of TypeAhead buffer */
#define	RASIZE		1		/* # Lines in Readahead buf */
#define	STACKSIZE	8192		/* Size of Thread Stacks    */
#define	TIMEOUT		100L		/* Timeout value on PutChar */
#define MAXTIMEOUT	-1L		/* Other timeout values     */
#define	WM_AVIOUPDATE	WM_USER		/* Message to update screen */
#define WM_MSGBOX	WM_USER + 1
/*
    Message Box Error definitions
*/
#define	MBE_OPENPORT	(MPARAM) 0
#define	MBE_WRITEPORT	(MPARAM) 1
#define	MBE_COMREAD	(MPARAM) 2
#define	MBE_QUEUEFULL	(MPARAM) 3
#define	MBE_NUMMSGS	4
extern	char aszMessage[MBE_NUMMSGS][MAXLINELEN];
/*
    Exportable routines
*/
void ThdInitialize(HWND hWnd, COM Term);
void ThdTerminate(void);
void ThdDoBreak(void);
int  ThdPutChar(char ch);
int  ThdPutString(char s[], int n);
void ThdReset(void);
extern BOOL fNoUpdate;
