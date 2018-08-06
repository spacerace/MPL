/*

Figure 16
=========

*/

/*
 * DDE MAZE - FUNCTION DEFINITIONS
 *
 * LANGUAGE 	: Microsoft C
 * MODEL	: Small
 * STATUS	: Operational
 *
 * 09/22/87 - Kevin P. Welch - initial creation.
 *
 */
 
VOID		BounceBall( HWND );
WORD		DestroyMaze( WORD );
ATOM		BumpGlobalAtom( ATOM );
BOOL		FindLink( WORD *, HWND );
BOOL		Advise( HWND, HWND, ATOM );
HWND		CreateMaze( HANDLE, HANDLE, WORD );
BOOL		Transmit( HWND, HWND, WORD, ATOM, BOOL, LONG, LONG );

LPSTR FAR PASCAL	lstrcpy( LPSTR, LPSTR );
LONG FAR PASCAL	MazeWndFn( HWND, WORD, WORD, LONG );
