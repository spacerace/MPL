/*
    Header file for EDPLINE
    Created by Microsoft Corporation, 1989
*/

/*
    Resource Ids
*/

/* resources loaded via WinCreateStdWindow */
#define IDR_EDPLINE	0x01


/*
    IDD - ID for Dialog boxes
    IDM - ID for Menu commands
    IDS - ID for String table
*/

/* dialogs */

#define	IDD_ABOUT	 0x0100

/* menus */

#define IDM_PRIM 	 0x0200
#define IDM_NOPRIM	 0x0201
#define IDM_POLYLINE	 0x0202
#define IDM_POLYFILLET	 0x0203
#define IDM_POLYSPLINE	 0x0204
#define IDM_POINTARC	 0x0205
#define	IDM_ABOUT	 0x0206

#define IDM_POINTS 	 0x0400
#define IDM_CTLPOINTS	 0x0401
#define IDM_CLEARALL	 0x0402

/* strings */

#define IDS_TITLE		1




/*
    Constants
*/

#define MAJOR_VERSION	1
#define MINOR_VERSION	1

#define CPTLMAX 20
#define CCHSTR	15




/*
    Aspect ratio structure
*/

typedef struct
{
    LONG lHorz;
    LONG lVert;
} ASPECT;




/*
    Handy macros to eliminate some typing.
*/

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
