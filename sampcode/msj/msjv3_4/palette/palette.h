/*
 * WINDOWS COLOR PALETTE UTILITY - HEADER FILE
 *
 * LANGUAGE : Microsoft C 5.0
 * TOOLKIT  : Windows 2.03 SDK
 * MODEL    : Small or Medium
 * STATUS   : Operational
 *
 * 03/20/88 - Kevin P. Welch - initial creation.
 *
 */

/* macro definitions */
#define	ABS(x)					((x<0)?(-(x)):(x))

#define	MAX(a,b)					((a>b)?a:b)
#define	MIN(a,b)					((a<b)?a:b)

#define	MAXIMUM(a,b,c)			((a>b)?MAX(a,c):MAX(b,c))
#define	MINIMUM(a,b,c)			((a<b)?MIN(a,c):MIN(b,c))

#define	SPECTRUM1				GetDlgItem(hDlg,ID_SPECTRUM1)
#define	SPECTRUM2				GetDlgItem(hDlg,ID_SPECTRUM2)
#define	SPECTRUM3				GetDlgItem(hDlg,ID_SPECTRUM3)

#define	INSTANCE					GetWindowWord(hDlg,GWW_HINSTANCE)

/* color definition structures */
typedef struct {
	DWORD		cRed		: 8;
	DWORD		cGreen	: 8;
	DWORD		cBlue		: 8;
	DWORD		cUnused	: 8;
} CRT;

typedef struct {
	float		fRed;
	float		fGreen;
	float		fBlue;
} RGB;

typedef struct {
	float		fCyan;
	float		fMagenta;
	float		fYellow;
} CMY;

typedef struct {
	float		fHue;
	float		fSaturation;
	float		fValue;
} HSV;

/**/

/* PALETTE DIALOG BOX DEFINITIONS */

/* Color Palette Dialog Box */
#define	ID_RGB					100		/* red, green, blue */
#define	ID_CMY					101		/* cyan, magenta, yellow */
#define	ID_HSV					102		/* hue, saturation & value */
#define	ID_RED					103		/* red equivalent */
#define	ID_GREEN					104		/* green equivalent */
#define	ID_BLUE					105		/* blue equivalent */
#define	ID_SAMPLE				106		/* sample color patch */
#define	ID_TITLE1				107		/* 1st spectrum title */
#define	ID_TITLE2				108		/* 2nd spectrum title */
#define	ID_TITLE3				109		/* 3rd spectrum title */
#define	ID_SPECTRUM1			110		/* 1st spectrum */
#define	ID_SPECTRUM2			111		/* 2nd spectrum */
#define	ID_SPECTRUM3			112		/* 3rd spectrum */
#define	ID_VALUE1				113		/* 1st spectrum value */
#define	ID_VALUE2				114		/* 2nd spectrum value */
#define	ID_VALUE3				115		/* 3rd spectrum value */

/**/

/* PALETTE FUNCTION DEFINITIONS */

/* Color conversion function definitions */
BOOL					RGBtoCRT( RGB *, CRT * );
BOOL					RGBtoCMY( RGB *, CMY * );
BOOL					RGBtoHSV( RGB *, HSV * );
BOOL					CRTtoRGB( CRT *, RGB * );
BOOL					CMYtoRGB( CMY *, RGB * );
BOOL  				HSVtoRGB( HSV *, RGB * );

/* General function definitions */
void					SelectMixingModel( HANDLE, WORD );
BOOL FAR PASCAL	PaletteDlgFn( HWND, WORD, WORD, LONG );



