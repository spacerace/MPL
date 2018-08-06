/*
    dialogs.c		Dialog procedures for PM Clock Application

    Created by Microsoft Corporation, 1989
*/
#define INCL_WINDIALOGS
#define INCL_WINBUTTONS
#define INCL_WINSYS
#include <os2def.h>
#include <pmwin.h>
#include "dialogs.h"

/* defined in paint.c */
extern USHORT usMajorTickPref ;
extern USHORT usMinorTickPref ;
extern LONG clrBackground ;
extern LONG clrFace ;
extern LONG clrHourHand ;
extern LONG clrMinuteHand ;

/* defined in clock.c */
extern HWND hwndFrame ;

/*
    ClkAboutDlgProc() 		"About..." dialog

    Returns:	MRESULT, 0 or return value from WinDefDlgProc
*/
MRESULT EXPENTRY ClkAboutDlgProc ( HWND hwnd , USHORT usMsg ,
				   MPARAM mp1 , MPARAM mp2 )
{
    if ( usMsg == WM_COMMAND ) {
	WinDismissDlg ( hwnd , TRUE ) ;
	return 0L ;
    }
    else return WinDefDlgProc ( hwnd , usMsg , mp1 , mp2 ) ;
}


/*
    ClkTicksDlgProc()		"Ticks..." dialog

    Returns: MRESULT, 0 or return value from WinDefDlgProc
*/
MRESULT EXPENTRY ClkTicksDlgProc ( HWND hwnd , USHORT usMsg ,
				   MPARAM mp1 , MPARAM mp2 )
{
    static USHORT usMajorTickSel ;
    static USHORT usMinorTickSel ;
    USHORT usButtonID ;

    switch ( usMsg ) {

	case WM_INITDLG :

	    /* show the current major tick preference */
	    WinSendMsg ( WinWindowFromID ( hwnd ,
					   CLKTM_MAJOR | usMajorTickPref ) ,
			 BM_SETCHECK , MPFROM2SHORT ( TRUE , NULL ) , NULL ) ;

	    /* show the current minor tick preference */
	    WinSendMsg ( WinWindowFromID ( hwnd ,
					   CLKTM_MINOR | usMinorTickPref ) ,
			 BM_SETCHECK , MPFROM2SHORT ( TRUE , NULL ) , NULL ) ;

	    /* load the selection values from the preferences */
	    usMajorTickSel = usMajorTickPref ;
	    usMinorTickSel = usMinorTickPref ;

	    /* let the default dialog procedure handle anything else */
	    break ;

	case WM_COMMAND :

	    switch ( LOUSHORT ( mp1 ) ) {

		case DID_OK :

		    /* store away selections as preferences */
		    usMajorTickPref = usMajorTickSel ;
		    usMinorTickPref = usMinorTickSel ;

		    /* repaint with the new preferences */
		    WinInvalidateRect ( hwndFrame , NULL, TRUE ) ;

		case DID_CANCEL :
		    WinDismissDlg ( hwnd , TRUE ) ;
	    }

	    return NULL ;

	case WM_CONTROL :

	    if ( SHORT2FROMMP ( mp1 ) == BN_CLICKED ) {

		usButtonID = SHORT1FROMMP ( mp1 ) ;

		switch ( usButtonID & 0xff00 ) {

		    case CLKTM_MAJOR :
			usMajorTickSel = LOBYTE ( usButtonID ) ;
			break ;

		    case CLKTM_MINOR :
			usMinorTickSel = LOBYTE ( usButtonID ) ;
			break ;
		}
	    }

	    /* fall through to the default control processing */
    }

    return WinDefDlgProc ( hwnd , usMsg , mp1 , mp2 ) ;
}


/*
    ClkColorsDlgProc()		"Clock Color Preferences" Dialog

    Returns: MRESULT, 0 or return value from WinDefDlgProc
*/
MRESULT EXPENTRY ClkColorsDlgProc ( HWND hwnd , USHORT usMsg ,
				   MPARAM mp1 , MPARAM mp2 )
{
    USHORT usButtonID ;
    static USHORT usCheckedButtonID ;
    HWND hwndButton ;
    RECTL rclButton , rclButtonInterior ;
    static LONG clrBackgroundNew ;
    static LONG clrFaceNew ;
    static LONG clrHourHandNew ;
    static LONG clrMinuteHandNew ;
    static LONG * pclrNew ;

    switch ( usMsg ) {

	case WM_INITDLG :

	    /* load the new values from the current ones */
	    clrBackgroundNew = clrBackground ;
	    clrFaceNew = clrFace ;
	    clrHourHandNew = clrHourHand ;
	    clrMinuteHandNew = clrMinuteHand ;

	    /* click the "Background" radio button */
	    WinSendMsg ( WinWindowFromID ( hwnd , CLKCLR_BACKGROUND ) ,
			 BM_CLICK , MPFROMSHORT ( TRUE ) , NULL ) ;

	    /* let the default dialog procedure handle anything else */
	    break ;

	case WM_COMMAND :

	    switch ( LOUSHORT ( mp1 ) ) {
		case DID_OK :

		    /* store the new values */
		    clrBackground = clrBackgroundNew ;
		    clrFace = clrFaceNew ;
		    clrHourHand = clrHourHandNew ;
		    clrMinuteHand = clrMinuteHandNew ;

		    /* repaint with the new colors */
		    WinInvalidateRect ( hwndFrame , NULL, TRUE ) ;

		case DID_CANCEL :

		    WinDismissDlg ( hwnd , TRUE ) ;
	    }
	    return NULL ;

	case WM_CONTROL :

	    usButtonID = SHORT1FROMMP ( mp1 ) ;

	    /* selecting a new object */
	    if ( usButtonID & CLKCLR_OBJECTS ) {

		switch ( usButtonID ) {
		    case CLKCLR_BACKGROUND :
			pclrNew = & clrBackgroundNew ;
			break ;
		    case CLKCLR_FACE :
			pclrNew = & clrFaceNew ;
			break ;
		    case CLKCLR_HOURHAND :
			pclrNew = & clrHourHandNew ;
			break ;
		    case CLKCLR_MINUTEHAND :
			pclrNew = & clrMinuteHandNew ;
		}

		/* click the button for the new object's current color */
		WinSendMsg (
		    WinWindowFromID ( hwnd ,
			CLKCLR_BUTTONSHIFT + ( USHORT ) * pclrNew ) ,
		    BM_CLICK , MPFROMSHORT ( TRUE ) , NULL ) ;

		break ;
	    }

	    switch ( SHORT2FROMMP ( mp1 ) ) {

		case BN_CLICKED :

		    * pclrNew = ( LONG ) usButtonID - CLKCLR_BUTTONSHIFT ;

		    /* turn off the check state of the previously checked
		     * button and turn on the check state of the button
		     * just clicked */

		    WinSendMsg ( WinWindowFromID ( hwnd , usCheckedButtonID ) ,
				 BM_SETCHECK , MPFROM2SHORT ( FALSE , NULL ) ,
				 NULL ) ;
		    WinSendMsg ( WinWindowFromID ( hwnd , usButtonID ) ,
				 BM_SETCHECK , MPFROM2SHORT ( TRUE , NULL ) ,
				 NULL ) ;

		    usCheckedButtonID = usButtonID ;

		    break ;

		case BN_PAINT :

		    /* fill only the interior of the button, so we don't
		     * conflict with the focus indicator */

		    hwndButton = ( ( PUSERBUTTON ) mp2 ) -> hwnd ;
		    WinQueryWindowRect ( hwndButton , & rclButton ) ;
		    rclButton . xLeft ++ ;
		    rclButton . yBottom ++ ;
		    rclButton . xRight -- ;
		    rclButton . yTop -- ;
		    WinFillRect ( ( ( PUSERBUTTON ) mp2 ) -> hps ,
				  & rclButton ,
				  ( LONG ) usButtonID - CLKCLR_BUTTONSHIFT ) ;

		    /* hollow out those buttons which aren't checked */
		    if ( ! WinSendMsg ( WinWindowFromID ( hwnd , usButtonID ) ,
				      BM_QUERYCHECK , NULL , NULL ) ) {
			rclButtonInterior . xLeft = rclButton . xLeft + 4 ;
			rclButtonInterior . yBottom = rclButton . yBottom + 4 ;
			rclButtonInterior . xRight = rclButton . xRight - 4 ;
			rclButtonInterior . yTop = rclButton . yTop - 4 ;
			WinFillRect ( ( ( PUSERBUTTON ) mp2 ) -> hps ,
				      & rclButtonInterior , SYSCLR_WINDOW ) ;
		    }

		    break ;
	    }

	    /* fall through to the default control processing */
    }

    return WinDefDlgProc ( hwnd , usMsg , mp1 , mp2 ) ;
}
