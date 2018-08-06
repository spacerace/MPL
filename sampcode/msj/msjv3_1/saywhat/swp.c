/*
Microsoft Systems Journal
Volume 3; Issue 1; January, 1988

Code Listing For:

	Saywhat
	pp. 09-30

Author(s): Michael Geary
Title:     Converting Windows Applications For Microsoft OS/2 PM


Figure 6pm
=========

*/

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *\
 *  SWP.C - C code for SayWhat - Presentation Manager version    *
\* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifndef LINT_ARGS
#define LINT_ARGS  /* turn on argument checking for C runtime */
#endif

#include <os2pm.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "swp.h"

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#define MIN_INTERVAL    1       /* limits for nInterval */
#define MAX_INTERVAL    999

#define MIN_DISTANCE    1       /* limits for nDistance */
#define MAX_DISTANCE    99

#define COLORDATAMAX    5

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Static variables  */

HAB     hAB = NULL;             /* anchor block handle */
HMQ     hMsgQ = NULL;           /* message queue handle */

HWND    hWndWhatFrame;          /* frame window handle */
HWND    hWndWhat;               /* client window handle */
HWND    hWndPanel;              /* control panel dialog handle */

CHAR    szAppName[10];          /* window class name */
CHAR    szTitle[15];            /* main window title */

CHAR    szText[40];             /* current "what" text */
PSZ     npszText = szText;      /* ptr into szText for icon mode */
CHAR    cBlank = ' ';           /* a blank we can point to */

RECT    rcText;                 /* current text rectangle */
POINT   ptAdvance;              /* increments for SayAdvanceText */
POINT   ptCharSize;             /* X and Y size of a character */
POINT   ptScreenSize;           /* X and Y size of the screen */

LONG    lColorMax;              /* number of available colors */
LONG    lColor;                 /* current text color index */

SHORT   nInterval = 40;         /* current "Interval" setting */
SHORT   nDistance = 30;         /* current "Distance" setting */
SHORT   nDistLeft = 0;          /* change direction when hits 0 */
BOOL    bCleanPaint = TRUE;     /* clean or flickery painting? */
BOOL    bMouseDown = FALSE;     /* is mouse down? */
BOOL    bIconic = FALSE;        /* is main window iconic? */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Full prototypes for our functions to get type checking  */

ULONG FAR PASCAL SayAboutDlgProc( HWND, USHORT, ULONG, ULONG );
VOID             SayAdvanceTextChar( HWND );
VOID             SayAdvanceTextPos( HWND );
VOID             SayChangeColor( HWND );
VOID             SayDoBarMsg( HWND, USHORT, USHORT, SHORT );
VOID             SayExitApp( INT );
VOID             SayFillRect( HPS, SHORT, SHORT, SHORT, SHORT );
VOID             SayInitBar( HWND, SHORT, SHORT, SHORT, SHORT );
BOOL             SayInitApp( VOID );
VOID             SayInvalidateText( HWND );
VOID             SayLimitTextPos( HWND );
VOID             SayMoveText( HWND, POINT );
VOID             SaySetBar( HWND, SHORT *, SHORT );
ULONG FAR PASCAL SayWhatDlgProc( HWND, USHORT, ULONG, ULONG );
VOID             SayWhatPaint( HWND );
ULONG FAR PASCAL SayWhatWndProc( HWND, USHORT, ULONG, ULONG );
void  cdecl      main( INT, PSZ );

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Dialog function for the "About" box  */

ULONG FAR PASCAL SayAboutDlgProc( hWndDlg, wMsg, lParam1, lParam2 )
    HWND        hWndDlg;
    USHORT      wMsg;
    ULONG       lParam1;
    ULONG       lParam2;
{
    switch( wMsg )
    {
      case WM_COMMAND:
        switch( LOUSHORT(lParam1) )
        {
          case IDOK:
            WinDismissDlg( hWndDlg, TRUE );
            break;
        }
    }
    return WinDefDlgProc( hWndDlg, wMsg, lParam1, lParam2 );
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Advances to next display character in iconic mode.
 *  Forces in a blank when it reaches the end of string.
 */

VOID SayAdvanceTextChar( hWnd )
    HWND        hWnd;
{
    if( ! bIconic )
        return;

    if( npszText == &cBlank )
        npszText = szText;
    else if( ! *(++npszText) )
        npszText = &cBlank;

    SayChangeColor( hWnd );
    SayInvalidateText( hWnd );
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Advances text position according to ptAdvance.  Decrements
 *  nDistLeft first, and when it reaches zero, sets a new
 *  randomized ptAdvance and nDistLeft, also changes color.
 *  Does nothing if mouse is down, so text will track mouse.
 */

VOID SayAdvanceTextPos( hWnd )
    HWND        hWnd;
{
    SHORT       i;

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
        rcText.xLeft   += ptAdvance.x;
        rcText.xRight  += ptAdvance.x;
        rcText.yTop    += ptAdvance.y;
        rcText.yBottom += ptAdvance.y;
    }

    SayInvalidateText( hWnd );
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Changes color to a random selection, if color is available.
 *  Forces a color change - if the random selection is the same
 *  as the old one, it tries again.
 */

VOID SayChangeColor( hWnd )
    HWND        hWnd;
{
    HPS         hPS;
    LONG        lWindow;
    LONG        lNew;

    hPS = WinGetPS( hWnd );

    if( lColorMax <= 2 ) {
        lColor = GpiQueryColorIndex(
            hPS,
            WinQuerySysColor( hAB, SCLR_WINDOWTEXT ),
            0L
        );
    } else {
        lWindow = GpiQueryColorIndex(
            hPS,
            WinQuerySysColor( hAB, SCLR_WINDOW ),
            0L
        );
        do {
            lNew = rand() % lColorMax;
        } while( lNew == lWindow || lNew == lColor );
        lColor = lNew;
    }

    WinReleasePS( hPS );

}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Handles scroll bar messages from the control dialog box.
 *  Adjusts scroll bar position, taking its limits into account,
 *  copies the scroll bar value into the adjacent edit control,
 *  then sets the nDistance or nInterval variable appropriately.
 */

VOID SayDoBarMsg( hWndDlg, idBar, wCode, nThumb )
    HWND        hWndDlg;
    USHORT      idBar;    
    USHORT      wCode;
    SHORT       nThumb;
{
    SHORT       nPos;
    SHORT       nOldPos;
    ULONG       lRange;

    nOldPos = nPos =
        (SHORT)WinSendDlgItemMsg(
            hWndDlg, idBar, SBM_QUERYPOS, 0L, 0L
        );

    lRange =
        WinSendDlgItemMsg(
            hWndDlg, idBar, SBM_QUERYRANGE, 0L, 0L
        );

    switch( wCode ) {

        case SB_LINEUP:         --nPos;                   break;        

        case SB_LINEDOWN:       ++nPos;                   break;    

        case SB_PAGEUP:         nPos -= 10;               break;    

        case SB_PAGEDOWN:       nPos += 10;               break;    

        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:     nPos = nThumb;            break;    

        case SB_TOP:            nPos = LOUSHORT(lRange);  break;    

        case SB_BOTTOM:         nPos = HIUSHORT(lRange);  break;    

    }

    if( nPos < LOUSHORT(lRange) )
        nPos = LOUSHORT(lRange);

    if( nPos > HIUSHORT(lRange) )
        nPos = HIUSHORT(lRange);

    if( nPos == nOldPos )
        return;

    WinSendDlgItemMsg(
        hWndDlg, idBar, SBM_SETPOS, (LONG)nPos, 0L
    );

    WinSetDlgItemShort( hWndDlg, idBar+1, nPos, FALSE );

    switch( idBar )
    {
      case ITEM_DISTBAR:
        nDistance = nPos;
        break;

      case ITEM_INTBAR:
        WinStopTimer( hAB, hWndWhat, TIMER_MOVE );
        nInterval = nPos;
        WinStartTimer( hAB, hWndWhat, TIMER_MOVE, nInterval );
        WinInvalidateRect( hWndWhat, NULL );
        break;
    }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Terminates the application, freeing up allocated resources.
 *  Note that this function does NOT return to the caller, but
 *  exits the program.
 */

VOID SayExitApp( nRet )
    INT         nRet;
{
    if( hWndWhatFrame )
        WinDestroyWindow( hWndWhatFrame );

    if( hMsgQ )
        WinDestroyMsgQueue( hMsgQ );

    if( hAB )
        WinTerminate( hAB );

    exit( nRet );
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Fills a specified rectangle with the background color.
 *  Checks that the rectangle is non-empty first.
 */

VOID SayFillRect( hPS, xLeft, xBottom, xRight, xTop )
    HPS         hPS;
    SHORT       xLeft;
    SHORT       xBottom;
    SHORT       xRight;
    SHORT       xTop;

{
    RECT        rcFill;

    if( xLeft >= xRight  ||  xBottom >= xTop )
        return;

    WinSetRect( hAB, &rcFill, xLeft, xBottom, xRight, xTop );    

    WinFillRect(
        hPS,
        &rcFill,
        WinQuerySysColor( hAB, SCLR_WINDOW )
    );
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Initializes the application.  */

BOOL SayInitApp()
{
    HPS         hPS;
    BOOL        bOK;

    hAB = WinInitialize();
    if( ! hAB )
        return FALSE;

    hMsgQ = WinCreateMsgQueue( hAB, 0 );
    if( ! hMsgQ )
        return FALSE;

    WinLoadString(
        hAB, NULL, STR_NAME,  szAppName, sizeof(szAppName)
    );
    WinLoadString(
        hAB, NULL, STR_TITLE, szTitle,   sizeof(szTitle)
    );
    WinLoadString(
        hAB, NULL, STR_WHAT,  szText,    sizeof(szText)
    );

    bOK = WinRegisterClass(
        hAB,
        szAppName,
        (LPFNWP)SayWhatWndProc,
        CS_SYNCPAINT,
        0,
        NULL
    );
    if( ! bOK )
        return FALSE;

    hWndWhatFrame = WinCreateStdWindow(
        (HWND)NULL,
        FS_TITLEBAR | FS_SYSMENU |
            FS_MENU | FS_MINMAX | FS_SIZEBORDER,
        szAppName,
        szTitle,
        0L,
        (HMODULE)NULL,
        MENU_WHAT,
        &hWndWhat
    );

    if( ! hWndWhatFrame )
        return FALSE;

    WinShowWindow( hWndWhat, TRUE );

    return TRUE;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Initializes one scroll bar in the control dialog.  */

VOID SayInitBar( hWndDlg, idBar, nValue, nMin, nMax )
    HWND        hWndDlg;
    SHORT       idBar;
    SHORT       nValue;
    SHORT       nMin;
    SHORT       nMax;
{
    HWND        hWndBar;

    hWndBar = WinWindowFromID( hWndDlg, idBar );

    WinSendDlgItemMsg(
        hWndDlg,
        idBar,
        SBM_SETSCROLLBAR,
        (LONG)nValue,
        MAKELONG( nMin, nMax )
    );

    WinSetDlgItemShort( hWndDlg, idBar+1, nValue, FALSE );
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Invalidates the text within the main window, adjusting the
 *  text rectangle if it's gone out of bounds.
 */

VOID SayInvalidateText( hWnd )
    HWND        hWnd;
{
    SayLimitTextPos( hWnd );
    WinInvalidateRect( hWnd, &rcText );
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Checks the text position against the window client area
 *  rectangle.  If it's moved off the window in any direction,
 *  forces it back inside, and also reverses the ptAdvance value
 *  for that direction so it will "bounce" off the edge.  Handles
 *  both the iconic and open window cases.
 */

VOID SayLimitTextPos( hWnd )
    HWND        hWnd;
{
    RECT        rcClient;
    POINT       ptTextSize;

    ptTextSize = ptCharSize;

    if( ! bIconic ) {
        npszText = szText;
        ptTextSize.x *= strlen(szText);
    }

    WinQueryWindowRect( hWndWhat, &rcClient );

    if( rcText.xLeft > rcClient.xRight - ptTextSize.x ) {
        rcText.xLeft = rcClient.xRight - ptTextSize.x;
        ptAdvance.x = -ptAdvance.x;
    }

    if( rcText.xLeft < rcClient.xLeft ) {
        rcText.xLeft = rcClient.xLeft;
        ptAdvance.x = -ptAdvance.x;
    }

    if( rcText.yBottom < rcClient.yBottom ) {
        rcText.yBottom = rcClient.yBottom;
        ptAdvance.y = -ptAdvance.y;
    }

    if( rcText.yBottom > rcClient.yTop - ptTextSize.y ) {
        rcText.yBottom = rcClient.yTop - ptTextSize.y;
        ptAdvance.y = -ptAdvance.y;
    }

    rcText.xRight = rcText.xLeft   + ptTextSize.x;
    rcText.yTop   = rcText.yBottom + ptTextSize.y;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Moves the text within the window, by invalidating the old
 *  position, adjusting rcText according to ptMove, and then
 *  invalidating the new position.
 */

VOID SayMoveText( hWnd, ptMove )
    HWND        hWnd;
    POINT       ptMove;
{
    SayInvalidateText( hWnd );
    rcText.xLeft   =
        ptMove.x - (rcText.xRight - rcText.xLeft    >> 1);
    rcText.yBottom =
        ptMove.y - (rcText.yTop   - rcText.yBottom  >> 1);
    SayInvalidateText( hWnd );
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Sets one of the dialog scroll bars to *pnValue.  If that value
 *  is out of range, limits it to the proper range and forces
 *  *pnValue to be within the range as well.
 */

VOID SaySetBar( hWndDlg, pnValue, idBar )
    HWND        hWndDlg;
    SHORT *     pnValue;
    SHORT       idBar;
{
    ULONG       lRange;        
    SHORT       nValue;
    BOOL        bOK;

    lRange =
        WinSendDlgItemMsg(
            hWndDlg, idBar, SBM_QUERYRANGE, 0L, 0L
        );

    nValue =
        WinQueryDlgItemShort( hWndDlg, idBar+1, &bOK, FALSE );

    if(
        bOK  &&
        nValue >= LOUSHORT(lRange)  &&
        nValue <= HIUSHORT(lRange)
    ) {
        *pnValue = nValue;
        WinSendDlgItemMsg(
            hWndDlg, idBar, SBM_SETPOS, (LONG)nValue, (LONG)TRUE
        );
    } else {
        WinSetDlgItemShort(
            hWndDlg,
            idBar + 1,
            (INT)WinSendDlgItemMsg(
                hWndDlg, idBar, SBM_QUERYPOS, 0L, 0L
            ),
            FALSE
        );
    }
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Dialog function for the control panel dialog box.  */

ULONG FAR PASCAL SayWhatDlgProc( hWndDlg, wMsg, lParam1, lParam2 )
    HWND        hWndDlg;
    USHORT      wMsg;
    ULONG       lParam1;
    ULONG       lParam2;
{
    HWND        hWndBar;
    RECT        rcWin;
    SHORT       n;

    switch( wMsg )
    {
    case WM_COMMAND:
        switch( LOUSHORT(lParam1) )
        {
          case IDOK:
            WinStopTimer( hAB, hWndWhat, TIMER_MOVE );
            WinQueryWindowText(
                WinWindowFromID( hWndDlg, ITEM_WHAT ),
                sizeof(szText),
                szText
            );
            if( strlen(szText) == 0 )
                WinLoadString(
                    hAB, NULL, STR_WHAT, szText, sizeof(szText)
                );
            npszText = szText;
            SaySetBar( hWndDlg, &nInterval, ITEM_INTBAR );
            SaySetBar( hWndDlg, &nDistance, ITEM_DISTBAR );
            WinStartTimer( hAB, hWndWhat, TIMER_MOVE, nInterval );
            WinInvalidateRect( hWndWhat, NULL );
            break;

          case IDCANCEL:
            WinDestroyWindow( hWndDlg );
            break;

          case ITEM_CLEAN:
          case ITEM_FLICKER:
            bCleanPaint = (BOOL)WinSendDlgItemMsg(
                hWndDlg, ITEM_CLEAN,
                BM_QUERYCHECK, 0L, 0L
            );
            break;
        }
        break;

    case WM_DESTROY:
        hWndPanel = NULL;
        break;

    case WM_HSCROLL:
        SayDoBarMsg(
            hWndDlg, LOUSHORT(lParam1),
            HIUSHORT(lParam2), LOUSHORT(lParam2)
        );
        break;

    case WM_INITDLG:
        WinQueryWindowRect( hWndDlg, &rcWin );
        WinMapWindowPoints(
            hWndWhat, (HWND)NULL, (LPPOINT)&rcWin.xLeft, 2
        );
        n = rcWin.xRight - ptScreenSize.x + ptCharSize.x;
        if( n > 0 )
            rcWin.xLeft -= n;
        rcWin.xLeft &= ~7;  /* byte align */
        n = rcWin.yTop - ptScreenSize.y + ptCharSize.y;
        if( n > 0 )
            rcWin.yBottom -= n;
        WinSetWindowPos(
            hWndDlg,
            (HWND)NULL,
            rcWin.xLeft,
            rcWin.yBottom,
            0, 0,
            SWP_MOVE
        );
        WinSetWindowText(
            WinWindowFromID( hWndDlg, ITEM_WHAT ), szText
        );
        WinSendDlgItemMsg(
            hWndDlg, ITEM_WHAT, EM_SETTEXTLIMIT,
            (LONG)sizeof(szText)-1, 0L
        );
        SayInitBar(
            hWndDlg, ITEM_INTBAR,  nInterval,
            MIN_INTERVAL, MAX_INTERVAL
        );
        SayInitBar(
            hWndDlg, ITEM_DISTBAR, nDistance,
            MIN_DISTANCE, MAX_DISTANCE
        );
        WinSendDlgItemMsg(
            hWndDlg, ITEM_CLEAN, BM_SETCHECK, (LONG)TRUE, 0L
        );
        break;
    }
    return WinDefDlgProc( hWndDlg, wMsg, lParam1, lParam2 );
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Painting procedure for the main window.  Handles both the
 *  clean and flickery painting methods for demonstration
 *  purposes.
 */

VOID SayWhatPaint( hWnd )
    HWND        hWnd;
{
    HPS         hPS;
    RECT        rcPaint;
    GPOINT      gptText;

    hPS = WinBeginPaint( hWnd, (HPS)NULL, &rcPaint );

    GpiSetColor( hPS, lColor );

    SayLimitTextPos( hWnd );

    gptText.x = (LONG)rcText.xLeft;
    gptText.y = (LONG)rcText.yBottom;

    if( bCleanPaint ) {

        /* Clean painting, avoid redundant erasing */

        GpiSetBackMix( hPS, MIX_OVERPAINT );

        GpiCharStringAt(
            hPS,
            &gptText,
            (LONG)( bIconic ? 1 : strlen(szText) ),
            npszText
        );

        SayFillRect(
            hPS,
            rcPaint.xLeft,
            rcPaint.yBottom,
            rcText.xLeft,
            rcPaint.yTop
        );

        SayFillRect(
            hPS,
            rcText.xLeft,
            rcText.yTop,
            rcText.xRight,
            rcPaint.yTop
        );

        SayFillRect(
            hPS,
            rcText.xLeft,
            rcPaint.yBottom,
            rcText.xRight,
            rcText.yBottom
        );

        SayFillRect(
            hPS,
            rcText.xRight,
            rcPaint.yBottom,
            rcPaint.xRight,
            rcPaint.yTop
        );

    } else {
        
        /* Flickery painting, erase background and
           paint traditionally */

        WinFillRect(
            hPS,
            &rcPaint,
            WinQuerySysColor( hAB, SCLR_WINDOW )
        );

        GpiCharStringAt(
            hPS,
            &gptText,
            (LONG)( bIconic ? 1 : strlen(szText) ),
            npszText
        );
    }

    WinEndPaint( hPS );

    if( ! nInterval )
        SayInvalidateText( hWnd );
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Window function for the main window.  */

ULONG FAR PASCAL SayWhatWndProc( hWnd, wMsg, lParam1, lParam2 )
    HWND        hWnd;
    USHORT      wMsg;
    ULONG       lParam1;
    ULONG       lParam2;
{
    POINT       ptMouse;
    GSIZE       gsChar;
    HPS         hPS;
    LONG        ColorData[COLORDATAMAX];
    BOOL        bNowIconic;

    switch( wMsg )
    {
      case WM_BUTTON1DOWN:
        if( bMouseDown )
            break;
        WinStopTimer( hAB, hWnd, TIMER_MOVE );
        bMouseDown = TRUE;
        WinSetCapture( hAB, hWnd );
        ptMouse.x = LOUSHORT(lParam1);
        ptMouse.y = HIUSHORT(lParam1);
        SayMoveText( hWnd, ptMouse );
        return 0L;

      case WM_BUTTON1UP:
        if( ! bMouseDown )
            break;
        bMouseDown = FALSE;
        WinSetCapture( hAB, (HWND)NULL );
        ptMouse.x = LOUSHORT(lParam1);
        ptMouse.y = HIUSHORT(lParam1);
        SayMoveText( hWnd, ptMouse );
        WinStartTimer( hAB, hWnd, TIMER_MOVE, nInterval );
        return 0L;

      case WM_CHAR:
        if(
            ( LOUSHORT(lParam1) & KC_KEYUP )  ||
            ! ( LOUSHORT(lParam1) & KC_VIRTUALKEY )
        ) {
            break;
        }
        SayInvalidateText( hWnd );
        switch( HIUSHORT(lParam2) )
        {
          case VK_LEFT:
            rcText.xLeft -= ptCharSize.x;
            ptAdvance.x   = -1;
            ptAdvance.y   = 0;
            break;
          case VK_RIGHT:
            rcText.xLeft += ptCharSize.x;
            ptAdvance.x   = 1;
            ptAdvance.y   = 0;
            break;
          case VK_UP:
            rcText.yBottom -= ptCharSize.y >> 1;
            ptAdvance.x     = 0;
            ptAdvance.y     = -1;
            break;
          case VK_DOWN:
            rcText.yBottom += ptCharSize.y >> 1;
            ptAdvance.x     = 0;
            ptAdvance.y     = 1;
            break;
          default:
            return 0L;
        }
        SayInvalidateText( hWnd );
        nDistLeft = nDistance;
        return 0L;

      case WM_COMMAND:
        switch( LOUSHORT(lParam1) )
        {
          case CMD_ABOUT:
            WinDlgBox(
                (HWND)NULL, hWnd, (LPFNWP)SayAboutDlgProc,
                NULL, DLG_ABOUT, NULL
            );
            return 0L;

          case CMD_EXIT:
            WinDestroyWindow( hWndWhatFrame );
            return 0L;

          case CMD_WHAT:
            if( hWndPanel ) {
                WinSetWindowPos(
                    hWndPanel,
                    HWND_TOP,
                    0, 0, 0, 0,
                    SWP_ZORDER | SWP_ACTIVATE 
                );
            } else {
                hWndPanel = WinLoadDlg(
                    (HWND)NULL,
                    (HWND)NULL,
                    (LPFNWP)SayWhatDlgProc,
                    NULL,
                    DLG_WHAT,
                    NULL
                );
            }
        }
        return 0L;

      case WM_CREATE:
        /* find out character/screen sizes, number of colors */
        hPS = WinGetPS( hWnd );
        GpiQueryCharBox( hPS, &gsChar );
        GpiQueryColorData( hPS, (LONG)COLORDATAMAX, ColorData );
        WinReleasePS( hPS );
        lColorMax = ColorData[3];
        ptCharSize.x = gsChar.width;
        ptCharSize.y = gsChar.height;
        ptScreenSize.x =
          WinQuerySysValue( (HWND)NULL, SV_CXSCREEN );
        ptScreenSize.y =
          WinQuerySysValue( (HWND)NULL, SV_CYSCREEN );
        /* initialize timer */
        srand( (INT)time(NULL) );
        WinStartTimer( hAB, hWnd, TIMER_MOVE, nInterval );
        return 0L;

      case WM_DESTROY:
        if( hWndPanel )
            WinDestroyWindow( hWndPanel );
        WinPostQueueMsg( hMsgQ, WM_QUIT, 0L, 0L );
        return 0L;

      case WM_ERASEBACKGROUND:
        return 1L;  /* don't erase */

      case WM_MINMAX:
        bNowIconic = ( LOUSHORT(lParam1) == SWP_MINIMIZE );
        if( bIconic != bNowIconic ) {
            bIconic = bNowIconic;
            if( bIconic )
                WinStopTimer( hAB, hWnd, TIMER_CHAR );
            else
                WinStartTimer( hAB, hWnd, TIMER_CHAR, 1000 );
        }
        return 1L;

      case WM_MOUSEMOVE:
        if( bMouseDown ) {
            ptMouse.x = LOUSHORT(lParam1);
            ptMouse.y = HIUSHORT(lParam1);
            SayMoveText( hWnd, ptMouse );
        }
        return 0L;

      case WM_PAINT:
        SayWhatPaint( hWnd );
        return 0L;

      case WM_SIZE:
        SayInvalidateText( hWnd );
        nDistLeft = 0;
        SayAdvanceTextPos( hWnd );
        return 0L;

      case WM_TIMER:
        switch( LOUSHORT(lParam1) ) {
            case TIMER_MOVE:
                SayAdvanceTextPos( hWnd );
                break;
            case TIMER_CHAR:
                SayAdvanceTextChar( hWnd );
                break;
        }
        return 0L;
    }
    return WinDefWindowProc( hWnd, wMsg, lParam1, lParam2 );
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*  Main function for the application.  */

void cdecl main( nArgs, pArgs )
    INT         nArgs;
    PSZ         pArgs;
{
    QMSG        qMsg;

    if( ! SayInitApp() )
        SayExitApp( 1 );

    while( WinGetMsg( hAB, &qMsg, (HWND)NULL, 0, 0 ) ) {

        if( hWndPanel  &&  WinProcessDlgMsg( hWndPanel, &qMsg ) )
            continue;

        WinDispatchMsg( hAB, &qMsg );
    }

    SayExitApp( 0 );
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
