/*
 * WINDOWS SPECTRUM CONTROL
 *
 * LANGUAGE : Microsoft C 5.0
 * TOOLKIT  : Windows 2.03 SDK
 * MODEL    : Small or Medium
 * STATUS   : Operational
 *
 * 03/20/88 - Kevin P. Welch - initial creation.
 *
 */
 
#include <windows.h>
#include "spectrum.h"

/* general spectrum definitions */
#define	ID							GetWindowWord( hWnd, GWW_ID )
#define	PARENT					GetWindowWord( hWnd, GWW_HWNDPARENT )
#define	INSTANCE					GetWindowWord( hWnd, GWW_HINSTANCE )

/* spectrum specific definitions */
#define	SPECTRUM_EXTRA			12				

#define	RANGE						GetWindowWord( hWnd, 0 )
#define	TABLE						GetWindowWord( hWnd, 2 )
#define	WIDTH						GetWindowWord( hWnd, 4 )
#define	HEIGHT					GetWindowWord( hWnd, 6 )
#define	CHOICE					GetWindowWord( hWnd, 8 )
#define	CAPTURE					GetWindowWord( hWnd, 10 )

#define	SET_RANGE(x)			SetWindowWord( hWnd, 0, x )
#define	SET_TABLE(x)			SetWindowWord( hWnd, 2, x )
#define	SET_WIDTH(x)			SetWindowWord( hWnd, 4, x )
#define	SET_HEIGHT(x)			SetWindowWord( hWnd, 6, x )
#define	SET_CHOICE(x)			SetWindowWord( hWnd, 8, x )
#define	SET_CAPTURE(x)			SetWindowWord( hWnd, 10, x )

/* caret related definitions */
#define	CARET_XPOS				((CHOICE*WIDTH)+3)
#define	CARET_YPOS				(3)
#define	CARET_WIDTH				(WIDTH-6)
#define	CARET_HEIGHT			(HEIGHT-6)

/* selector related definitions */
#define	SELECTOR_XPOS			((CHOICE*WIDTH)+1)
#define	SELECTOR_YPOS			(1)
#define	SELECTOR_WIDTH			(WIDTH-1)
#define	SELECTOR_HEIGHT		(HEIGHT-1)

/**/

/*
 * RegisterSpectrum( hAppInstance ) : BOOL
 *
 *		hAppInstance		application instance handle
 *
 * This function is responsible for the definition and registration
 * of the spectrum window class.  Note that this function should
 * only be called ONCE (typically during the initialization phase
 * of the host application) within a program.
 *
 * A value of TRUE is returned if the registration operation was
 * performed sucessfully.
 *
 */

BOOL FAR PASCAL RegisterSpectrum( hAppInstance )
	HANDLE		hAppInstance;
{
	/* local variables */
	WNDCLASS		WndClass;				/* window class data structure */
 
	/* Define spectrum window class.  Note that no check is made to
	 * see if it has been previously registed!  It is up to the host
	 * application to prevent multiple registrations.
	 */
	 
	memset( &WndClass, 0, sizeof(WNDCLASS) );
	
	WndClass.lpszClassName = 	(LPSTR)"Spectrum";
	WndClass.hCursor =			LoadCursor( NULL, IDC_ARROW );
	WndClass.lpszMenuName =		(LPSTR)NULL;
	WndClass.style =				CS_HREDRAW|CS_VREDRAW;
	WndClass.lpfnWndProc =		SpectrumWndFn;
	WndClass.hInstance =			hAppInstance;
	WndClass.hIcon =				NULL;
	WndClass.cbWndExtra =		SPECTRUM_EXTRA;
	WndClass.hbrBackground =	(HBRUSH)(COLOR_WINDOW + 1 );
	
	/* register spectrum window class & return */
	return( RegisterClass( (LPWNDCLASS)&WndClass ) );

}
 
/**/

/*
 * SetSpectrum( hWnd, pwEntry ) : BOOL
 *
 *		hWnd					handle to spectrum control
 *		pwEntry				new selected entry
 *
 * This function enables the caller to define the entry provided
 * as the selected color.  The selected color will default to
 * zero if the entry provided is out of range.
 *
 * A value of TRUE is returned if a selection was sucessfully made.
 *
 */

BOOL FAR PASCAL SetSpectrum( hWnd, pwEntry )
	HWND			hWnd;
	WORD *		pwEntry;
{
	/* local variables */
	LONG FAR *		lprgbEntry;				/* rgb color list */
	
	/* update selection & redraw spectrum */
	SET_CHOICE( (*pwEntry >= RANGE) ? 0 : *pwEntry );
	InvalidateRect( hWnd, NULL, TRUE );

	/* retrieve new result & notify parent window */
	lprgbEntry = (LONG FAR *)GlobalLock( TABLE );
	SendMessage( PARENT, WM_COMMAND, ID, lprgbEntry[RANGE+CHOICE] );
	GlobalUnlock( TABLE );

	/* normal return */
	return( TRUE );

}
 
/**/

/*
 * GetSpectrum( hWnd, pwEntry ) : BOOL
 *
 *		hWnd					handle to spectrum control
 *		pwEntry				currently selected entry
 *
 * This function enables the caller to retrieve an index to
 * the currently selected color.  Using this index the caller
 * can then lookup the associated rgb color or table value.
 *
 * A value of TRUE is return if the call was successful.
 *
 */
 
BOOL FAR PASCAL GetSpectrum( hWnd, pwEntry )
	HWND			hWnd;
	WORD *		pwEntry;
{
	*pwEntry = CHOICE;
	return( TRUE );
}
 
/**/

/*
 * SetSpectrumColors( hWnd, pwRange, prgbList ) : BOOL
 *
 *		hWnd					handle to spectrum control in dialog box
 *		pwRange				number of color entries in rgb list
 *		prgbList				list of rgb colors & values for spectrum
 *
 * This function enables the caller to define a new spectrum
 * with associated lookup values for the control.  It is left
 * up to the calling routine to determine the appropriate rgb
 * colors and lookup values.  Note that the selected color is
 * always reset to the first available entry whenever a new
 * range is defined.
 *
 * A value of TRUE is returned if a new rgb color list was
 * sucessfully defined.
 *
 */
 
BOOL FAR PASCAL SetSpectrumColors( hWnd, pwRange, prgbList )
	HWND			hWnd;
	WORD *		pwRange;
	LONG *		prgbList;
{
	/* local variables */
	WORD			wEntry;					/* temporary color entry */
	HANDLE		hrgbList;				/* handle to rgb list */
	LONG FAR *	lprgbEntry;				/* pointer to rgb list */
	RECT			rectClient;				/* client area rectangle */
	
	/* release previous table from memory */
	GlobalFree( TABLE );
	
	hrgbList = GlobalAlloc(	GMEM_MOVEABLE,	sizeof(LONG)*(*pwRange)*2L);		
	if ( hrgbList ) {
			
		/* define initial rgb colors & values */
		lprgbEntry = (LONG FAR *)GlobalLock( hrgbList );		
		for ( wEntry=0; wEntry < *pwRange; wEntry++ ) {
			lprgbEntry[wEntry] = prgbList[wEntry];		
			lprgbEntry[(*pwRange)+wEntry] = prgbList[(*pwRange)+wEntry];
		}	
		GlobalUnlock( hrgbList );
			
		/* retrieve current window dimensions */
		GetClientRect( hWnd, &rectClient );

		/* re-define instance variables */
		SET_RANGE( *pwRange );
		SET_TABLE( hrgbList );
		SET_WIDTH( (rectClient.right-rectClient.left)/(*pwRange) );
		SET_HEIGHT( rectClient.bottom-rectClient.top );
		SET_CHOICE( 0 );
	
		/* update window & notify parent of new selection */
		InvalidateRect( hWnd, NULL, TRUE );
		SendMessage( PARENT, WM_COMMAND, ID, lprgbEntry[RANGE+CHOICE] );

		/* normal return */
		return( TRUE );
		
	} else
		return( FALSE );	
		
}

/**/
 
/*
 * GetSpectrumColors( hWnd, pwRange, prgbList ) : BOOL
 *
 *		hWnd					handle to spectrum control in dialog box
 *		pwRange				number of color entries in rgb list
 *		prgbList				list of rgb color values for spectrum
 *
 * This function enables the caller to retrieve the current
 * colors & values maintained by the spectrum control.  If the
 * color range has never been defined the spectrum represents
 * a set of default colors and values.
 *
 * A value of TRUE is returned if this function was successful.
 *
 */

BOOL FAR PASCAL GetSpectrumColors( hWnd, pwRange, prgbList )
	HWND			hWnd;
	WORD *		pwRange;
	LONG *		prgbList;
{
	/* local variables */
	WORD			wEntry;					/* index to color table  */
	LONG FAR *	lprgbEntry;				/* pointer to rgb value */

	/* retrieve number of colors */
	*pwRange = RANGE;
	
	/* retrieve rgb color list */
	lprgbEntry = (LONG FAR *)GlobalLock( TABLE );		
	for ( wEntry=0; wEntry < RANGE; wEntry++ ) {
		prgbList[wEntry] = lprgbEntry[wEntry];		
		prgbList[(*pwRange)+wEntry] = lprgbEntry[(*pwRange)+wEntry];
	}
	GlobalUnlock( TABLE );
	
	return( TRUE );
	
}

/**/

/*
 * SpectrumWndFn( hWnd, wMsg, wParam, lParam ) : LONG
 *
 *		hWnd					handle to spectrum window
 *		wMsg					message number
 *		wParam				single word parameter
 *		lParam				double word parameter
 *
 * This function is responsible for processing all the messages
 * which relate to the spectrum control window.  Note how the
 * code is written to avoid potential problems when re-entrancy
 * happens - this involves the use of extra bytes associated
 * with the window data structure.
 *
 * The LONG value returned by this function is the conventional
 * result of the default window procedure or the internal
 * handling of a message.
 *
 */

LONG FAR PASCAL SpectrumWndFn( hWnd, wMsg, wParam, lParam )
	HWND			hWnd;
	WORD			wMsg;
	WORD			wParam;
	LONG			lParam;
{
	/* local variables */
	LONG			lResult;					/* temporary result variable */
	
	/* initialization */
	lResult = TRUE;
	
	/* process message */
	switch( wMsg )
		{
	case WM_GETDLGCODE : /* capture all key strokes */
		lParam = DLGC_WANTARROWS;		
		break;
	case WM_CREATE : /* create pallette window */
		
		{
			/* temporary variables */
			HANDLE		hrgbList;				/* handle to rgb list */
			LONG FAR *	lprgbEntry;				/* pointer to rgb list */
			
			/* allocate space for rgb color list */
			hrgbList = GlobalAlloc( GMEM_MOVEABLE, sizeof(LONG)*16L );
			if ( hrgbList ) {
			
				/* define initial rgb color & value list - note that
				 * eight default colors are selected with the values
				 * matching each of the colors.
				 */

				lprgbEntry = (LONG FAR *)GlobalLock( hrgbList );
				lprgbEntry[0] = RGB( 0x00, 0x00, 0x00 );
				lprgbEntry[1] = RGB( 0x00, 0x00, 0xFF );
				lprgbEntry[2] = RGB( 0x00, 0xFF, 0x00 );
				lprgbEntry[3] = RGB( 0xFF, 0x00, 0x00 );
				lprgbEntry[4] = RGB( 0x00, 0xFF, 0xFF );
				lprgbEntry[5] = RGB( 0xFF, 0xFF, 0x00 );
				lprgbEntry[6] = RGB( 0xFF, 0x00, 0xFF );
				lprgbEntry[7] = RGB( 0xFF, 0xFF, 0xFF );
				lprgbEntry[8] = RGB( 0x00, 0x00, 0x00 );
				lprgbEntry[9] = RGB( 0x00, 0x00, 0xFF );
				lprgbEntry[10] = RGB( 0x00, 0xFF, 0x00 );
				lprgbEntry[11] = RGB( 0xFF, 0x00, 0x00 );
				lprgbEntry[12] = RGB( 0x00, 0xFF, 0xFF );
				lprgbEntry[13] = RGB( 0xFF, 0xFF, 0x00 );
				lprgbEntry[14] = RGB( 0xFF, 0x00, 0xFF );
				lprgbEntry[15] = RGB( 0xFF, 0xFF, 0xFF );
				GlobalUnlock( hrgbList );
			
				/* define instance variables */
				SET_RANGE( 8 );
				SET_TABLE( hrgbList );
				SET_WIDTH( ((LPCREATESTRUCT)lParam)->cx / 8 );
				SET_HEIGHT( ((LPCREATESTRUCT)lParam)->cy );
				SET_CHOICE( 0 );
				SET_CAPTURE( FALSE );
	
			} else
				DestroyWindow( hWnd );
				
		}
		
		break;
	case WM_SIZE : /* window being resized */
	
		/* redefine width & height instance variables */
		SET_WIDTH( LOWORD(lParam) / 8 );
		SET_HEIGHT( HIWORD(lParam) );

		break;
	case WM_PAINT : /* paint control window */
		
		{
			PAINTSTRUCT		Ps;					/* paint structure */
			WORD				wEntry;				/* current color entry */
			HANDLE			hBrush;				/* handle to new brush */
			HANDLE			hOldBrush;			/* handle to old brush */
			LONG FAR *		lprgbEntry;			/* pointer to rgb list */
					
			/* start paint operation */
			BeginPaint( hWnd, (LPPAINTSTRUCT)&Ps );
			
			/* iteratively paint each color patch */
			lprgbEntry = (LONG FAR *)GlobalLock( TABLE );
			for ( wEntry=0; wEntry<RANGE; wEntry++ ) {

				/* create solid brush for patch & select */
				hBrush = CreateSolidBrush( lprgbEntry[wEntry] );
				hOldBrush = SelectObject( Ps.hdc, hBrush );
	
				/* draw rectangle with brush fill */
				Rectangle(
					Ps.hdc,
					wEntry*WIDTH,
					0,
					(wEntry*WIDTH)+WIDTH,
					HEIGHT
				);

				/* unselect brush and delete */
				SelectObject( Ps.hdc, hOldBrush );
				DeleteObject( hBrush );
	
			}
			GlobalUnlock( TABLE );
					
			/* define current selection & end paint operation */
			DrawSelector( hWnd, Ps.hdc );
			EndPaint( hWnd, (LPPAINTSTRUCT)&Ps );

		}
		
		break;
	case WM_KEYDOWN : /* key being pressed */
		
		{
			/* local variables */
			HDC				hDC;					/* display context handle */
			LONG FAR *		lprgbEntry;			/* pointer to rgb list */	
		
			/* retrieve display context & unmark current selection */
			hDC = GetDC( hWnd );
			DrawSelector( hWnd, hDC );			

			/* process virtual key codes */
			switch( wParam )
				{
			case VK_HOME : /* home key */
				SET_CHOICE( 0 );
				break;
			case VK_LEFT : /* left cursor key */
				SET_CHOICE( (CHOICE > 0) ? CHOICE-1 : RANGE-1 );
				break;
			case VK_RIGHT : /* right cursor key */
			case VK_SPACE : /* space bar - move right */
				SET_CHOICE( (CHOICE < RANGE-1) ? CHOICE+1 : 0 );
				break;
			case VK_END : /* end key */
				SET_CHOICE( RANGE-1 );
				break;
			default : /* some other key */
				lResult = FALSE;
				break;
			}

			/* mark new selection & release display context */
			DrawSelector( hWnd, hDC );
			ReleaseDC( hWnd, hDC );

			/* move caret to new position */
			SetCaretPos( CARET_XPOS, CARET_YPOS );

			/* notify parent of new selection */
			lprgbEntry = (LONG FAR *)GlobalLock( TABLE );
			SendMessage(PARENT,WM_COMMAND,ID,lprgbEntry[RANGE+CHOICE]);
			GlobalUnlock( TABLE );
					
		}
		
		break;
	case WM_SETFOCUS : /* get focus - display caret */

		/* create caret & display */
		CreateCaret( hWnd, NULL, CARET_WIDTH, CARET_HEIGHT );
		SetCaretPos( CARET_XPOS, CARET_YPOS );
		ShowCaret( hWnd );

		break;
	case WM_LBUTTONDOWN : /* left button depressed - fall through */

		{
			/* local variables */
			HDC				hDC;					/* display context handle */
			LONG FAR *		lprgbEntry;			/* pointer to rgb list */	

			/* retrieve display context */
			hDC = GetDC ( hWnd );

			/* unmark old selection & mark new one */
			DrawSelector( hWnd, hDC );			
			SET_CHOICE( LOWORD(lParam)/WIDTH );
			DrawSelector( hWnd, hDC );
		
			/* release display context & move caret */
			ReleaseDC( hWnd, hDC );

			/* capture focus & move caret */
			if ( hWnd == SetFocus(hWnd) )
				SetCaretPos( CARET_XPOS, CARET_YPOS );

			/* notify parent of new selection */
			lprgbEntry = (LONG FAR *)GlobalLock( TABLE );
			SendMessage(PARENT,WM_COMMAND,ID,lprgbEntry[RANGE+CHOICE]);
			GlobalUnlock( TABLE );
			
			/* activate capture */
			SetCapture( hWnd );
			SET_CAPTURE( TRUE );			

		}
		
		break;
	case WM_MOUSEMOVE : /* mouse being moved */

		/* track mouse only if capture on */
		if ( CAPTURE ) {
		
			/* local variables */
			HDC				hDC;					/* display context handle */
			WORD				wNewChoice;			/* new mouse selection */
			LONG FAR *		lprgbEntry;			/* pointer to rgb list */	
		
			/* calculate new selection */
			wNewChoice = ( *((int*)&lParam) <= 0 ) ?
				0 :
				( LOWORD(lParam)/WIDTH >= RANGE ) ?
					RANGE - 1 :
					LOWORD(lParam) / WIDTH;
			
			/* update display if different */
			if ( wNewChoice != CHOICE ) {

				/* retrieve display context */
				hDC = GetDC ( hWnd );

				/* unmark old selection & mark new one */
				DrawSelector( hWnd, hDC );			
				SET_CHOICE( wNewChoice );
				DrawSelector( hWnd, hDC );
		
				/* release display context & move caret */
				ReleaseDC( hWnd, hDC );
				SetCaretPos( CARET_XPOS, CARET_YPOS );
			
				/* notify parent of new selection */
				lprgbEntry = (LONG FAR *)GlobalLock( TABLE );
				SendMessage(PARENT,WM_COMMAND,ID,lprgbEntry[RANGE+CHOICE]);
				GlobalUnlock( TABLE );
			
			}
			
		}
		
		break;
	case WM_LBUTTONUP : /* left button released */

		/* release capture if active */
		if ( CAPTURE ) {
			SET_CAPTURE( FALSE );
			ReleaseCapture();
		}			

		break;
	case WM_KILLFOCUS : /* kill focus - hide caret */
		DestroyCaret();
		break;
	case WM_DESTROY : /* window being destroyed */
		GlobalFree( TABLE );		
		break;
	default : /* default window message processing */
		lResult = DefWindowProc( hWnd, wMsg, wParam, lParam );
		break;
	}
	
	/* return final result */
	return( lResult );

}

/**/

/*
 * DrawSelector( hWnd, hDC )
 *
 *		hWnd			window handle
 *		hDC			handle to display context
 *
 *	This function is responsible for drawing the selector
 * which surrounds the active color patch.  This drawing
 * process involves the use of the R2_NOT operator to
 * simplify the drawing & re-drawing process.
 *
 */
 
static DrawSelector( hWnd, hDC )
	HWND			hWnd;
	HDC			hDC;
{
	/* local variables */
	HANDLE		hOldPen;					/* old pen */
	WORD			wOldROP2;				/* old raster op code */
	WORD			wOldBkMode;				/* old background mode */
	
	/* setup display context */
	wOldROP2 = SetROP2( hDC, R2_NOT );
	wOldBkMode = SetBkMode( hDC, TRANSPARENT );

	hOldPen = SelectObject( hDC, CreatePen(0,2,RGB(0,0,0)) );
	
	/* draw selector rectangle */
	Rectangle(
		hDC,
		SELECTOR_XPOS,
		SELECTOR_YPOS,
		SELECTOR_XPOS+SELECTOR_WIDTH,
		SELECTOR_YPOS+SELECTOR_HEIGHT
	);

	DeleteObject( SelectObject(hDC,hOldPen) );

	/* restore display context */
	SetBkMode( hDC, wOldBkMode );
	SetROP2( hDC, wOldROP2 );

}

