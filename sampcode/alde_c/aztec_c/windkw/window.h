struct _WINDOW	{		/* --- window control block	---	*/
	int	ulx;		/* upper left corner x coordinate	*/
	int	uly;		/* ditto y coordinate			*/
	int	xsize;		/* line width of inside of window	*/
	int	ysize;		/* number of lines inside of window	*/
	int	cx;		/* current cursor offset in window	*/
	int	cy;
	int	xmargin;	/* x offset to be used in window	*/ 
	int	ymargin;	/* y offset to be used in window	*/
	int	atrib;		/* attribute to be used in window	*/
	int	*scrnsave;	/* pointer to screen save buffer */
	int	oldx;		/* cursor position when window was	*/
	int	oldy;		/* opened (used for screen restore)	*/
} ;

/* ********************************************************************	*/
/*	Structure which determins the border characters for the window. */
/* ******************************************************************** */
struct  _BORDER {
	int ul;		/* upper left corner character.		*/
	int ur;		/* upper right corner character.	*/
	int ll;		/* lower left corner character.		*/
	int lr;		/* lower right corner character.	*/
	int v;		/* vertical character.			*/
	int h;		/* Horizontal character.		*/
} ;

#define WINDOW struct _WINDOW
#define BORDER struct _BORDER

/* ********************************************************************	*/
/* Cursor pad key codes 						*/
/* ********************************************************************	*/

/* Each code is preceeded by a null character i.e. '\0',0xZZ.		*/
#define CUR_UP		72	/* Cursor up key code 			*/
#define CUR_DWN		80
#define CUR_LEFT	75
#define CUR_RIGHT	77
#define HOME		71
#define PG_UP		73
#define PG_DWN		81
#define END		79
#define INSERT		82
#define CENTER		76
#define DELETE		83

/* ********************************************************************	*/
/*	Codes passed from window output functions that indicate when	*/
/*	a write to a window over shot the window's boundries.		*/
/* ******************************************************************** */
#define LEFT_OF		0xC000
#define RIGHT_OF	0xA000
#define TOP_OF		0x9000
#define BOTTOM_OF	0x8800

/* ********************************************************************	*/
/*	Monochrome video atribute codes used by the IBM PC.		*/
/* ******************************************************************** */
#define INVISIBLE	0x00
#define NORMAL		0x01
#define ULINE		0x21
#define HLITE		0x0a
#define REVERSE		0x70
#define UL_HLITE	0x09

