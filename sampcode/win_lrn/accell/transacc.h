
#define TRANSACCELMENU     1
#define TRANSACCELMENU1	   1
#define TRANSACCELMENU2	   2

#define ID_NEXT 	  100
#define ID_PREV 	  101

/* function declarations */

BOOL WinInit (HANDLE);
long FAR PASCAL TransAccelWindowProc(HWND,unsigned,WORD,LONG);
void TransAccelMenuProc (HWND,WORD);


