extern void  FAR PASCAL _lclose( int );
extern WORD  FAR PASCAL _lread( int, LPSTR, int );
long FAR PASCAL WndProc( HWND, unsigned, WORD, LONG );
void FAR PASCAL DummyEntry (  );
void IconTestCommand ( HWND, WORD );
 
#define     CMD_GETICON  1
