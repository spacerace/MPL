/********************************** Jigsaw  ***********************************/
/*                                                                            */
/* Created 1988, Microsoft Corporation. 				      */
/*                                                                            */
/* Purpose:  To illustrate the use of Gpi retained segments.		      */
/*                                                                            */
/* Summary:  This program provides a jigsaw puzzle, based on a decomposition  */
/*   of an arbitrary bitmap loaded from a file.  The user can jumble the      */
/*   pieces, then drag them individually by means of the mouse.  The image    */
/*   can be zoomed in and out and scrolled up/down and left/right.	      */
/*                                                                            */
/*   Each piece of the puzzle is a retained segment.  When a piece is	      */
/*   selected for dragging, it is made dynamic.  A menu option allows the     */
/*   selected piece to be dragged as an outline or as a normal-looking piece. */
/*                                                                            */
/*   Individual pieces are made to "move" by changing their model transforms. */
/*   Scrolling and zooming of the whole picture is done by changing the       */
/*   default viewing transform. 					      */
/*                                                                            */
/* Optimizations:  While it is possible to implement this puzzle using a very */
/*   naive approach, this is liable to lead to a rather slowly-operating      */
/*   program.  The following optimizations dramatically improve program       */
/*   performance:							      */
/*                                                                            */
/*   1> BitBlt only as much of the bitmap through a clip path as necessary.   */
/*   Each piece of the puzzle is drawn by defining a clip path, blitting      */
/*   through the path, and drawing an outline on the same path.  The naive    */
/*   approach is to blit the whole bitmap through the clip path.  A more      */
/*   clever approach is to compute the piece's bounding box and only use      */
/*   the source and destination rectangles which correspond to this box.      */
/*   This leads to an order-of-magnitude speedup in the time to draw one      */
/*   piece.								      */
/*                                                                            */
/*   2> Make the source and target rectangles for BitBlt the same size	      */
/*   in device coordinates.  A BitBlt in a retained segment must be done      */
/*   with GpiWCBitBlt and the target rectangle must be specified in world     */
/*   coordinates, so you must use GpiConvert (taking into account that in     */
/*   world space rectangles are inclusive-inclusive while in device space     */
/*   rectangles are inclusive-exclusive) to compute what target world space   */
/*   rectangle will be converted to the desired device space rectangle.       */
/*   Making the sizes of the source and converted target rectangles differ    */
/*   by even one pel will cause strectching or compression to occur, with     */
/*   a dramatic loss in speed.	Unfortunately, due to rounding effects, it is */
/*   not always possible to guarantee that adding an offset to the	      */
/*   transformation applied to a segment will leave the size of the	      */
/*   rectangle defined by the orders in the segment unchanged.		      */
/*                                                                            */
/*   3> Use auxiliary information to reduce the number of segments which      */
/*   must be checked for correlation.  The naive approach to hit-testing is   */
/*   to test the whole chain, even though generally only a small fraction of  */
/*   the segments in the chain could possibly get a hit.  A more clever       */
/*   approach is to take the bounding box for each segment and only include   */
/*   the segment in the correlation check if the box contains the correlation */
/*   point.  eg.							      */
/*    a> Edit the chain by adjusting the ATTR_CHAINED attribute of each       */
/*    segment to reflect candidacy for being hit.  Afterwards, fix up by      */
/*    adding back removed segments to the chain.			      */
/*    b> Even faster is to keep an auxiliary data structure which records     */
/*    the priority of the segments (placed in the SEGLIST chain).  Run	      */
/*    through the priority list from high-priority to low-priority and do a   */
/*    correlation test on each segment which passes the bounding-box test.    */
/*                                                                            */
/*   4> When repainting through a clip region, only draw those segments which */
/*   overlap the clip region.  The naive approach is to set up the clip       */
/*   region and do a GpiDrawChain on the whole chain.  The drawback to this   */
/*   is that much time will be spent running through the orders in segments   */
/*   which are not visible through the clip region.  Very often, most of the  */
/*   segments in the picture can be eliminated from needing to be drawn by    */
/*   recognizing that there is no overlap between the bounding boxes of the   */
/*   segment and the clip region.					      */
/*                                                                            */
/*   5> Do WinScrollWindow horizontally in multiples of 8 pels when possible. */
/*   For example, horizontal scrolls by 7 or 9 pels are much slower than a    */
/*   a horizontal scroll by 8 pels.					      */
/*                                                                            */
/******************************************************************************/

#define INCL_BITMAPFILEFORMAT
 
#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES
#define INCL_DOSMEMMGR

#define INCL_DEV

#define INCL_WINWINDOWMGR
#define INCL_WINMESSAGEMGR
#define INCL_WININPUT
#define INCL_WINRECTANGLES
#define INCL_WINPOINTERS
#define INCL_WINMENUS
#define INCL_WINSCROLLBARS
#define INCL_WINFRAMEMGR
#define INCL_WINSWITCHLIST
#define INCL_WINSYS

#define INCL_GPIBITMAPS
#define INCL_GPICONTROL
#define INCL_GPITRANSFORMS
#define INCL_GPIPRIMITIVES
#define INCL_GPIMETAFILES
#define INCL_GPIPATHS
#define INCL_GPIREGIONS
#define INCL_GPISEGMENTS
#define INCL_GPISEGEDITING
#define INCL_GPICORRELATION
#define INCL_GPILCIDS
 
#define INCL_ERRORS
 
#include <os2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <opendlg.h>
#include "jigsaw.h"
 
 
 
/*----------------------- inter-thread messages ------------------------------*/
 
#define UM_DIE	      WM_USER+1        /* instruct async thread to terminate  */
#define UM_DRAW       WM_USER+2        /* draw the current picture	      */
#define UM_VSCROLL    WM_USER+3        /* perform scroll by recalculating the */
                                       /* default viewing transform           */
#define UM_HSCROLL    WM_USER+4        /* perform scroll by recalculating the */
                                       /* default viewing transform           */
#define UM_SIZING     WM_USER+5        /* perform sizing by recalculating the */
                                       /* default viewing transform           */
#define UM_ZOOM_IN    WM_USER+6        /* zoom the picture by recalculating   */
                                       /* the default viewing transform       */
#define UM_ZOOM_OUT   WM_USER+7        /* zoom the picture by recalculating   */
                                       /* the default viewing transform       */
#define UM_REDRAW     WM_USER+8
#define UM_JUMBLE     WM_USER+9
#define UM_LOAD       WM_USER+10
#define UM_DUMMY      WM_USER+11       /* all commands not forcing redraw     */
                                       /* must come after this one            */
 
#define UM_LEFTDOWN   WM_USER+12       /* mouse button down in the client area*/
				       /* perform a correlation on the current*/
				       /* picture, setting any picked segment */
				       /* to dynamic			      */
#define UM_MOUSEMOVE  WM_USER+13       /* mousemove command, remove, repositon*/
				       /* and redraw any dynamic sements      */
#define UM_LEFTUP     WM_USER+14       /* mouse button up in the client area  */
                                       /* set any dynamic segment to normal   */
#define UM_FASTDRAG   WM_USER+15       /* toggle fast-drag (outline) mode     */
#define UM_DRAWDONE   WM_USER+16       /* Async DrawChain has completed       */
#define UM_FLUSH      WM_USER+17
 
 
/*------------------------ element label values  -----------------------------*/
 
#define FILLPATH       222L
#define BITBLT_TOP     232L
#define BITBLT_BOTTOM  233L
 
 
/*------------------------- correlation parameters ---------------------------*/
 
#define HITS	1L		       /* maximum number of hits to return    */
#define DEPTH	2L		       /* max depth of seg calls to return    */
 
 
/*-------------------------- general definitions -----------------------------*/
 
 
HAB	habMain=NULL;		       /* main thread anchor block handle     */
HMQ	hmqMain=NULL;		       /* main thread queue handle	      */
HWND    hwndFrame=NULL;                /* frame control handle                */
HWND	hwndClient=NULL;	       /* client area handle		      */
HDC	hdcClient=NULL; 	       /* window dc handle		      */
HPS	hpsClient=NULL; 	       /* client area Gpi ps handle	      */
SIZEL	sizlMaxClient;		       /* max client area size		      */
HPS     hpsPaint=NULL;                 /* ps for use in Main Thread           */
HRGN	hrgnInvalid = NULL;	       /* handle to the invalid region	      */
 
HAB	habAsync=NULL;		       /* async thread anchor block handle    */
HMQ	hmqAsync=NULL;		       /* async thread queue handle	      */
TID     tidAsync;                      /* async thread id                     */
SEL	selStack;		       /* async thread stack selector	      */
#define STACKSIZE  4096 	       /* async thread stack size	      */
SHORT	sPrty = -1;		       /* async thread priority 	      */
 
HWND	hwndHorzScroll=NULL;	       /* horizontal scroll bar window	      */
HWND	hwndVertScroll=NULL;	       /* vertical scroll bar window	      */
POINTS	ptsScrollPos, ptsOldScrollPos;
POINTS	ptsScrollMax, ptsHalfScrollMax;
POINTS	ptsScrollLine = { 8, 8};
POINTS	ptsScrollPage = { 64, 64};
 
#define UNITY	       65536L
MATRIXLF matlfIdentity = { UNITY, 0, 0, 0, UNITY, 0, 0, 0, 1 };
LONG	lScale = 0;		       /* current zoom level		      */
#define ZOOM_MAX       8
#define ZOOM_IN_ARG    1
#define ZOOM_OUT_ARG   -1
 
#define CALLSEG_BASE   1000
POINTL	ptlOffset;
POINTL	ptlBotLeft  = { 0, 0};
POINTL	ptlTopRight = { 300, 300};
LONG	lLastSegId;		       /* last segment id assigned to a piece */
LONG	lPickedSeg;		       /* seg id of piece selected for drag   */
RECTL	rclBounds;		       /* pict bounding box in model coords.  */
POINTL	ptlOldMouse = {0L, 0L};        /* current mouse posn		      */
BOOL	fButtonDown = FALSE;	       /* only drag if mouse down	      */
BOOL	fFastDrag = FALSE;	       /* show only outline of dragging piece */


/*-------------------------- segment list ------------------------------------*/

typedef struct _SEGLIST {	       /* sl				      */
    LONG		  lSegId;
    struct _SEGLIST FAR * pslPrev;
    struct _SEGLIST FAR * pslNext;
    POINTL		  ptlLocation; /* piece location, world coordinates   */
    RECTL		  rclCurrent;  /* segment bounding box, model coords  */
    RECTL		  rclBitBlt;   /* segment bounding box, world coords  */
} SEGLIST ;
typedef SEGLIST FAR *PSEGLIST;	       /* psl				      */
typedef PSEGLIST FAR *PPSEGLIST;       /* ppsl				      */
PSEGLIST pslHead = NULL;	       /* head of the list		      */
PSEGLIST pslTail = NULL;	       /* tail of the list		      */
PSEGLIST pslPicked = NULL;	       /* picked segment's list member        */
#define   ADD_HEAD_SEG	 1
#define   ADD_TAIL_SEG	 2
#define        DEL_SEG	 3
 
/*-------------------------- bitmap-related data -----------------------------*/

typedef struct _LOADINFO {	       /* li				      */
    HFILE   hf;
    CHAR    szFileName[MAX_FNAME_LEN];
} LOADINFO ;
typedef LOADINFO FAR *PLOADINFO;       /* pli				      */

HPS		   hpsBitmapFile=NULL, hpsBitmapTemp=NULL, hpsBitmapDrag=NULL;
HDC		   hdcBitmapFile=NULL, hdcBitmapTemp=NULL, hdcBitmapDrag=NULL;
HBITMAP 	   hbmBitmapFile=NULL, hbmBitmapTemp=NULL, hbmBitmapDrag=NULL;
BITMAPINFOHEADER   bmpBitmapFile   = {12L, 0, 0, 0, 0};
BITMAPINFOHEADER   bmpBitmapTemp   = {12L, 0, 0, 0, 0};
BITMAPINFOHEADER   bmpBitmapDrag   = {12L, 0, 0, 0, 0};
BITMAPINFO	   bmiBitmap	   = {12L, 0, 0, 0, 0, {{0, 0, 0}}};
static DEVOPENSTRUC dop = { NULL
			  , "DISPLAY"
			  , NULL
			  , NULL
			  , NULL
			  , NULL
			  , NULL
			  , NULL
			  , NULL };


/*-------------------------- old-style bitmap header -------------------------*/

typedef struct {
    USHORT    wType;
    ULONG     dwSize;
    int       xHotspot;
    int       yHotspot;
    ULONG     dwBitsOffset;
    USHORT    bmWidth;
    USHORT    bmHeight;
    USHORT    bmPlanes;
    USHORT    bmBitcount;
} RCBITMAP;
typedef RCBITMAP FAR *PRCBITMAP;


/*--------------------------- Miscellaneous ----------------------------------*/
 
ULONG	ulTerminateSem = 0;	       /* main thread blocks while async dies */
HSEM	hsemTerminate  = &ulTerminateSem;

ULONG	ulSzFmt   = 0;		       /* serializes access to sprintf()      */
HSEM	hsemSzFmt = &ulSzFmt;
CHAR	szFmt[50];		       /* buffer used by sprintf()	      */

SWCNTRL swctl = { 0, 0, 0, 0, 0, SWL_VISIBLE, SWL_JUMPABLE, NULL, 0 };
HSWITCH hsw;			       /* handle to a switch list entry       */
char	szTitle[80];		       /* Title bar text		      */

BOOL	fErrMem = FALSE;	       /* set if alloc async stack fails      */
 
 
/*------------------------- Function Prototypes ------------------------------*/

VOID	 CalcBounds( VOID);
VOID	 CalcTransform( HWND);
MRESULT CALLBACK ClientWndProc( HWND, USHORT, MPARAM, MPARAM);
BOOL	 CreateBitmapHdcHps( HDC, HPS);
BOOL	 CreateThread( VOID);
BOOL	 CreatePicture( VOID);
VOID	 DestroyThread( VOID);
BOOL	 DoDraw( HRGN);
VOID	 DoHorzScroll( VOID);
VOID	 DoVertScroll( VOID);
BOOL	 DumpPicture( VOID);
VOID	 Finalize( VOID);
BOOL	 Initialize( VOID);
VOID	 Jumble( VOID);
VOID	 LeftDown( MPARAM);
VOID	 LeftUp( VOID);
VOID	 Load( PLOADINFO);
VOID cdecl main( VOID);
VOID	 MessageInt( HWND, INT, PCH);
VOID	 MouseMove( MPARAM);
VOID	 MyMessageBox( HWND, PSZ);
VOID FAR NewThread( VOID);
BOOL	 PrepareBitmap( VOID);
BOOL	 ReadBitmap( HFILE);
VOID	 Redraw( VOID);
VOID	 ReportError( HAB);
BOOL	 SegListCheck( INT);
PSEGLIST SegListGet( LONG);
BOOL	 SegListUpdate( USHORT, PSEGLIST);
BOOL	 SendCommand( USHORT, ULONG);
VOID	 SetDVTransform( FIXED, FIXED, FIXED, FIXED, LONG, LONG, LONG);
VOID	 SetRect( PSEGLIST);
VOID	 ToggleFastDrag( VOID);
VOID	 Translate( PSEGLIST, PPOINTL);
MRESULT  WndProcCommand( HWND, USHORT, MPARAM, MPARAM);
MRESULT  WndProcCreate( HWND);
MRESULT  WndProcPaint( VOID);
MRESULT  WndProcSize( MPARAM, MPARAM);
VOID	 Zoom( SHORT);
VOID	 ZoomMenuItems( VOID);

 
/******************************************************************************/
/*									      */
/*  MyMessageBox							      */
/*									      */
/*  Displays a message box with the given string.  To simplify matters,       */
/*  the box will always have the same title ("Jigsaw"), will always	      */
/*  have a single button ("Ok"), will always have an exclamation point	      */
/*  icon, and will always be application modal. 			      */
/*									      */
/******************************************************************************/
VOID
MyMessageBox( hWnd, psz)

HWND  hWnd;
PSZ   psz;
{
    WinMessageBox( HWND_DESKTOP
		 , hWnd
		 , psz
		 , szTitle
		 , NULL
		 , MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL );
}

/******************************************************************************/
/*                                                                            */
/* Main thread will initialize the process for PM services and process	      */
/* the application message queue until a WM_QUIT message is received. It will */
/* then destroy all PM resources and terminate. Any error during	      */
/* initialization will be reported and the process terminated.                */
/*                                                                            */
/******************************************************************************/
VOID cdecl
main()
{
  QMSG	qmsg;
 
  if( Initialize())
      while( WinGetMsg( habMain, &qmsg, NULL, NULL, NULL))
	  WinDispatchMsg( habMain, &qmsg);
  else
      ReportError( habMain);
  Finalize();
}
 
 
/******************************************************************************/
/*                                                                            */
/* The Initialize function will initialize the PM interface,		      */
/* create an application message queue, a standard frame window and a new     */
/* thread to control drawing operations.  It will also initialize static      */
/* strings.                                                                   */
/*                                                                            */
/******************************************************************************/
BOOL
Initialize()
{
  ULONG   flCreate;
  PID	  pid;
  TID	  tid;
 
  WinShowPointer( HWND_DESKTOP, TRUE);
  habMain = WinInitialize( NULL);
  if( !habMain)
      return( FALSE);
 
  hmqMain = WinCreateMsgQueue( habMain,0);
  if( !hmqMain)
      return( FALSE);
 
  WinLoadString( habMain, NULL, TITLEBAR, sizeof(szTitle), szTitle);
  if( !WinRegisterClass( habMain
		       , (PCH)szTitle
		       , (PFNWP)ClientWndProc
		       , CS_SIZEREDRAW
		       , 0 ))
      return( FALSE);
 
  flCreate =   (FCF_STANDARD | FCF_VERTSCROLL | FCF_HORZSCROLL)
	     & ~(ULONG)FCF_TASKLIST;
  hwndFrame = WinCreateStdWindow( HWND_DESKTOP
				, WS_VISIBLE
				, &flCreate
				, szTitle
				, szTitle
				, WS_VISIBLE
				, NULL
				, APPMENU
				, &hwndClient);
  if( !hwndFrame)
      return( FALSE);

  WinQueryWindowProcess( hwndFrame, &pid, &tid);
  swctl.hwnd	  = hwndFrame;
  swctl.idProcess = pid;
  lstrcpy( swctl.szSwtitle, szTitle);
  hsw = WinAddSwitchEntry( &swctl);

  if( !CreateThread())		      /* create async thread		     */
      return ( FALSE);
  if( !CreateBitmapHdcHps( &hdcBitmapFile, &hpsBitmapFile))
      return( FALSE);
  if( !CreateBitmapHdcHps( &hdcBitmapTemp, &hpsBitmapTemp))
      return( FALSE);
  if( !CreateBitmapHdcHps( &hdcBitmapDrag, &hpsBitmapDrag))
      return( FALSE);
 
  return( TRUE);
}
 
/******************************************************************************/
/*                                                                            */
/* Finalize will destroy the asynchronous drawing thread, all Presentation    */
/* Manager resources, and terminate the process.                              */
/*                                                                            */
/******************************************************************************/
VOID
Finalize()
{
  DestroyThread();

  while( pslHead != NULL )
      SegListUpdate( DEL_SEG, pslHead);
  if( hrgnInvalid)
      GpiDestroyRegion( hpsClient, hrgnInvalid);
  if( hpsClient)
      GpiAssociate( hpsClient, NULL);
  if( hpsPaint)
      GpiAssociate( hpsPaint, NULL);
  if( hpsBitmapFile)
      GpiAssociate( hpsBitmapFile, NULL);
  if( hpsBitmapTemp)
      GpiAssociate( hpsBitmapTemp, NULL);
  if( hpsBitmapDrag)
      GpiAssociate( hpsBitmapDrag, NULL);
  if( hwndFrame)
      WinDestroyWindow( hwndFrame);
  if( hmqMain)
      WinDestroyMsgQueue( hmqMain);
  if( habMain)
      WinTerminate( habMain);

  DosExit( EXIT_PROCESS, 0);
}
 
 
/******************************************************************************/
/*                                                                            */
/* ReportError	will display the latest error information for the required    */
/* thread. No resources to be loaded if out of memory error.                  */
/*                                                                            */
/******************************************************************************/
VOID
ReportError( hab)

HAB hab;
{
  PERRINFO  perriBlk;
  PSZ	    pszErrMsg;
  USHORT *  TempPtr;
 
  if( !hwndFrame)
      return;
  if( !fErrMem)
  {
      perriBlk = WinGetErrorInfo(hab);
      if( !perriBlk)
          return;
      SELECTOROF( pszErrMsg) = SELECTOROF(perriBlk);
      SELECTOROF( TempPtr)   = SELECTOROF(perriBlk);
      OFFSETOF( TempPtr)     = perriBlk->offaoffszMsg;
      OFFSETOF( pszErrMsg)   = *TempPtr;
      WinMessageBox( HWND_DESKTOP
		   , hwndFrame
		   , pszErrMsg
		   , szTitle
		   , 0
		   , MB_CUACRITICAL | MB_ENTER);
      WinFreeErrorInfo( perriBlk);
  } else
      WinMessageBox( HWND_DESKTOP
		   , hwndFrame
		   , "ERROR - Out Of Memory"
		   , szTitle
		   , 0
		   , MB_CUACRITICAL | MB_ENTER);
}
 
 
/******************************************************************************/
/*                                                                            */
/* CreateThread  creates the asynchronous drawing thread. It will allocate    */
/* stack space and create the thread.                                         */
/*                                                                            */
/******************************************************************************/
BOOL
CreateThread()
{
  PBYTE pbAsyncStack;		      /* long pointer to stack for new thread */
 
 
  if( DosAllocSeg( STACKSIZE, (PSEL)&selStack, 0 ))
  {
      fErrMem = TRUE;
      return( FALSE);
  }
  OFFSETOF(pbAsyncStack) = STACKSIZE-2;
  SELECTOROF(pbAsyncStack) = selStack;
  if( DosCreateThread( (PFNTHREAD)NewThread, &tidAsync, pbAsyncStack ))
      return( FALSE);
  return( TRUE);
}
 
 
/******************************************************************************/
/*                                                                            */
/* DestroyThread  will send a message  to the asynchronous drawing thread     */
/* commanding it to terminate itself. If the send is successful it will wait  */
/* until the async thread has terminated. It will then release any stack space*/
/* used by that thread.                                                       */
/*                                                                            */
/******************************************************************************/
VOID
DestroyThread()
{
  if( tidAsync)
  {
      DosSemSet( hsemTerminate);
      if( SendCommand( (USHORT)UM_DIE, (ULONG)NULL))
	  DosSemWait( hsemTerminate, SEM_INDEFINITE_WAIT);
  }
  if( selStack)
      DosFreeSeg( selStack);
}
 
 
/******************************************************************************/
/*                                                                            */
/* SendCommand	will attempt to post the required command and parameters to   */
/* the asynchronous drawing thread's message queue. The command will only     */
/* be posted if the queue exists.					      */
/*                                                                            */
/******************************************************************************/
BOOL
SendCommand( usCommand, ulInfo)

USHORT	usCommand;
ULONG	ulInfo;
{
  if( !hmqAsync)
      return( FALSE);

  switch( usCommand)
  {
    case UM_DIE:
    case UM_LEFTDOWN:
    case UM_LEFTUP:
    case UM_MOUSEMOVE:
    case UM_DRAW:
    case UM_HSCROLL:
    case UM_VSCROLL:
    case UM_ZOOM_IN:
    case UM_ZOOM_OUT:
    case UM_REDRAW:
    case UM_SIZING:
    case UM_FASTDRAG:
    case UM_JUMBLE:
    case UM_LOAD:
 
	return( WinPostQueueMsg( hmqAsync
			       , usCommand
			       , MPFROMLONG( ulInfo)
			       , MPFROMLONG( NULL  ) ) );
	break;

    default:
	return( TRUE);
  }
}
 
 
/******************************************************************************/
/*                                                                            */
/* ClientWndProd is the window procedure associated with the client window.   */
/*                                                                            */
/******************************************************************************/
MRESULT CALLBACK
ClientWndProc( hwnd, msg, mp1, mp2)

HWND	hwnd;
USHORT  msg;
MPARAM	mp1;
MPARAM	mp2;
{
  CHAR  szTemp[128];
 
  switch( msg)
  {
    case WM_CREATE:
      return( WndProcCreate( hwnd));
      break;
 
    case WM_CLOSE:
      WinLoadString( habMain, NULL, TERMINATE, sizeof(szTemp), (PSZ)szTemp );
      if( WinMessageBox( HWND_DESKTOP
		       , hwndFrame
		       , szTemp
		       , szTitle
		       , 0
		       , MB_CUAWARNING | MB_YESNO | MB_DEFBUTTON2)
	       == MBID_YES)
	  WinPostMsg( hwnd, WM_QUIT, NULL, NULL);
      break;
 
    case WM_PAINT:
      return( WndProcPaint());
      break;
 
    /**************************************************************************/
    /*									      */
    /**************************************************************************/
    case WM_ERASEBACKGROUND:
      WinFillRect( (HPS)mp1, (PRECTL)mp2, CLR_BACKGROUND);
      return( FALSE);
      break;
 
    /**************************************************************************/
    /*									      */
    /**************************************************************************/
    case WM_MINMAXFRAME:
      if( (((PSWP)mp1)->fs & SWP_RESTORE)  ||
	  (((PSWP)mp1)->fs & SWP_MAXIMIZE) )
	SendCommand( (USHORT)UM_SIZING, 0L);
      break;
 
    /**************************************************************************/
    /* Process menu item commands, and commands generated from the keyboard   */
    /* via the accelerator table. Most are handled by the async thread        */
    /**************************************************************************/
    case WM_COMMAND:
      return( WndProcCommand( hwnd, msg, mp1, mp2));
      break;
 
    /**************************************************************************/
    /* Scrolling is handled by the async drawing thread. Simply pass on the   */
    /* command and parameters                                                 */
    /**************************************************************************/
    case WM_HSCROLL:
      SendCommand( (USHORT)UM_HSCROLL, LONGFROMMP(mp2));
      break;
 
    case WM_VSCROLL:
      SendCommand( (USHORT)UM_VSCROLL, LONGFROMMP(mp2));
      break;
 
    /************************************************************************/
    /* The client area is being resized.                                    */
    /************************************************************************/
    case WM_SIZE:
      return( WndProcSize( mp1, mp2));
      break;
 
    /**************************************************************************/
    /* Mouse commands are handled by the async thread. Simply send on the     */
    /* command and parameters.                                                */
    /**************************************************************************/
    case WM_BUTTON1DBLCLK:
    case WM_BUTTON1DOWN:
      if( hwnd != WinQueryFocus( HWND_DESKTOP, FALSE))
	  WinSetFocus( HWND_DESKTOP, hwnd);
      if( !fButtonDown)
      {
	  fButtonDown = TRUE;
	  SendCommand( (USHORT)UM_LEFTDOWN, LONGFROMMP(mp1));
      }
      return( TRUE);
      break;
 
    case WM_BUTTON1UP:
      if( !fButtonDown)
	  return( TRUE);
      if( SendCommand( (USHORT)UM_LEFTUP, LONGFROMMP(mp1)))
          fButtonDown = FALSE;
      else
	  WinAlarm( HWND_DESKTOP, WA_WARNING);
      return( TRUE);
      break;
 
    case WM_MOUSEMOVE:
      if( fButtonDown && (pslPicked != NULL))
	  SendCommand( (USHORT)UM_MOUSEMOVE, LONGFROMMP(mp1));
      return( WinDefWindowProc( hwnd, msg, mp1, mp2));
      break;

    /**************************************************************************/
    /* Default for the rest						      */
    /**************************************************************************/
    default:
      return( WinDefWindowProc( hwnd, msg, mp1, mp2));
  }
 
  return( FALSE);
}
 
/******************************************************************************/
/*									      */
/* Get the maximum client area size.  Create a window DC for the client       */
/* area and a normal GPI Presentation Space and associate the two.  The GPI   */
/* PS will be the maximum client area size and be in pels.		      */
/*									      */
/******************************************************************************/
MRESULT
WndProcCreate( hwnd)

HWND  hwnd;
{
  SIZEL sizlPickApp;		      /* pick aperture size		      */
 
  sizlMaxClient.cx = WinQuerySysValue( HWND_DESKTOP, SV_CXFULLSCREEN);
  sizlMaxClient.cy = WinQuerySysValue( HWND_DESKTOP, SV_CYFULLSCREEN);
 
  hdcClient = WinOpenWindowDC( hwnd);
  hpsClient = GpiCreatePS( habMain
			 , hdcClient
			 , &sizlMaxClient
			 , GPIA_ASSOC | PU_PELS );
  if( !hpsClient)
      return( TRUE);
  GpiSetAttrMode( hpsClient, AM_PRESERVE);

  hwndHorzScroll = WinWindowFromID( WinQueryWindow( hwnd, QW_PARENT, FALSE)
				  , FID_HORZSCROLL);

  hwndVertScroll = WinWindowFromID( WinQueryWindow( hwnd, QW_PARENT, FALSE)
				  , FID_VERTSCROLL);

  hpsPaint = GpiCreatePS( habMain, NULL, &sizlMaxClient, PU_PELS);
 
  hrgnInvalid = GpiCreateRegion( hpsClient, 0L, NULL);
 
  sizlPickApp.cx = sizlPickApp.cy = 1;
  GpiSetPickApertureSize( hpsClient, PICKAP_REC, &sizlPickApp);
  return( FALSE);
}
 
 
/*******************************************************************************/
/*									       */
/* WM_PAINT message							       */
/*									       */
/*******************************************************************************/
MRESULT
WndProcPaint()

{
  HRGN	 hrgnUpdt;
  SHORT  sRgnType;
 
  hrgnUpdt = GpiCreateRegion( hpsPaint, 0L, NULL);
  sRgnType = WinQueryUpdateRegion( hwndClient, hrgnUpdt);
  WinValidateRegion( hwndClient, hrgnUpdt, FALSE);
  SendCommand( UM_DRAW, (ULONG)hrgnUpdt);
  return( FALSE);
}
 
/******************************************************************************/
/* Process menu item commands, and commands generated from the keyboard via   */
/* the accelerator table.  Most are handled by the async thread 	      */
/******************************************************************************/
MRESULT
WndProcCommand( hwnd, msg, mp1, mp2)

HWND	hwnd;
USHORT  msg;
MPARAM	mp1, mp2;
{
  CHAR	    szTemp[128];
  DLF	    dlf;
  SEL	    sel;
  PLOADINFO pli;
  PSZ	    pszError, psz1, psz2;
 
  switch( SHORT1FROMMP(mp1))
  {
    case MENU_JUMBLE:
	SendCommand( UM_JUMBLE, 0L);
	break;

    case MENU_LOAD:
	DosAllocSeg( sizeof( LOADINFO), &sel, 0);
	pli = MAKEP( sel, 0);

	dlf.rgbAction	    = DLG_OPENDLG;
	dlf.rgbFlags	    = ATTRDIRLIST;
	dlf.phFile	    = &(pli->hf);
	dlf.pszExt	    = (PSZ)"\\*.bmp";
	dlf.pszAppName	    = szTitle;
	dlf.pszTitle	    = "Load Bitmap";
	dlf.pszInstructions = NULL;
	dlf.szFileName[0]   = '\0';
	dlf.szOpenFile[0]   = '\0';
	pszError	    = "Error reading file.";

	switch( DlgFile( hwnd, &dlf))
	{
	  case TDF_ERRMEM:
	  case TDF_INVALID:
	      MyMessageBox( hwnd, pszError);
	      break;

	  case TDF_NOOPEN:
	      break;

	  default:
	      for( psz1 = dlf.szFileName, psz2 = pli->szFileName
		 ; *psz2++ = *psz1++
		 ; )
		  ;
	      SendCommand( UM_LOAD, (LONG)pli);
	      break;
	}
	break;
    /**********************************************************************/
    /* EXIT command, menu item or F3 key pressed. Give the operator a	  */
    /* second chance, if confirmed post a WM_QUIT msg to the application  */
    /* msg queue. This will force the MAIN thread to terminate.           */
    /**********************************************************************/
    case MENU_EXIT:
      WinLoadString( habMain, NULL, TERMINATE, sizeof(szTemp), szTemp);
      if( WinMessageBox( HWND_DESKTOP
		       , hwndFrame
		       , szTemp
		       , szTitle
		       , 0
		       , MB_CUAWARNING | MB_YESNO | MB_DEFBUTTON2)
	    == MBID_YES)
	WinPostMsg( hwnd, WM_QUIT, NULL, NULL);
      break;
 
    /**********************************************************************/
    /* Pass on the rest to the async thread.				  */
    /**********************************************************************/
    case MENU_ZOOMIN:
      SendCommand( UM_ZOOM_IN, 0L);
      break;
 
    case MENU_ZOOMOUT:
      SendCommand( UM_ZOOM_OUT, 0L);
      break;

    case MENU_FASTDRAG:
      SendCommand( UM_FASTDRAG, 0L);
      break;
 
    /**********************************************************************/
    /* Unrecognised => default						  */
    /**********************************************************************/
    default:
      return( WinDefWindowProc(hwnd, msg, mp1, mp2));
  }
  return( FALSE);
}
 
/******************************************************************************/
/* Load a bitmap							      */
/******************************************************************************/
VOID
Load( pli)

PLOADINFO  pli;
{
    PSZ     pszError;
    RECTL   rclClient;

    pszError = (PSZ)"Error reading file.";

    DumpPicture();
    if( !ReadBitmap( pli->hf) )
    {
      MyMessageBox( hwndClient, pszError);
      return;
    }
    if( !PrepareBitmap() )
    {
      MyMessageBox( hwndClient, pszError);
      return;
    }

    lstrcpy( swctl.szSwtitle, szTitle);
    lstrcat( swctl.szSwtitle, ": ");
    lstrcat( swctl.szSwtitle, pli->szFileName);
    WinChangeSwitchEntry( hsw, &swctl);
    WinSetWindowText( hwndFrame, swctl.szSwtitle);

    CreatePicture();
    lScale = 0;

    WinQueryWindowRect( hwndClient, &rclClient);
    ptsScrollMax.x = (SHORT)(rclClient.xRight - rclClient.xLeft);
    ptsHalfScrollMax.x = ptsScrollMax.x >> 1;
    ptsScrollPos.x = ptsHalfScrollMax.x;
    ptsOldScrollPos.x = ptsHalfScrollMax.x;
    WinSendMsg( hwndHorzScroll
	      , SBM_SETSCROLLBAR
	      , MPFROMSHORT( ptsScrollPos.x)
	      , MPFROM2SHORT( 1, ptsScrollMax.x) );
    ptsScrollMax.y = (SHORT)(rclClient.yTop - rclClient.yBottom);
    ptsHalfScrollMax.y = ptsScrollMax.y >> 1;
    ptsScrollPos.y = ptsHalfScrollMax.y;
    ptsOldScrollPos.y = ptsHalfScrollMax.y;
    WinSendMsg( hwndVertScroll
	      , SBM_SETSCROLLBAR
	      , MPFROMSHORT( ptsScrollPos.y)
	      , MPFROM2SHORT( 1, ptsScrollMax.y) );

    CalcBounds();
    CalcTransform( hwndClient);
    DosFreeSeg( SELECTOROF( pli));
}
/******************************************************************************/
/* Throw the pieces around the screen.					      */
/******************************************************************************/
VOID
Jumble()
{
  LONG	    lWidth, lHeight;
  DATETIME  date;
  POINTL    ptl;
  RECTL     rclClient;
  PSEGLIST  psl;

  if( WinQueryWindowRect( hwndClient, &rclClient) )
  {
    lWidth  = rclClient.xRight - rclClient.xLeft;
    lHeight = rclClient.yTop   - rclClient.yBottom;
    if( (lWidth > 0) && (lHeight > 0) )
    {
      DosGetDateTime( &date);
      srand( (USHORT)date.hundredths);
      for( psl = pslHead; psl != NULL; psl = psl->pslNext)
      {
	ptl.x = rclClient.xLeft   + (rand() % lWidth);
	ptl.y = rclClient.yBottom + (rand() % lHeight);
	Translate( psl, &ptl);
	SetRect( psl);
      }
    }
  }
}

/******************************************************************************/
/* The client area is being resized.  The current scroll bar thumb position   */
/* and scroll bar range must be recalculated prior to recalculating the       */
/* default viewing transform for the picture.  Wait for subsequent WM_PAINT   */
/* to do any drawing.							      */
/******************************************************************************/
MRESULT
WndProcSize( mp1, mp2)

MPARAM	mp1, mp2;
{
  HWND	hwndFrameTemp;

  if( hwndFrame)
    hwndFrameTemp = hwndFrame;
  else
    hwndFrameTemp = WinQueryWindow( hwndClient, QW_PARENT, FALSE);

  ptsScrollMax.y = SHORT2FROMMP( mp2);
  ptsHalfScrollMax.y = ptsScrollMax.y >> 1;
  if( mp1)
  {
      ptsScrollPos.y = (SHORT)(((LONG)ptsScrollPos.y * (LONG)SHORT2FROMMP(mp2))/(LONG)SHORT2FROMMP(mp1));
      ptsOldScrollPos.y = (SHORT)(((LONG)ptsOldScrollPos.y * (LONG)SHORT2FROMMP(mp2))/(LONG)SHORT2FROMMP(mp1));
  } else
  {
      ptsScrollPos.y = ptsHalfScrollMax.y;     /* first sizing after window creation  */
      ptsOldScrollPos.y = ptsHalfScrollMax.y;
  }
  WinSendMsg( hwndVertScroll
	    , SBM_SETSCROLLBAR
	    , MPFROMSHORT( ptsScrollPos.y)
	    , MPFROM2SHORT( 1, ptsScrollMax.y) );


  ptsScrollMax.x = SHORT1FROMMP( mp2);
  ptsHalfScrollMax.x = ptsScrollMax.x >> 1;
  if( mp1)
  {
      ptsScrollPos.x = (SHORT)(((LONG)ptsScrollPos.x * (LONG)SHORT1FROMMP(mp2))/(LONG)SHORT1FROMMP(mp1));
      ptsOldScrollPos.x = (SHORT)(((LONG)ptsOldScrollPos.x * (LONG)SHORT1FROMMP(mp2))/(LONG)SHORT1FROMMP(mp1));
  } else
  {
      ptsScrollPos.x = ptsHalfScrollMax.x;     /* first sizing after window creation  */
      ptsOldScrollPos.x = ptsHalfScrollMax.x;
  }
  WinSendMsg( hwndHorzScroll
	    , SBM_SETSCROLLBAR
	    , MPFROMSHORT( ptsScrollPos.x)
	    , MPFROM2SHORT( 1, ptsScrollMax.x) );


  SendCommand( UM_SIZING, 0L);
  return( FALSE);
}
 
/******************************************************************************/
/*                                                                            */
/* NewThread is the asynchronous drawing thread. It is responsible for all    */
/* drawing.  It will initialize its PM interface and create an application    */
/* message queue.  It will then monitor its message queue and process any     */
/* commands received.							      */
/*                                                                            */
/******************************************************************************/
VOID FAR
NewThread()
{
  QMSG	  qmsgAsync, qmsgPeek;
  BOOL	  fDone;
 
  /****************************************************************************/
  /* Initialize the PM interface.  If it fails, terminate both threads.       */
  /****************************************************************************/
  habAsync = WinInitialize( NULL);
  if( !habAsync)
  {
      WinPostMsg( hwndClient, WM_QUIT, NULL, NULL);
      DosExit( EXIT_THREAD, 0);
  }
 
  /****************************************************************************/
  /* Create a message queue.  If it fails, terminate both threads.	      */
  /****************************************************************************/
  hmqAsync = WinCreateMsgQueue( habAsync, 80);
  if( !hmqAsync)
  {
      WinPostMsg( hwndClient, WM_QUIT, NULL, NULL);
      WinTerminate( habAsync);
      DosExit( EXIT_THREAD, 0);
  }
 
  DosSetPrty( PRTYS_THREAD, PRTYC_NOCHANGE, sPrty, (TID)NULL);
 
 
  while( TRUE)
  {
    WinGetMsg( habAsync, &qmsgAsync, NULL, 0, 0);
 
    /**************************************************************************/
    /* process the commands                                                   */
    /**************************************************************************/
    switch( qmsgAsync.msg)
    {
 
      /************************************************************************/
      /************************************************************************/
      case UM_LOAD:
	Load( (PLOADINFO)qmsgAsync.mp1);
	Redraw();
	break;

      /************************************************************************/
      case UM_JUMBLE:
	Jumble();
	Redraw();
	break;

      /************************************************************************/
      case UM_REDRAW:
	Redraw();
	break;

      /************************************************************************/
      /* DRAW will use the passed region containing the invalidated area of   */
      /* the screen, repaint it and then destroy the region.		      */
      /************************************************************************/
      case UM_DRAW:
	DoDraw( (HRGN)qmsgAsync.mp1);
	if( qmsgAsync.mp1)
	    GpiDestroyRegion( hpsClient, (HRGN)qmsgAsync.mp1);
        break;
 
 
      /************************************************************************/
      /* Get new scroll posn from command ( i.e. +/-1 +/-page) or new	      */
      /* absolute position from parameter, update scroll posn, change the     */
      /* transform and update the thumb posn.  Finally update the window.     */
      /************************************************************************/
      case UM_HSCROLL:
	switch( SHORT2FROMMP( qmsgAsync.mp1) )
	{
            case SB_LINEUP:
		ptsScrollPos.x -= ptsScrollLine.x;
                break;
            case SB_LINEDOWN:
		ptsScrollPos.x += ptsScrollLine.x;
                break;
	    case SB_SLIDERTRACK:
            case SB_SLIDERPOSITION:
		for( fDone = FALSE; !fDone ;)
		{
		  if( WinPeekMsg( habAsync
				, &qmsgPeek
				, NULL
				, UM_HSCROLL
				, UM_HSCROLL
				, PM_NOREMOVE))
		      if(   (SHORT2FROMMP( qmsgPeek.mp1) == SB_SLIDERTRACK)
			  ||(SHORT2FROMMP( qmsgPeek.mp1) == SB_SLIDERPOSITION) )
			  WinPeekMsg( habAsync
				    , &qmsgAsync
				    , NULL
				    , UM_HSCROLL
				    , UM_HSCROLL
				    , PM_REMOVE);
		      else
			  fDone = TRUE;
		  else
		      fDone = TRUE;
		}
		ptsScrollPos.x = SHORT1FROMMP( qmsgAsync.mp1);
                break;
            case SB_PAGEUP:
		ptsScrollPos.x -= ptsScrollPage.x;
                break;
            case SB_PAGEDOWN:
		ptsScrollPos.x += ptsScrollPage.x;
                break;
            case SB_ENDSCROLL:
                break;
            default:
                break;
	}
	DoHorzScroll();
        break;
 
      case UM_VSCROLL:
	switch( SHORT2FROMMP( qmsgAsync.mp1) )
	{
            case SB_LINEUP:
		ptsScrollPos.y -= ptsScrollLine.y;
                break;
            case SB_LINEDOWN:
		ptsScrollPos.y += ptsScrollLine.y;
                break;
	    case SB_SLIDERTRACK:
            case SB_SLIDERPOSITION:
		for( fDone = FALSE; !fDone ;)
		{
		  if( WinPeekMsg( habAsync
				, &qmsgPeek
				, NULL
				, UM_VSCROLL
				, UM_VSCROLL
				, PM_NOREMOVE))
		      if(   (SHORT2FROMMP( qmsgPeek.mp1) == SB_SLIDERTRACK)
			  ||(SHORT2FROMMP( qmsgPeek.mp1) == SB_SLIDERPOSITION) )
			  WinPeekMsg( habAsync
				    , &qmsgAsync
				    , NULL
				    , UM_VSCROLL
				    , UM_VSCROLL
				    , PM_REMOVE);
		      else
			  fDone = TRUE;
		  else
		      fDone = TRUE;
		}
		ptsScrollPos.y = SHORT1FROMMP( qmsgAsync.mp1);
                break;
            case SB_PAGEUP:
		ptsScrollPos.y -= ptsScrollPage.y;
                break;
            case SB_PAGEDOWN:
		ptsScrollPos.y += ptsScrollPage.y;
                break;
            case SB_ENDSCROLL:
                break;
            default:
                break;
	}
	DoVertScroll();
        break;
 
      /************************************************************************/
      /* recalc the picture transform                                         */
      /************************************************************************/
      case UM_SIZING:
	CalcBounds();
	CalcTransform( hwndClient);
        break;
 
      /************************************************************************/
      /* adjust zoom factor                                                   */
      /************************************************************************/
      case UM_ZOOM_IN:
	Zoom( ZOOM_IN_ARG);
        break;
 
      case UM_ZOOM_OUT:
	Zoom( ZOOM_OUT_ARG);
        break;

      /************************************************************************/
      /* toggle fast-drag						      */
      /************************************************************************/
      case UM_FASTDRAG:
	ToggleFastDrag();
	break;
 
      /************************************************************************/
      /* Button down will cause a correlate on the picture to test for a hit. */
      /* Any selected segment will be highlighted and redrawn as dynamic.     */
      /************************************************************************/
      case UM_LEFTDOWN:
	LeftDown( qmsgAsync.mp1);
        break;
 
      /************************************************************************/
      /* if a segment is being dragged it will be redrawn in a new posn       */
      /************************************************************************/
      case UM_MOUSEMOVE:
	for( fDone = FALSE; !fDone ;)
	{
	  if( WinPeekMsg( habAsync
			, &qmsgPeek
			, NULL
			, UM_MOUSEMOVE
			, UM_LEFTUP
			, PM_NOREMOVE))
	      if( qmsgPeek.msg == UM_MOUSEMOVE)
		  WinPeekMsg( habAsync
			    , &qmsgAsync
			    , NULL
			    , UM_MOUSEMOVE
			    , UM_MOUSEMOVE
			    , PM_REMOVE);
	      else
		  fDone = TRUE;
	  else
	      fDone = TRUE;
	}
	MouseMove( qmsgAsync.mp1);
        break;
 
      /************************************************************************/
      /* if a segment is being dragged it will be redrawn as normal	      */
      /************************************************************************/
      case UM_LEFTUP:
	LeftUp();
        break;
 
      /************************************************************************/
      /* destroy resources and terminate				     */
      /************************************************************************/
      case UM_DIE:
	WinDestroyMsgQueue( hmqAsync);
	WinTerminate( habAsync);
	DosEnterCritSec();
	DosSemClear( hsemTerminate);
        DosExit( EXIT_THREAD, 0);
        break;
 
      /************************************************************************/
      /* finish flush of commands from queue				      */
      /************************************************************************/
      case UM_FLUSH:
        break;
 
      default:
        break;
    }
  }
}
 
/******************************************************************************/
/* button down will cause one segment to be indicated and made dynamic	      */
/******************************************************************************/
VOID
LeftDown( mp)

MPARAM	 mp;
{
  HRGN	    hrgnUpdt;
  LONG	    alSegTag[HITS][DEPTH][2];
  POINTL    ptl, aptl[4];
  RECTL     rcl;
  MATRIXLF  matlf;
  LONG	    lOffset;
  BYTE	    bBuff[128];
  CHAR	    pszMsg[40];
  PSZ	    psz1, psz2;

  ptl.x = (LONG)(SHORT)SHORT1FROMMP( mp);
  ptl.y = (LONG)(SHORT)SHORT2FROMMP( mp);

  /****************************************************************************/
  /****************************************************************************/
  for( pslPicked = pslTail; pslPicked != NULL; pslPicked = pslPicked->pslPrev)
  {
    rcl = pslPicked->rclCurrent;
    GpiConvert( hpsClient, CVTC_MODEL, CVTC_DEVICE, 2L, (PPOINTL)&rcl);
    rcl.xRight++;
    rcl.yTop++;
    if( WinPtInRect( habAsync, &rcl, &ptl))
    {
	LONG lRet;

	GpiSetEditMode( hpsClient, SEGEM_INSERT);
	GpiOpenSegment( hpsClient, pslPicked->lSegId);
	GpiSetElementPointerAtLabel( hpsClient, FILLPATH);
	GpiFillPath( hpsClient, 1L, 0L);
	GpiCloseSegment( hpsClient);
	lRet = GpiCorrelateSegment( hpsClient
				  , pslPicked->lSegId
				  , PICKSEL_VISIBLE
				  , &ptl
				  , HITS
				  , DEPTH
				  , (PLONG)alSegTag );
	GpiOpenSegment( hpsClient, pslPicked->lSegId);
	GpiSetElementPointerAtLabel( hpsClient, FILLPATH);
	GpiOffsetElementPointer( hpsClient, 1L);
	GpiDeleteElement( hpsClient);
	GpiCloseSegment( hpsClient);

	if( lRet > 0)
	    break;
    }
  }
  if( pslPicked)
    lPickedSeg	 = pslPicked->lSegId;
  else
  {
    fButtonDown = FALSE;
    return;
  }
  if( (lPickedSeg < 1) || (lPickedSeg > lLastSegId) )
  {
    DosSemRequest( hsemSzFmt, SEM_INDEFINITE_WAIT);
    sprintf( szFmt, "Segment id out of range: %x", lPickedSeg);
    for( psz1 = szFmt, psz2 = pszMsg; *psz2++ = *psz1++; )
	;
    DosSemClear( hsemSzFmt);
    MyMessageBox( hwndClient, pszMsg);
    fButtonDown = FALSE;
    return;
  }

  /****************************************************************************/
  hrgnUpdt = GpiCreateRegion( hpsClient, 1L, &rcl);
  GpiSetSegmentAttrs( hpsClient, lPickedSeg, ATTR_VISIBLE, ATTR_OFF);

  GpiQuerySegmentTransformMatrix( hpsClient
				, lPickedSeg
				, 9L
				, &matlf );
  GpiBeginPath( hpsClient, 1L);
  GpiCallSegmentMatrix( hpsClient
		      , lPickedSeg + CALLSEG_BASE
		      , 9L
		      , &matlf
		      , TRANSFORM_REPLACE );
  GpiEndPath( hpsClient);
  GpiSetClipPath( hpsClient, 1L, SCP_AND);
  DoDraw( hrgnUpdt);
  GpiSetClipPath( hpsClient, 0L, SCP_RESET);
  GpiDestroyRegion( hpsClient, hrgnUpdt);

  /****************************************************************************/
  ptlOffset = ptlBotLeft;
  GpiConvert( hpsClient, CVTC_WORLD, CVTC_DEVICE, 1L, &ptlOffset);

  aptl[0].x = pslPicked->rclBitBlt.xLeft;
  aptl[0].y = pslPicked->rclBitBlt.yBottom;
  aptl[1].x = pslPicked->rclBitBlt.xRight;
  aptl[1].y = pslPicked->rclBitBlt.yTop;
  aptl[2] = aptl[0];
  aptl[3] = aptl[1];
  GpiConvert( hpsClient, CVTC_WORLD, CVTC_DEVICE, 2L, &aptl[2]);
  aptl[2].x -= ptlOffset.x;
  aptl[2].y -= ptlOffset.y;
  aptl[3].x -= ptlOffset.x - 1;
  aptl[3].y -= ptlOffset.y - 1;
  GpiSetEditMode( hpsClient, SEGEM_INSERT);

  for( lOffset = 0L; GpiGetData( hpsClient
			       , lPickedSeg
			       , &lOffset
			       , DFORM_NOCONV
			       , (LONG)sizeof( bBuff)
			       , bBuff) > 0; )
      ;

  GpiOpenSegment( hpsClient, lPickedSeg);
  GpiDeleteElementsBetweenLabels( hpsClient, BITBLT_TOP, BITBLT_BOTTOM);
  if( !fFastDrag)
      GpiWCBitBlt( hpsClient
		 , hbmBitmapDrag
		 , 4L
		 , aptl
		 , ROP_SRCCOPY
		 , BBO_IGNORE );
  GpiCloseSegment( hpsClient);

  for( lOffset = 0L; GpiGetData( hpsClient
			       , lPickedSeg
			       , &lOffset
			       , DFORM_NOCONV
			       , (LONG)sizeof( bBuff)
			       , bBuff) > 0; )
      ;

  /****************************************************************************/
  GpiSetSegmentAttrs( hpsClient, lPickedSeg, ATTR_VISIBLE, ATTR_ON);
  GpiSetSegmentAttrs( hpsClient, lPickedSeg, ATTR_DYNAMIC, ATTR_ON);
  GpiSetDrawControl( hpsClient, DCTL_DYNAMIC, DCTL_ON);
  GpiDrawSegment( hpsClient, lPickedSeg);

  WinSetCapture( HWND_DESKTOP, hwndClient);
}



 
/******************************************************************************/
/*                                                                            */
/* move the segment							      */
/*                                                                            */
/******************************************************************************/
VOID
MouseMove( mp)

MPARAM	 mp;
{
  RECTL   rcl;
  POINTL  ptl, ptlModel;
 
  ptl.x = (LONG)(SHORT)SHORT1FROMMP( mp);
  ptl.y = (LONG)(SHORT)SHORT2FROMMP( mp);

  ptlModel = ptl;
  GpiConvert( hpsClient, CVTC_DEVICE, CVTC_MODEL, 1L, &ptlModel);
  ptlModel.x = 5 * (ptlModel.x / 5);
  ptlModel.y = 5 * (ptlModel.y / 5);
  if( (ptlModel.x == ptlOldMouse.x) && (ptlModel.y == ptlOldMouse.y))
    return;
  ptlOldMouse.x = ptlModel.x;
  ptlOldMouse.y = ptlModel.y;

  /****************************************************************************/
  /* clip mouse coords to client window 				      */
  /****************************************************************************/
  WinQueryWindowRect(hwndClient, &rcl);
  if (rcl.xLeft > ptl.x)
    ptl.x = rcl.xLeft;
  if (rcl.xRight <= ptl.x)
    ptl.x = rcl.xRight;
  if (rcl.yBottom > ptl.y)
    ptl.y = rcl.yBottom;
  if (rcl.yTop <= ptl.y)
    ptl.y = rcl.yTop;

  GpiRemoveDynamics( hpsClient, lPickedSeg, lPickedSeg);
  Translate( pslPicked, &ptl);
  GpiDrawDynamics( hpsClient);
}
 
 
/******************************************************************************/
/*									      */
/* The dragged segment is being unselected.  Return it to its normal state.   */
/*									      */
/******************************************************************************/
VOID
LeftUp()
{
  SEGLIST    sl;
  POINTL     aptl[4];

  if( !lPickedSeg)
    return;
  GpiRemoveDynamics( hpsClient, lPickedSeg, lPickedSeg);
  GpiSetSegmentAttrs( hpsClient, lPickedSeg, ATTR_DYNAMIC, ATTR_OFF);

  /****************************************************************************/
  ptlOffset = ptlBotLeft;
  GpiConvert( hpsClient, CVTC_WORLD, CVTC_DEVICE, 1L, &ptlOffset);

  aptl[0].x = pslPicked->rclBitBlt.xLeft;
  aptl[0].y = pslPicked->rclBitBlt.yBottom;
  aptl[1].x = pslPicked->rclBitBlt.xRight;
  aptl[1].y = pslPicked->rclBitBlt.yTop;
  aptl[2] = aptl[0];
  aptl[3] = aptl[1];
  GpiConvert( hpsClient, CVTC_WORLD, CVTC_DEVICE, 2L, &aptl[2]);
  aptl[2].x -= ptlOffset.x;
  aptl[2].y -= ptlOffset.y;
  aptl[3].x -= ptlOffset.x - 1;
  aptl[3].y -= ptlOffset.y - 1;
  GpiSetEditMode( hpsClient, SEGEM_INSERT);
  GpiOpenSegment( hpsClient, lPickedSeg);
  GpiDeleteElementsBetweenLabels( hpsClient, BITBLT_TOP, BITBLT_BOTTOM);
  GpiWCBitBlt( hpsClient
	     , hbmBitmapTemp
	     , 4L
	     , aptl
	     , ROP_SRCCOPY
	     , BBO_IGNORE );
  GpiCloseSegment( hpsClient);

  /****************************************************************************/
  GpiDrawSegment( hpsClient, lPickedSeg);
  GpiSetSegmentPriority( hpsClient, lPickedSeg, 0L, LOWER_PRI); /* highest    */
  SetRect( pslPicked);

  sl = *pslPicked;
  SegListUpdate( DEL_SEG, pslPicked);
  SegListUpdate( ADD_TAIL_SEG, &sl);	    /* at tail => highest priority    */
  pslPicked = NULL;

  WinSetCapture( HWND_DESKTOP, (HWND)NULL);
}
 
 
/******************************************************************************/
/*                                                                            */
/* DoHorzScroll will horizontally scroll the current contents of	      */
/* the client area and redraw the invalidated area			      */
/*                                                                            */
/******************************************************************************/
VOID
DoHorzScroll()
{
  RECTL     rcl;
  HRGN	    hrgn;
  MATRIXLF  matlf;
 
  if( ptsScrollPos.x > ptsScrollMax.x)
      ptsScrollPos.x = ptsScrollMax.x;
  if( ptsScrollPos.x < 0)
      ptsScrollPos.x = 0;
 
  if( ptsOldScrollPos.x != ptsScrollPos.x)
      WinSendMsg( hwndHorzScroll
		, SBM_SETPOS
		, MPFROM2SHORT( ptsScrollPos.x, 0)
		, MPFROMLONG( NULL));
 
  /****************************************************************************/
  /* Scroll the window the reqd amount, using bitblt'ing (ScrollWindow)       */
  /* if any of the screen still in view, and paint into uncovered region;     */
  /* else repaint the whole client area.				      */
  /****************************************************************************/
  hrgn = GpiCreateRegion( hpsClient, 0L, NULL);
  if( abs( ptsScrollPos.x - ptsOldScrollPos.x) <= ptsScrollMax.x)
  {
      WinScrollWindow( hwndClient
		     , ptsOldScrollPos.x - ptsScrollPos.x
		     , 0
		     , NULL
		     , NULL
		     , hrgn
		     , &rcl
		     , 0);
  } else
  {
      WinQueryWindowRect( hwndClient, &rcl);
      GpiSetRegion( hpsClient, hrgn, 1L, &rcl);
  }
  GpiQueryDefaultViewMatrix( hpsClient, 9L, &matlf );
  matlf.lM31 -= ptsScrollPos.x - ptsOldScrollPos.x;
  GpiSetDefaultViewMatrix( hpsClient, 9L, &matlf, TRANSFORM_REPLACE);

  DoDraw( hrgn);
  ptsOldScrollPos.x = ptsScrollPos.x;
  GpiDestroyRegion( hpsClient, hrgn);
}
 
/******************************************************************************/
/*                                                                            */
/* DoVertScroll will vertically scroll the current contents of		      */
/* the client area and redraw the invalidated area			      */
/*                                                                            */
/******************************************************************************/
VOID
DoVertScroll()
{
  RECTL     rcl;
  HRGN	    hrgn;
  MATRIXLF  matlf;
 
  if( ptsScrollPos.y > ptsScrollMax.y)
      ptsScrollPos.y = ptsScrollMax.y;
  if( ptsScrollPos.y < 0)
      ptsScrollPos.y = 0;
 
  if( ptsOldScrollPos.y != ptsScrollPos.y)
      WinSendMsg( hwndVertScroll
		, SBM_SETPOS
		, MPFROM2SHORT( ptsScrollPos.y, 0)
		, MPFROMLONG( NULL));
 
  /****************************************************************************/
  /* Scroll the window the reqd amount, using bitblt'ing (ScrollWindow)       */
  /* if any of the screen still in view, and paint into uncovered region;     */
  /* else repaint the whole client area.				      */
  /****************************************************************************/
  hrgn = GpiCreateRegion( hpsClient, 0L, NULL);
  if( abs( ptsScrollPos.y - ptsOldScrollPos.y) <= ptsScrollMax.y)
  {
      WinScrollWindow( hwndClient
		     , 0
		     , ptsScrollPos.y - ptsOldScrollPos.y
		     , NULL
		     , NULL
		     , hrgn
		     , &rcl
		     , 0);
  } else
  {
      WinQueryWindowRect( hwndClient, &rcl);
      GpiSetRegion( hpsClient, hrgn, 1L, &rcl);
  }
  GpiQueryDefaultViewMatrix( hpsClient, 9L, &matlf );
  matlf.lM32 += ptsScrollPos.y - ptsOldScrollPos.y;
  GpiSetDefaultViewMatrix( hpsClient, 9L, &matlf, TRANSFORM_REPLACE);

  DoDraw( hrgn);
  ptsOldScrollPos.y = ptsScrollPos.y;
  GpiDestroyRegion( hpsClient, hrgn);
}
 
/******************************************************************************/
/*                                                                            */
/* Redraw the entire client window.					      */
/*                                                                            */
/******************************************************************************/
VOID
Redraw()
{
  RECTL   rclInvalid;
  HRGN	  hrgnUpdt;
 
  WinQueryWindowRect( hwndClient, &rclInvalid);
  hrgnUpdt = GpiCreateRegion( hpsClient, 1L, &rclInvalid);
  DoDraw( hrgnUpdt);
  GpiDestroyRegion( hpsClient, hrgnUpdt);
}


/******************************************************************************/
/*                                                                            */
/* toggle the fast-drag flag and update the menu check-box		      */
/*                                                                            */
/******************************************************************************/
VOID
ToggleFastDrag()
{
  MENUITEM mi;
  HWND	   hwndMenu, hwndOptions;

  hwndMenu = WinWindowFromID( hwndFrame, FID_MENU);
  WinSendMsg( hwndMenu
	    , MM_QUERYITEM
	    , MPFROM2SHORT( SM_OPTIONS, FALSE)
	    , MPFROMP( (PMENUITEM)&mi));
  hwndOptions = mi.hwndSubMenu;

  if( fFastDrag)
  {
    fFastDrag = FALSE;
    WinSendMsg( hwndOptions
	      , MM_SETITEMATTR
	      , MPFROM2SHORT( MENU_FASTDRAG, TRUE)
	      , MPFROM2SHORT( MIA_CHECKED, ~MIA_CHECKED) );
  }
  else
  {
    fFastDrag = TRUE;
    WinSendMsg( hwndOptions
	      , MM_SETITEMATTR
	      , MPFROM2SHORT( MENU_FASTDRAG, TRUE)
	      , MPFROM2SHORT( MIA_CHECKED, MIA_CHECKED) );
  }
}

/******************************************************************************/
/*                                                                            */
/* adjust zoom factor and recalc the picture transform, then do a redraw of   */
/* whole screen 							      */
/*                                                                            */
/******************************************************************************/
VOID
Zoom( sInOrOut)

SHORT sInOrOut;
{
  LONG	 lScaleOld;

  lScaleOld = lScale;
  lScale += sInOrOut;
  if( lScale > ZOOM_MAX)
    lScale = ZOOM_MAX;
  else
    if( lScale < -ZOOM_MAX)
      lScale = -ZOOM_MAX;
  if( lScale != lScaleOld)
  {
      ZoomMenuItems();
      CalcBounds();
      CalcTransform( hwndClient);
      Redraw();
  }
}
 
/******************************************************************************/
/*                                                                            */
/* enable/disable zoom menu items depending on scaling                        */
/*                                                                            */
/******************************************************************************/
VOID
ZoomMenuItems()
{
  MENUITEM  mi;
  HWND	    hwndMenu, hwndOptions;
 
  hwndMenu = WinWindowFromID( hwndFrame, FID_MENU);
  WinSendMsg( hwndMenu
	    , MM_QUERYITEM
	    , MPFROM2SHORT( SM_OPTIONS, FALSE)
	    , MPFROMP( (PMENUITEM)&mi));
  hwndOptions = mi.hwndSubMenu;
 
  if( lScale >= ZOOM_MAX)
  {
      WinSendMsg( hwndOptions
		, MM_SETITEMATTR
		, MPFROM2SHORT( MENU_ZOOMIN, TRUE)
		, MPFROM2SHORT( MIA_DISABLED, MIA_DISABLED));
      WinSendMsg( hwndOptions
		, MM_SETITEMATTR
		, MPFROM2SHORT( MENU_ZOOMOUT, TRUE)
		, MPFROM2SHORT( MIA_DISABLED, ~MIA_DISABLED));
  } else
  {
      if( lScale <= - ZOOM_MAX)
      {
	  WinSendMsg( hwndOptions
		    , MM_SETITEMATTR
		    , MPFROM2SHORT( MENU_ZOOMOUT, TRUE)
		    , MPFROM2SHORT( MIA_DISABLED, MIA_DISABLED));
	  WinSendMsg( hwndOptions
		    , MM_SETITEMATTR
		    , MPFROM2SHORT( MENU_ZOOMIN, TRUE)
		    , MPFROM2SHORT( MIA_DISABLED, ~MIA_DISABLED));
      } else
      {
	  WinSendMsg( hwndOptions
		    , MM_SETITEMATTR
		    , MPFROM2SHORT( MENU_ZOOMOUT, TRUE)
		    , MPFROM2SHORT( MIA_DISABLED, ~MIA_DISABLED));
	  WinSendMsg( hwndOptions
		    , MM_SETITEMATTR
		    , MPFROM2SHORT( MENU_ZOOMIN, TRUE)
		    , MPFROM2SHORT( MIA_DISABLED, ~MIA_DISABLED));
      }
  }
}
 
/******************************************************************************/
/*                                                                            */
/* Determine the bounding rect of a segment.				      */
/*                                                                            */
/******************************************************************************/
VOID
SetRect( psl)

PSEGLIST  psl;
{
  GpiResetBoundaryData( hpsClient);
  GpiSetDrawControl( hpsClient, DCTL_DISPLAY, DCTL_OFF);
  GpiSetDrawControl( hpsClient, DCTL_BOUNDARY, DCTL_ON);
  GpiDrawSegment( hpsClient, psl->lSegId);
  GpiSetDrawControl( hpsClient, DCTL_DISPLAY, DCTL_ON);
  GpiSetDrawControl( hpsClient, DCTL_BOUNDARY, DCTL_OFF);
  GpiQueryBoundaryData( hpsClient, &(psl->rclCurrent));
}
 
/******************************************************************************/
/*                                                                            */
/* Translate a segment							      */
/*                                                                            */
/******************************************************************************/
VOID
Translate( psl, pptlNew)

PSEGLIST  psl;
PPOINTL   pptlNew;
{
  POINTL    ptl;
  MATRIXLF  matlf;

  ptl = *pptlNew;
  GpiConvert( hpsClient, CVTC_DEVICE, CVTC_MODEL, 1L, &ptl);
  ptl.x = (ptl.x / 5) * 5;
  ptl.y = (ptl.y / 5) * 5;
  ptl.x -= 25;
  ptl.y -= 25;

  GpiQuerySegmentTransformMatrix( hpsClient
				, psl->lSegId
				, 9L
				, &matlf);
  matlf.lM31 = ptl.x - (psl->ptlLocation).x;
  matlf.lM32 = ptl.y - (psl->ptlLocation).y;
  GpiSetSegmentTransformMatrix( hpsClient
			      , psl->lSegId
			      , 9L
			      , &matlf
			      , TRANSFORM_REPLACE);
}
 
 
/******************************************************************************/
/*                                                                            */
/* set the default viewing transform					      */
/*                                                                            */
/******************************************************************************/
VOID
SetDVTransform( fx11, fx12, fx21, fx22, l31, l32, lType)

FIXED	fx11, fx12, fx21, fx22;
LONG	l31, l32, lType;
{
  MATRIXLF  matlf;

  matlf.fxM11 = fx11;
  matlf.fxM12 = fx12;
  matlf.lM13  = 0L;
  matlf.fxM21 = fx21;
  matlf.fxM22 = fx22;
  matlf.lM23  = 0L;
  matlf.lM31  = l31;
  matlf.lM32  = l32;
  matlf.lM33  = 1L;
  GpiSetDefaultViewMatrix( hpsClient, 9L, &matlf, lType);
}

/******************************************************************************/
/*                                                                            */
/* get bounding rect of whole picture in model coordinates		      */
/*                                                                            */
/******************************************************************************/
VOID
CalcBounds()
{
  PSEGLIST  psl;
  RECTL     rcl;

  if( !pslHead)
    return;
  rclBounds = pslHead->rclCurrent;
  for( psl = pslHead->pslNext; psl != NULL; psl = psl->pslNext)
  {
    rcl = psl->rclCurrent;
    if( rcl.xLeft < rclBounds.xLeft)
      rclBounds.xLeft = rcl.xLeft;
    if( rcl.xRight > rclBounds.xRight)
      rclBounds.xRight = rcl.xRight;
    if( rcl.yTop > rclBounds.yTop)
      rclBounds.yTop = rcl.yTop;
    if( rcl.yBottom < rclBounds.yBottom)
      rclBounds.yBottom = rcl.yBottom;
  }
}

/******************************************************************************/
/*                                                                            */
/* Calculate and set the default viewing transform based on zoom and scroll   */
/*                                                                            */
/******************************************************************************/
VOID
CalcTransform( hwnd)

HWND hwnd;
{
  RECTL     rclClient;
  POINTL    ptlCenter, ptlTrans, ptlScale, aptl[4];
  HRGN	    hrgn;
  PSEGLIST  psl;
 
  /****************************************************************************/
  /* from bounding rect of picture get center of picture		      */
  /****************************************************************************/
  ptlCenter.x = (rclBounds.xLeft   + rclBounds.xRight) / 2;
  ptlCenter.y = (rclBounds.yBottom + rclBounds.yTop  ) / 2;
 
  /****************************************************************************/
  /* translate center of picture to origin				      */
  /****************************************************************************/
  SetDVTransform( (FIXED)UNITY
		, (FIXED)0
		, (FIXED)0
		, (FIXED)UNITY
		, -ptlCenter.x
		, -ptlCenter.y
		, TRANSFORM_REPLACE);
 
  /****************************************************************************/
  /* scale down to 60% of max client area				      */
  /****************************************************************************/
  ptlScale.x = (6 * UNITY * sizlMaxClient.cx) /
	       (10 * (ptlTopRight.x - ptlBotLeft.x));
  ptlScale.y = (6 * UNITY * sizlMaxClient.cy) /
	       (10 * (ptlTopRight.y - ptlBotLeft.y));
 
  /****************************************************************************/
  /* add in zoom scale							      */
  /****************************************************************************/
  ptlScale.x += ptlScale.x * lScale / (ZOOM_MAX + 1);
  ptlScale.y += ptlScale.y * lScale / (ZOOM_MAX + 1);

  SetDVTransform( (FIXED)ptlScale.x
		, (FIXED)0
		, (FIXED)0
		, (FIXED)ptlScale.y
		, 0L
		, 0L
		, TRANSFORM_ADD);
 
  /****************************************************************************/
  /* translate center of picture to center of client window		      */
  /****************************************************************************/
  WinQueryWindowRect( hwnd, &rclClient);
  ptlTrans.x = (rclClient.xRight - rclClient.xLeft)   / 2;
  ptlTrans.y = (rclClient.yTop	 - rclClient.yBottom) / 2;
 
  /****************************************************************************/
  /* add in horizontal and vertical scrolling factors			      */
  /****************************************************************************/
  ptlTrans.x += ptsScrollPos.x - ptsHalfScrollMax.x;
  ptlTrans.y += ptsScrollPos.y - ptsHalfScrollMax.y;
  SetDVTransform( (FIXED)UNITY
		, (FIXED)0
		, (FIXED)0
		, (FIXED)UNITY
		, ptlTrans.x
		, ptlTrans.y
		, TRANSFORM_ADD);

  /****************************************************************************/
  /* create a shadow bitmap of the original, sized to the current output size */
  /****************************************************************************/
  aptl[0] = ptlBotLeft;
  aptl[1] = ptlTopRight;
  GpiConvert( hpsClient, CVTC_WORLD, CVTC_DEVICE, 2L, aptl);
  ptlOffset = aptl[0];

  aptl[0].x -= ptlOffset.x;
  aptl[0].y -= ptlOffset.y;
  aptl[1].x -= ptlOffset.x - 1;
  aptl[1].y -= ptlOffset.y - 1;
  aptl[2].x = 0L;
  aptl[2].y = 0L;
  aptl[3].x = bmpBitmapFile.cx;
  aptl[3].y = bmpBitmapFile.cy;
  GpiSetBitmap( hpsBitmapTemp, hbmBitmapTemp);
  GpiBitBlt( hpsBitmapTemp
	   , hpsBitmapFile
	   , 4L
	   , aptl
	   , ROP_SRCCOPY
	   , BBO_IGNORE);
  GpiSetBitmap( hpsBitmapTemp, NULL);

  /****************************************************************************/
  /* create a copy of the shadow bitmap, adjusted to appear normal when       */
  /* bitblt'd in XOR mode onto a CLR_BACKGROUND background (dynamic segment)  */
  /****************************************************************************/
  GpiSetBitmap( hpsBitmapDrag, hbmBitmapDrag);
  GpiSetColor( hpsBitmapDrag, CLR_BACKGROUND);
  hrgn = GpiCreateRegion( hpsBitmapDrag, 1L, (PRECTL)aptl);
  GpiPaintRegion( hpsBitmapDrag, hrgn);
  GpiDestroyRegion( hpsBitmapDrag, hrgn);
  GpiBitBlt( hpsBitmapDrag
	   , hpsBitmapFile
	   , 4L
	   , aptl
	   , ROP_SRCINVERT
	   , BBO_IGNORE);
  GpiSetBitmap( hpsBitmapDrag, NULL);

  for( psl = pslHead; psl != NULL; psl = psl->pslNext)
  {
    aptl[0].x = psl->rclBitBlt.xLeft;
    aptl[0].y = psl->rclBitBlt.yBottom;
    aptl[1].x = psl->rclBitBlt.xRight;
    aptl[1].y = psl->rclBitBlt.yTop;
    aptl[2] = aptl[0];
    aptl[3] = aptl[1];
    GpiConvert( hpsClient, CVTC_WORLD, CVTC_DEVICE, 2L, &aptl[2]);
    aptl[2].x -= ptlOffset.x;
    aptl[2].y -= ptlOffset.y;
    aptl[3].x -= ptlOffset.x - 1;
    aptl[3].y -= ptlOffset.y - 1;
    GpiSetEditMode( hpsClient, SEGEM_INSERT);
    GpiOpenSegment( hpsClient, psl->lSegId);
    GpiDeleteElementsBetweenLabels( hpsClient, BITBLT_TOP, BITBLT_BOTTOM);
    GpiWCBitBlt( hpsClient
	       , hbmBitmapTemp
	       , 4L
	       , aptl
	       , ROP_SRCCOPY
	       , BBO_IGNORE );
    GpiCloseSegment( hpsClient);
  }
}
 
 
/******************************************************************************/
/*                                                                            */
/* Draw the picture, using the passed region for clipping.		      */
/* Test each segment to see if its bounding box intersects the bounding box   */
/* of the clipping region.  Draw only if there is an intersection.	      */
/*                                                                            */
/******************************************************************************/
BOOL
DoDraw( hrgn)

HRGN	hrgn;
{
  HRGN	    hrgnOld;
  RECTL     rcl, rclRegion, rclDst;
  PSEGLIST  psl;

  GpiSetColor( hpsClient, CLR_BACKGROUND);
  GpiPaintRegion( hpsClient, hrgn);

  GpiQueryRegionBox( hpsClient, hrgn, &rclRegion);
  GpiSetClipRegion( hpsClient, hrgn, &hrgnOld);
  for( psl = pslHead; psl != NULL; psl = psl->pslNext)
  {
    rcl = psl->rclCurrent;
    GpiConvert( hpsClient, CVTC_MODEL, CVTC_DEVICE, 2L, (PPOINTL)&rcl);
    rcl.xRight++;
    rcl.yTop++;
    if( WinIntersectRect( habAsync, &rclDst, &rcl, &rclRegion))
	GpiDrawSegment( hpsClient, psl->lSegId);
  }
  GpiSetClipRegion( hpsClient, NULL, &hrgnOld);

  return( TRUE);
}
 
/******************************************************************************/
/*                                                                            */
/* Return a pointer to a segment list member, based on segment id.	      */
/*                                                                            */
/******************************************************************************/
PSEGLIST
SegListGet( lSeg)

LONG	   lSeg;
{
  PSEGLIST  psl;

  for( psl = pslHead; psl != NULL; psl = psl->pslNext)
    if( psl->lSegId == lSeg)
      return( psl);
  return( NULL);
}

/******************************************************************************/
/*                                                                            */
/* Check the segment list for obvious errors.				      */
/*									      */
/******************************************************************************/
BOOL
SegListCheck( iLoc)

INT   iLoc;
{
  PSEGLIST   psl;
  CHAR	     pszMsg[50];
  PSZ	     psz1, psz2;

  pszMsg[0] = '\0';
  for( psl = pslHead; psl != NULL; psl = psl->pslNext)
    if( (psl->lSegId < 1) || (psl->lSegId > lLastSegId) )
    {
      DosSemRequest( hsemSzFmt, SEM_INDEFINITE_WAIT);
      sprintf( szFmt, "Bad head segment list, location %d", iLoc);
      for( psz1 = szFmt, psz2 = pszMsg; *psz2++ = *psz1++; )
	  ;
      DosSemClear( hsemSzFmt);
      MyMessageBox( hwndClient, pszMsg);
      return( FALSE);
    }
  for( psl = pslTail; psl != NULL; psl = psl->pslPrev)
    if( (psl->lSegId < 1) || (psl->lSegId > lLastSegId) )
    {
      DosSemRequest( hsemSzFmt, SEM_INDEFINITE_WAIT);
      sprintf( szFmt, "Bad head segment list, location %d", iLoc);
      for( psz1 = szFmt, psz2 = pszMsg; *psz2++ = *psz1++; )
	  ;
      DosSemClear( hsemSzFmt);
      MyMessageBox( hwndClient, pszMsg);
      return( FALSE);
    }
  return( TRUE);
}
/******************************************************************************/
/*                                                                            */
/* Add (at head or tail) or delete a specified segment list member.	      */
/*                                                                            */
/******************************************************************************/
BOOL
SegListUpdate( usOperation, pslUpdate)

USHORT	 usOperation;
PSEGLIST pslUpdate;
{
  PSEGLIST psl;
  SEL	   sel;

  switch( usOperation)
  {
    case ADD_HEAD_SEG:
      DosAllocSeg( sizeof( SEGLIST), &sel, 0);
      if( pslHead == NULL)
      {
	pslHead = MAKEP( sel, 0);
	if( pslHead == NULL)
	  return( FALSE);
	*pslHead = *pslUpdate;
	pslHead->pslPrev = NULL;
	pslHead->pslNext = NULL;
	pslTail = pslHead;
      } else
      {
	psl = MAKEP( sel, 0);
	if( psl == NULL)
	  return( FALSE);
	*psl = *pslUpdate;
	pslHead->pslPrev = psl;
	psl->pslNext = pslHead;
	psl->pslPrev = NULL;
	pslHead = psl;
      }
      return( TRUE);
      break;

    case ADD_TAIL_SEG:
      DosAllocSeg( sizeof( SEGLIST), &sel, 0);
      if( pslTail == NULL)
      {
	pslHead = MAKEP( sel, 0);
	if( pslHead == NULL)
	  return( FALSE);
	*pslHead = *pslUpdate;
	pslHead->pslPrev = NULL;
	pslHead->pslNext = NULL;
	pslTail = pslHead;
      } else
      {
	psl = MAKEP( sel, 0);
	if( psl == NULL)
	  return( FALSE);
	*psl = *pslUpdate;
	pslTail->pslNext = psl;
	psl->pslPrev = pslTail;
	psl->pslNext = NULL;
	pslTail = psl;
      }
      return( TRUE);
      break;

    case DEL_SEG:
      for( psl = pslHead; psl != NULL; psl = psl->pslNext)
      {
	if( psl->lSegId == pslUpdate->lSegId)
	{
	  if( psl == pslHead)
	  {
	    pslHead = psl->pslNext;
	    if( pslHead == NULL)
	      pslTail = NULL;
	    else
	      pslHead->pslPrev = NULL;
	  }else if( psl == pslTail)
	  {
	    pslTail = psl->pslPrev;
	    pslTail->pslNext = NULL;
	  } else
	  {
	    (psl->pslPrev)->pslNext = psl->pslNext;
	    (psl->pslNext)->pslPrev = psl->pslPrev;
	  }
	  DosFreeSeg( SELECTOROF(psl));
	  return( TRUE);
	  break;
	}
      }
      return( FALSE);
      break;

    default:
      return( FALSE);
  }
}



/******************************************************************************/
/*                                                                            */
/* DumpPicture will free the list and segment store for the picture	      */
/*                                                                            */
/******************************************************************************/
BOOL
DumpPicture()
{
  while( pslHead != NULL )
    SegListUpdate( DEL_SEG, pslHead);
  GpiDeleteSegments( hpsClient, 1L, CALLSEG_BASE + lLastSegId);
  GpiSetBitmap( hpsBitmapFile, NULL);
  if( hbmBitmapFile)
      GpiDeleteBitmap( hbmBitmapFile);
  GpiSetBitmap( hpsBitmapTemp, NULL);
  if( hbmBitmapTemp)
      GpiDeleteBitmap( hbmBitmapTemp);
  GpiSetBitmap( hpsBitmapDrag, NULL);
  if( hbmBitmapDrag)
      GpiDeleteBitmap( hbmBitmapDrag);

  return( TRUE);
}
 
/******************************************************************************/
/*                                                                            */
/* Draw the picture into segment store. 				      */
/*                                                                            */
/******************************************************************************/
BOOL
CreatePicture()
{
 
  POINTL    ptl, aptlSides[12], aptlControl[12];
  SEGLIST   sl;
  LONG	    lCallSegId, l;

  /****************************************************************************/
  /* reset the default viewing transform to identity			      */
  /****************************************************************************/
  SetDVTransform( (FIXED)UNITY
		, (FIXED)0
		, (FIXED)0
		, (FIXED)UNITY
		, 0L
		, 0L
		, TRANSFORM_REPLACE);

  /****************************************************************************/
  /* set to store mode							      */
  /****************************************************************************/
  GpiSetDrawingMode( hpsClient, DM_RETAIN);
 
  /****************************************************************************/
  /* chaining and detectability off, fastchaining off			      */
  /****************************************************************************/
  GpiSetInitialSegmentAttrs( hpsClient, ATTR_CHAINED, ATTR_OFF);
  GpiSetInitialSegmentAttrs( hpsClient, ATTR_DETECTABLE, ATTR_OFF);
  GpiSetInitialSegmentAttrs( hpsClient, ATTR_FASTCHAIN, ATTR_OFF);
 
  /****************************************************************************/
  /* draw the pieces							      */
  /****************************************************************************/
  lLastSegId = 0;
  lCallSegId = CALLSEG_BASE;
  for( ptl.x = ptlBotLeft.x; ptl.x < ptlTopRight.x; ptl.x += 50)
  {
    for( ptl.y = ptlBotLeft.y; ptl.y < ptlTopRight.y; ptl.y += 50)
    {
      /************************************************************************/
      /* compute the piece outline control points			      */
      /************************************************************************/
      aptlControl[0].x = 10L;
      aptlControl[0].y = 10L;
      aptlControl[1].x = 40L;
      aptlControl[1].y = -10L;
      aptlControl[2].x = 50L;
      aptlControl[2].y = 0L;

      aptlControl[3].x = 40L;
      aptlControl[3].y = 10L;
      aptlControl[4].x = 60L;
      aptlControl[4].y = 40L;
      aptlControl[5].x = 50L;
      aptlControl[5].y = 50L;

      aptlControl[6].x = 40L;
      aptlControl[6].y = 40L;
      aptlControl[7].x = 10L;
      aptlControl[7].y = 60L;
      aptlControl[8].x = 0L;
      aptlControl[8].y = 50L;

      aptlControl[9].x	= 10L;
      aptlControl[9].y	= 40L;
      aptlControl[10].x = -10L;
      aptlControl[10].y = 10L;
      aptlControl[11].x = 0L;
      aptlControl[11].y = 0L;

      if( ptl.y == ptlBotLeft.y)
      {
	aptlControl[0].y = 0L;
	aptlControl[1].y = 0L;
      }

      if( (ptl.x + 50) == ptlTopRight.x)
      {
	aptlControl[3].x = 50L;
	aptlControl[4].x = 50L;
      }

      if( (ptl.y + 50) == ptlTopRight.y)
      {
	aptlControl[6].y = 50L;
	aptlControl[7].y = 50L;
      }

      if( ptl.x == ptlBotLeft.x)
      {
	aptlControl[ 9].x = 0L;
	aptlControl[10].x = 0L;
      }

      for( l=0; l<12; l++)
      {
	aptlSides[l].x = ptl.x + aptlControl[l].x;
	aptlSides[l].y = ptl.y + aptlControl[l].y;
      }

      GpiOpenSegment( hpsClient, ++lCallSegId);
      GpiMove( hpsClient, &ptl);
      GpiPolyLine( hpsClient, 12L, aptlSides);
      GpiCloseSegment( hpsClient);

      /************************************************************************/
      /* draw the root segment						      */
      /************************************************************************/
      GpiOpenSegment( hpsClient, ++lLastSegId);
      GpiSetTag( hpsClient, lLastSegId);

      /************************************************************************/
      /* store the piece location					      */
      /************************************************************************/
      sl.ptlLocation = ptl;

      /************************************************************************/
      /* compute the dimensions of the matching rects for BitBlt	      */
      /************************************************************************/
      sl.rclBitBlt.xLeft   = ptl.x - 10;
      sl.rclBitBlt.yBottom = ptl.y - 10;
      sl.rclBitBlt.xRight  = ptl.x + 60;
      sl.rclBitBlt.yTop    = ptl.y + 60;
      if( ptl.x == ptlBotLeft.x)
	sl.rclBitBlt.xLeft += 10;
      if( ptl.y == ptlBotLeft.y)
	sl.rclBitBlt.yBottom += 10;
      if( (ptl.x + 50) == ptlTopRight.x)
	sl.rclBitBlt.xRight -= 10;
      if( (ptl.y + 50) == ptlTopRight.y)
	sl.rclBitBlt.yTop -= 10;

      /************************************************************************/
      /* draw one piece 						      */
      /************************************************************************/
      GpiBeginPath( hpsClient, 1L);
      GpiMove( hpsClient, &ptl);
      GpiPolyLine( hpsClient, 12L, aptlSides);
      GpiEndPath( hpsClient);
      GpiSetColor( hpsClient, CLR_BLACK);
      GpiLabel( hpsClient, FILLPATH);

      GpiSetClipPath( hpsClient, 0L, SCP_RESET);
      GpiBeginPath( hpsClient, 1L);
      GpiMove( hpsClient, &ptl);
      GpiPolyLine( hpsClient, 12L, aptlSides);
      GpiEndPath( hpsClient);
      GpiSetClipPath( hpsClient, 1L, SCP_AND);
      GpiLabel( hpsClient, BITBLT_TOP);
      GpiLabel( hpsClient, BITBLT_BOTTOM);

      GpiSetClipPath( hpsClient, 0L, SCP_RESET);
      GpiSetColor( hpsClient, CLR_RED);
      GpiMove( hpsClient, &ptl);
      GpiPolyLine( hpsClient, 12L, aptlSides);

      GpiCloseSegment( hpsClient);
      GpiSetSegmentAttrs( hpsClient, lLastSegId, ATTR_CHAINED, ATTR_ON);
      GpiSetSegmentAttrs( hpsClient, lLastSegId, ATTR_DETECTABLE, ATTR_ON);

      sl.lSegId = lLastSegId;
      sl.pslNext = NULL;
      sl.pslPrev = NULL;
      SetRect( &sl);
      SegListUpdate( ADD_TAIL_SEG, &sl);
    }
  }
  return( TRUE);
}
 
/******************************************************************************/
/*                                                                            */
/* Create the Temp and Drag bitmaps.					      */
/*                                                                            */
/******************************************************************************/
BOOL
PrepareBitmap()
{
  bmpBitmapTemp    = bmpBitmapFile;
  bmpBitmapTemp.cx = LOUSHORT( (sizlMaxClient.cx * 6L) / 5L);
  bmpBitmapTemp.cy = LOUSHORT( (sizlMaxClient.cy * 6L) / 5L);
  hbmBitmapTemp    = GpiCreateBitmap( hpsBitmapTemp
				    , &bmpBitmapTemp
				    , 0L
				    , NULL
				    , NULL);
  if( !hbmBitmapTemp)
    return( FALSE);

  bmpBitmapDrag    = bmpBitmapFile;
  bmpBitmapDrag.cx = LOUSHORT( (sizlMaxClient.cx * 6L) / 5L);
  bmpBitmapDrag.cy = LOUSHORT( (sizlMaxClient.cy * 6L) / 5L);
  hbmBitmapDrag    = GpiCreateBitmap( hpsBitmapDrag
				    , &bmpBitmapDrag
				    , 0L
				    , NULL
				    , NULL);
  if( !hbmBitmapDrag)
    return( FALSE);
  return( TRUE);
}

/******************************************************************************/
/*                                                                            */
/* Create a memory DC and an associated PS.				      */
/*                                                                            */
/******************************************************************************/
BOOL
CreateBitmapHdcHps( phdc, phps)

PHDC  phdc;
PHPS  phps;
{
  SIZEL    sizl;
  HDC	   hdc;
  HPS	   hps;

  hdc = DevOpenDC( habMain, OD_MEMORY, "*", 3L, (PDEVOPENDATA)&dop, NULL);
  if( !hdc)
    return( FALSE);

  sizl.cx = sizl.cy = 0L;
  hps = GpiCreatePS( habMain
		   , hdc
		   , &sizl
		   , PU_PELS | GPIA_ASSOC );
  if( !hps)
    return( FALSE);

  *phdc = hdc;
  *phps = hps;
  return( TRUE);
}

/******************************************************************************/
/*									      */
/* Get the bitmap from disk.						      */
/* Note that there are 2 formats for bitmap files, one of which is archaic.   */
/* Both formats are supported here.  All new bitmaps should follow the format */
/* in BITMAPFILEHEADER. 						      */
/*									      */
/******************************************************************************/
BOOL
ReadBitmap( hfile)

HFILE  hfile;
{
    ULONG cScans;
    ULONG ulSize;	 /* Number of bytes occupied by bitmap bits.	      */
    USHORT cSegs;	 /* Number of 64K segments in ulSize.		      */
    USHORT cbExtra;	 /* Bytes in last segment of ulSize.		      */
    SEL sel;		 /* Base selector to file data. 		      */
    USHORT hugeshift;	 /* Segment index shift value.			      */
    USHORT cbRead1;	 /* Number of bytes to read first call to DosRead     */
    USHORT cbRead2;	 /* Number of bytes to read second call to DosRead    */
    USHORT cbRead;	 /* Number of bytes read by DosRead.		      */
    BOOL fRet = FALSE;	 /* Function return code.			      */
    INT  i;		 /* Generic loop index. 			      */
    FILESTATUS fsts;
    PBITMAPFILEHEADER pbfh;
    PRCBITMAP  rb;
    PBYTE pImage;


    /**************************************************************************/
    /* Find out how big the file is so we can read the whole thing in.	      */
    /**************************************************************************/

    if( DosQFileInfo( hfile, 1, &fsts, sizeof(FILESTATUS)) != 0)
	goto ReadBitmap_close_file;

    ulSize  = fsts.cbFile;
    cSegs   = (USHORT)(ulSize/0x10000L);
    cbExtra = (USHORT)(ulSize%0x10000L);
    if (DosAllocHuge(cSegs, cbExtra, (PSEL)&sel, 0, 0))
	goto ReadBitmap_close_file;
    if (DosGetHugeShift( &hugeshift))
	goto ReadBitmap_free_bits;

    pImage = (PBYTE)MAKEP(sel, 0);
    rb	   = (PRCBITMAP)pImage;
    pbfh   = (PBITMAPFILEHEADER)pImage;


    /**************************************************************************/
    /* Read the bits in from the file. The DosRead function allows a	      */
    /* maximum of 64K-1 bytes read at a time.  We get around this	      */
    /* by reading two 32K chunks for each 64K segment, and reading the	      */
    /* last segment in one piece.					      */
    /**************************************************************************/

    for (i = 0; i <= cSegs; ++i)
    {
	if (i < cSegs)
	{
	    /* This segment is 64K bytes long, so split it up. */
	    cbRead1 = 0x8000;
	    cbRead2 = 0x8000;
	}
	else
	{
	    /* This segment is less than 64K bytes long, so read it all. */
	    cbRead1 = cbExtra;
	    cbRead2 = 0;
	}

	/* There's a possibility that cbExtra will be 0, so check
	 * to avoid an unnecessary system call.
	 */
	if (cbRead1 > 0)
	{
	    if (DosRead( hfile
		       , (PVOID)MAKEP(sel+(i<<hugeshift), 0)
		       , cbRead1
		       , &cbRead))
		goto ReadBitmap_free_bits;
	    if (cbRead1 != cbRead)
		goto ReadBitmap_free_bits;
	}

	/* This will always be skipped on the last partial segment. */
	if (cbRead2 > 0)
	{
	    if (DosRead( hfile
		       , (PVOID)MAKEP(sel+(i<<hugeshift), cbRead1)
		       , cbRead2
		       , &cbRead))
		goto ReadBitmap_free_bits;
	    if (cbRead2 != cbRead)
		goto ReadBitmap_free_bits;
	}
    }


    /**************************************************************************/
    /* Tell GPI to put the bits into the thread's PS. The function returns    */
    /* the number of scan lines of the bitmap that were copied.  We want      */
    /* all of them at once.						      */
    /**************************************************************************/

    if (pbfh->bmp.cbFix != sizeof(BITMAPINFOHEADER))
    {
	bmpBitmapFile.cx	= rb->bmWidth;
	bmpBitmapFile.cy	= rb->bmHeight;
	bmpBitmapFile.cPlanes	= rb->bmPlanes;
	bmpBitmapFile.cBitCount = rb->bmBitcount;
	hbmBitmapFile = GpiCreateBitmap( hpsBitmapFile
				       , &bmpBitmapFile
				       , 0L
				       , NULL
				       , NULL);
	if( !hbmBitmapFile)
	    goto ReadBitmap_free_bits;
	GpiSetBitmap( hpsBitmapFile, hbmBitmapFile);

        pImage += rb->dwBitsOffset;
        rb->dwBitsOffset = sizeof(BITMAPINFOHEADER);
	cScans = GpiSetBitmapBits( hpsBitmapFile
				 , 0L
				 , (LONG)rb->bmHeight
				 , pImage
				 , (PBITMAPINFO)&(rb->dwBitsOffset));
	if (cScans != (LONG)rb->bmHeight)  /* original number of scans ? */
	    goto ReadBitmap_free_bits;
    }
    else
    {
	bmpBitmapFile.cx	= pbfh->bmp.cx;
	bmpBitmapFile.cy	= pbfh->bmp.cy;
	bmpBitmapFile.cPlanes	= pbfh->bmp.cPlanes;
	bmpBitmapFile.cBitCount = pbfh->bmp.cBitCount;
	hbmBitmapFile = GpiCreateBitmap( hpsBitmapFile
				       , &bmpBitmapFile
				       , 0L
				       , NULL
				       , NULL);
	if( !hbmBitmapFile)
	    goto ReadBitmap_free_bits;
	GpiSetBitmap( hpsBitmapFile, hbmBitmapFile);

	cScans = GpiSetBitmapBits( hpsBitmapFile
				 , 0L
				 , (LONG)pbfh->bmp.cy
				 , pImage + pbfh->offBits
				 , (PBITMAPINFO)&(pbfh->bmp));
	if (cScans != (LONG)pbfh->bmp.cy)  /* original number of scans ? */
	    goto ReadBitmap_free_bits;
    }

    fRet = TRUE;     /* okey-dokey */


    /**************************************************************************/
    /* Close the file, free the buffer space and leave.  This is a	      */
    /* common exit point from the function.  Since the same cleanup	      */
    /* operations need to be performed for such a large number of	      */
    /* possible error conditions, this is concise way to do the right	      */
    /* thing.								      */
    /**************************************************************************/

ReadBitmap_free_bits:
    DosFreeSeg( sel);
ReadBitmap_close_file:
    DosClose( hfile);
    return fRet;
}
