/*
    FatPel Application Resource Ids
    Created by Microsoft Corporation, 1989
*/

/* resources loaded via WinCreateStdWindow */
#define IDR_FATPEL		0x01


/* pointers */
#define IDR_DRAGSIZEPTR 	0x02
#define IDR_DRAGCOLORPTR	0x03


/* dialog boxes */
#define IDR_ABOUTDLG		0x04
#define IDR_COLORSDLG		0x05
#define IDR_PELSIZEDLG		0x06




/**********************************************************************\
*  IDD_ - ID for Dialog item
*  IDM_ - ID for Menu commands
*  IDS - ID for String table
*  IDT - ID for Timers
\**********************************************************************/

/* common dialog box control values */

#define IDNULL		       -1
#define IDD_OK		       -2
#define IDD_CANCEL	       -3


/* about ... */

#define IDD_VERSION		0x00


/* set colors ... */

#define IDD_MATHOBJ		0x01
#define IDD_RENDEREDOBJ		0x02
#define IDD_FIELD		0x03
#define IDD_CROSSHAIRS		0x04
#define IDD_INTERSTICE		0x05
#define IDD_CTLPOINTS		0x06
#define IDD_RGB			0x07


/* set pel size ... */

#define IDD_PELWIDTH		0x10
#define IDD_PELHEIGHT		0x11
#define IDD_SQUARE		0x12
#define IDD_DIAMOND		0x13
#define IDD_CIRCLE		0x14




/* menus */

#define IDM_FILE 	0x000
#define IDM_SAVE 	 0x001
#define IDM_ABOUT	 0x002

#define IDM_DISPLAY	0x100
#define IDM_RENDEREDOBJ	 0x101
#define IDM_MATHOBJ	 0x102
#define IDM_CTLPOINTS	 0x103
#define IDM_CROSSHAIRS	 0x104
#define IDM_PELBORDER	 0x105
#define IDM_ROUNDPOINTS	 0x106
#define IDM_AUTOREDRAW	 0x107

#define IDM_PRIM 	0x200
#define IDM_NOPRIM	 0x201
#define IDM_POLYLINE	 0x202
#define IDM_POLYFILLET	 0x203
#define IDM_POLYSPLINE	 0x204
#define IDM_POINTARC	 0x205

#define IDM_COLORS	0x300
#define IDM_SETCOLORS	 0x301
#define IDM_EDITPELCOLORS 0x302

#define IDM_POINTS	0x400
#define IDM_CLEARALL	 0x401

#define IDM_PELSIZE	0x500
#define IDM_SETPELSIZE	 0x501
#define IDM_DRAGPELSIZE	 0x502

#define IDM_REDRAW	0x600




/* strings */

#define IDS_TITLE		1




/************************************************************************
*
*   Constants
*
*   Miscellaneous constants.
*
************************************************************************/

#define MAJOR_VERSION	 0
#define MINOR_VERSION	11

#define CPTLMAX 50
#define CCHSTR	15

#define NO_POINT	-1

/* flags for Paint() options */
#define IGNORED 		0x00	/* no bits set ==> ignored */
#define OVERRIDE_RENDERED_OBJ	0x01
#define CLEAR_BACKGROUND	0x02
#define CLEAR_FAT_BITMAP	0x04
#define RENDER_MATH_OBJ 	0x08




/************************************************************************
*
*   Handy macros to eliminate some typing.
*
************************************************************************/

#define TOGGLE_BOOL(x)	((x)=(!(x)))

#ifdef INCL_WINDIALOGS

#define SET_CHECK_BOX(x, y, z)				    \
	WinSendDlgItemMsg((x),(y),BM_SETCHECK,		    \
	    MPFROM2SHORT((z),0),0L)

#define TOGGLE_MENU_ITEM(x, y, z)			    \
	WinSendDlgItemMsg((x), FID_MENU, MM_SETITEMATTR,    \
	    MPFROM2SHORT((y),TRUE),			    \
	    MPFROM2SHORT(MIA_CHECKED,(z)?MIA_CHECKED:NULL))

#define CHECK_MENU_ITEM(x, y)				    \
	WinSendDlgItemMsg((x), FID_MENU, MM_SETITEMATTR,    \
	    MPFROM2SHORT((y),TRUE),			    \
	    MPFROM2SHORT(MIA_CHECKED,MIA_CHECKED))

#define UNCHECK_MENU_ITEM(x, y) 			    \
	WinSendDlgItemMsg((x), FID_MENU, MM_SETITEMATTR,    \
	    MPFROM2SHORT((y),TRUE),			    \
	    MPFROM2SHORT(MIA_CHECKED,NULL))

#define ENABLE_MENU_ITEM(x, y)				    \
	WinSendDlgItemMsg((x), FID_MENU, MM_SETITEMATTR,    \
	    MPFROM2SHORT((y),TRUE),			    \
	    MPFROM2SHORT(MIA_DISABLED,NULL))

#define DISABLE_MENU_ITEM(x, y) 			    \
	WinSendDlgItemMsg((x), FID_MENU, MM_SETITEMATTR,    \
	    MPFROM2SHORT((y),TRUE),			    \
	    MPFROM2SHORT(MIA_DISABLED,MIA_DISABLED))

#endif
