/*
Microsoft Systems Journal
Volume 3; Issue 1; January, 1988

Code Listing For:

	Saywhat
	pp. 09-30

Author(s): Michael Geary
Title:     Converting Windows Applications For Microsoft OS/2 PM


Figure 6w
=========

*/

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *\
 *  SW.C - C code for SayWhat - Windows version                  * 
\* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifndef LINT_ARGS
#define LINT_ARGS  /* turn on argument checking for C runtime */
#endif

#include <windows.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sw.h"

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#define MIN_INTERVAL    1       /* limits for nInterval */
#define MAX_INTERVAL    999

#define MIN_DISTANCE    1       /* limits for nDistance */
#define MAX_DISTANCE    99

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Static variables  */

HANDLE  hInstance;              /* application instance handle */

HWND    hWndWhat;               /* main window handle */
HWND    hWndPanel;              /* contral panel dialog handle */

FARPROC lpfnDlgProc;            /* ProcInstance for dialog */

char    szAppName[10];          /* window class name */
char    szTitle[15];            /* main window title */

char    szText[40];             /* current "what" text */
NPSTR   pText = szText;         /* ptr into szText for icon mode */
char    cBlank = ' ';           /* a blank we can point to */

RECT    rcText;                 /* current text rectangle */
POINT   ptAdvance;              /* increments for SayAdvanceText */
POINT   ptCharSize;             /* X and Y size of a character */
POINT   ptScreenSize;           /* X and Y size of the screen */

int     nDisplayPlanes;         /* number of display planes */
DWORD   rgbTextColor;           /* current text color */
HBRUSH  hbrBkgd;                /* brush for erasing background */

int     nInterval = 40;         /* current "Interval" setting */
int     nDistance = 30;         /* current "Distance" setting */
int     nDistLeft = 0;          /* change direction when hits 0 */
BOOL    bCleanPaint = TRUE;     /* clean or flickery painting? */
BOOL    bMouseDown = FALSE;     /* is mouse down? */
BOOL    bIconic = FALSE;        /* is main window iconic? */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Full prototypes for our functions to get type checking  */

BOOL FAR PASCAL SayAboutDlgProc( HWND, unsigned, WORD, LONG );
void            SayAdvanceTextChar( HWND );
void            SayAdvanceTextPos( HWND );
void            SayChangeColor( HWND );
void            SayDoBarMsg( HWND, HWND, WORD, int );
void            SayFillRect( HDC, int, int, int, int );
void            SayInitBar( HWND, int, int, int, int );
BOOL            SayInitApp( HANDLE, int );
void            SayInvalidateText( HWND );
void            SayLimitTextPos( HWND );
void            SayMoveText( HWND, POINT );
void            SaySetBar( HWND, int *, int );
void            SayExitApp( int );
BOOL FAR PASCAL SayWhatDlgProc( HWND, unsigned, WORD, LONG );
void            SayWhatPaint( HWND );
LONG FAR PASCAL SayWhatWndProc( HWND, unsigned, WORD, LONG );
void     PASCAL WinMain( HANDLE, HANDLE, LPSTR, int );

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Dialog function for the "About" box  */

BOOL FAR PASCAL SayAboutDlgProc( hWndDlg, wMsg, wParam, lParam )
    HWND        hWndDlg;
    unsigned    wMsg;
    WORD        wParam;
    LONG        lParam;
{
    switch( wMsg )
    {
      case WM_COMMAND:
        switch( wParam )
        {
          case IDOK:
            EndDialog( hWndDlg, TRUE );
            return TRUE;
        }
    }
    return FALSE;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Advances to next display character in iconic mode.
 *  Forces in a blank when it reaches the end of string.
 */

void SayAdvanceTextChar( hWnd )
    HWND        hWnd;
{
    if( ! bIconic )
        return;

    if( pText == &cBlank )
        pText = szText;
    else if( ! *(++pText) )
        pText = &cBlank;

    SayChangeColor( hWnd );
    SayInvalidateText( hWnd );
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Advances text position according to ptAdvance.  Decrements
 *  nDistLeft first, and when it reaches zero, sets a new
 *  randomized ptAdvance and nDistLeft, also changes color.
 *  Does nothing if mouse is down, so text will track mouse.
 */

void SayAdvanceTextPos( hWnd )
    HWND        hWnd;
{
    int         i;

    if( bMouseDown )
        return;

    SayInvalidateText( hWnd );

    if( nDistLeft-- < 0 ) {
        nDistLeft = rand() % nDistance + 1;
        do {
            i = rand();
            ptAdvance.x = (
                i < SHRT_MAX/3   ? -1
              : i < SHRT_MAX/3*2 ?  0
              :                     1
            );
            i = rand();
            ptAdvance.y = (
                i < SHRT_MAX/3   ? -1
              : i < SHRT_MAX/3*2 ?  0
              :                     1
            );
        } while( ptAdvance.x == 0  &&  ptAdvance.y == 0 );
        if( ! bIconic )
            SayChangeColor( hWnd );
    } else {
        rcText.left   += ptAdvance.x;
        rcText.right  += ptAdvance.x;
        rcText.top    += ptAdvance.y;
        rcText.bottom += ptAdvance.y;
    }

    SayInvalidateText( hWnd );
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Changes color to a random selection, if color is available.
 *  Forces a color change - if the random selection is the same
 *  as the old one, it tries again.
 */

void SayChangeColor( hWnd )
    HWND        hWnd;
{
    HDC         hDC;
    DWORD       rgbNew;
    DWORD       rgbWindow;

    if( nDisplayPlanes <= 1 ) {
        rgbTextColor = GetSysColor(COLOR_WINDOWTEXT);
    } else {
        rgbWindow = GetSysColor(COLOR_WINDOW);
        hDC = GetDC( hWnd );
        do {
            rgbNew = GetNearestColor(
                hDC,
                MAKELONG( rand(), rand() ) & 0x00FFFFFFL
            );
        } while( rgbNew == rgbWindow || rgbNew == rgbTextColor );
        rgbTextColor = rgbNew;    
        ReleaseDC( hWnd, hDC );
    }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Handles scroll bar messages from the control dialog box.
 *  Adjusts scroll bar position, taking its limits into account,
 *  copies the scroll bar value into the adjacent edit control,
 *  then sets the nDistance or nInterval variable appropriately.
 */

void SayDoBarMsg( hWndDlg, hWndBar, wCode, nThumb )
    HWND        hWndDlg;
    HWND        hWndBar;
    WORD        wCode;
    int         nThumb;
{
    int         nPos;
    int         nOldPos;
    int         nMin;
    int         nMax;
    int         idBar;

    idBar = GetWindowWord( hWndBar, GWW_ID );

    nOldPos = nPos = GetScrollPos( hWndBar, SB_CTL );
    GetScrollRange( hWndBar, SB_CTL, &nMin, &nMax );

    switch( wCode )
    {
        case SB_LINEUP:         --nPos;             break;        

        case SB_LINEDOWN:       ++nPos;             break;    

        case SB_PAGEUP:         nPos -= 10;         break;    

        case SB_PAGEDOWN:       nPos += 10;         break;    

        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:     nPos = nThumb;      break;    

        case SB_TOP:            nPos = nMin;        break;    

        case SB_BOTTOM:         nPos = nMax;        break;    
    }

    if( nPos < nMin )
        nPos = nMin;

    if( nPos > nMax )
        nPos = nMax;

    if( nPos == nOldPos )
        return;

    SetScrollPos( hWndBar, SB_CTL, nPos, TRUE );

    SetDlgItemInt( hWndDlg, idBar+1, nPos, FALSE );

    switch( idBar )
    {
      case ITEM_DISTBAR:
        nDistance = nPos;
        break;

      case ITEM_INTBAR:
        KillTimer( hWndWhat, TIMER_MOVE );
        nInterval = nPos;
        SetTimer( hWndWhat, TIMER_MOVE, nInterval, NULL );
        InvalidateRect( hWndWhat, NULL, FALSE );
        break;
    }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Terminates the application, freeing up allocated resources.
 *  Note that this function does NOT return to the caller, but
 *  exits the program.
 */

void SayExitApp( nRet )
    int         nRet;
{
    if( GetModuleUsage(hInstance) == 1 ) {
        DeleteObject( hbrBkgd );
    }

    exit( nRet );
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Fills a specified rectangle with the background color.
 *  Checks that the rectangle is non-empty first.
 */

void SayFillRect( hDC, nLeft, nTop, nRight, nBottom )
    HDC         hDC;
    int         nLeft;
    int         nTop;
    int         nRight;
    int         nBottom;

{
    RECT        rcFill;

    if( nLeft >= nRight  ||  nTop >= nBottom )
        return;

    SetRect( &rcFill, nLeft, nTop, nRight, nBottom );    

    FillRect( hDC, &rcFill, hbrBkgd );
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Initializes the application.  */

BOOL SayInitApp( hPrevInstance, nCmdShow )
    HANDLE      hPrevInstance;
    int         nCmdShow;
{
    WNDCLASS    Class;
    HDC         hDC;
    TEXTMETRIC  Metrics;

    LoadString(
        hInstance, STR_NAME,  szAppName, sizeof(szAppName)
    );
    LoadString(
        hInstance, STR_TITLE, szTitle,   sizeof(szTitle)
    );
    LoadString(
        hInstance, STR_WHAT,  szText,    sizeof(szText)
    );

    hDC = CreateIC( "DISPLAY", NULL, NULL, NULL );
    GetTextMetrics( hDC, &Metrics );
    nDisplayPlanes = GetDeviceCaps( hDC, PLANES );
    DeleteDC( hDC );

    ptCharSize.x = Metrics.tmMaxCharWidth;
    ptCharSize.y = Metrics.tmHeight;

    ptScreenSize.x = GetSystemMetrics(SM_CXSCREEN);
    ptScreenSize.y = GetSystemMetrics(SM_CYSCREEN);

    if( ! hPrevInstance ) {

        hbrBkgd = CreateSolidBrush( GetSysColor(COLOR_WINDOW) );

        Class.style         = 0; /* CS_HREDRAW | CS_VREDRAW; */
        Class.lpfnWndProc   = SayWhatWndProc;
        Class.cbClsExtra    = 0;
        Class.cbWndExtra    = 0;
        Class.hInstance     = hInstance;
        Class.hIcon         = NULL;
        Class.hCursor       = LoadCursor( NULL, IDC_ARROW );
        Class.hbrBackground = COLOR_WINDOW + 1;
        Class.lpszMenuName  = szAppName;
        Class.lpszClassName = szAppName;

        if( ! RegisterClass( &Class ) )
            return FALSE;

    } else {

        GetInstanceData(
            hPrevInstance, (NPSTR)&hbrBkgd, sizeof(hbrBkgd)
        );
    }

    hWndWhat = CreateWindow(
        szAppName,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0,
        CW_USEDEFAULT, 0,
        (HWND)NULL,
        (HMENU)NULL,
        hInstance,
        (LPSTR)NULL
    );

    if( ! hWndWhat )
        return FALSE;

    ShowWindow( hWndWhat, nCmdShow );
    UpdateWindow( hWndWhat );

    return TRUE;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Initializes one scroll bar in the control dialog.  */

void SayInitBar( hWndDlg, idBar, nValue, nMin, nMax )
    HWND        hWndDlg;
    int         idBar;
    int         nValue;
    int         nMin;
    int         nMax;
{
    HWND        hWndBar;

    hWndBar = GetDlgItem( hWndDlg, idBar );

    SetScrollRange( hWndBar, SB_CTL, nMin, nMax, FALSE );
    SetScrollPos( hWndBar, SB_CTL, nValue, FALSE );

    SetDlgItemInt( hWndDlg, idBar+1, nValue, FALSE );
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Invalidates the text within the main window, adjusting the
 *  text rectangle if it's gone out of bounds.
 */

void SayInvalidateText( hWnd )
    HWND        hWnd;
{
    SayLimitTextPos( hWnd );
    InvalidateRect( hWnd, &rcText, FALSE );
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Checks the text position against the window client area
 *  rectangle.  If it's moved off the window in any direction,
 *  forces it back inside, and also reverses the ptAdvance value
 *  for that direction so it will "bounce" off the edge.  Handles
 *  both the iconic and open window cases.
 */

void SayLimitTextPos( hWnd )
    HWND        hWnd;
{
    RECT        rcClient;
    POINT       ptTextSize;

    ptTextSize = ptCharSize;

    if( ! bIconic ) {
        pText = szText;
        ptTextSize.x *= strlen(szText);
    }

    GetClientRect( hWndWhat, &rcClient );

    if( rcText.left > rcClient.right - ptTextSize.x ) {
        rcText.left = rcClient.right - ptTextSize.x;
        ptAdvance.x = -ptAdvance.x;
    }

    if( rcText.left < rcClient.left ) {
        rcText.left = rcClient.left;
        ptAdvance.x = -ptAdvance.x;
    }

    if( rcText.top > rcClient.bottom - ptTextSize.y ) {
        rcText.top = rcClient.bottom - ptTextSize.y;
        ptAdvance.y = -ptAdvance.y;
    }

    if( rcText.top < rcClient.top ) {
        rcText.top = rcClient.top;
        ptAdvance.y = -ptAdvance.y;
    }

    rcText.right  = rcText.left + ptTextSize.x;
    rcText.bottom = rcText.top  + ptTextSize.y;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Moves the text within the window, by invalidating the old
 *  position, adjusting rcText according to ptMove, and then
 *  invalidating the new position.
 */

void SayMoveText( hWnd, ptMove )
    HWND        hWnd;
    POINT       ptMove;
{
    SayInvalidateText( hWnd );
    rcText.left = ptMove.x - (rcText.right  - rcText.left  >> 1);
    rcText.top  = ptMove.y - (rcText.bottom - rcText.top   >> 1);
    SayInvalidateText( hWnd );
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Sets one of the dialog scroll bars to *pnValue.  If that value
 *  is out of range, limits it to the proper range and forces
 *  *pnValue to be within the range as well.
 */

void SaySetBar( hWndDlg, pnValue, idBar )
    HWND        hWndDlg;
    int *       pnValue;
    int         idBar;
{
    HWND        hWndBar;
    int         nMin;
    int         nMax;
    int         nValue;
    BOOL        bOK;

    hWndBar = GetDlgItem( hWndDlg, idBar );

    GetScrollRange( hWndBar, SB_CTL, &nMin, &nMax );

    nValue = GetDlgItemInt( hWndDlg, idBar+1, &bOK, FALSE );

    if( bOK  &&  nValue >= nMin  &&  nValue <= nMax ) {
        *pnValue = nValue;
        SetScrollPos( hWndBar, SB_CTL, nValue, TRUE );
    } else {
        SetDlgItemInt(
            hWndDlg,
            idBar+1,
            GetScrollPos( hWndBar, SB_CTL ),
            FALSE
        );
    }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Dialog function for the control panel dialog box.  */

BOOL FAR PASCAL SayWhatDlgProc( hWndDlg, wMsg, wParam, lParam )
    HWND        hWndDlg;
    unsigned    wMsg;
    WORD        wParam;
    LONG        lParam;
{
    HWND        hWndBar;
    RECT        rcWin;
    int         n;

    switch( wMsg )
    {
      case WM_COMMAND:
        switch( wParam )
        {
          case IDOK:
            KillTimer( hWndWhat, TIMER_MOVE );
            GetDlgItemText(
                hWndDlg, ITEM_WHAT, szText, sizeof(szText)
            );
            if( strlen(szText) == 0 )
                LoadString(
                    hInstance, STR_WHAT, szText, sizeof(szText)
                );
            pText = szText;
            SaySetBar( hWndDlg, &nInterval, ITEM_INTBAR );
            SaySetBar( hWndDlg, &nDistance, ITEM_DISTBAR );
            SetTimer( hWndWhat, TIMER_MOVE, nInterval, NULL );
            InvalidateRect( hWndWhat, NULL, FALSE );
            return TRUE;

          case IDCANCEL:
            DestroyWindow( hWndDlg );
            return TRUE;

          case ITEM_CLEAN:
          case ITEM_FLICKER:
            bCleanPaint =
                IsDlgButtonChecked( hWndDlg, ITEM_CLEAN );
            return TRUE;
        }
        return FALSE;

      case WM_HSCROLL:
        if( HIWORD(lParam) )
            SayDoBarMsg(
                hWndDlg, HIWORD(lParam), wParam, LOWORD(lParam)
            );
        return TRUE;

      case WM_INITDIALOG:
        GetWindowRect( hWndDlg, &rcWin );
        ClientToScreen( hWndWhat, (LPPOINT)&rcWin.left );
        ClientToScreen( hWndWhat, (LPPOINT)&rcWin.right );
        n = rcWin.right - ptScreenSize.x + ptCharSize.x;
        if( n > 0 )
            rcWin.left -= n;
        rcWin.left &= ~7;  /* byte align */
        n = rcWin.bottom - ptScreenSize.y + ptCharSize.y;
        if( n > 0 ) 
            rcWin.top -= n;
        SetWindowPos(
            hWndDlg,
            (HWND)NULL,
            rcWin.left,
            rcWin.top,
            0, 0,
            SWP_NOSIZE | SWP_NOZORDER |
                SWP_NOREDRAW | SWP_NOACTIVATE
        );
        SetDlgItemText( hWndDlg, ITEM_WHAT, szText );
        SendDlgItemMessage(
            hWndDlg, ITEM_WHAT,
            EM_LIMITTEXT, sizeof(szText)-1, 0L
        );
        SayInitBar(
            hWndDlg, ITEM_INTBAR,
            nInterval, MIN_INTERVAL, MAX_INTERVAL
        );
        SayInitBar(
            hWndDlg, ITEM_DISTBAR,
            nDistance, MIN_DISTANCE, MAX_DISTANCE
        );
        CheckDlgButton( hWndDlg, ITEM_CLEAN, TRUE );
        return TRUE;

      case WM_NCDESTROY:
        FreeProcInstance( lpfnDlgProc );
        hWndPanel = NULL;
        return FALSE;
    }
    return FALSE;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Painting procedure for the main window.  Handles both the
 *  clean and flickery painting methods for demonstration
 *  purposes.
 */

void SayWhatPaint( hWnd )
    HWND        hWnd;
{
    PAINTSTRUCT ps;

    BeginPaint( hWnd, &ps );

    SetTextColor( ps.hdc, rgbTextColor );

    SayLimitTextPos( hWnd );

    if( bCleanPaint ) {

        /* Clean painting, avoid redundant erasing */

        TextOut(
            ps.hdc,
            rcText.left,
            rcText.top,
            pText,
            bIconic ? 1 : strlen(szText)
        );

        SayFillRect(
            ps.hdc,
            ps.rcPaint.left,
            ps.rcPaint.top,
            rcText.left,
            ps.rcPaint.bottom
        );

        SayFillRect(
            ps.hdc,
            rcText.left,
            ps.rcPaint.top,
            rcText.right,
            rcText.top
        );

        SayFillRect(
            ps.hdc,
            rcText.left,
            rcText.bottom,
            rcText.right,
            ps.rcPaint.bottom
        );

        SayFillRect(
            ps.hdc,
            rcText.right,
            ps.rcPaint.top,
            ps.rcPaint.right,
            ps.rcPaint.bottom
        );

    } else {
        
        /* Flickery painting, erase background and
           paint traditionally */

        FillRect( ps.hdc, &ps.rcPaint, hbrBkgd );

        TextOut(
            ps.hdc,
            rcText.left,
            rcText.top,
            pText,
            bIconic ? 1 : strlen(szText)
        );

    }

    EndPaint( hWnd, &ps );

    if( ! nInterval )
        SayAdvanceTextPos( hWnd );
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Window function for the main window.  */

LONG FAR PASCAL SayWhatWndProc( hWnd, wMsg, wParam, lParam )
    HWND        hWnd;
    unsigned    wMsg;
    WORD        wParam;
    LONG        lParam;
{
    FARPROC     lpfnAbout;

    switch( wMsg )
    {
      case WM_COMMAND:
        switch( wParam )
        {
          case CMD_ABOUT:
            lpfnAbout =
                MakeProcInstance( SayAboutDlgProc, hInstance );
            DialogBox(
                hInstance, MAKEINTRESOURCE(DLG_ABOUT),
                hWnd, lpfnAbout
            );
            FreeProcInstance( lpfnAbout );
            return 0L;

          case CMD_EXIT:
            DestroyWindow( hWndWhat );
            return 0L;

          case CMD_WHAT:
            if( hWndPanel ) {
                BringWindowToTop( hWndPanel );
            } else {
                lpfnDlgProc =
                    MakeProcInstance( SayWhatDlgProc, hInstance );
                if( ! lpfnDlgProc )
                    return 0L;
                hWndPanel = CreateDialog(
                    hInstance,
                    MAKEINTRESOURCE(DLG_WHAT),
                    (HWND)NULL,
                    lpfnDlgProc
                );
                if( ! hWndPanel )
                    FreeProcInstance( lpfnDlgProc );
            }
        }
        break;

      case WM_CREATE:
        srand( (int)time(NULL) );
        SetTimer( hWnd, TIMER_MOVE, nInterval, NULL );
        return 0L;

      case WM_DESTROY:
        if( hWndPanel )
            DestroyWindow( hWndPanel );
        PostQuitMessage( 0 );
        return 0L;

      case WM_KEYDOWN:
        SayInvalidateText( hWnd );
        switch( wParam )
        {
          case VK_LEFT:
            rcText.left -= ptCharSize.x;
            ptAdvance.x  = -1;
            ptAdvance.y  = 0;
            break;

          case VK_RIGHT:
            rcText.left += ptCharSize.x;
            ptAdvance.x  = 1;
            ptAdvance.y  = 0;
            break;

          case VK_UP:
            rcText.top  -= ptCharSize.y >> 1;
            ptAdvance.x  = 0;
            ptAdvance.y  = -1;
            break;

          case VK_DOWN:
            rcText.top  += ptCharSize.y >> 1;
            ptAdvance.x  = 0;
            ptAdvance.y  = 1;
            break;

          default:
            return 0L;
        }
        SayInvalidateText( hWnd );
        nDistLeft = nDistance;
        return 0L;

      case WM_LBUTTONDOWN:
        if( bMouseDown )
            break;
        KillTimer( hWnd, TIMER_MOVE );
        bMouseDown = TRUE;
        SetCapture( hWnd );
        SayMoveText( hWnd, MAKEPOINT(lParam) );
        break;

      case WM_LBUTTONUP:
        if( ! bMouseDown )
            break;
        bMouseDown = FALSE;
        ReleaseCapture();
        SayMoveText( hWnd, MAKEPOINT(lParam) );
        SetTimer( hWnd, TIMER_MOVE, nInterval, NULL );
        break;

      case WM_MOUSEMOVE:
        if( bMouseDown )
            SayMoveText( hWnd, MAKEPOINT(lParam) );
        break;

      case WM_PAINT:
        SayWhatPaint( hWnd );
        return 0L;

      case WM_SIZE:
        if( wParam == SIZEICONIC ) {
            if( ! bIconic )
                SetTimer( hWnd, TIMER_CHAR, 1000, NULL );
            bIconic = TRUE;
        } else {
            if( bIconic )
                KillTimer( hWnd, TIMER_CHAR );
            bIconic = FALSE;
        }
        SayInvalidateText( hWnd );
        nDistLeft = 0;
        SayAdvanceTextPos( hWnd );
        return 0L;

      case WM_TIMER:
        switch( wParam )
        {
          case TIMER_MOVE:
            SayAdvanceTextPos( hWnd );
            break;

          case TIMER_CHAR:
            SayAdvanceTextChar( hWnd );
            break;
        }
        return 0L;
    }
    return DefWindowProc( hWnd, wMsg, wParam, lParam );
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Main function for the application.  */

void PASCAL WinMain( hInst, hPrevInst, lpszCmdLine, nCmdShow )
    HANDLE      hInst;
    HANDLE      hPrevInst;
    LPSTR       lpszCmdLine;
    int         nCmdShow;
{
    MSG         msg;

    hInstance = hInst;

    if( ! SayInitApp( hPrevInst, nCmdShow ) )
        SayExitApp( 1 );

    while( GetMessage( &msg, NULL, 0, 0 ) ) {

        if( hWndPanel  &&  IsDialogMessage( hWndPanel, &msg ) )
            continue;

        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }


    SayExitApp( msg.wParam );
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
