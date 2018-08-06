/*
    wndproc.c -- Window Procedure for Clock Client Window
    Created by Microsoft Corporation, 1989
*/
#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include "clock.h"
#include "res.h"

MRESULT EXPENTRY ClkAboutDlgProc ( HWND , USHORT , MPARAM , MPARAM ) ;
MRESULT EXPENTRY ClkTicksDlgProc ( HWND , USHORT , MPARAM , MPARAM ) ;
MRESULT EXPENTRY ClkColorsDlgProc ( HWND , USHORT , MPARAM , MPARAM ) ;

extern HAB hab ;
extern HWND hwndFrame ;
extern BOOL fStartAsIcon ;
extern VOID ClkDrawFace( HPS );
extern VOID ClkDrawHand( HPS, SHORT, SHORT );

VOID ClkPaint ( HWND ) ;
VOID ClkCreate ( HWND ) ;
VOID ClkSize ( HWND ) ;
VOID ClkTimer ( VOID ) ;
VOID ClkCommand ( HWND , MPARAM ) ;

VOID ClkHideFrameControls ( HWND ) ;
VOID ClkShowFrameControls ( HWND ) ;

SWP swp ;
HPS hps ;
HDC hdc ;
RECTL rclPage ;
DATETIME dt ;
BOOL f ;
BOOL fIconic , fShowSecondHand ;
BOOL fControlsHidden = FALSE ;
SIZEL sizl = { 200 , 200 } ;
ULONG cxRes , cyRes ;

HWND hwndTitleBar , hwndSysMenu , hwndMinMax , hwndMenu ;


/*
    ClkWndProc() -- Window Procedure for Clock Client Window
    Returns:  MRESULT (Message Result)
*/
MRESULT EXPENTRY ClkWndProc ( HWND hwnd , USHORT usMsg ,
				MPARAM mp1 , MPARAM mp2 )
{

    switch ( usMsg ) {

	case WM_TIMER :
	    ClkTimer ( ) ;
	    return NULL ;

	case WM_PAINT :
	    ClkPaint ( hwnd ) ;
	    return NULL ;

	 case WM_SIZE :
	    ClkSize ( hwnd ) ;
	    return NULL ;

	case WM_COMMAND :
	    ClkCommand ( hwnd , mp1 ) ;
	    return NULL ;

	case WM_BUTTON1DOWN :
	    return WinSendMsg ( hwndFrame , WM_TRACKFRAME ,
		    ( MPARAM ) ( LOUSHORT ( mp2 ) | TF_MOVE ) , NULL ) ;

	case WM_CHAR:
	    if ( fControlsHidden )
		ClkShowFrameControls ( hwndFrame ) ;
	    return NULL ;

	case WM_BUTTON1DBLCLK :
	    if ( fControlsHidden )
		ClkShowFrameControls ( hwndFrame ) ;
	    else
		ClkHideFrameControls ( hwndFrame ) ;
	    return NULL ;

	case WM_CREATE :
	    ClkCreate ( hwnd ) ;
	    return NULL ;

	default :
	    /* let default window procedure handle it. */
	    return ( WinDefWindowProc ( hwnd , usMsg , mp1 , mp2 ) ) ;
    }
}


/*
    ClkCreate() -- Initialize newly created client window
*/
VOID ClkCreate ( HWND hwnd )
{
    LONG cxScreen , cyScreen ;	/* screen dimensions */
    LONG xLeft , yBottom ;	/* frame window location */
    LONG cxWidth , cyHeight ;	/* frame window size */
    HWND hwndFrame , hwndMenu ;

    /* we are called before the global hwndFrame is valid */
    hwndFrame = WinQueryWindow ( hwnd , QW_PARENT , FALSE ) ;

    /* load our menus */
    hwndMenu = WinLoadMenu ( hwndFrame , (HMODULE) NULL , ID_RESOURCE ) ;

    /* open a device context and create a presentation space */
    hdc = WinOpenWindowDC ( hwnd ) ;
    hps = GpiCreatePS ( hab , hdc , & sizl ,
			PU_ARBITRARY | GPIT_MICRO | GPIA_ASSOC ) ;

    /* determine screen dimensions */
    cxScreen = WinQuerySysValue (HWND_DESKTOP , SV_CXSCREEN ) ;
    cyScreen = WinQuerySysValue (HWND_DESKTOP , SV_CYSCREEN ) ;

    /* calculate an initial window position and size */
    xLeft = cxScreen / 8 ;
    yBottom = cyScreen / 2 ;
    cxWidth = cxScreen / 3 ;
    cyHeight = cyScreen / 2 ;

    /* get the device resolutions so we can make the face appear circular */
    DevQueryCaps ( hdc , CAPS_VERTICAL_RESOLUTION , 1L , & cyRes ) ;
    DevQueryCaps ( hdc , CAPS_HORIZONTAL_RESOLUTION , 1L , & cxRes ) ;

    /* position the window and make it visible */
    WinSetWindowPos ( hwndFrame , HWND_TOP ,
		      (SHORT) xLeft , (SHORT) yBottom ,
		      (SHORT) cxWidth , (SHORT) cyHeight ,
		      SWP_SIZE | SWP_MOVE | SWP_ACTIVATE ) ;

    /* have we been asked to start as an icon? */
    if ( fStartAsIcon )
	WinSetWindowPos ( hwndFrame , NULL , 0 , 0 , 0 , 0 , SWP_MINIMIZE ) ;

    WinShowWindow ( hwndFrame , TRUE ) ;

    /* get the time in a format for dislaying */
    DosGetDateTime ( & dt ) ;
    dt . hours = (UCHAR)(( dt . hours * 5 ) % 60 + dt . minutes / 12 );

    /* start a timer */
    WinStartTimer ( hab , hwnd , ID_RESOURCE , 1000 ) ;
}


/*
    ClkSize() -- Window Sizing Processing

    When the window has been sized, we calculate a page
    rectangle which:  (a) fills the window rectangle in
    either the x or y dimension, (b) appears square, and
    (c) is centered in the window rectangle.
*/
VOID ClkSize ( HWND hwnd )
{
    HWND hwndFrame ;
    RECTL rclWindow ;
    ULONG cxWidth , cyHeight ;
    ULONG cxSquare , cySquare , cxEdge , cyEdge ;

    /* get the width and height of the window rectangle */
    WinQueryWindowRect ( hwnd , & rclWindow ) ;
    cxWidth = rclWindow . xRight - rclWindow . xLeft ;
    cyHeight = rclWindow . yTop - rclWindow . yBottom ;

    /* assume the size of the page rectangle is constrained in the y dimension,
     * compute the x size which would make the rectangle appear square, then
     * check the assumption and do the reverse calculation if necessary */

    cySquare = cyHeight ;
    cxSquare = ( cyHeight * cxRes ) / cyRes ;
    if ( cxWidth < cxSquare ) {
	cxSquare = cxWidth ;
	cySquare = ( cxWidth * cyRes ) / cxRes ;
    }

    /* fill in the page rectangle and set the page viewport */
    cxEdge = ( cxWidth - cxSquare ) / 2 ;
    cyEdge = ( cyHeight - cySquare ) / 2 ;
    rclPage . xLeft = cxEdge ;
    rclPage . xRight = cxWidth - cxEdge ;
    rclPage . yBottom = cyEdge ;
    rclPage . yTop = cyHeight - cyEdge ;
    f = GpiSetPageViewport ( hps , & rclPage ) ;

    /* are we iconic? */
    hwndFrame = WinQueryWindow ( hwnd , QW_PARENT , FALSE ) ;
    f = WinQueryWindowPos ( hwndFrame , & swp ) ;
    fIconic = swp . fs & SWP_MINIMIZE ;
    fShowSecondHand = ! fIconic ;
}


/*
    ClkTimer() -- Handles timer events
*/
VOID ClkTimer ( VOID )
{
    DATETIME dtNew ;

    /* get the new time */
    DosGetDateTime ( & dtNew ) ;

    /* adjust the hour hand */
    dtNew . hours =(UCHAR)( ( dtNew . hours * 5 ) % 60 + dtNew . minutes / 12 );

    /* if we must move the hour and minute hands, redraw it all */
    if ( dtNew . minutes != dt . minutes ) {

	ClkDrawFace ( hps ) ;
	ClkDrawHand ( hps , HT_HOUR , dtNew . hours ) ;
	ClkDrawHand ( hps , HT_MINUTE , dtNew . minutes ) ;

	if ( fShowSecondHand ) {
	    GpiSetMix ( hps , FM_INVERT ) ;
	    ClkDrawHand ( hps , HT_SECOND , dtNew . seconds ) ;
	}
    }

    /* otherwise just undraw the old second hand and draw the new */
    else if ( fShowSecondHand ) {
	GpiSetMix ( hps , FM_INVERT ) ;
	ClkDrawHand ( hps , HT_SECOND , dt . seconds ) ;
	ClkDrawHand ( hps , HT_SECOND , dtNew . seconds ) ;
    }

    dt = dtNew ;
}


/*
    ClkCommand() -- Handle WM_COMMAND events
*/
VOID ClkCommand ( HWND hwnd , MPARAM mp1 )
{
    switch ( SHORT1FROMMP ( mp1 ) ) {

	case IDM_ABOUT :
	    WinDlgBox ( HWND_DESKTOP , hwnd , ClkAboutDlgProc , (HMODULE) NULL ,
			IDD_ABOUT , NULL ) ;
	    break ;

	case IDM_TICKS :
	    WinDlgBox ( HWND_DESKTOP , hwnd , ClkTicksDlgProc , (HMODULE) NULL ,
			IDD_TICKS , NULL ) ;
	    break ;

	case IDM_COLORS :
	    WinDlgBox ( HWND_DESKTOP , hwnd , ClkColorsDlgProc , (HMODULE) NULL ,
			IDD_COLORS , NULL ) ;
	    break ;

	case IDM_HIDECONTROLS :
	    ClkHideFrameControls ( hwndFrame ) ;
	    break ;
    }
}


/*
    ClkHideFrameControls() -- Hide the title bar and associated controls
*/
VOID ClkHideFrameControls ( HWND hwndFrame )
{

    hwndTitleBar = WinWindowFromID ( hwndFrame , FID_TITLEBAR ) ;
    hwndSysMenu = WinWindowFromID ( hwndFrame , FID_SYSMENU ) ;
    hwndMinMax = WinWindowFromID ( hwndFrame , FID_MINMAX ) ;
    hwndMenu = WinWindowFromID ( hwndFrame , FID_MENU ) ;

    WinSetParent ( hwndTitleBar , HWND_OBJECT , FALSE ) ;
    WinSetParent ( hwndSysMenu , HWND_OBJECT , FALSE ) ;
    WinSetParent ( hwndMinMax , HWND_OBJECT , FALSE ) ;
    WinSetParent ( hwndMenu , HWND_OBJECT , FALSE ) ;

    WinSendMsg ( hwndFrame , WM_UPDATEFRAME ,
	( MPARAM ) ( FCF_TITLEBAR | FCF_SYSMENU | FCF_MINMAX | FCF_MENU ) ,
	NULL ) ;

    fControlsHidden = TRUE ;
}


/*
    ClkShowFrameControls() -- Show the title bar and associated controls
*/
VOID ClkShowFrameControls ( HWND hwndFrame )
{

    WinSetParent ( hwndTitleBar , hwndFrame , FALSE ) ;
    WinSetParent ( hwndSysMenu , hwndFrame , FALSE ) ;
    WinSetParent ( hwndMinMax , hwndFrame , FALSE ) ;
    WinSetParent ( hwndMenu , hwndFrame , FALSE ) ;

    WinSendMsg ( hwndFrame , WM_UPDATEFRAME ,
	( MPARAM ) ( FCF_TITLEBAR | FCF_SYSMENU | FCF_MINMAX | FCF_MENU ) ,
	NULL ) ;
    WinInvalidateRect ( hwndFrame , NULL , TRUE ) ;

    fControlsHidden = FALSE ;
}
