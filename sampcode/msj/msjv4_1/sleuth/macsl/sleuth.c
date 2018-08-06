/*-----------------------------------------------------------------*/
/* Sleuth.c                                                        */
/* Window snooper for Presentation Manager                         */
/*-----------------------------------------------------------------*/

#include "MyOs2.h"

#undef NULL

#include "Machine.h"

#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Sleuth.h"

/*-----------------------------------------------------------------*/
/*  The display for a window looks like this in collapsed mode:    */
/*                                                                 */
/*  Window HHHH:HHHH [id] {class} (L,B;R,T) "text"                 */
/*                                                                 */
/*  or like this in expanded mode:                                 */
/*                                                                 */
/*      Window handle: HHHH:HHHH   Owner window: HHHH:HHHH         */
/*        Class name: {class name}                                 */
/*        Window text: "text"                                      */
/*        Class style:  HHHHHHHH                                   */
/*        Window style: HHHHHHHH                                   */
/*        Class function: HHHH:HHHH   Window function: HHHH:HHHH   */
/*        Window ID: DDDDD   Process ID: HHHH   Thread ID: HHHH    */
/*        Message queue handle: HHHH   Window lock count: DDDDD    */
/*        Window extra alloc: DDDDD                                */
/*        Window rectangle: Left=D, Bottom=D, Right=D, Top=D       */
/*      {blank line}                                               */
/*                                                                 */
/*  Total number of lines for one window display: 11               */
/*-----------------------------------------------------------------*/

#define LINES_PER_WINDOW    11
#define WINDOW_WIDTH        160

/*-----------------------------------------------------------------*/
/* Structure of information for each window.                       */
/*-----------------------------------------------------------------*/

#define CLASSMAX    40
#define TEXTMAX     40

typedef struct {
    HWND    hwnd;                   /* Window handle */
    CLASSINFO class;                /* Class info */
    CHAR    szClass[CLASSMAX];      /* Class name */
    CHAR    szText[TEXTMAX];        /* Window title or contents */
    ULONG   flStyle;                /* WS_ window style */
    HWND    hwndOwner;              /* Owner window handle */
    PFNWP   pfnwp;                  /* Window function */
    USHORT  usWindowID;             /* Window ID */
    PID     ProcessID;              /* Process ID */
    TID     ThreadID;               /* Thread ID */
    HMQ     hmq;                    /* Message queue handle */
    RECTL   rclWindow;              /* Window rect, screen coord. */
    SHORT   sLockCount;             /* Window lock count */
    SHORT   sLevel;                 /* Child window nesting level */
    FLAG    fSelect:1;              /* Is this window selected? */
    FLAG    fHasText:1;             /* Does the window have text? */
} INFO;

typedef INFO * PINFO;               /* Pointer to INFO array */

/*-----------------------------------------------------------------*/
/* Static variables.                                               */
/*-----------------------------------------------------------------*/

PIDINFO     pidi;

HAB         hab;
HMQ         hmq;
HWND        hwndDesktop, hwndObject;
HWND        hwndSleuth, hwndFrame, hwndHorzScroll, hwndVertScroll;

PINFO       pInfoBase;
SHORT       sWinCount;              /* # of windows in system */
BOOL        fExpand = FALSE;        /* Expanded display mode? */
SHORT       sLinesEach = 1;         /* 1 or LINES_PER_WINDOW */
SHORT       sCharX;                 /* Character width in pixels */
SHORT       sCharY;                 /* Character height in pixels */
SHORT       sDescenderY;            /* Descender height */
SHORT       sExtLeading;            /* Vert. space between chars */
HPS         hpsPaint;               /* PS for SleuthPaint */
POINTL      ptlPaint;               /* Current pos for SleuthPaint */
CHAR        szClass[10];            /* Our window class name */
CHAR        szTitle[40];            /* Our window title */

/*-----------------------------------------------------------------*/
/* Function prototypes.                                            */
/*-----------------------------------------------------------------*/

VOID                main( VOID );
PSZ                 SleuthClassName( PSZ );
BOOL                SleuthGetAllWindows( VOID );
VOID                SleuthGetWindowTree( HWND hwnd, SHORT sLevel );
BOOL                SleuthInit( VOID );
PSZ                 SleuthMsgName( USHORT );
VOID                SleuthPaint( HWND hwndPaint );
VOID      CDECL     SleuthPaint1( CHAR* szFormat, ... );
VOID                SleuthQueryScrollBar( HWND hwndBar, SHORT* psPos,
                                          SHORT* psMin,
                                          SHORT* psMax );
SHORT               SleuthScroll( HWND hwnd, USHORT msg,
                                  USHORT idBar,
                                  USHORT cmd, SHORT sPos );
VOID                SleuthSetOneBar( HWND hwndBar, SHORT sMax );
VOID                SleuthSetScrollBars( VOID );
VOID                SleuthTerminate( VOID );
MRESULT   EXPENTRY  SleuthWndProc( HWND, USHORT, MPARAM, MPARAM );

/*-----------------------------------------------------------------*/
/* Application main program.                                       */
/*-----------------------------------------------------------------*/

VOID main()
{
    QMSG        qmsg;

    /* Initialize application, quit if any errors */

    if( ! SleuthInit() )
      return;

    /* Main message processing loop */

    while( WinGetMsg( hab, &qmsg, NULL, 0, 0 ) )
      WinDispatchMsg( hab, &qmsg );

    /* Application termination */

    SleuthTerminate();
}

/*-----------------------------------------------------------------*/
/* Convert a class name into its printable form.  Normal class     */
/* names are returned unchanged; the special WC_ "names" are       */
/* converted into text.                                            */
/*-----------------------------------------------------------------*/

typedef struct _CLASSNAMES {
    NPSZ        szNum;
    NPSZ        szName;
} CLASSNAMES;

typedef CLASSNAMES NEAR * NPCLASSNAMES;

CLASSNAMES aClassNames[] = {
    { "#1",  "WC_FRAME"      },
    { "#2",  "WC_DIALOG"     },
    { "#3",  "WC_BUTTON"     },
    { "#4",  "WC_MENU"       },
    { "#5",  "WC_STATIC"     },
    { "#6",  "WC_ENTRYFIELD" },
    { "#7",  "WC_LISTBOX"    },
    { "#8",  "WC_SCROLLBAR"  },
    { "#9",  "WC_TITLEBAR"   },
    { "#10", "WC_SIZEBORDER" },
    { NULL,  NULL            }
};

PSZ SleuthClassName( pszClass )
    PSZ         pszClass;
{
    NPCLASSNAMES npNames;

    if( pszClass[0] != '#' )
      return pszClass;

    for( npNames = &aClassNames[0];  npNames->szNum;  npNames++ )
      if( strcmp( pszClass, npNames->szNum ) == 0 )
        return npNames->szName;

    return pszClass;
}

/*-----------------------------------------------------------------*/
/* Gather up information on all windows in PM and fill in the      */
/* INFO structure for them.                                        */
/*-----------------------------------------------------------------*/

BOOL SleuthGetAllWindows()
{
    sWinCount = 0;

    /* Pick up both trees, from hwndDesktop and hwndObject */

    SleuthGetWindowTree( hwndDesktop, 0 );
    SleuthGetWindowTree( hwndObject,  0 );

    /* Set scroll bars based on new window count */

    SleuthSetScrollBars();

    /* Force our window to be repainted */

    WinInvalidateRect( hwndSleuth, NULL, TRUE );

    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Gather information on all windows in the tree starting at hwnd, */
/* and add an entry to the INFO array for each one.                */
/*-----------------------------------------------------------------*/

VOID SleuthGetWindowTree( hwnd, sLevel )
    HWND        hwnd;
    SHORT       sLevel;
{
    PINFO       pInfo;
/*  HENUM       henum;  */
    HWND        hwndChild;

    /* Count the window and allocate an INFO entry */

    sWinCount++;

    if( ! pInfoBase )
      pInfoBase = malloc( sizeof(INFO) );
    else
      pInfoBase = realloc( pInfoBase, sWinCount*sizeof(INFO) );

    if( ! pInfoBase )
      exit( 9 );

    pInfo = pInfoBase + sWinCount - 1;  /* -> INFO for this window */

    /* Gather up this window's information */

    pInfo->hwnd = hwnd;
    pInfo->fSelect = FALSE;
    pInfo->fHasText = FALSE;
    pInfo->class.flClassStyle  = 0L;
    pInfo->class.pfnWindowProc = 0L;
    pInfo->class.cbWindowData  = 0;

    pInfo->flStyle = WinQueryWindowULong( hwnd, QWL_STYLE );
    pInfo->hwndOwner = WinQueryWindow( hwnd, QW_OWNER, FALSE );
    pInfo->pfnwp = WinQueryWindowPtr( hwnd, QWP_PFNWP );
    pInfo->usWindowID = WinQueryWindowUShort( hwnd, QWS_ID );
    WinQueryWindowProcess( hwnd, &pInfo->ProcessID,
                                 &pInfo->ThreadID );
    pInfo->hmq = WinQueryWindowPtr( hwnd, QWL_HMQ );
    WinQueryWindowRect( hwnd, &pInfo->rclWindow );
    pInfo->sLockCount = WinQueryWindowLockCount( hwnd );
    pInfo->sLevel = sLevel;

    if( hwnd == hwndDesktop )
      strcpy( pInfo->szClass, "WC_DESKTOP" );
    else if( hwnd == hwndObject )
      strcpy( pInfo->szClass, "WC_OBJECT" );
    else
    {
      WinQueryClassName( hwnd, sizeof(pInfo->szClass),
                         pInfo->szClass );
      WinQueryClassInfo( hab, pInfo->szClass, &pInfo->class );
      if( ! WinIsRectEmpty( hab, &pInfo->rclWindow ) )
        WinMapWindowRect( hwnd, WinQueryWindow(hwnd,QW_PARENT,FALSE),
                          &pInfo->rclWindow );
    }

    pInfo->szText[0] = '\0';
    if( pInfo->ProcessID == pidi.pid )
      pInfo->fHasText =  /* wrong... */
        !! WinQueryWindowText( hwnd, sizeof(pInfo->szText),
                               pInfo->szText );

    /* Recurse through all child windows.  The #if'd out code is the
       "right" way to do this, but it crashed PM in the preliminary
       version I used.  The other code works OK, but doesn't protect
       against windows being destroyed by other threads while we're
       executing! */

#if 0
    henum = WinBeginEnumWindows( hwnd );

    while( hwndChild = WinGetNextWindow(henum) )
      SleuthGetWindowTree( hwndChild, sLevel+1 );

    WinEndEnumWindows( henum );
#else
    for( hwndChild = WinQueryWindow( hwnd, QW_TOP, FALSE );
         hwndChild;
         hwndChild = WinQueryWindow( hwndChild, QW_NEXT, FALSE ) )
      SleuthGetWindowTree( hwndChild, sLevel+1 );
#endif
}

/*-----------------------------------------------------------------*/
/* Initialize the application.                                     */
/*-----------------------------------------------------------------*/

BOOL SleuthInit()
{
    HDC         hps;
    FONTMETRICS fm;
    SHORT       sScreenX;
    SHORT       sScreenY;
    ULONG       flFrameFlags;

    /* Pick up the basic information we need */

    DosGetPID( &pidi );

    hab = WinInitialize( 0 );
    hmq = WinCreateMsgQueue( hab, 0 );

    hwndDesktop = WinQueryDesktopWindow( hab, NULL );
    hwndObject  = WinQueryObjectWindow( hwndDesktop );

    sScreenX = (SHORT)WinQuerySysValue( hwndDesktop, SV_CXSCREEN );
    sScreenY = (SHORT)WinQuerySysValue( hwndDesktop, SV_CYSCREEN );

    /* Calculate character size for system font */

    hps = WinGetPS( hwndDesktop );

    GpiQueryFontMetrics( hps, (LONG)sizeof(fm), &fm );

    sCharX = (SHORT)fm.lAveCharWidth;
    sCharY = (SHORT)fm.lMaxBaselineExt;
    sDescenderY = (SHORT)fm.lMaxDescender;

    WinReleasePS( hps );

    /* Load strings from resource file */

    WinLoadString( hab, NULL, IDS_CLASS, sizeof(szClass), szClass );
    WinLoadString( hab, NULL, IDS_TITLE, sizeof(szTitle), szTitle );

    /* Register our window class and create main window */

    WinRegisterClass( hab, szClass, SleuthWndProc, 0L, 0 );

/* original ------

    flFrameFlags = FCF_STANDARD | FCF_VERTSCROLL | FCF_HORZSCROLL;
    hwndFrame =
      WinCreateStdWindow( hwndDesktop, FS_ICON, &flFrameFlags,
                          szClass, szTitle, 0L, NULL,
                          ID_SLEUTH, &hwndSleuth );

------- original - change made by Charles Petzold */

        flFrameFlags = FCF_SIZEBORDER | FCF_TITLEBAR | 
                        FCF_MINMAX | FCF_SYSMENU | FCF_MENU | FCF_ICON | 
                        FCF_VERTSCROLL | FCF_HORZSCROLL;
        hwndFrame =
        WinCreateStdWindow (hwndDesktop, WS_VISIBLE, &flFrameFlags,
                                szClass, szTitle, 0L, NULL,
                                ID_SLEUTH, &hwndSleuth) ;

    hwndHorzScroll = WinWindowFromID( hwndFrame, FID_HORZSCROLL );
    hwndVertScroll = WinWindowFromID( hwndFrame, FID_VERTSCROLL );

    /* Set the window position.  Change the #if 1 to #if 0 for more
       convenient debugging under Lightspeed's debugger.  This will
       put our window in the top part of the screen, up above the
       debugger windows. */

#if 1
    WinSetWindowPos( hwndFrame, NULL,
                     sScreenX *  1 / 20,     /* X: 5% from left */
                     sScreenY *  2 / 10,     /* Y  20% from bottom */
                     sScreenX *  9 / 10,     /* nWidth: 90% */
                     sScreenY *  7 / 10,     /* nHeight: 70% */
                     SWP_MOVE | SWP_SIZE );
#else
    WinSetWindowPos( hwndFrame, NULL,
                     sScreenX *  1 / 10,     /* X: 10% from left */
                     sScreenY *  6 / 10,     /* Y  60% from bottom */
                     sScreenX *  8 / 10,     /* nWidth: 80% */
                     sScreenY *  3 / 10,     /* nHeight: 30% */
                     SWP_MOVE | SWP_SIZE );
#endif

    /* Make our window visible now, so it's included in the list */

    WinShowWindow( hwndFrame, TRUE );

    /* Post a message to ourself to trigger the first  display */

    WinPostMsg( hwndSleuth, WM_COMMAND, MPFROMSHORT(CMD_LOOK), 0L );

    return TRUE;
//  return FALSE;
}

/*-----------------------------------------------------------------*/
/* Paint our window.                                               */
/*-----------------------------------------------------------------*/

VOID SleuthPaint( hwnd )
    HWND        hwnd;
{
    SHORT       sWin;
    SHORT       X;
    SHORT       sScrollY, sScrollX;
    RECTL       rclPaint, rclClient;
    PINFO       pInfo;
    CHAR        szQuote[2];

    /* Get the PS and erase it */

    hpsPaint = WinBeginPaint( hwnd, NULL, &rclPaint );

    GpiErase( hpsPaint );

    /* Find out how big the window is and how it's scrolled */

    WinQueryWindowRect( hwnd, &rclClient );
    sScrollX =
      (SHORT)WinSendMsg( hwndHorzScroll, SBM_QUERYPOS, 0, 0 );
    sScrollY =
      (SHORT)WinSendMsg( hwndVertScroll, SBM_QUERYPOS, 0, 0 );

    /* Calculate horizontal paint pos from scroll bar pos */

    X = /* ( 1 */ - sScrollX /* ) */ * sCharX;

    /* Calculate index into INFO array and vertical paint pos,
       from scroll bar pos and top of painting rectangle */

    sWin =
      ( ( (SHORT)rclClient.yTop - (SHORT)rclPaint.yTop ) / sCharY
        + sScrollY )
      / sLinesEach;

    ptlPaint.y =
      (SHORT)rclClient.yTop + sDescenderY
        - ( sWin * sLinesEach - sScrollY + 1 ) * sCharY;

    pInfo = pInfoBase + sWin;

    /* Loop through and paint each entry */

    while( sWin < sWinCount  &&
           (SHORT)ptlPaint.y + sCharY >= (SHORT)rclPaint.yBottom )
    {
      /* Set X position and indent child windows */

      ptlPaint.x = X + pInfo->sLevel * sCharX * (fExpand ? 4 : 2);

      szQuote[0] = szQuote[1] = '\0';
      if( pInfo->fHasText )
        szQuote[0] = '"';

      if( ! fExpand )
      {
        /* Paint the one-liner */

        SleuthPaint1(
          "%08lX [%04X] {%s} (%d,%d;%d,%d) %s%s%s",
          pInfo->hwnd,
          pInfo->usWindowID,
          SleuthClassName( pInfo->szClass ),
          (INT)pInfo->rclWindow.xLeft,
          (INT)pInfo->rclWindow.yBottom,
          (INT)pInfo->rclWindow.xRight,
          (INT)pInfo->rclWindow.yTop,
          szQuote, pInfo->szText, szQuote
        );
      }
      else
      {
#if 0
        /* Paint the expanded form, first the window handle */

        Paint(
          "%s handle: %04X",
          pTypeName,
          lpInfo->winHWnd
        );

        /* Paint the rest of the info, indented two spaces more */

        sPaintX += sCharX * 2;

        Paint( "Class name: %Fs", lpInfo->winClass );
        Paint( "Window title: %Fs", lpInfo->winTitle );
        Paint( "Parent window handle: %04X", lpInfo->winHWndParent );
        Paint(
          "Class function, Window function: %p, %p",
          lpInfo->winClassProc,
          lpInfo->winWndProc
        );
        Paint(
          "Class module handle, Window instance handle: %04X, %04X",
          lpInfo->winClassModule,
          lpInfo->winInstance
        );
        Paint(
          "Class extra alloc, Window extra alloc: %d, %d",
          lpInfo->winClsExtra,
          lpInfo->winWndExtra
        );
        Paint(
          "Class style, Window style: %04X, %08lX",
          lpInfo->winClassStyle,
          lpInfo->winStyle
        );
        Paint(
          lpInfo->winStyle & WS_CHILD ?  "Control ID: %d" :
                                         "Menu handle: %04X",
          lpInfo->winControlID
        );
        Paint(
          "Brush, Cursor, Icon handles: %04X, %04X, %04X",
          lpInfo->winBkgdBrush,
          lpInfo->winCursor,
          lpInfo->winIcon
        );
        Paint(
          "Window rect: Left=%4d, Top=%4d, Right=%4d, Bottom=%4d",
          lpInfo->winWindowRect.left,
          lpInfo->winWindowRect.top,
          lpInfo->winWindowRect.right,
          lpInfo->winWindowRect.bottom
        );
        Paint(
          "Client rect: Left=%4d, Top=%4d, Right=%4d, Bottom=%4d",
          lpInfo->winClientRect.left,
          lpInfo->winClientRect.top,
          lpInfo->winClientRect.right,
          lpInfo->winClientRect.bottom
        );

        /* Make a blank line */

        sPaintY -= sCharY;
#endif
      }

      /* Increment to next INFO entry */
      sWin++;
      pInfo++;
    }

    WinEndPaint( hpsPaint );
}

/*-----------------------------------------------------------------*/
/* Paint one line of text, using the global variables hpsPaint and */
/* ptlPaint.  The #ifdef PM_MACINTOSH is because Lightspeed C      */
/* doesn't like the ... notation, and Microsoft C doesn't like to  */
/* do without it!                                                  */
/*-----------------------------------------------------------------*/

#ifdef PM_MACINTOSH
VOID CDECL SleuthPaint1( szFormat )
#else
VOID CDECL SleuthPaint1( szFormat, ... )
#endif
    CHAR *      szFormat;
{
    va_list     pArgs;
    CHAR        szBuf[160];

    va_start( pArgs, szFormat );

    GpiCharStringAt(
      hpsPaint, &ptlPaint,
      (LONG)vsprintf( szBuf, szFormat, pArgs ),
      szBuf
    );

    va_end( pArgs );

    ptlPaint.y -= sCharY;
}

/*-----------------------------------------------------------------*/
/* Get a scroll bar's range and position.  More convenient than    */
/* sending the messages every time.                                */
/*-----------------------------------------------------------------*/

VOID SleuthQueryScrollBar( hwndBar, psPos, psMin, psMax )
    HWND        hwndBar;
    SHORT*      psPos;
    SHORT*      psMin;
    SHORT*      psMax;
{
    MRESULT     mrRange;

    *psPos  = (SHORT)WinSendMsg( hwndBar, SBM_QUERYPOS, 0, 0 );

    mrRange = WinSendMsg( hwndBar, SBM_QUERYRANGE, 0, 0 );
    *psMin = SHORT1FROMMR(mrRange);
    *psMax = SHORT2FROMMR(mrRange);
}

/*-----------------------------------------------------------------*/
/* Scroll hwnd and adjust idBar according to cmd and sPos.         */
/*-----------------------------------------------------------------*/

SHORT SleuthScroll( hwnd, msg, idBar, cmd, sPos )
    HWND        hwnd;
    USHORT      msg;
    USHORT      idBar;
    USHORT      cmd;
    SHORT       sPos;
{
    HWND        hwndBar;
    SHORT       sOldPos;
    SHORT       sDiff;
    SHORT       sMin;
    SHORT       sMax;
    SHORT       sPageSize;
    RECTL       rcl;

    /* Get old scroll position and scroll range */

    hwndBar =
      WinWindowFromID( WinQueryWindow(hwnd,QW_PARENT,FALSE), idBar );

    SleuthQueryScrollBar( hwndBar, &sOldPos, &sMin, &sMax );

    /* Calculate page size, horizontal or vertical as needed */

    WinQueryWindowRect( hwnd, &rcl );

    if( msg == WM_HSCROLL )
      sPageSize = ( (SHORT)rcl.xRight - (SHORT)rcl.xLeft) / sCharX;
    else
      sPageSize = ( (SHORT)rcl.yTop - (SHORT)rcl.yBottom) / sCharY;

    /* Select the amount to scroll by, based on the scroll message */

    switch( cmd )
    {
      case SB_LINEUP:
        sDiff = -1;
        break;

      case SB_LINEDOWN:
        sDiff = 1;
        break;

      case SB_PAGEUP:
        sDiff = -sPageSize;
        break;

      case SB_PAGEDOWN:
        sDiff = sPageSize;
        break;

      case SB_SLIDERPOSITION:
        sDiff = sPos - sOldPos;
        break;

      case SBX_TOP:
        sDiff = -30000;  /* Kind of a kludge but it works... */
        break;

      case SBX_BOTTOM:
        sDiff = 30000;
        break;

      default:
        return 0;
    }

    /* Limit scroll destination to sMin..sMax */

    if( sDiff < sMin - sOldPos )
      sDiff = sMin - sOldPos;

    if( sDiff > sMax - sOldPos )
      sDiff = sMax - sOldPos;

    /* Return if net effect is nothing */

    if( sDiff == 0 )
      return 0;

    /* Set the new scroll bar position and scroll the window */

    WinSendMsg( hwndBar, SBM_SETPOS, MRFROMSHORT(sOldPos+sDiff), 0 );

    WinScrollWindow(
      hwnd,
      msg == WM_HSCROLL ?  -sDiff*sCharX : 0,
      msg == WM_VSCROLL ?   sDiff*sCharY : 0,
      NULL, NULL, NULL, NULL,
      SW_INVALIDATERGN
    );

    /* Force an immediate update for cleaner appearance */

    WinUpdateWindow( hwnd );

    return sDiff;
}

/*-----------------------------------------------------------------*/
/* Set one scroll bar's position and range.                        */
/*-----------------------------------------------------------------*/

VOID SleuthSetOneBar( hwndBar, sMax )
    HWND        hwndBar;
    SHORT       sMax;
{
    SHORT       sPos, sOldMin, sOldMax;

    SleuthQueryScrollBar( hwndBar, &sPos, &sOldMin, &sOldMax );

    if( sMax <= 0 )
      sMax = sPos = 0;

    if( sMax != sOldMax )
    {
      WinSendMsg( hwndBar, SBM_SETSCROLLBAR,
                  MPFROMSHORT(sPos), MPFROM2SHORT(0,sMax) );

      WinEnableWindow( hwndBar, !!(sMax) );
    }
}

/*-----------------------------------------------------------------*/
/* Set both scroll bars according to the window size and the       */
/* number of INFO entries.                                         */
/*-----------------------------------------------------------------*/

VOID SleuthSetScrollBars()
{
    RECTL       rcl;

    WinQueryWindowRect( hwndSleuth, &rcl );

    SleuthSetOneBar( hwndHorzScroll,
                     WINDOW_WIDTH - (SHORT)rcl.xRight / sCharX );

    SleuthSetOneBar( hwndVertScroll,
                     sWinCount*sLinesEach - (SHORT)rcl.yTop/sCharY );
}

/*-----------------------------------------------------------------*/
/* Terminate the application.                                      */
/*-----------------------------------------------------------------*/

VOID SleuthTerminate()
{
    WinDestroyWindow( hwndFrame );
    WinDestroyMsgQueue( hmq );
    WinTerminate( hab );

    exit( 0 );
}

/*-----------------------------------------------------------------*/
/* Window function for Sleuth's main window.                       */
/*-----------------------------------------------------------------*/

MRESULT EXPENTRY SleuthWndProc( hwnd, msg, mp1, mp2 )
    HWND          hwnd;
    USHORT        msg;
    MPARAM        mp1;
    MPARAM        mp2;
{
    switch( msg )
    {
      /* Tell PM that we don't need no stinkin' upside down
         coordinates! */

      case WM_CALCVALIDRECTS:
        return MRFROMSHORT( CVR_ALIGNLEFT | CVR_ALIGNTOP );

      /* Menu command message - process the command */

      case WM_COMMAND:
        switch( COMMANDMSG(&msg)->cmd )
        {
          case CMD_ABOUT:
            return 0L;

          case CMD_EXIT:
            WinPostMsg( hwnd, WM_QUIT, 0L, 0L );
            return 0L;

          case CMD_LOOK:
            SleuthGetAllWindows();
            return 0L;
        }
        return 0L;

        /* Scroll messages - scroll the window */

        case WM_HSCROLL:
        case WM_VSCROLL:
          SleuthScroll( hwnd, msg, SHORT1FROMMP(mp1),
                        SHORT2FROMMP(mp2), SHORT1FROMMP(mp2) );
          return 0L;

        /* Key-down message - handle cursor keys, ignore others */

        case WM_CHAR:
          switch( CHARMSG(&msg)->vkey )
          {
            case VK_LEFT:
            case VK_RIGHT:
              return WinSendMsg( hwndHorzScroll, msg, mp1, mp2 );
            case VK_UP:
            case VK_DOWN:
            case VK_PAGEUP:
            case VK_PAGEDOWN:
              return WinSendMsg( hwndVertScroll, msg, mp1, mp2 );
          }
          return 0L;

        /* Paint message - repaint all or part of our window */

        case WM_PAINT:
          SleuthPaint( hwnd );
          return 0L;

        /* Size message - recalculate our scroll bars */

        case WM_SIZE:
          SleuthSetScrollBars();
          return 0L;
    }

    /* All other messages go to DefWindowProc */

    return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}

/*-----------------------------------------------------------------*/
