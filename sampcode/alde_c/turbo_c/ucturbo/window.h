/*************************************************************************
 *                         window.h                                      *
 *                                                                       *
 *    This header is to be used for all window functions and programs    *
 *    using window functions.                                            *
 *                                                                       *
 *                      Copyright (c) 1987                               *
 *                              by                                       *
 *                       David A.  Miller                                *
 *                       Unicorn Software                                *
 *                                                                       *
 *************************************************************************/


struct WNDOW  	{		/* --- window control block	---	*/
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



struct  BRDER  {
	int ul;		/* upper left corner character.		*/
	int ur;		/* upper right corner character.	*/
	int ll;		/* lower left corner character.		*/
	int lr;		/* lower right corner character.	*/
	int v;		/* vertical character.			*/
	int h;		/* Horizontal character.		*/
   int att;    /* Widow Attribute.           */
} ;

#define WINDOW struct WNDOW
#define BORDER struct BRDER

/**********************************************************************
 *                   Define the keys for Key pad                      *
 **********************************************************************/

/* A null always preceeds these codes		*/

#define CUR_UP		 72	                  /* Cursor up key code 			*/
#define CUR_DWN 	 80
#define CUR_LEFT	 75
#define CUR_RIGHT	 77
#define HOME		 71
#define PG_UP		 73
#define PG_DWN		 81
#define END		    79
#define INSERT		 82
#define CENTER		 76
#define DELETE		 83

/*********************************************************************
 *  These are return codes in case the window overshoots its borders *
 *	 when a write to that window is attempted                         *
 *********************************************************************/

#define LEFT_OF		0xC000
#define RIGHT_OF	   0xA000
#define TOP_OF		   0x9000
#define BOTTOM_OF  	0x8800

/*********************************************************************
 *  Attributes if a monochrome monitor is used.                      *
 *********************************************************************/

#define INVISIBLE	0x00
#define NORMAL		0x01
#define ULINE		0x21
#define HLITE		0x0a
#define REVERSE		0x70
#define UL_HLITE	0x09

