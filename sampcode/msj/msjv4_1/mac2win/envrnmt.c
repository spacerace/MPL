void debug();

/***************************************************************
    ENVRNMT.C
****************************************************************/    


#include "CONFIG.H"
 

#ifdef MAC /*----------------MACINTOSH SPECIFIC-----------------*/

#include "MacTypes.h"
#include "QuickDraw.h"
#include "WindowMgr.h"
#include "MemoryMgr.h"
#include "EventMgr.h"
#include "MenuMgr.h"
#include "FileMgr.h"
#include "ResourceMgr.h"
#include "StdFilePkg.h"
#include "OSUtil.h"
#include "DialogMgr.h"

#include "pascal.h"

#else /*---------------------MS_WINDOWS SPECIFIC----------------*/

#include "windows.h"

#endif /*---------------- END OF WINDOWS SPECIFIC----------------*/

#include "STDDEFS.H"
#include "APPLCATN.H"
#include "ENVRNMT.H"


/****************************************************************/

#ifdef MAC
PUBLIC VOID main() /*windows has winMain(). Both call app_Main()*/
{
    app_MainProcedure();
}
#endif

/****************************************************************/

PUBLIC BOOL env_InitEnvironment()
{
#ifdef MAC
	InitGraf(&thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs((ProcPtr) env_ExitProgram);
	InitCursor();
	FlushEvents( everyEvent, 0 );
		
	MaxApplZone();
#else
	/* MS-Windows environment already initialized by WinMain() */
#endif
	env_InitCanvases();
	return TRUE;
}

PUBLIC VOID env_ExitProgram()
{
#ifdef MAC
	ExitToShell();
#endif
}

#ifdef MAC
PUBLIC VOID env_FatalError(str,x,y,z)
	STRING str; unsigned x,y,z;
{
#ifdef DBG
	ut_PutString("RV|FatalError: ",str);
#endif
	env_ExitProgram();
}

PUBLIC VOID env_NotImplemented(str,x,y,z)
	STRING str; unsigned x,y,z;
{
#ifdef DBG
	ut_PutString("RV|NotImplemented: ",str);
#endif
}
#endif

/****************************************************************/

PUBLIC BOOL	env_PtInRect(pt, r)
    Point *	pt;
    pRect	r;
{
    return (   pt->v >= r->top  
	    && pt->v <  r->bottom 
	    && pt->h >= r->left 
	    && pt->h <  r->right);
}

/****************************************************************/
PUBLIC VOID env_OffsetRect(r, dh, dv)
    Rect *	r;
    int		dh, dv;
{
    r->left   += dh;
    r->right  += dh;
    r->top    += dv;
    r->bottom += dv;
}


/****************************************************************
    MEMORY ALLOCATION
****************************************************************/


PUBLIC MEMBLOCK	env_AllocMem(size)
	MEMSIZE	size;
{
#ifdef MAC
	return (MEMBLOCK) NewHandle((Size) size);
#else
	return GlobalAlloc(GMEM_MOVEABLE, size);
#endif
}

PUBLIC VOID		env_ReAllocMem(handle,size)
	MEMBLOCK *	handle;
	MEMSIZE		size;
{
#ifdef MAC
	SetHandleSize((Handle)*handle,(Size)size);
#else
#ifdef AS
	*handle = GlobalReAlloc(*handle,size,GMEM_MOVEABLE);
#else
	HANDLE *h = (HANDLE *)handle;
	*h = GlobalReAlloc(*h,size,GMEM_MOVEABLE);
#endif	
#endif
}

PUBLIC MEMPTR env_GraspMem( handle)
	MEMBLOCK	handle;
{
#ifdef MAC
	HLock((Handle)handle);
	return (MEMPTR) *handle;
#else
	return (MEMPTR) GlobalLock(handle);
#endif
}

PUBLIC VOID env_UnGraspMem( handle)
	MEMBLOCK	handle;
{
#ifdef MAC
	HUnlock((Handle)handle);
#else
	GlobalUnlock(handle);
#endif
}

PUBLIC VOID env_FreeMem(handle)
	MEMBLOCK	handle;
{
#ifdef MAC
	DisposHandle((Handle)handle);
#else
	GlobalFree(handle);
#endif
}

#ifdef NOT_IMPLEMENTED
PUBLIC MEMSIZE env_GetMemblkSize() {;}
#endif

/****************************************************************
    OPERATIONS ON WINDOWS
****************************************************************/

#include "view.h"

#ifndef MAC
PUBLIC BOOL window_is_being_created; /*flag for critical section*/
#endif

PUBLIC BOOL env_NewWindow(title, bounds, vu, canvas)

    STRING		title;
    Rect *		bounds;
    INDEX		vu,canvas;
{
#ifdef MAC
#define SIZEOF_STR 128
 	WindowPtr	wPtr;
 	char str[SIZEOF_STR];
	
    ut_PrintMacRect("RV|envNewWindow: rect",bounds);
    
    if(strlen(title) >= SIZEOF_STR) title[SIZEOF_STR-1] = '\0';
    strcpy(str,title);
 	        
	wPtr = NewWindow( (WindowPtr) env_GetAddrDevBlock(canvas), 
					bounds,
					CtoPstr(str),
					TRUE /*visible*/,
					documentProc /*standard window type*/,
					(WindowPtr) -1 /*front-most window*/,
					TRUE /*go-away*/,
					(long) vu /*refcon field*/
					);
					
    ut_PrintHex("RV|NewWindow returns wptr",(long)wPtr);
    if(wPtr==(WindowPtr)NULL) return FALSE;

    env_SetCanvasDev(canvas,(long)wPtr);
    env_CalcSizeCanvas(canvas);
    return TRUE;
#else
    IMPORT HANDLE win_GethAppInstance();
    IMPORT HWND   win_GethAppWnd();
    HWND	hWnd;

    ut_PrintMacRect("RV|envNewWindow: rect",bounds);
    ut_Print2Hex("RV|envNewWindow: hAppInst, hAppWnd",
		(long)win_GethAppInstance(),(long)win_GethAppWnd());
	    
    window_is_being_created = TRUE;
    
    hWnd = CreateWindow(
    		(LPSTR) "Child",
		(LPSTR) title,
		WS_CHILD 
		    | WS_CAPTION 
		    | WS_SYSMENU 
		    | WS_SIZEBOX 
		    | WS_CLIPSIBLINGS
		    | WS_VISIBLE
		    | WS_BORDER,
		bounds->left, bounds->top,
		(bounds->right  - bounds->left),
		(bounds->bottom - bounds->top ),
		win_GethAppWnd(),
		(HMENU)NULL,
		win_GethAppInstance(),
		(LPSTR) NULL
	);

    ut_PrintHex("RV|CreateWindow returns hWnd",(long)hWnd);
    if(hWnd==(HWND)NULL) return FALSE;

    env_SetCanvasDev(canvas,(long)hWnd);
    SetWindowWord(hWnd, 0, canvas);
    window_is_being_created = FALSE;
    
    ShowWindow(hWnd, SHOW_OPENWINDOW);
    env_CalcSizeCanvas(canvas);
    /*>>BringWindowToTop(hWnd);*/
    /*>>UdateWindow(hWnd);*/
    return TRUE;
#endif
}



PUBLIC VOID env_GetScreenSize(rect)
    Rect *rect;
{
#ifdef MAC
	GrafPtr	theWMgrPort;
	/*assumes that InitWindows() has been called*/
	GetWMgrPort( &theWMgrPort);
	*rect = theWMgrPort->portRect; /*structure copy */
#else
    IMPORT VOID win_UpdateAppWndSize();
    win_UpdateAppWndSize(rect);
#endif
}

/****************************************************************
    GRAPHICS DRAWING
****************************************************************/

#ifndef MAC

LOCAL VOID select_fill_pattern(hDC,pattern) 
    HDC hDC; PATT pattern;
{
    switch (pattern)
    {
    case PATT_WHITE:
	    DeleteObject(SelectObject(hDC,GetStockObject(WHITE_BRUSH)));break;
    case PATT_BLACK:
	    DeleteObject(SelectObject(hDC,GetStockObject(BLACK_BRUSH)));break;
    case PATT_GRAY:
	    DeleteObject(SelectObject(hDC,GetStockObject(GRAY_BRUSH))); break;
    case PATT_LTGRAY:
	    DeleteObject(SelectObject(hDC,GetStockObject(LTGRAY_BRUSH)));break;
    case PATT_DKGRAY:
	    DeleteObject(SelectObject(hDC,GetStockObject(DKGRAY_BRUSH))); break;
    case PATT_FWD:
    case PATT_BACK:
    case PATT_HORL:
    case PATT_VERT:
    case PATT_VERTX:
    case PATT_DIAGX:
    case PATT_NULLPA:
    case PATT_DOT:
    case PATT_DASH:
    case PATT_DASHDO:
    case PATT_DASHDD:
	    DeleteObject(SelectObject(hDC,GetStockObject(GRAY_BRUSH))); break;
    }
}	    


LOCAL VOID null_brush(hDC) HDC hDC;
{
    DeleteObject( SelectObject( hDC, GetStockObject(NULL_BRUSH) ) );
}

LOCAL VOID null_pen(hDC) HDC hDC;
{
    DeleteObject( SelectObject( hDC, GetStockObject(NULL_PEN) ) );
}

#endif



/****************************************************************/
	
typedef struct 
{
    long			userdata;
    short			right, bottom;
    short			x,y;
#ifdef MAC
	GrafPtr			grafptr;
	WindowRecord 	wRecord;
    short			scale;
#else
    PAINTSTRUCT		ps;
    HWND			hWnd;
    Rect			winRect; /*entire window*/
    Rect			portRect; /*client area*/
    Rect			scrBounds; /*position of screen relative to win*/
#endif
} CANVAS;

/* macros to simplify access to the environment block */

#ifdef MAC
#define SCROLL_BAR_WIDTH	16

#define CANVAS_GP(hCanvas)		(theSetOfCanvases[hCanvas].grafptr)
#define CANVAS_WP(hCanvas)		((WindowPtr)(theSetOfCanvases[hCanvas].grafptr))
#define CANVAS_WREC(hCanvas)	(theSetOfCanvases[hCanvas].wRecord)
#define CANVAS_SCALE(hCanvas)	(theSetOfCanvases[hCanvas].scale)
#define CANVAS_PORTRECT(hCanvas) (CANVAS_WP(hCanvas)->portRect)
#else
#define CANVAS_PS(hCanvas)		(theSetOfCanvases[hCanvas].ps)
#define CANVAS_HDC(hCanvas)		(CANVAS_PS(hCanvas).hdc)
#define CANVAS_HWND(hCanvas)	(theSetOfCanvases[hCanvas].hWnd)
#define CANVAS_PORTRECT(hCanvas) (theSetOfCanvases[hCanvas].portRect)
#endif

#define CANVAS_USERDATA(hCanvas) (theSetOfCanvases[hCanvas].userdata)

#define CANVAS_RIGHT(hCanvas)	(theSetOfCanvases[hCanvas].right)
#define CANVAS_BOTTOM(hCanvas)	(theSetOfCanvases[hCanvas].bottom)
#define CANVAS_X(hCanvas)		(theSetOfCanvases[hCanvas].x)
#define CANVAS_Y(hCanvas)		(theSetOfCanvases[hCanvas].y)
	
#define NUM_CANVAS_BLOCKS		10	

#define ILLEGAL_USERDATA	((long)-1)

LOCAL CANVAS theSetOfCanvases[NUM_CANVAS_BLOCKS] = {0};

#ifdef MAC

/* the first macro takes one argument, 
 * the second macro takes two args.
 */
 
#define SWITCH_PORT(gp,func,rect)	\
{									\
	GrafPtr oldport = thePort;		\
	SetPort(gp);					\
	func(rect);						\
	SetPort(oldport);				\
}

#define SWITCH_PORT_XY(gp,func,x,y)	\
{									\
	GrafPtr oldport = thePort;		\
	SetPort(gp);					\
	func(x,y);						\
	SetPort(oldport);				\
}
#endif

/**********************************************************************/
LOCAL VOID env_IsCanvasValid(hCanvas)
	HCANVAS hCanvas;
{
	if (hCanvas >= NUM_CANVAS_BLOCKS)
		env_FatalError("No such hCanvas: %u", hCanvas);
#ifndef MAC
	/* Windows lets us do a little more error checking */
	if (! IsWindow(CANVAS_HWND(hCanvas)))
		env_FatalError("Illegal hWnd %u in hCanvas %u", 
			    CANVAS_HWND(hCanvas), hCanvas);
#endif
}

/**********************************************************************/


PUBLIC VOID env_InitCanvases()
{
	register HCANVAS hCanvas;
	for (hCanvas=0; hCanvas < NUM_CANVAS_BLOCKS; hCanvas++)
		CANVAS_USERDATA(hCanvas) = ILLEGAL_USERDATA;
}

PUBLIC HCANVAS env_NewCanvas(userdata)
	LONG	userdata;
{
	register HCANVAS hCanvas;
	for (hCanvas=0; hCanvas < NUM_CANVAS_BLOCKS; hCanvas++)
	{
		if (CANVAS_USERDATA(hCanvas)==ILLEGAL_USERDATA)
		{
			CANVAS_USERDATA(hCanvas) = userdata;
			return hCanvas;
		}
	}
	/* still here */
	return INVALID_INDEX;
}

#ifdef MAC
PUBLIC BOOL env_IsDevData(devdata)
	LONG devdata;
{
	register HCANVAS hCanvas;
	for (hCanvas=0; hCanvas < NUM_CANVAS_BLOCKS; hCanvas++)
		if(CANVAS_USERDATA(hCanvas) != ILLEGAL_USERDATA
			&& devdata==(long)CANVAS_GP(hCanvas))
				return TRUE;
	/*else*/
	return FALSE;
}

PUBLIC MEMPTR env_GetAddrDevBlock(hCanvas)
	HCANVAS	hCanvas;
{
	env_IsCanvasValid(hCanvas);
	return (MEMPTR) &CANVAS_WREC(hCanvas);
}
#endif

#if 0

PUBLIC HCANVAS env_FindCanvas(hWnd)
	HWND hWnd;
{
	register HCANVAS hCanvas;
	for (hCanvas=0; hCanvas < NUM_CANVAS_BLOCKS; hCanvas++)
		if (CANVAS_HWND(hCanvas) == hWnd)
			return hCanvas;
	/* still here:  create an env for them */
	return env_NewCanvas(hWnd);
}
#endif

/**********************************************************************/

PUBLIC VOID env_FreeCanvas(hCanvas)
	HCANVAS hCanvas;
{
	env_IsCanvasValid(hCanvas);
	/* set all fields to zero */
	memset((MEMPTR)&theSetOfCanvases[hCanvas], 0, sizeof(CANVAS));
	CANVAS_USERDATA(hCanvas) = ILLEGAL_USERDATA;
}

/**********************************************************************/

PUBLIC VOID env_ActivateCanvas(hCanvas)
	HCANVAS hCanvas;
{
#ifdef MAC
#else
	env_IsCanvasValid(hCanvas);
	BringWindowToTop( CANVAS_HWND(hCanvas) );
#endif
}

PUBLIC VOID env_InvalidateCanvas(hCanvas, rect)
	HCANVAS hCanvas;
	Rect *	rect;
{
#ifdef MAC
	GrafPtr	savePort;
	env_IsCanvasValid(hCanvas);
	GetPort( &savePort );
	SetPort( CANVAS_GP(hCanvas) );
	InvalRect( &(CANVAS_PORTRECT(hCanvas)));
	SetPort( savePort);
#else
	env_IsCanvasValid(hCanvas);
	if(rect==(Rect *)NULL)
	    InvalidateRect(CANVAS_HWND(hCanvas),(LPRECT)NULL,(BOOL)TRUE);
	else
	    env_FatalError("InvalidateCanvas: not impl!");
#endif

}

/**********************************************************************/
#ifdef MAC
LOCAL GrafPtr	saved_port;
#endif

PUBLIC VOID env_StartDrawing(hCanvas)
	HCANVAS	hCanvas;
{
#ifdef MAC
	env_IsCanvasValid(hCanvas);
	GetPort( &saved_port );
	SetPort( CANVAS_GP(hCanvas) );
#else
	env_IsCanvasValid(hCanvas);
	CANVAS_HDC(hCanvas) = GetDC(CANVAS_HWND(hCanvas));
#endif
}

PUBLIC VOID env_EndDrawing(hCanvas)
	HCANVAS	hCanvas;
{
#ifdef MAC
	SetPort(saved_port);
	saved_port = (GrafPtr)0;
#else
	env_IsCanvasValid(hCanvas);
	ReleaseDC( CANVAS_HWND(hCanvas), CANVAS_HDC(hCanvas));
	CANVAS_HDC(hCanvas) = 0;
#endif
}

/**********************************************************************/

PUBLIC VOID env_SetCanvasDev(hCanvas, devdata)
	HCANVAS	hCanvas;
	LONG	devdata;
{
#ifdef MAC
	env_IsCanvasValid(hCanvas);
	CANVAS_GP(hCanvas) = (GrafPtr) devdata;
#else
	/*env_IsCanvasValid(hCanvas);<<<is not yet valid<<<*/
	CANVAS_HWND(hCanvas) = LOWORD(devdata);
	ut_PrintHex("RV|SetCanvasDev: hWnd=",(LONG) CANVAS_HWND(hCanvas));
#endif
}

PUBLIC LONG env_GetCanvasDev(hCanvas)
	HCANVAS	hCanvas;
{
#ifdef MAC
	env_IsCanvasValid(hCanvas);
	return (LONG) CANVAS_GP(hCanvas);
#else
	env_IsCanvasValid(hCanvas);
	return (LONG) CANVAS_HWND(hCanvas);
#endif
}

/**********************************************************************
	GRAPHICS PRIMITIVES
/*********************************************************************/

PUBLIC VOID env_TextOut(hCanvas, x, y, str)
	HCANVAS hCanvas;
	PIXEL	x,y;
	STRING	str;
{
#ifdef MAC
    env_MoveTo(hCanvas, x, y);
    DrawText((Ptr) str, 0, strlen(str));
#else
	/*need to add windows code */
#endif
}



PUBLIC VOID env_MoveRel(hCanvas, x, y)
	HCANVAS hCanvas;
	short x,y;
{
#ifdef MAC
	env_IsCanvasValid(hCanvas);
	
	x *= CANVAS_SCALE(hCanvas);
	y *= CANVAS_SCALE(hCanvas);

	if (CANVAS_GP(hCanvas) != thePort)
		SWITCH_PORT_XY(CANVAS_GP(hCanvas),Move,x,y)
	else
		Move(x,y);
#else
	env_IsCanvasValid(hCanvas);
			
	CANVAS_X(hCanvas) += x;
	CANVAS_Y(hCanvas) += y;
	MoveTo(CANVAS_HDC(hCanvas), CANVAS_X(hCanvas), CANVAS_Y(hCanvas));
#endif
}

PUBLIC VOID env_Line(hCanvas, x, y)
	HCANVAS hCanvas;
	short x,y;
{
#ifdef MAC
	env_IsCanvasValid(hCanvas);

	x *= CANVAS_SCALE(hCanvas);
	y *= CANVAS_SCALE(hCanvas);

	if (CANVAS_GP(hCanvas) != thePort)
		SWITCH_PORT_XY(CANVAS_GP(hCanvas),Line,x,y)
	else
		Line(x,y);
#else
	env_IsCanvasValid(hCanvas);
	CANVAS_X(hCanvas) += x;
	CANVAS_Y(hCanvas) += y;
	LineTo(CANVAS_HDC(hCanvas), CANVAS_X(hCanvas), CANVAS_Y(hCanvas));
#endif
}

PUBLIC VOID env_MoveTo(hCanvas, x, y)
	HCANVAS hCanvas;
	short x,y;
{
#ifdef MAC
	env_IsCanvasValid(hCanvas);

	x *= CANVAS_SCALE(hCanvas);
	y *= CANVAS_SCALE(hCanvas);

	if (CANVAS_GP(hCanvas) != thePort)
		SWITCH_PORT_XY(CANVAS_GP(hCanvas),Line,x,y)
	else
		MoveTo(x,y);
#else
	env_IsCanvasValid(hCanvas);
	CANVAS_X(hCanvas) = x;
	CANVAS_Y(hCanvas) = y;
	MoveTo(CANVAS_HDC(hCanvas), CANVAS_X(hCanvas), CANVAS_Y(hCanvas));
#endif
}

PUBLIC VOID env_LineTo(hCanvas, x, y)
	HCANVAS hCanvas;
	short x,y;
{
#ifdef MAC
	env_IsCanvasValid(hCanvas);

	x *= CANVAS_SCALE(hCanvas);
	y *= CANVAS_SCALE(hCanvas);

	if (CANVAS_GP(hCanvas) != thePort)
		SWITCH_PORT_XY(CANVAS_GP(hCanvas),Line,x,y)
	else
		LineTo(x,y);
#else
	env_IsCanvasValid(hCanvas);
	CANVAS_X(hCanvas) = x;
	CANVAS_Y(hCanvas) = y;
	LineTo(CANVAS_HDC(hCanvas), CANVAS_X(hCanvas), CANVAS_Y(hCanvas));
#endif
}


PUBLIC VOID env_PaintRect(hCanvas, r)
	HCANVAS hCanvas;
	Rect *	r;
{
#ifdef MAC
	env_IsCanvasValid(hCanvas);
	if (CANVAS_GP(hCanvas) != thePort)
		SWITCH_PORT(CANVAS_GP(hCanvas),PaintRect,r)
	else
		PaintRect(r);
#else
	env_IsCanvasValid(hCanvas);
	null_pen(CANVAS_HDC(hCanvas));
	Rectangle(CANVAS_HDC(hCanvas), r->left, r->top, r->right, r->bottom);
#endif
}

PUBLIC VOID env_FrameRect(hCanvas, r)
	HCANVAS hCanvas;
	Rect *	r;
{
#ifdef MAC
	env_IsCanvasValid(hCanvas);
	if (CANVAS_GP(hCanvas) != thePort)
		SWITCH_PORT(CANVAS_GP(hCanvas),FrameRect,r)
	else
		FrameRect(r);
#else
	ut_PrintMacRect("RV|envFrameRect: r",r);
	env_IsCanvasValid(hCanvas);
	null_brush(CANVAS_HDC(hCanvas));
	Rectangle(CANVAS_HDC(hCanvas), r->left, r->top, r->right, r->bottom);
#endif
}


PUBLIC VOID env_FillRect(hCanvas, r, pat)
	HCANVAS hCanvas;
	Rect *r;
	PATT pat;
{
#ifdef MAC
	/*need to add mac equivalent*/
#else
	env_IsCanvasValid(hCanvas);
	/* do without Windows FillRect() function */	
	select_fill_pattern(CANVAS_HDC(hCanvas),pat);
	SetROP2(CANVAS_HDC(hCanvas), R2_COPYPEN);
	Rectangle(CANVAS_HDC(hCanvas),r->left,r->top,r->right,r->bottom);
#endif
}


#if 0
PUBLIC VOID env_GetXY(hCanvas, x, y)
	HCANVAS hCanvas;
	short *x, *y;
{
#ifdef MAC
	env_IsCanvasValid(hCanvas);
	*x = CANVAS_GP(hCanvas)->pnLoc.h * CANVAS_SCALE(hCanvas);
	*y = CANVAS_GP(hCanvas)->pnLoc.v * CANVAS_SCALE(hCanvas);
#else
	env_IsCanvasValid(hCanvas);
	*x = CANVAS_X(hCanvas);
	*y = CANVAS_Y(hCanvas);
#endif
}

PUBLIC VOID env_GetSizeCanvas(hCanvas, x, y)
	HCANVAS hCanvas;
	short *x, *y;
{
#ifdef MAC
	Rect r;
	env_IsCanvasValid(hCanvas);
	r = CANVAS_GP(hCanvas)->portRect;		/* structure copy */
	*x = (r.right - r.left) - SCROLL_BAR_WIDTH;
	*y = (r.bottom - r.top) - SCROLL_BAR_WIDTH;
#else
	env_IsCanvasValid(hCanvas);
	*x = CANVAS_RIGHT(hCanvas);
	*y = CANVAS_BOTTOM(hCanvas);
#endif
}
#endif


/***********************************************************************/

PUBLIC VOID env_ResizeCanvas(hCanvas,where)
	HCANVAS hCanvas; Point *where;
{
#ifdef MAC
	long		new_size;
	Rect		r;
	Point		pt;
	pt = *where; /*structure copy*/
	env_IsCanvasValid(hCanvas);
	env_GetScreenSize(&r);
	new_size = GrowWindow( CANVAS_GP(hCanvas), pt, &r);
	SizeWindow( CANVAS_GP(hCanvas), LoWord(new_size), HiWord(new_size), TRUE);
	
	/*should do inval rect for just the scroll bars */
	InvalRect( &(CANVAS_GP(hCanvas)->portRect));
#endif
}

/***********************************************************************/

PUBLIC VOID env_MoveCanvas(hCanvas,where)
	HCANVAS hCanvas; Point *where;
{
#ifdef MAC
	Rect		r;
	Point		pt;
	env_GetScreenSize(&r);
	pt = *where; /*structure copy*/
	DragWindow( CANVAS_GP(hCanvas), pt, &r );
#else
	/*not needed in MS Windows*/
#endif
}

/****************************************************************/

PUBLIC VOID env_CalcSizeCanvas(hCanvas)
	HCANVAS hCanvas;
{
#ifdef MAC /*Not Needed on mac*/
	env_IsCanvasValid(hCanvas);
#else
	IMPORT VOID win_GetAppWndRect(),win_ScreenToAppl();
	HWND		hWnd;
	RECT		ms_rect,r;
	HDC		hDC;
	long		window_origin;
	POINT		origin_pt,p1,p2;
	CANVAS *	canvas;
	
	env_IsCanvasValid(hCanvas);

#ifdef AS
	/* WHAT'S THIS canvas->hWnd STUFF???? */
#endif
	canvas = &(theSetOfCanvases[hCanvas]);
	hDC = CANVAS_HDC(hCanvas);

	/* get entire extent of current window in SCREEN coordinates */
	GetWindowRect(canvas->hWnd, (LPRECT) &(canvas->winRect));
	
	ut_Print4Shorts("RV|envResizeCanvas: winRect PHYS",
	    canvas->winRect.left,  canvas->winRect.top, 
	    canvas->winRect.right, canvas->winRect.bottom);

	/* convert window extent to APPLICATION coordinates, 
	 * i.e., Mac "Global" coords
         */

	p1.x = canvas->winRect.left;  
	p1.y = canvas->winRect.top;		win_ScreenToAppl(&p1);
	p2.x = canvas->winRect.right;
	p2.y = canvas->winRect.bottom;	win_ScreenToAppl(&p2);

	ut_Print4Shorts("RV|envResizeCanvas: W in APPL coords",
			p1.x,p1.y,p2.x,p2.y);

	canvas->winRect.left   = p1.x;    canvas->winRect.top    = p1.y;
	canvas->winRect.right  = p2.x;    canvas->winRect.bottom = p2.y;    

	GetClientRect(canvas->hWnd, (LPRECT) &ms_rect);

	p1.x = ms_rect.left;	p1.y = ms_rect.top;
	p2.x = ms_rect.right;	p2.y = ms_rect.bottom;

	ClientToScreen(canvas->hWnd, (LPPOINT)&p1); win_ScreenToAppl(&p1);
	ClientToScreen(canvas->hWnd, (LPPOINT)&p2); win_ScreenToAppl(&p2);
	
	canvas->portRect.left   = 0;    
	canvas->portRect.top    = 0; 
	canvas->portRect.right  = (p2.x - p1.x);
	canvas->portRect.bottom = (p2.y - p1.y);
	
#ifdef AS
	/* right now, hDC is zero! */
	env_OffsetRect(&(canvas->portRect), 0, 0);
#else
	window_origin = GetWindowOrg(hDC);

	origin_pt  = MAKEPOINT(window_origin);
	
	env_OffsetRect(&(canvas->portRect), origin_pt.x, origin_pt.y);
#endif	
	
	win_GetAppWndRect(&r);		    

	canvas->scrBounds.left   = r.left   + canvas->portRect.left  - p1.x;
	canvas->scrBounds.top    = r.top    + canvas->portRect.top   - p1.y;
	canvas->scrBounds.right  = r.right  + canvas->portRect.left  - p1.x;
	canvas->scrBounds.bottom = r.bottom + canvas->portRect.top   - p1.y;

	ut_PrintMacRect("RV|envResizeCanvas: pBbounds WITH org",
		&(canvas->scrBounds));
#endif
}

/****************************************************************/

PUBLIC VOID	env_GlobalToLocal(hCanvas,pt)
    HCANVAS	hCanvas;
    Point *	pt;	
{
#ifdef MAC
	env_IsCanvasValid(hCanvas);
	if (CANVAS_GP(hCanvas) != thePort)
		SWITCH_PORT(CANVAS_GP(hCanvas),GlobalToLocal,pt)
	else
		GlobalToLocal(pt);
#else
	CANVAS *	canvas;
	
	env_IsCanvasValid(hCanvas);

	canvas = &(theSetOfCanvases[hCanvas]);

	ut_Print2Shorts("RV|GlobalToLocal: p GLOBAL",pt->h,pt->v);
	ut_PrintMacRect("RV|GlobalToLocal: scrBnds ",&(canvas->scrBounds));

	pt->h += canvas->scrBounds.left;
	pt->v += canvas->scrBounds.top;
	ut_Print2Shorts("RV|GlobalToLocal: p LOCAL",pt->h,pt->v);
#endif
}

/****************************************************************/

PUBLIC VOID env_OffsetOrgCanvas(hCanvas, x, y)
	HCANVAS hCanvas;
	short x, y;
{
#if 0
	env_IsCanvasValid(hCanvas);
	OffsetViewportOrg(CANVAS_HDC(hCanvas), x, y);
	/* Mac hCanvas will need to keep around old origin? */
#endif
}

PUBLIC VOID env_SetScaleCanvas(hCanvas, x, y, units)
	HCANVAS hCanvas;
	short x, y, units;
{
#ifdef MAC
	env_IsCanvasValid(hCanvas);
	CANVAS_SCALE(hCanvas) = min(x,y) / units;
#else
	env_IsCanvasValid(hCanvas);
	SetMapMode(CANVAS_HDC(hCanvas), MM_ISOTROPIC);
	SetWindowExt(CANVAS_HDC(hCanvas), units, units);
	SetViewportExt(CANVAS_HDC(hCanvas), x, y);
#endif
}

/**********************************************************************/

PUBLIC VOID env_BeginUpdate(hCanvas)
	HCANVAS hCanvas;
{
#ifdef MAC
	env_IsCanvasValid(hCanvas);
    
	/*rect = (**((GrafPtr)canvas)->visRgn).rgnBBox; structure copy 
	EraseRect( &(canvas->portRect));*/
	    
	BeginUpdate(CANVAS_WP(hCanvas));
#else
	env_IsCanvasValid(hCanvas);
	BeginPaint(CANVAS_HWND(hCanvas), &CANVAS_PS(hCanvas));
#endif
}

PUBLIC VOID env_EndUpdate(hCanvas)
	HCANVAS hCanvas;
{
#ifdef MAC
	env_IsCanvasValid(hCanvas);
	EndUpdate(CANVAS_WP(hCanvas));
	DrawGrowIcon( CANVAS_WP(hCanvas));
#else
	env_IsCanvasValid(hCanvas);
	EndPaint(CANVAS_HWND(hCanvas), &CANVAS_PS(hCanvas));
	CANVAS_HDC(hCanvas) = 0;
#endif
}


/****************************************************************
    EVENT PROCESSING
****************************************************************/

PUBLIC VOID env_GetAppEvent(appEvent)
	APPEVENT *		appEvent;
{
#ifdef MAC
	IMPORT BOOL mac_EventLoop();
	/* inner event loop handles system and internal events*/
	while( mac_EventLoop(appEvent))
		/*continue*/; 
#else
	IMPORT VOID win_GetAppEvent();
	win_GetAppEvent(appEvent);
#endif
}

/****************************************************************
    MACINTOSH SPECIFIC EVENT HANDLING (Internal Event Loop)
****************************************************************/    
#ifdef MAC

#include "EventMgr.h"

#ifdef DBG /*for debugging*/
LOCAL STRING event_types[] =
{
    "nullEvent","mouseDown","mouseUp","keyDown",
    "keyUp","autoKey","updateEvt","diskEvt",
    "activateEvt","event 9","networkEvt","driverEvt",
    "app1Evt","app2Evt","app3Evt","app4Evt"
};

LOCAL STRING mousedown_types[] =
{
    "inDesk","inMenuBar","inSysWindow","inContent",
    "inDrag","inGrow","inGoAway","inZoomIn","inZoomOut"
};
#endif


LOCAL BOOL mac_EventLoop(appEvent)
	APPEVENT *		appEvent;
{
	EventRecord		macEvent;
	WindowPtr		whichWindow;
	
	SystemTask();
	
#if 0 /*to be done*/
	MaintainCursor();
	MaintainMenus();
	TEIdle(TEH);
#endif
	
	if (GetNextEvent(everyEvent, &macEvent) )
	{
		ut_PutString("RV|EventLoop: event.what",
			    event_types[macEvent.what]);

		switch (macEvent.what) 
		{
		case mouseDown:
		{
			int mousedown_code = FindWindow( macEvent.where, &whichWindow );
			ut_PutString("RV|MainLoop: FindWindow returns",
				 mousedown_types[mousedown_code]);
			ut_PrintHex("RV|MainLoop: mousedown in wptr",whichWindow);
			switch(mousedown_code) 
			{
			case inSysWindow:
			{
				SystemClick( &macEvent, whichWindow );
				return TRUE;
			} break;
			
			case inDesk: 	
			{
				SysBeep(10);
				return TRUE;	
			} break;
			
			case inMenuBar: 
			{
				long menu_selection = MenuSelect(macEvent.where);
				int  menu_category  = HiWord(menu_selection);
				int  menu_item      = LoWord(menu_selection);
				
				if( menu_category)
				{
#ifdef DBG
					if(ut_FilterMenuSelection(menu_selection))
						return TRUE;
#endif
					appEvent->cmd_category = menu_category;
					appEvent->cmd_item     = menu_item; 
					appEvent->event_type   = CMD_EVENT;
					return FALSE;
				}
				else
					return TRUE;
			} break;
			
			case inGoAway:
			{
				if (vu_IsAppView((LONG) whichWindow)	
					&& TrackGoAway( whichWindow, macEvent.where) )
				{
					appEvent->event_type   = CLOSEVIEW_EVENT;
					appEvent->canvas       = LoWord(((WindowPeek)whichWindow)->refCon);
					appEvent->where        = macEvent.where;
					return FALSE;
				}
				else
					return TRUE;
			} break;
				
			case inDrag:
			{
				if (vu_IsAppView((LONG)whichWindow) )
				{
					appEvent->event_type   = MOVEVIEW_EVENT;
					appEvent->canvas       = LoWord(((WindowPeek)whichWindow)->refCon);
					appEvent->where        = macEvent.where;
					return FALSE;
				}
				else
					return TRUE;
				} break;
				
			case inContent:
			{
				if (whichWindow != FrontWindow())
				{
					SelectWindow(whichWindow);
					SetPort((GrafPtr)whichWindow);
					return TRUE;
				}
				else if (vu_IsAppView((LONG)whichWindow) )
				{
					appEvent->event_type   = MOUSEDOWN_EVENT;
					appEvent->canvas       = LoWord(((WindowPeek)whichWindow)->refCon);
					appEvent->where        = macEvent.where;
					return FALSE;
				}
				else 
					return TRUE;
			} break;

			case inGrow:
			{
				if (vu_IsAppView((LONG)whichWindow) )
				{
					appEvent->event_type   = RESIZEVIEW_EVENT;
					appEvent->canvas       = LoWord(((WindowPeek)whichWindow)->refCon);
					appEvent->where        = macEvent.where;
					return FALSE;
				}
				else 
					return TRUE;	
			} break;
				
		   } /* end switch on mousedown_code from FindWindow() */
			
		} break; /*end case mouseDown */
	
		case autoKey: 
			/*FALL THROUGH */
		case keyDown:					
		{
			REG char	theChar = macEvent.message & charCodeMask;
					
			if ((macEvent.modifiers & cmdKey) != 0) 
			{
				long menu_selection = MenuKey( theChar );
				int  menu_category  = HiWord(menu_selection);
				int  menu_item      = LoWord(menu_selection);
				if( menu_category)
				{
					appEvent->cmd_category = menu_category;
					appEvent->cmd_item     = menu_item; 
					appEvent->event_type   = CMD_EVENT;
					return FALSE;
				}
				else
					return TRUE;
			}
			else 
			{
				appEvent->the_char		= theChar;
				appEvent->event_type	= CHAR_EVENT;
				return FALSE;
			}
		} break;

		case activateEvt:
		{
			if (vu_IsAppView((LONG)(macEvent.message)) )
			{
				appEvent->event_type	= ACTIVATEVIEW_EVENT;
				appEvent->canvas		= LoWord(((WindowPeek)(macEvent.message))->refCon);
				return FALSE;
			}
#ifdef DBG
			else if(ut_IsDebugWindow((WindowPtr)(macEvent.message)) )
			{
				ut_ActivateDebugWindow();
				return TRUE;
			}
#endif
			else 
				return TRUE;
		}	break;
		
		case updateEvt:
		{
			if (vu_IsAppView((LONG)(macEvent.message)) )
			{
				appEvent->event_type	= UPDATEVIEW_EVENT;
				appEvent->canvas		= LoWord(((WindowPeek)(macEvent.message))->refCon);
				return FALSE;
			}

#ifdef DBG
			else if(ut_IsDebugWindow((WindowPtr)(macEvent.message)) )
			{
				ut_UpdateDebugWindow();
				return TRUE;
			}
#endif
			else
			{
				BeginUpdate((WindowPtr)(macEvent.message));
				EndUpdate((WindowPtr)(macEvent.message));
				return TRUE;
			}
		} break;
		
		} /* end of case macEvent.what */
	} /* if */
	
	return TRUE;
}
#endif /*macintosh specific internal event loop*/


/****************************************************************
    MENU/COMMAND PROCESSING
****************************************************************/    

#ifdef MAC  /*macintosh specific is here. See WINMAIN.c for MSWINDOWS */

#include "dispatch.h"

#include "MenuMgr.h"

LOCAL MenuHandle theMenuCategories[NUM_MENU_CATEGORIES];

PUBLIC BOOL env_InitMenus()
{
	int		i;
	
	/*this is optional check to see if we can access the resource file */
	OpenResFile("\pgenapp.rsrc"); 
									
	if (GetResource('MENU', R_ID_FILE_MENU)==0) 
	{
		env_FatalError("miniEdit.c: Can't open resource file.");
		/*call does not return*/
		return FALSE;
	}
	theMenuCategories[SYS_MENU] = NewMenu( R_ID_SYS_MENU, "\p\024" );
	AddResMenu( theMenuCategories[SYS_MENU], 'DRVR' );
	
	theMenuCategories[FILE_MENU] = GetMenu(R_ID_FILE_MENU);
	theMenuCategories[EDIT_MENU] = GetMenu(R_ID_EDIT_MENU);
	
	theMenuCategories[VIEW_MENU] = NewMenu(R_ID_VIEW_MENU, "\pView");
	AppendMenu(theMenuCategories[VIEW_MENU],"\pGraphics View");
	AppendMenu(theMenuCategories[VIEW_MENU],"\pText View");

	theMenuCategories[PALETTE_MENU] = NewMenu(R_ID_PALETTE_MENU,"\pPalette");
	AppendMenu(theMenuCategories[PALETTE_MENU],"\pSelection Tool");
	AppendMenu(theMenuCategories[PALETTE_MENU],"\pCreate Rect Tool");

	for ( i = 0; i < NUM_MENU_CATEGORIES; i++ ) 
		InsertMenu(theMenuCategories[i], 0) ;

	DrawMenuBar();
	return TRUE;
}

#else
PUBLIC BOOL env_InitMenus(){return TRUE;} /*ms-windows version*/
#endif

PUBLIC VOID env_DoSysMenu(menu_item)
	int menu_item;
{
#ifdef MAC
	Str255	name;
	
	GetItem(theMenuCategories[SYS_MENU], menu_item, &name);
	OpenDeskAcc( &name );
	vu_SetCurrentView();
#endif /*windows version not needed*/
}



/****************************************************************
    DEBUGGING
****************************************************************/
#ifdef MAC
#include <stdio.h>
typedef char *va_list;
#define va_start(ap,v)		ap=(va_list)&v+sizeof(v)

#define SOME_RESOURCE_ID		1		/* ???? */

/* can't take an HCANVAS param because it might be wrong! */
void debug(mask)
	char *mask;
{
	char buf[255];
	va_list args;
	
	va_start(args, mask);
	vsprintf(buf, mask, args);
	ParamText(buf, 0L, 0L, 0L);
	StopAlert(SOME_RESOURCE_ID, 0L);
}
#else
#include <stdarg.h>

/* can't take an HCANVAS param because it might be wrong! */
VOID debug(mask)
	char *mask;
{
	char buf[255];
	va_list args;
	
	va_start(args, mask);
	vsprintf(buf, mask, args);
	MessageBox(GetActiveWindow(), buf, "Debug!", MB_OK);
}
#endif

#ifdef MAC
PUBLIC VOID memset(ptr,value,count)
	MEMPTR ptr;int value; int count;
{
	while(count--) *ptr++ = value;
}
#endif
