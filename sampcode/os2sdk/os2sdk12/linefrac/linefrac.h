/**********************************************************************\
*  General include for LineFractal application
*
*  Created by Microsoft Corporation, 1989
*
\**********************************************************************/

/*
    Type definition
*/
typedef double far * PDBL;



/*
    Resource Ids for Linefractal application
*/

/* resources loaded via WinCreateStdWindow */
#define IDR_LINEFRACTAL 	0x01

/* pointers */
#define IDR_TRACKINGPTR 	0x10
#define IDR_PASTINGPTR		0x11
#define IDR_KOCH0		0x12
#define IDR_KOCH1		0x13
#define IDR_KOCH2		0x14
#define IDR_KOCH3		0x15

/* dialog boxes */
#define IDR_ABOUTDLG		0x20
#define IDR_TIMERDELAYDLG	0x21
#define IDR_BITMAPPARAMSDLG	0x22
#define IDR_SWITCHDELAYDLG	0x23
#define IDR_LINEATTRSDLG	0x24
#define IDR_MARKERATTRSDLG	0x25
#define IDR_AREAATTRSDLG	0x26
#define IDR_IMAGEATTRSDLG	0x27
#define IDR_MISCATTRSDLG	0x28
#define IDR_DIMENSIONSDLG	0x29
#define IDR_PTRPREFERENCESDLG	0x2A




/**********************************************************************\
*  IDD_ - ID for Dialog item
*  IDM_ - ID for Menu commands
*  IDS - ID for String table
*  IDT - ID for Timers
\**********************************************************************/

/* common dialog box control values */
#define ID_NULL		       0x0FE
#define IDD_OK		          0x0FD
#define IDD_CANCEL	       0x0FC

/* redraw timer delay */
#define IDD_MINDELAY		0x001
#define IDD_MAXDELAY		0x002

/* bitmap parameters */
#define IDD_BMCX 		0x010
#define IDD_BMCY 		0x011
#define IDD_BMPLANES		0x012
#define IDD_BMBPP		0x013

/* thread-switch timer delay */
#define IDD_SWITCHDELAY		0x020

/* line attributes */
#define IDD_LINECOLOR		0x030
#define IDD_LINEMIX		0x031
#define IDD_LINEWIDTH		0x032
#define IDD_LINEGEOMWIDTH	0x033
#define IDD_LINETYPE		0x034
#define IDD_LINEEND		0x035
#define IDD_LINEJOIN		0x036

/* marker attributes */
#define IDD_MARKCOLOR		0x040
#define IDD_MARKBACKCOLOR	0x041
#define IDD_MARKMIX		0x042
#define IDD_MARKBACKMIX		0x043
#define IDD_MARKSET		0x044
#define IDD_MARKSYMBOL		0x045
#define IDD_MARKCELLWIDTH	0x046
#define IDD_MARKCELLHEIGHT	0x047

/* area attributes */
#define IDD_AREACOLOR		0x050
#define IDD_AREABACKCOLOR	0x051
#define IDD_AREAMIX		0x052
#define IDD_AREABACKMIX		0x053
#define IDD_AREASET		0x054
#define IDD_AREASYMBOL		0x055
#define IDD_AREAREFX		0x056
#define IDD_AREAREFY		0x057

/* image attributes */
#define IDD_IMAGECOLOR		0x060
#define IDD_IMAGEBACKCOLOR	0x061
#define IDD_IMAGEMIX		0x062
#define IDD_IMAGEBACKMIX 	0x063

/* miscellaneous fractal attributes */
#define IDD_RECURSION		0x070
#define IDD_CPTMAX		0x071
#define IDD_NUMSIDES		0x072
#define IDD_ROTATION		0x073

/* fractal dimensions */
#define IDD_XOFF 		0x080
#define IDD_YOFF 		0x081
#define IDD_XSCALE		0x082
#define IDD_YSCALE		0x083

/* pointer preferences */
#define IDD_PTRTHRESHOLD 	0x090
#define IDD_USERPTR0		0x091
#define IDD_USERPTR1		0x092
#define IDD_USERPTR2		0x093
#define IDD_USERPTR3		0x094




/* menus */

#define IDM_FILE 		0x100
#define IDM_LOAD 		 0x101
#define IDM_SAVE 		 0x102
#define IDM_ABOUT		 0x103

#define IDM_EDIT 		0x200
#define IDM_SELECT		 0x201
#define IDM_SELECTALL		 0x202
#define IDM_CUT			 0x203
#define IDM_COPY 		 0x204
#define IDM_PASTE		 0x205
#define IDM_USECLIPBRD		 0x206
#define IDM_PASTES		 0x207
#define IDM_PASTEDSA		 0x208
#define IDM_PASTEDSO		 0x209
#define IDM_PASTEDSX		 0x20A
#define IDM_PASTESN		 0x20B
#define IDM_PASTEDSAN		 0x20C
#define IDM_PASTEDSON		 0x20D
#define IDM_PASTEDSXN		 0x20E

#define IDM_THREAD		0x300
#define IDM_NEWTHREAD		 0x310
#define IDM_DCDIRECT		  0x311
#define IDM_DCMEMORY		  0x312
#define IDM_DCPOSTSCRIPT 	  0x313
#define IDM_DCPROPRINTER 	  0x314
#define IDM_THREADTOTOP		 0x320
#define IDM_THR0TOTOP		  0x321
#define IDM_THR1TOTOP		  0x322
#define IDM_THR2TOTOP		  0x323
#define IDM_THR3TOTOP		  0x324
#define IDM_THR4TOTOP		  0x325
#define IDM_THR5TOTOP		  0x326
#define IDM_THR6TOTOP		  0x327
#define IDM_THR7TOTOP		  0x328
#define IDM_THR8TOTOP		  0x329
#define IDM_THR9TOTOP		  0x32A
#define IDM_EDITTHREADPARAMS	 0x330
#define IDM_KILLTOPTHREAD	 0x340
#define IDM_KILLALLTHREADS	 0x350
#define IDM_AUTOSWITCH		 0x360
#define IDM_SWITCHDELAY		 0x370
#define IDM_UPDATEMENUS		 0x380
#define IDM_THREADINHERITANCE	 0x390

#define IDM_CONTROL		0x400
#define IDM_REDRAW		 0x401
#define IDM_ABORT		 0x402
#define IDM_CLEARBITMAP		 0x403
#define IDM_COPYBITMAPTOSCREEN	 0x404
#define IDM_SQUAREBITMAP 	 0x405
#define IDM_RESIZEBITMAPTOWINDOW  0x406
#define IDM_MOUSECHANGESRECURSION 0x407
#define IDM_ANIMATEPTR		 0x408
#define IDM_PTRPREFERENCES	 0x409
#define IDM_TIMERDELAY		 0x40A

#define IDM_FRACTAL		0x500
#define IDM_FRACREDRAW		 0x501
#define IDM_SHARKTOOTH		 0x502
#define IDM_SAWTOOTH		 0x503
#define IDM_KOCH 		 0x504
#define IDM_SPACEFILLER		 0x505
#define IDM_STOVE		 0x506
#define IDM_SQUAREWAVE		 0x507
#define IDM_HOURGLASS		 0x508

#define IDM_PRIMITIVE		0x600
#define IDM_PRIMREDRAW		 0x601
#define IDM_POLYLINE		 0x602
#define IDM_POLYFILLET		 0x603
#define IDM_POLYSPLINE		 0x604
#define IDM_POLYMARKER		 0x605
#define IDM_PEANO		 0x606

#define IDM_ATTRIBUTES		0x700
#define IDM_ATTRREDRAW		 0x701
#define IDM_LINEATTRS		 0x702
#define IDM_MARKERATTRS		 0x703
#define IDM_AREAATTRS		 0x704
#define IDM_IMAGEATTRS		 0x705
#define IDM_MISCATTRS		 0x706
#define IDM_DIMENSIONS		 0x707
#define IDM_DRAGDIMS		 0x708

#define IDM_ENVIRONMENT		0x800
#define IDM_AUTORESIZE		 0x801
#define IDM_AUTOSELECTDIMS	 0x802
#define IDM_CLEARONREDRAW	 0x803
#define IDM_AUTOSTARTREDRAW	 0x804
#define IDM_COLLECTBOUNDS	 0x805





#define IDS_TITLE		1
#define IDT_AUTOSTARTREDRAW	1
#define IDT_AUTOSWITCH		2




/************************************************************************
*
*   Constants
*
*   Miscellaneous constants.
*
************************************************************************/

#define CCHSTR	12		/* work buffer size for MyGetWindowLong, etc */

#define MAX_POINT_COUNT 	0x10000/sizeof(POINTL) /* number of points buffered up */

#define PI	3.1415926		/* useful in computing angles */
#define TWO_PI	(2 * PI)		/*   ditto		      */

#define EOLIST	0			/* End-of-list marker for xform */

#define NUM_PTR_SHAPES 4




/************************************************************************
*
*   Aspect ratio structure.
*
************************************************************************/

typedef struct _aspect
{
    LONG lHorz;
    LONG lVert;
} ASPECT;




/************************************************************************
*
*   Line fractal transform structure.
*
*   The structure LINEFRAC defines a transform for a single line
*   segment within a linefractal similarity transform.	A proper
*   similarity transform would be a collection of segment transforms in
*   the form of a linked list.
*
*   next		link to the next line segment
*   angle		relative angle in radians, positive being
*			  counterclockwise
*   length		relative length of a segment, as percentage of
*			  length segment on which transform was defined
*   flip		orientation of transform at a segment
*			  FALSE ==> rotate transform by  angle[]
*			  TRUE  ==> rotate transform by -angle[]
*				    (i.e. flip it)
*
************************************************************************/

typedef struct _linefrac
{
    struct _linefrac *next;
    double angle;
    double length;
    BOOL flip;
} LINEFRAC;
typedef LINEFRAC *PLINEFRAC;




/************************************************************************
*
*   Transform-defaults structure.
*
************************************************************************/

typedef struct
{
    PLINEFRAC pXform;
    double xOff;
    double yOff;
    double xScale;
    double yScale;
    USHORT usSides;
} XFORMDATA;




/************************************************************************
*
*   Private bitmap info header.  This is to avoid having the extra
*   cbFix space taken up in the per-thread data structure.  I use this
*   only to store the data that will go into the real BITMAPINFOHEADER
*   structure later.
*
************************************************************************/

typedef struct
{
    USHORT cx;
    USHORT cy;
    USHORT cPlanes;
    USHORT cBitCount;
} MYBMINFO;




#ifdef INCL_GLOBALS
#define INCL_THREADS
#endif

#ifdef INCL_THREADS
/************************************************************************
*
*   Per-thread overhead structure.
*
************************************************************************/

typedef struct _threadoh
{
    /* environment preferences */

    BOOL       fAutoSizePS;	/* PS resized on various events */
    BOOL       fAutoSelectDims; /* scale and offset from fractal defaults */
    BOOL       fClearOnRedraw;	/* clear surface at start of redraw */
    BOOL       fAutoStartRedraw;/* random automatic redraw enabled */
    BOOL       fCollectBounds;	/* collect bounds on primitives */

    BOOL       fFracRedraw;	/* redraw if fractal similarity xform changes */
    BOOL       fPrimRedraw;	/* redraw if primitive changes */
    BOOL       fAttrRedraw;	/* redraw if any attributes change */

    /* fractal attributes */

    LINEBUNDLE	 lb;
    ULONG	 flLineAttrs;
#define LFA_LINECOLOR		LBB_COLOR
#define LFA_LINEMIX		LBB_MIX_MODE
#define LFA_LINEWIDTH		LBB_WIDTH
#define LFA_LINEGEOMWIDTH	LBB_GEOM_WIDTH
#define LFA_LINETYPE		LBB_TYPE
#define LFA_LINEEND		LBB_END
#define LFA_LINEJOIN		LBB_JOIN
#define LFA_LINEALL		(LBB_COLOR|LBB_MIX_MODE|LBB_WIDTH|LBB_GEOM_WIDTH|\
				  LBB_TYPE|LBB_END|LBB_JOIN)


    MARKERBUNDLE mb;
    ULONG	 flMarkerAttrs;
#define LFA_MARKCOLOR		MBB_COLOR
#define LFA_MARKBACKCOLOR	MBB_BACK_COLOR
#define LFA_MARKMIX		MBB_MIX_MODE
#define LFA_MARKBACKMIX 	MBB_BACK_MIX_MODE
#define LFA_MARKSET		MBB_SET
#define LFA_MARKSYMBOL		MBB_SYMBOL
#define LFA_MARKCELLWIDTH	MBB_BOX     /* part of sizfxCell field */
#define LFA_MARKCELLHEIGHT	MBB_BOX     /* part of sizfxCell field */
#define LFA_MARKALL		(MBB_COLOR|MBB_BACK_COLOR|MBB_MIX_MODE|MBB_BACK_MIX_MODE|\
				  MBB_SET|MBB_SYMBOL|MBB_BOX)


    AREABUNDLE	 ab;
    ULONG	 flAreaAttrs;
#define LFA_AREACOLOR		ABB_COLOR
#define LFA_AREABACKCOLOR	ABB_BACK_COLOR
#define LFA_AREAMIX		ABB_MIX_MODE
#define LFA_AREABACKMIX 	ABB_BACK_MIX_MODE
#define LFA_AREASET		ABB_SET
#define LFA_AREASYMBOL		ABB_SYMBOL
#define LFA_AREAREFX		ABB_REF_POINT /* part of ptlRefPoint field */
#define LFA_AREAREFY		ABB_REF_POINT /* part of ptlRefPoint field */
#define LFA_AREAALL		(ABB_COLOR|ABB_BACK_COLOR|ABB_MIX_MODE|ABB_BACK_MIX_MODE|\
				  ABB_SET|ABB_SYMBOL|ABB_REF_POINT)


    IMAGEBUNDLE  ib;
    ULONG	 flImageAttrs;
#define LFA_IMAGECOLOR		IBB_COLOR
#define LFA_IMAGEBACKCOLOR	IBB_BACK_COLOR
#define LFA_IMAGEMIX		IBB_MIX_MODE
#define LFA_IMAGEBACKMIX	IBB_BACK_MIX_MODE
#define LFA_IMAGEALL		(IBB_COLOR|IBB_BACK_COLOR|IBB_MIX_MODE|IBB_BACK_MIX_MODE)


    /* miscellaneous other attributes */

    USHORT     usCurPrim;		/* primitive to draw with */
    USHORT     usCurXform;		/* fractal to draw with */
    USHORT     usRecursion;		/* level of recursion at which to draw */
    USHORT     cptMax;			/* max batch size for point buffer */
    USHORT     usPolygonSides;		/* number of sides on polygonal frame */
    double     dblRotation;		/* rotation in radians of image */
    double     dblXOff; 		/* offset expressed as percentage of surface extents */
    double     dblYOff;
    double     dblXScale;		/* scale expressed as percentage of surface extents */
    double     dblYScale;
    ULONG      cxWCS;		/* dimensions of drawing in world space */
    ULONG      cyWCS;

    ULONG      flMiscAttrs;
#define LFA_CURPRIM		0x0001L
#define LFA_CURXFORM		0x0002L
#define LFA_RECURSION		0x0004L
#define LFA_POLYGONSIDES	0x0008L
#define LFA_CPTMAX		0x0010L
#define LFA_XOFF		0x0020L
#define LFA_YOFF		0x0040L
#define LFA_XSCALE		0x0080L
#define LFA_YSCALE		0x0100L
#define LFA_ROTATION		0x0200L
#define LFA_CXWCS		0x0400L
#define LFA_CYWCS		0x0800L

#define LFA_MISCALL		0x0FFFL


    /* fixed per-thread overhead */

    USHORT     dcType;		/* code identifying type of DC */
    RECTL      rcl;		/* surface dimensions as a rectangle */
    USHORT     cPlanes; 	/* if bitmap, number of color planes */
    USHORT     cBitCount;	/* if bitmap, number of color bits per pel */
    ASPECT     AspectRatio;	/* DC's aspect ratio */


    /********************************************************************
    *
    *  Per-process per-thread overhead.
    *
    *  Data after this point should not be saved
    *  as part of a fractal context.
    *
    ********************************************************************/

    TID        tid;		/* thread id */
    SEL        selStack;	/* thread stack selector */
    HDC        hdc;
    HBITMAP    hbm;
    HPS        hps;
    LONG       lSemRedraw;	/* redraw semaphore */
    BOOL       fInterrupted;	/* fractal suicide flag recognized in recursion */
    BOOL       fTimeToDie;	/* thread suicide flag */
    BOOL       fUpdateAttrs;	/* TRUE --> new attributes since redraw */
    BOOL       fBusy;		/* TRUE --> not waiting on semaphore */
    PPOINTL    pptl;		/* fractal point buffer */
    ULONG      cptl;		/* number of points in buffer */
    RECTL      rclBounds;	/* bounds of last primitive drawn */
    double     x;		/* current position */
    double     y;
    PMATRIXLF  pmatlf;		/* --> matrices for each side of ngon */

} THR;
typedef THR *PTHR;
#endif




#ifdef INCL_GLOBALS
/************************************************************************
*
*   Global data structure.  This makes it easier to save the state of
*   the program to a file.
*
************************************************************************/

#define MAX_THREADS 10

typedef struct _globaldata
{
    /********************************************************************
    *
    *  LineFrac session data.
    *
    *  The following can be saved to a file to record the user's
    *  session.
    *
    ********************************************************************/

    BOOL      fAutoSwitch;		 /* TRUE --> automatically cycle through threads */
    BOOL      fUpdateMenusOnThreadSwitch;/* TRUE --> per-thread menu items not updated during animation */
    BOOL      fThreadInheritance;	 /* TRUE --> new threads are copies of top one */
    USHORT    usSwitchDelay;		 /* delay between automatic thread switches */
    USHORT    usMinTimerDelay;		 /* lower limit of random redraw delay */
    USHORT    usMaxTimerDelay;		 /* upper limit of random redraw delay */
    BOOL      fMouseChangesRecursion;	 /* TRUE --> mouse clicks change recursion */
    BOOL      fAnimatePtr;		 /* TRUE --> pointer changes with mouse movements */
    USHORT    usUserPtr;		 /* pointer to use when not animated */
    USHORT    usPtrThreshold;		 /* relates to speed of pointer animation */
    USHORT    usRopIndex;		 /* index into paste rop table */
    BOOL      fUseClipbrd;		 /* TRUE  --> edit operations use system clipboard */
					 /* FALSE -->  ... use private bitmaps */
    RECTL     rcl;			 /* window dimensions */
    USHORT    cThr;			 /* number of threads */
    USHORT    iThrTop;			 /* top thread */


    /********************************************************************
    *
    *  Per-process global data.
    *
    *  Data after this point should not be saved
    *  as part of a linefrac session.
    *
    ********************************************************************/

    /* attributes:  user's input is put here; thread picks it up when
     * told to redraw.
     */

    BOOL	 fUpdateAttrs;	/* TRUE --> no thread has read attrs yet */
    LINEBUNDLE	 lb;
    ULONG	 flLineAttrs;
    MARKERBUNDLE mb;
    ULONG	 flMarkerAttrs;
    AREABUNDLE	 ab;
    ULONG	 flAreaAttrs;
    IMAGEBUNDLE  ib;
    ULONG	 flImageAttrs;
    USHORT	 usCurPrim;	 /* primitive to draw with */
    USHORT	 usCurXform;	 /* fractal to draw with */
    USHORT	 usRecursion;	 /* level of recursion at which to draw */
    USHORT	 cptMax;	 /* max batch size for point buffer */
    USHORT	 usPolygonSides; /* number of sides on polygonal frame */
    double	 dblRotation;	 /* rotation in radians of image */
    double	 dblXOff;	 /* offset expressed as percentage of surface extents */
    double	 dblYOff;
    double	 dblXScale;	 /* scale expressed as percentage of surface extents */
    double	 dblYScale;
    ULONG	 cxWCS; 	 /* dimensions of drawing in world space */
    ULONG	 cyWCS;
    ULONG	 flMiscAttrs;

    HAB       hab;
    HMQ       hMsgQ;
    HWND      hwnd;
    HWND      hwndFrame;
    HPOINTER  hptr[NUM_PTR_SHAPES];  /* array of pointer shapes to animate */
    HPOINTER  hptrTrack;	/* pointer used when dragging a new rect */
    HPOINTER  hptrPaste;	/* pointer used when dragging paste rect */
    HPOINTER  hptrWait; 	/* hourglass pointer for when top thread is busy */
    USHORT    usPtrIncr;	/* +1 or -1, so pointer doesn't wrap in array */
    USHORT    usPtrCounter;	/* counts WM_MOUSEMOVE messages */
    USHORT    usCurPtr; 	/* index into pointer array of current one */
    BOOL      fFirstThread;	/* TRUE --> no threads initialized yet */
    BOOL      fTimerOn; 	/* TRUE --> auto redraw timer is on */
    HDC       CutCopyDC;	/* DC for cut/copy private bit storage */
    HPS       CutCopyPS;	/* PS for cut/copy private bit storage */
    HBITMAP   CutCopyBM;	/* BM for cut/copy private bit storage */
    RECTL     rclSelect;	/* most recently dragged selection rect */
    RECTL     rclCutCopy;	/* most recent cut/copy rect */
    ULONG     ulPasteROP;	/* BitBlt ROP to use when pasting */
    BOOL      fHaveCutCopyDC;	/* TRUE --> cut/copy DC exists */
    BOOL      fTracking;	/* TRUE --> dragging dimensions */
    BOOL      fSelecting;	/* TRUE --> dragging cut/copy rect */
    BOOL      fPasting; 	/* TRUE --> dragging paste rect */
    BOOL      fTempSelect;	/* TRUE --> selection rect just dragged */
    BOOL      fShowSelectRc;	/* TRUE --> display selection rect */
    BOOL      fHaveSelectRc;	/* TRUE --> have valid selection rect */
    MYBMINFO  bm;		/* scratch space for various bitmap functions */
    PTHR      pThrTop;		/* top thread */
    PTHR      aThr[MAX_THREADS];/* array of threads */

} GLOBALDATA;
#endif




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
	    MPFROM2SHORT(MIA_CHECKED,(z)?MIA_CHECKED:0))

#define CHECK_MENU_ITEM(x, y)				    \
	WinSendDlgItemMsg((x), FID_MENU, MM_SETITEMATTR,    \
	    MPFROM2SHORT((y),TRUE),			    \
	    MPFROM2SHORT(MIA_CHECKED,MIA_CHECKED))

#define UNCHECK_MENU_ITEM(x, y) 			    \
	WinSendDlgItemMsg((x), FID_MENU, MM_SETITEMATTR,    \
	    MPFROM2SHORT((y),TRUE),			    \
	    MPFROM2SHORT(MIA_CHECKED,0))

#define ENABLE_MENU_ITEM(x, y)				    \
	WinSendDlgItemMsg((x), FID_MENU, MM_SETITEMATTR,    \
	    MPFROM2SHORT((y),TRUE),			    \
	    MPFROM2SHORT(MIA_DISABLED,0))

#define DISABLE_MENU_ITEM(x, y) 			    \
	WinSendDlgItemMsg((x), FID_MENU, MM_SETITEMATTR,    \
	    MPFROM2SHORT((y),TRUE),			    \
	    MPFROM2SHORT(MIA_DISABLED,MIA_DISABLED))

#endif
