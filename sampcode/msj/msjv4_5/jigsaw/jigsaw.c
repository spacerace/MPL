/********************************** Jigsaw ************************************/
/*                                                                            */
/* Created 1988, Microsoft Corporation. 				      */
/*                                                                            */
/* Purpose:  To illustrate the use of Gpi.				      */
/*                                                                            */
/* Summary:  This program provides a jigsaw puzzle, based on a decomposition  */
/*   of an arbitrary bitmap loaded from a file.  The user can jumble the      */
/*   pieces, then drag them individually by means of the mouse.  The image    */
/*   can be zoomed in and out and scrolled up/down and left/right.	      */
/*                                                                            */
/*   JIGSAW uses BitBlt with clip paths to create a collection of picture     */
/*   fragments which are the puzzle pieces.  Each piece is associated with    */
/*   with a single retained segment and an auxiliary data structure, used     */
/*   for drawing and for selection with the mouse.  To retain responsiveness  */
/*   to user requests, the real work is done in a second thread, with	      */
/*   work requests transmitted from the main thread in the form of	      */
/*   messages.	This arrangement makes it possible for the user to over-      */
/*   ride lengthy drawing operations with a higher-priority request	      */
/*   (eg. program termination, magnification change, etc.).		      */
/*                                                                            */
/*   Individual pieces are made to "move" by changing their model transforms. */
/*   Scrolling and zooming of the whole picture is done by changing the       */
/*   default viewing transform. 					      */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

#define INCL_BITMAPFILEFORMAT
 
#define INCL_DOSINFOSEG
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
#define INCL_GPIPATHS
#define INCL_GPIREGIONS
#define INCL_GPISEGMENTS
#define INCL_GPICORRELATION
#define INCL_GPILCIDS
 
#define INCL_ERRORS
 
#include <os2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <opendlg.h>
#include "jigsaw.h"
 


/*----------------------- helper macros --------------------------------------*/

#define ROUND_DOWN_MOD( arg, mod)	    \
    {					    \
	if( arg < 0)			    \
	{				    \
	    arg -= mod - 1;		    \
	    arg += arg % mod;		    \
	} else				    \
	    arg -= arg % mod;		    \
    }

#define ROUND_UP_MOD( arg, mod) 	    \
    {					    \
	if( arg < 0)			    \
	{				    \
	    arg -= arg % mod;		    \
	} else				    \
	{				    \
	    arg += mod - 1;		    \
	    arg -= arg % mod;		    \
	}				    \
    }

 
 
/*----------------------- inter-thread messages ------------------------------*/
 
#define UM_DIE	      WM_USER+1        /* instruct async thread to terminate  */
#define UM_DRAW       WM_USER+2        /* draw the current picture	      */
#define UM_VSCROLL    WM_USER+3        /* perform scroll by recalculating the */
                                       /* default viewing transform           */
#define UM_HSCROLL    WM_USER+4        /* perform scroll by recalculating the */
                                       /* default viewing transform           */
#define UM_SIZING     WM_USER+5        /* perform sizing by recalculating the */
                                       /* default viewing transform           */
#define UM_ZOOM       WM_USER+6        /* zoom the picture by recalculating   */
                                       /* the default viewing transform       */
#define UM_REDRAW     WM_USER+8        /* redraw the entire client window     */
#define UM_JUMBLE     WM_USER+9        /* scatter the pieces on the window    */
#define UM_LOAD       WM_USER+10       /* load a bitmap from disk	      */
#define UM_LEFTDOWN   WM_USER+11       /* correlate and prepare to drag       */
#define UM_MOUSEMOVE  WM_USER+12       /* remove, reposition, and redraw      */
#define UM_LEFTUP     WM_USER+13       /* stop dragging 		      */
#define UM_LAST       WM_USER+15
 
 
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
POINTS	ptsScrollLine;
POINTS	ptsScrollPage;
 
#define UNITY	       65536L
MATRIXLF matlfIdentity = { UNITY, 0, 0, 0, UNITY, 0, 0, 0, 1 };
LONG	lScale; 		       /* current zoom level		      */
POINTL	ptlScaleRef;		       /* scale factor, detects size change   */
#define ZOOM_MAX    8
#define ZOOM_IN     1
#define ZOOM_OUT   -1
 
POINTL	ptlOffset;
POINTL	ptlBotLeft  = { 0, 0};
POINTL	ptlTopRight = { 3000, 3000};
POINTL	ptlMoveStart;		       /* model space point at start of move  */
LONG	lLastSegId;		       /* last segment id assigned to a piece */
LONG	lPickedSeg = 0L;	       /* seg id of piece selected for drag   */
POINTL	ptlOffStart;		       /* segment xform xlate at move start   */
RECTL	rclBounds;		       /* pict bounding box in model coords.  */
POINTL	ptlOldMouse = {0L, 0L};        /* current mouse posn		      */
BOOL	fButtonDownMain = FALSE;       /* only drag if mouse down	      */
BOOL	fButtonDownAsync = FALSE;      /* only drag if mouse down	      */

POINTL	ptlUpdtRef;
POINTL	aptlUpdt[3];
BOOL	fUpdtFirst;
BOOL	fFirstLoad = TRUE;

/*-------------------------- segment list ------------------------------------*/

typedef struct _SEGLIST {	       /* sl				      */
  LONG			lSegId;
  struct _SEGLIST FAR * pslPrev;
  struct _SEGLIST FAR * pslNext;
  POINTL		ptlLocation;   /* piece location, world coordinates   */
  RECTL 		rclCurrent;    /* segment bounding box, model coords  */
  RECTL 		rclBitBlt;     /* segment bounding box, world coords  */
  POINTL		aptlBitBlt[4]; /* BitBlt parameters		      */
  LONG			lAdjacent[8];  /* adjacent segments in original pict  */
  struct _SEGLIST FAR * pslNextIsland; /* next member in island list	      */
  BOOL			fIslandMark;   /* used for marking island members     */
  POINTL		aptlSides[12]; /* control points for piece outline    */
  HDC			hdcHole;       /* used to punch hole in target	      */
  HPS			hpsHole;
  HBITMAP		hbmHole;
  HDC			hdcFill;       /* used to fill hole		      */
  HPS			hpsFill;
  HBITMAP		hbmFill;
  POINTL		ptlModelXlate; /* lM31 and lM31 for piece	      */
} SEGLIST ;
typedef SEGLIST FAR *PSEGLIST;	       /* psl				      */
typedef PSEGLIST FAR *PPSEGLIST;       /* ppsl				      */
PSEGLIST pslHead = NULL;	       /* head of the list		      */
PSEGLIST pslTail = NULL;	       /* tail of the list		      */
PSEGLIST pslPicked = NULL;	       /* picked segment's list member        */
#define   ADD_HEAD_SEG	 1
#define   ADD_TAIL_SEG	 2
#define        DEL_SEG	 3
#define  MAKE_TAIL_SEG	 4
 
/*-------------------------- bitmap-related data -----------------------------*/

typedef struct _LOADINFO {	       /* li				      */
    HFILE   hf;
    CHAR    szFileName[MAX_FNAME_LEN];
} LOADINFO ;
typedef LOADINFO FAR *PLOADINFO;       /* pli				      */

HPS		   hpsBitmapFile=NULL; /* bitmap straight from the file       */
HDC		   hdcBitmapFile=NULL;
HBITMAP 	   hbmBitmapFile=NULL;
BITMAPINFOHEADER   bmpBitmapFile   = {12L, 0, 0, 0, 0};

HPS		   hpsBitmapSize=NULL; /* bitmap sized to the current size    */
HDC		   hdcBitmapSize=NULL;
HBITMAP 	   hbmBitmapSize=NULL;

HPS		   hpsBitmapBuff=NULL; /* image composed here, copied to scrn */
HDC		   hdcBitmapBuff=NULL;
HBITMAP 	   hbmBitmapBuff=NULL;

HPS		   hpsBitmapSave=NULL; /* save part of screen during dragging */
HDC		   hdcBitmapSave=NULL;
HBITMAP 	   hbmBitmapSave=NULL;
BITMAPINFOHEADER   bmpBitmapSave   = {12L, 0, 0, 0, 0};

DEVOPENSTRUC dop = { NULL
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

ULONG	ulDrawOn   = 0; 	       /* set iff draw enabled		      */
HSEM	hsemDrawOn = &ulDrawOn;

PSZ	pszLoadMsg  = "Patience, preparing puzzle pieces ...";
ULONG	ulLoadMsg   = 0;	       /* set iff load in progress	      */
HSEM	hsemLoadMsg = &ulLoadMsg;

SWCNTRL swctl = { 0, 0, 0, 0, 0, SWL_VISIBLE, SWL_JUMPABLE, NULL, 0 };
HSWITCH hsw;			       /* handle to a switch list entry       */
char	szTitle[80];		       /* Title bar text		      */

BOOL	fErrMem = FALSE;	       /* set if alloc async stack fails      */
 
LONG	lByteAlignX, lByteAlignY;      /* memory alignment constants	      */
 
/*------------------------- Function Prototypes ------------------------------*/

VOID	 CalcBounds( VOID);
VOID	 CalcSize( MPARAM, MPARAM);
VOID	 CalcTransform( HWND);
MRESULT CALLBACK ClientWndProc( HWND, USHORT, MPARAM, MPARAM);
BOOL	 CreateBitmapHdcHps( HDC, HPS);
BOOL	 CreateThread( VOID);
BOOL	 CreatePicture( VOID);
BOOL	 DoDraw( HPS, HRGN, BOOL);
VOID	 DoHorzScroll( VOID);
VOID	 DoVertScroll( VOID);
VOID	 DrawPiece( HPS, PSEGLIST, BOOL);
BOOL	 DumpPicture( VOID);
VOID	 Finalize( VOID);
BOOL	 Initialize( VOID);
VOID	 Jumble( VOID);
VOID	 LeftDown( MPARAM);
VOID	 LeftUp( VOID);
VOID	 Load( PLOADINFO);
VOID cdecl main( VOID);
VOID	 MarkIsland( PSEGLIST, BOOL);
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
PSEGLIST SegListUpdate( USHORT, PSEGLIST);
BOOL	 SendCommand( USHORT, MPARAM, MPARAM);
VOID	 SetDVTransform( FIXED, FIXED, FIXED, FIXED, LONG, LONG, LONG);
VOID	 SetRect( PSEGLIST);
VOID	 ShowMessage( HPS, PSZ);
VOID	 ToggleMenuItem( USHORT, USHORT, PBOOL);
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

  ZoomMenuItems();

  if( !CreateThread())		      /* create async thread		     */
      return ( FALSE);
  if( !CreateBitmapHdcHps( &hdcBitmapFile, &hpsBitmapFile))
      return( FALSE);
  if( !CreateBitmapHdcHps( &hdcBitmapSize, &hpsBitmapSize))
      return( FALSE);
  if( !CreateBitmapHdcHps( &hdcBitmapBuff, &hpsBitmapBuff))
      return( FALSE);
  if( !CreateBitmapHdcHps( &hdcBitmapSave, &hpsBitmapSave))
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
  if( tidAsync)
  {
      DosSemClear( hsemDrawOn);
      DosSemSet( hsemTerminate);
  }

  while( pslHead != NULL )
  {
    GpiSetBitmap( pslHead->hpsFill, NULL);
    GpiDeleteBitmap( pslHead->hbmFill);
    GpiDestroyPS( pslHead->hpsFill);
    DevCloseDC( pslHead->hdcFill);

    GpiSetBitmap( pslHead->hpsHole, NULL);
    GpiDeleteBitmap( pslHead->hbmHole);
    GpiDestroyPS( pslHead->hpsHole);
    DevCloseDC( pslHead->hdcHole);

    SegListUpdate( DEL_SEG, pslHead);
  }
  GpiDeleteSegments( hpsClient, 1L, lLastSegId);

  if( hrgnInvalid)
      GpiDestroyRegion( hpsClient, hrgnInvalid);
  if( hpsClient)
  {
      GpiAssociate( hpsClient, NULL);
      GpiDestroyPS( hpsClient);
  }
  if( hpsPaint)
      GpiDestroyPS( hpsPaint);

  if( hpsBitmapFile)
  {
      GpiSetBitmap( hpsBitmapFile, NULL);
      GpiDeleteBitmap( hbmBitmapFile);
      GpiDestroyPS( hpsBitmapFile);
      DevCloseDC( hdcBitmapFile);
  }
  if( hpsBitmapSize)
  {
      GpiSetBitmap( hpsBitmapSize, NULL);
      GpiDeleteBitmap( hbmBitmapSize);
      GpiDestroyPS( hpsBitmapSize);
      DevCloseDC( hdcBitmapSize);
  }
  if( hpsBitmapBuff)
  {
      GpiSetBitmap( hpsBitmapBuff, NULL);
      GpiDeleteBitmap( hbmBitmapBuff);
      GpiDestroyPS( hpsBitmapBuff);
      DevCloseDC( hdcBitmapBuff);
  }
  if( hpsBitmapSave)
  {
      GpiSetBitmap( hpsBitmapSave, NULL);
      GpiDeleteBitmap( hbmBitmapSave);
      GpiDestroyPS( hpsBitmapSave);
      DevCloseDC( hdcBitmapSave);
  }

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
/* SendCommand	will attempt to post the required command and parameters to   */
/* the asynchronous drawing thread's message queue. The command will only     */
/* be posted if the queue exists.					      */
/*                                                                            */
/******************************************************************************/
BOOL
SendCommand( usCommand, mp1, mp2)

USHORT	usCommand;
MPARAM	mp1, mp2;
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
    case UM_ZOOM:
    case UM_REDRAW:
    case UM_SIZING:
    case UM_JUMBLE:
    case UM_LOAD:
 
	return( WinPostQueueMsg( hmqAsync
			       , usCommand
			       , mp1
			       , mp2 ) );
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
      return( TRUE);
      break;
 
    /**************************************************************************/
    /*									      */
    /**************************************************************************/
    case WM_MINMAXFRAME:
      if( (((PSWP)mp1)->fs & SWP_RESTORE)  ||
	  (((PSWP)mp1)->fs & SWP_MAXIMIZE) )
	{
	DosSemClear( hsemDrawOn);
	SendCommand( UM_SIZING, 0, 0);
	}
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
      SendCommand( UM_HSCROLL, mp2, 0);
      break;
 
    case WM_VSCROLL:
      SendCommand( UM_VSCROLL, mp2, 0);
      break;
 
    /************************************************************************/
    /* The client area is being resized.                                    */
    /************************************************************************/
    case WM_SIZE:
      DosSemClear( hsemDrawOn);
      SendCommand( UM_SIZING, mp1, mp2);
      break;
 
    /**************************************************************************/
    /* Mouse commands are handled by the async thread. Simply send on the     */
    /* command and parameters.                                                */
    /**************************************************************************/
    case WM_BUTTON1DBLCLK:
    case WM_BUTTON1DOWN:
      if( hwnd != WinQueryFocus( HWND_DESKTOP, FALSE))
	  WinSetFocus( HWND_DESKTOP, hwnd);
      if( !fButtonDownMain)
      {
	  fButtonDownMain = TRUE;
	  SendCommand( UM_LEFTDOWN, mp1, 0);
      }
      return( TRUE);
      break;
 
    case WM_BUTTON1UP:
      if( !fButtonDownMain)
	  return( TRUE);
      if( SendCommand( UM_LEFTUP, mp1, 0))
	  fButtonDownMain = FALSE;
      else
	  WinAlarm( HWND_DESKTOP, WA_WARNING);
      return( TRUE);
      break;
 
    case WM_MOUSEMOVE:
      if( fButtonDownMain)
	  SendCommand( UM_MOUSEMOVE, mp1, 0);
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

  lByteAlignX = WinQuerySysValue( HWND_DESKTOP, SV_CXBYTEALIGN);
  lByteAlignY = WinQuerySysValue( HWND_DESKTOP, SV_CYBYTEALIGN);

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
 
 
/******************************************************************************/
/*									      */
/* WM_PAINT message							      */
/*									      */
/******************************************************************************/
MRESULT
WndProcPaint()

{
  HRGN	 hrgnUpdt;
  SHORT  sRgnType;
 
  hrgnUpdt = GpiCreateRegion( hpsPaint, 0L, NULL);
  sRgnType = WinQueryUpdateRegion( hwndClient, hrgnUpdt);
  SendCommand( UM_DRAW, (MPARAM)hrgnUpdt, 0);
  if( DosSemWait( hsemLoadMsg, SEM_IMMEDIATE_RETURN))
    ShowMessage( hpsClient, pszLoadMsg);
  WinValidateRegion( hwndClient, hrgnUpdt, FALSE);
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
	DosSemClear( hsemDrawOn);
	SendCommand( UM_JUMBLE, 0, 0);
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
	      DosSemClear( hsemDrawOn);
	      SendCommand( UM_LOAD, (MPARAM)pli, 0);
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
      DosSemClear( hsemDrawOn);
      SendCommand( UM_ZOOM, (MPARAM)ZOOM_IN, 0);
      break;
 
    case MENU_ZOOMOUT:
      DosSemClear( hsemDrawOn);
      SendCommand( UM_ZOOM, (MPARAM)ZOOM_OUT , 0);
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
/* Put a message on the screen						      */
/******************************************************************************/
VOID
ShowMessage( hps, psz)

HPS  hps;
PSZ  psz;
{
  HRGN	 hrgn;
  RECTL  rclClient;
  POINTL ptl;

  WinQueryWindowRect( hwndClient, &rclClient);
  hrgn = GpiCreateRegion( hps, 1L, &rclClient);
  GpiSetColor( hps, CLR_BACKGROUND);
  GpiPaintRegion( hps, hrgn);
  GpiDestroyRegion( hps, hrgn);

  GpiSetColor( hps, CLR_NEUTRAL);
  ptl.x = 0L;
  ptl.y = 0L;

  GpiMove( hps, (PPOINTL)&ptl);
  GpiCharString( hps, (LONG)strlen( psz), psz);
}

/******************************************************************************/
/* Load a bitmap							      */
/******************************************************************************/
VOID
Load( pli)

PLOADINFO  pli;
{
    PSZ       pszError;
    RECTL     rclClient;

    pszError = (PSZ)"Error reading file.";

    ShowMessage( hpsClient, pszLoadMsg);
    DosSemSet( hsemLoadMsg);

    if( !fFirstLoad)
      DumpPicture();

    if( !ReadBitmap( pli->hf) )
    {
      MyMessageBox( hwndClient, pszError);
      DosSemClear( hsemLoadMsg);
      return;
    }
    if( !PrepareBitmap() )
    {
      MyMessageBox( hwndClient, pszError);
      DosSemClear( hsemLoadMsg);
      return;
    }

    lstrcpy( swctl.szSwtitle, szTitle);
    lstrcat( swctl.szSwtitle, ": ");
    lstrcat( swctl.szSwtitle, pli->szFileName);
    WinChangeSwitchEntry( hsw, &swctl);
    WinSetWindowText( hwndFrame, swctl.szSwtitle);

    WinQueryWindowRect( hwndClient, &rclClient);
    ptsScrollMax.x = (SHORT)(rclClient.xRight - rclClient.xLeft);
    ptsHalfScrollMax.x = ptsScrollMax.x >> 1;
    ptsScrollPage.x = ptsScrollMax.x >> 3;
    ROUND_DOWN_MOD( ptsScrollPage.x, (SHORT)lByteAlignX);
    ptsScrollLine.x = ptsScrollMax.x >> 5;
    ROUND_DOWN_MOD( ptsScrollLine.x, (SHORT)lByteAlignX);
    ptsScrollPos.x = ptsHalfScrollMax.x;
    ptsOldScrollPos.x = ptsHalfScrollMax.x;
    WinSendMsg( hwndHorzScroll
	      , SBM_SETSCROLLBAR
	      , MPFROMSHORT( ptsScrollPos.x)
	      , MPFROM2SHORT( 1, ptsScrollMax.x) );
    ptsScrollMax.y = (SHORT)(rclClient.yTop - rclClient.yBottom);
    ptsHalfScrollMax.y = ptsScrollMax.y >> 1;
    ptsScrollPage.y = ptsScrollMax.y >> 3;
    ROUND_DOWN_MOD( ptsScrollPage.y, (SHORT)lByteAlignY);
    ptsScrollLine.y = ptsScrollMax.y >> 5;
    ROUND_DOWN_MOD( ptsScrollLine.y, (SHORT)lByteAlignY);
    ptsScrollPos.y = ptsHalfScrollMax.y;
    ptsOldScrollPos.y = ptsHalfScrollMax.y;
    WinSendMsg( hwndVertScroll
	      , SBM_SETSCROLLBAR
	      , MPFROMSHORT( ptsScrollPos.y)
	      , MPFROM2SHORT( 1, ptsScrollMax.y) );

    CreatePicture();
    lScale = 0;

    CalcBounds();
    ptlScaleRef.x = ptlScaleRef.y = 0L;
    CalcTransform( hwndClient);
    DosFreeSeg( SELECTOROF( pli));
    fFirstLoad = FALSE;
    DosSemClear( hsemLoadMsg);
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
  MATRIXLF  matlf;

  if( WinQueryWindowRect( hwndClient, &rclClient) )
  {
    lWidth  = rclClient.xRight - rclClient.xLeft;
    lHeight = rclClient.yTop   - rclClient.yBottom;
    if( (lWidth > 0) && (lHeight > 0) )
    {
      matlf = matlfIdentity;
      DosGetDateTime( &date);
      srand( (USHORT)date.hundredths);
      for( psl = pslHead; psl != NULL; psl = psl->pslNext)
      {
	psl->pslNextIsland = psl;    /* reset island pointer		      */
	psl->fIslandMark = FALSE;    /* clear island mark		      */
	ptl.x = rclClient.xLeft   + (rand() % lWidth);
	ptl.y = rclClient.yBottom + (rand() % lHeight);
	GpiConvert( hpsClient, CVTC_DEVICE, CVTC_MODEL, 1L, &ptl);
	ptl.x = 50 * (ptl.x / 50) - 250;
	ptl.y = 50 * (ptl.y / 50) - 250;
	matlf.lM31 = ptl.x - psl->ptlLocation.x;
	matlf.lM32 = ptl.y - psl->ptlLocation.y;
	GpiSetSegmentTransformMatrix( hpsClient
				    , psl->lSegId
				    , 9L
				    , &matlf
				    , TRANSFORM_REPLACE);
	psl->ptlModelXlate.x = matlf.lM31;
	psl->ptlModelXlate.y = matlf.lM32;
	SetRect( psl);
      }
    }
  }
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
  SHORT   sZoomArg;
  POINTL  aptlDraw[3];
 
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
  hmqAsync = WinCreateMsgQueue( habAsync, 150);
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

    if( WinPeekMsg( habAsync, &qmsgPeek, NULL, UM_DIE, UM_DIE, PM_NOREMOVE))
	qmsgAsync = qmsgPeek;

    if( WinPeekMsg( habAsync, &qmsgPeek, NULL, UM_SIZING, UM_LOAD, PM_NOREMOVE))
	DosSemClear( hsemDrawOn);
    else
	DosSemSet( hsemDrawOn);
 
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

	if( qmsgAsync.mp1)
	{
	  DoDraw( hpsBitmapBuff, (HRGN)qmsgAsync.mp1, TRUE);
	  GpiQueryRegionBox( hpsClient, (HRGN)qmsgAsync.mp1, (PRECTL)aptlDraw);
	  GpiDestroyRegion( hpsClient, (HRGN)qmsgAsync.mp1);
	  WinMapWindowPoints( hwndClient, HWND_DESKTOP, aptlDraw, 3);
	  ROUND_DOWN_MOD( aptlDraw[0].x, lByteAlignX);	  /* round down       */
	  ROUND_DOWN_MOD( aptlDraw[0].y, lByteAlignY);	  /* round down       */
	  ROUND_UP_MOD(   aptlDraw[1].x, lByteAlignX);	  /* round up	      */
	  ROUND_UP_MOD(   aptlDraw[1].y, lByteAlignY);	  /* round up	      */
	  WinMapWindowPoints( HWND_DESKTOP, hwndClient, aptlDraw, 3);
	  aptlDraw[2] = aptlDraw[0];
	  GpiBitBlt( hpsClient
		   , hpsBitmapBuff
		   , 3L
		   , aptlDraw
		   , ROP_SRCCOPY
		   , BBO_IGNORE );
	}
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
		ROUND_DOWN_MOD( ptsScrollPos.x, (SHORT)lByteAlignX);
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
		ROUND_DOWN_MOD( ptsScrollPos.y, (SHORT)lByteAlignY);
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
      /* the window is being resized					      */
      /************************************************************************/
      case UM_SIZING:
	CalcBounds();
	CalcTransform( hwndClient);
        break;
 
      /************************************************************************/
      /* adjust zoom factor                                                   */
      /************************************************************************/
      case UM_ZOOM:
	sZoomArg = (SHORT)qmsgAsync.mp1;
	while( WinPeekMsg( habAsync
			 , &qmsgPeek
			 , NULL
			 , WM_USER
			 , UM_LAST
			 , PM_NOREMOVE))
	{
	    if( qmsgPeek.msg == UM_ZOOM)
		WinPeekMsg( habAsync
			  , &qmsgPeek
			  , NULL
			  , WM_USER
			  , UM_LAST
			  , PM_REMOVE);
	    else
		break;
	    sZoomArg += (SHORT)qmsgPeek.mp1;
	}
	if( WinPeekMsg( habAsync
		      , &qmsgPeek
		      , NULL
		      , UM_SIZING
		      , UM_LOAD
		      , PM_NOREMOVE))
	    DosSemClear( hsemDrawOn);
	else
	    DosSemSet( hsemDrawOn);
	Zoom( sZoomArg);
        break;

      /************************************************************************/
      /* Button down will cause a correlate on the picture to test for a hit. */
      /* Any selected segment will be highlighted and redrawn as dynamic.     */
      /************************************************************************/
      case UM_LEFTDOWN:
	if( !fButtonDownAsync)
	{
	    fButtonDownAsync = TRUE;
	    LeftDown( qmsgAsync.mp1);
	}
        break;
 
      /************************************************************************/
      /* if a segment is being dragged it will be redrawn in a new posn       */
      /************************************************************************/
      case UM_MOUSEMOVE:
	if( !fButtonDownAsync)
	    break;
	for( fDone = FALSE; !fDone ;)
	{
	  if( WinPeekMsg( habAsync	      /* look through first button-up */
			, &qmsgPeek
			, NULL
			, UM_MOUSEMOVE
			, UM_LEFTUP
			, PM_NOREMOVE))
	      if( qmsgPeek.msg == UM_MOUSEMOVE) /* only collapse move msgs    */
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
	MouseMove( qmsgAsync.mp1);	/* process last move before button-up */
        break;
 
      /************************************************************************/
      /* if a segment is being dragged it will be redrawn as normal	      */
      /************************************************************************/
      case UM_LEFTUP:
	if( fButtonDownAsync)
	{
	    LeftUp();
	    fButtonDownAsync = FALSE;
	}
        break;
 
      /************************************************************************/
      /* destroy resources and terminate				     */
      /************************************************************************/
      case UM_DIE:
	WinDestroyMsgQueue( hmqAsync);
	WinTerminate( habAsync);
        DosExit( EXIT_THREAD, 0);
        break;
 
      /************************************************************************/
      default:
        break;
    }
  }
}
 
/******************************************************************************/
/*									      */
/******************************************************************************/
VOID CalcSize( mp1, mp2)

MPARAM	mp1, mp2;
{
  ptsScrollMax.y = SHORT2FROMMP( mp2);
  ptsHalfScrollMax.y = ptsScrollMax.y >> 1;
  ptsScrollPage.x = ptsScrollMax.x >> 3;
  ROUND_DOWN_MOD( ptsScrollPage.x, (SHORT)lByteAlignX);
  ptsScrollLine.x = ptsScrollMax.x >> 5;
  ROUND_DOWN_MOD( ptsScrollLine.x, (SHORT)lByteAlignX);
  ptsScrollPos.y = (SHORT)(
			   (  (LONG)ptsScrollPos.y
			    * (LONG)SHORT2FROMMP(mp2)
			   )/ (LONG)SHORT2FROMMP(mp1)
			  );
  ptsOldScrollPos.y = (SHORT)(
			      (  (LONG)ptsOldScrollPos.y
			       * (LONG)SHORT2FROMMP(mp2)
			      )/ (LONG)SHORT2FROMMP(mp1)
			     );
  WinSendMsg( hwndVertScroll
	    , SBM_SETSCROLLBAR
	    , MPFROMSHORT( ptsScrollPos.y)
	    , MPFROM2SHORT( 1, ptsScrollMax.y) );

  ptsScrollMax.x = SHORT1FROMMP( mp2);
  ptsHalfScrollMax.x = ptsScrollMax.x >> 1;
  ptsScrollPage.y = ptsScrollMax.y >> 3;
  ROUND_DOWN_MOD( ptsScrollPage.y, (SHORT)lByteAlignY);
  ptsScrollLine.y = ptsScrollMax.y >> 5;
  ROUND_DOWN_MOD( ptsScrollLine.y, (SHORT)lByteAlignY);
  ptsScrollPos.x = (SHORT)(
			   (  (LONG)ptsScrollPos.x
			    * (LONG)SHORT1FROMMP(mp2)
			   )/(LONG)SHORT1FROMMP(mp1)
			  );
  ptsOldScrollPos.x = (SHORT)(
			      (  (LONG)ptsOldScrollPos.x
			       * (LONG)SHORT1FROMMP(mp2)
			      )/ (LONG)SHORT1FROMMP(mp1)
			     );
  WinSendMsg( hwndHorzScroll
	    , SBM_SETSCROLLBAR
	    , MPFROMSHORT( ptsScrollPos.x)
	    , MPFROM2SHORT( 1, ptsScrollMax.x) );
}

/******************************************************************************/
/* button down will cause one segment to be indicated and made dynamic	      */
/******************************************************************************/
VOID
LeftDown( mp)

MPARAM	 mp;
{
#ifdef fred
  LONG	    alSegTag[HITS][DEPTH][2];
#endif
  POINTL  aptl[2];
  LONG	  lColor;

  POINTL    ptl;
  HRGN	    hrgn, hrgnUpdt, hrgnUpdtDrag;
  RECTL     rcl;
  MATRIXLF  matlf;
  CHAR	    pszMsg[40];
  PSZ	    psz1, psz2;
  BOOL	    fFirst;
  PSEGLIST  psl;

  ptl.x = (LONG)(SHORT)SHORT1FROMMP( mp);
  ptl.y = (LONG)(SHORT)SHORT2FROMMP( mp);

  /****************************************************************************/
  /****************************************************************************/
  aptl[0] = aptl[1] = ptl;
  aptl[1].x++;
  aptl[1].y++;
  GpiBitBlt( hpsBitmapSave, NULL, 2L, aptl, ROP_ONE, BBO_IGNORE);
  lColor = GpiQueryPel( hpsBitmapSave, &ptl);
  for( pslPicked = pslTail; pslPicked != NULL; pslPicked = pslPicked->pslPrev)
  {
    rcl = pslPicked->rclCurrent;
    GpiConvert( hpsClient, CVTC_MODEL, CVTC_DEVICE, 2L, (PPOINTL)&rcl);
    rcl.xRight++;
    rcl.yTop++;
    if( WinPtInRect( habAsync, &rcl, &ptl))    /*  is point in bounding box?  */
#ifdef fred
	if( 0L < GpiCorrelateSegment( hpsClient
				    , pslPicked->lSegId
				    , PICKSEL_VISIBLE
				    , &ptl
				    , HITS
				    , DEPTH
				    , (PLONG)alSegTag ))
#endif
    {
	DrawPiece( hpsBitmapSave, pslPicked, FALSE);
	if( GpiQueryPel( hpsBitmapSave, &ptl) != lColor)
	    break;			       /*  got a hit		      */
    }
  }
  if( pslPicked)
    lPickedSeg	 = pslPicked->lSegId;
  else
  {
    fButtonDownAsync = FALSE;
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
    fButtonDownAsync = FALSE;
    return;
  }

  /****************************************************************************/
  GpiQuerySegmentTransformMatrix( hpsClient, lPickedSeg, 9L, &matlf );
  ptlOffStart.x = matlf.lM31;
  ptlOffStart.y = matlf.lM32;
  ptlMoveStart = ptl;
  GpiConvert( hpsClient, CVTC_DEVICE, CVTC_MODEL, 1L, &ptlMoveStart);
  ptlMoveStart.x = (ptlMoveStart.x / 50) * 50;
  ptlMoveStart.y = (ptlMoveStart.y / 50) * 50;
  ptlUpdtRef = ptlMoveStart;
  GpiConvert( hpsClient, CVTC_MODEL, CVTC_DEVICE, 1L, &ptlUpdtRef);

  /****************************************************************************/
  hrgnUpdt = GpiCreateRegion( hpsClient, 0L, NULL);
  for( psl = pslPicked, fFirst = TRUE
     ; (psl != pslPicked) || fFirst
     ; psl = psl->pslNextIsland, fFirst = FALSE )
  {
    rcl = psl->rclCurrent;   /* get model space bounding box of piece	      */
    GpiConvert( hpsClient, CVTC_MODEL, CVTC_DEVICE, 2L, (PPOINTL)&rcl);
    rcl.xRight++;	     /* adjust rectangle for conversion to dev space  */
    rcl.yTop++;
    rcl.xRight	+= 2;				 /* should not need	      */
    rcl.yTop	+= 2;				 /* should not need	      */
    rcl.xLeft	-= 4;				 /* should not need	      */
    rcl.yBottom -= 4;				 /* should not need	      */
    hrgn = GpiCreateRegion( hpsClient, 1L, &rcl);
    GpiCombineRegion( hpsClient, hrgnUpdt, hrgnUpdt, hrgn, CRGN_OR);
    GpiDestroyRegion( hpsClient, hrgn);

    GpiSetSegmentAttrs( hpsClient, psl->lSegId, ATTR_VISIBLE, ATTR_OFF);
  }

  GpiQueryRegionBox( hpsClient, hrgnUpdt, (PRECTL)aptlUpdt);
  WinMapWindowPoints( hwndClient, HWND_DESKTOP, aptlUpdt, 3);
  ROUND_DOWN_MOD( aptlUpdt[0].x, lByteAlignX);		  /* round down       */
  ROUND_DOWN_MOD( aptlUpdt[0].y, lByteAlignY);		  /* round down       */
  ROUND_UP_MOD(   aptlUpdt[1].x, lByteAlignX);		  /* round up	      */
  ROUND_UP_MOD(   aptlUpdt[1].y, lByteAlignY);		  /* round up	      */
  WinMapWindowPoints( HWND_DESKTOP, hwndClient, aptlUpdt, 3);
  hrgnUpdtDrag = GpiCreateRegion( hpsBitmapBuff, 1L, (PRECTL)aptlUpdt);

  aptlUpdt[2] = aptlUpdt[0];
  DoDraw( hpsBitmapBuff, hrgnUpdtDrag, TRUE);
  GpiDestroyRegion( hpsClient, hrgnUpdt);
  GpiDestroyRegion( hpsBitmapBuff, hrgnUpdtDrag);
  GpiBitBlt( hpsBitmapSave
	   , hpsBitmapBuff
	   , 3L
	   , aptlUpdt
	   , ROP_SRCCOPY
	   , BBO_IGNORE );

  /****************************************************************************/
  for( psl = pslPicked, fFirst = TRUE
     ; (psl != pslPicked) || fFirst
     ; psl = psl->pslNextIsland, fFirst = FALSE )
  {
    GpiSetSegmentAttrs( hpsClient, psl->lSegId, ATTR_VISIBLE, ATTR_ON);
    DrawPiece( hpsBitmapBuff, psl, TRUE);
  }
  GpiBitBlt( hpsClient
	   , hpsBitmapBuff
	   , 3L
	   , aptlUpdt
	   , ROP_SRCCOPY
	   , BBO_IGNORE );
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
  RECTL     rcl;
  POINTL    ptl, ptlModel, ptlDevice;
  POINTL    aptlUpdtRef[3], aptlUpdtNew[3];
  PSEGLIST  psl;
  BOOL	    fFirst;
  MATRIXLF  matlf;

  if( !lPickedSeg || !pslPicked)
    return;

  ptl.x = (LONG)(SHORT)SHORT1FROMMP( mp);
  ptl.y = (LONG)(SHORT)SHORT2FROMMP( mp);

  /****************************************************************************/
  /* clip mouse coords to client window 				      */
  /****************************************************************************/
  WinQueryWindowRect( hwndClient, &rcl);
  if (rcl.xLeft > ptl.x)
    ptl.x = rcl.xLeft;
  if (rcl.xRight <= ptl.x)
    ptl.x = rcl.xRight;
  if (rcl.yBottom > ptl.y)
    ptl.y = rcl.yBottom;
  if (rcl.yTop <= ptl.y)
    ptl.y = rcl.yTop;

  ptlModel = ptl;
  GpiConvert( hpsClient, CVTC_DEVICE, CVTC_MODEL, 1L, &ptlModel);
  ptlModel.x = 50 * (ptlModel.x / 50);
  ptlModel.y = 50 * (ptlModel.y / 50);
  if( (ptlModel.x == ptlOldMouse.x) && (ptlModel.y == ptlOldMouse.y))
    return;
  ptlOldMouse.x = ptlModel.x;
  ptlOldMouse.y = ptlModel.y;
  ptlDevice = ptlModel;
  GpiConvert( hpsClient, CVTC_MODEL, CVTC_DEVICE, 1L, &ptlDevice);

  GpiBitBlt( hpsBitmapBuff
	   , hpsBitmapSave
	   , 3L
	   , aptlUpdt
	   , ROP_SRCCOPY
	   , BBO_IGNORE );
  aptlUpdtRef[0] = aptlUpdt[0];
  aptlUpdtRef[1] = aptlUpdt[1];

  aptlUpdt[0].x += ptlDevice.x - ptlUpdtRef.x;
  aptlUpdt[0].y += ptlDevice.y - ptlUpdtRef.y;
  aptlUpdt[1].x += ptlDevice.x - ptlUpdtRef.x;
  aptlUpdt[1].y += ptlDevice.y - ptlUpdtRef.y;
  WinMapWindowPoints( hwndClient, HWND_DESKTOP, aptlUpdt, 3);
  ROUND_DOWN_MOD( aptlUpdt[0].x, lByteAlignX);		  /* round down       */
  ROUND_DOWN_MOD( aptlUpdt[0].y, lByteAlignY);		  /* round down       */
  ROUND_UP_MOD(   aptlUpdt[1].x, lByteAlignX);		  /* round up	      */
  ROUND_UP_MOD(   aptlUpdt[1].y, lByteAlignY);		  /* round up	      */
  WinMapWindowPoints( HWND_DESKTOP, hwndClient, aptlUpdt, 3);
  aptlUpdt[2] = aptlUpdt[0];
  ptlUpdtRef = ptlDevice;
  GpiBitBlt( hpsBitmapSave
	   , hpsBitmapBuff
	   , 3L
	   , aptlUpdt
	   , ROP_SRCCOPY
	   , BBO_IGNORE );


  GpiQuerySegmentTransformMatrix( hpsClient
				, lPickedSeg
				, 9L
				, &matlf);
  for( psl = pslPicked, fFirst = TRUE
     ; (psl != pslPicked) || fFirst
     ; psl = psl->pslNextIsland, fFirst = FALSE )
  {
    matlf.lM31 = ptlOffStart.x + ptlModel.x - ptlMoveStart.x;
    matlf.lM32 = ptlOffStart.y + ptlModel.y - ptlMoveStart.y;
    GpiSetSegmentTransformMatrix( hpsClient
				, psl->lSegId
				, 9L
				, &matlf
				, TRANSFORM_REPLACE);
    psl->ptlModelXlate.x = matlf.lM31;
    psl->ptlModelXlate.y = matlf.lM32;
    DrawPiece( hpsBitmapBuff, psl, TRUE);
  }

  WinUnionRect( habMain
	      , (PRECTL)aptlUpdtNew
	      , (PRECTL)aptlUpdt
	      , (PRECTL)aptlUpdtRef);
  WinMapWindowPoints( hwndClient, HWND_DESKTOP, aptlUpdtNew, 2);
  ROUND_DOWN_MOD( aptlUpdtNew[0].x, lByteAlignX);	  /* round down       */
  ROUND_DOWN_MOD( aptlUpdtNew[0].y, lByteAlignY);	  /* round down       */
  ROUND_UP_MOD(   aptlUpdtNew[1].x, lByteAlignX);	  /* round up	      */
  ROUND_UP_MOD(   aptlUpdtNew[1].y, lByteAlignY);	  /* round up	      */
  WinMapWindowPoints( HWND_DESKTOP, hwndClient, aptlUpdtNew, 2);
  aptlUpdtNew[2] = aptlUpdtNew[0];
  GpiBitBlt( hpsClient
	   , hpsBitmapBuff
	   , 3L
	   , aptlUpdtNew
	   , ROP_SRCCOPY
	   , BBO_IGNORE );
}
 
 
/******************************************************************************/
/*									      */
/* The dragged segment is being unselected.  Return it to its normal state.   */
/*									      */
/******************************************************************************/
VOID
LeftUp()
{
  PSEGLIST   psl, pslTemp;
  POINTL     ptlShift;
  BOOL	     fFirst;
  MATRIXLF   matlf;
  LONG	     l;

  if( !lPickedSeg || !pslPicked)
    return;

  for( psl = pslPicked, fFirst = TRUE
     ; (psl != pslPicked) || fFirst
     ; psl = psl->pslNextIsland, fFirst = FALSE )
  {

    SetRect( psl);
    SegListUpdate( MAKE_TAIL_SEG, psl);       /* at tail => highest priority  */
    psl->fIslandMark = TRUE;		      /* mark as island member	      */
  }
  GpiQuerySegmentTransformMatrix( hpsClient
				, lPickedSeg
				, 9L
				, &matlf);
  ptlShift.x = matlf.lM31;
  ptlShift.y = matlf.lM32;

  for( psl = pslHead; psl != NULL; psl = psl->pslNext)
    if( !psl->fIslandMark)
      for( l = 0; l < 8; l++)
	if( pslPicked->lAdjacent[l] == psl->lSegId)
	{
	  GpiQuerySegmentTransformMatrix( hpsClient, psl->lSegId, 9L, &matlf);
	  if( (ptlShift.x == matlf.lM31) && (ptlShift.y == matlf.lM32))
	  {
	    DosBeep( 600, 100);
	    DosBeep( 1200, 50);
	    MarkIsland( psl, TRUE);	      /* mark the whole new island    */
	    pslTemp = psl->pslNextIsland;     /* swap island ptrs	      */
	    psl->pslNextIsland = pslPicked->pslNextIsland;
	    pslPicked->pslNextIsland = pslTemp;
	  }
	}
  MarkIsland( pslPicked, FALSE);	      /* unmark the island	      */

  pslPicked = NULL;
  lPickedSeg = NULL;

  WinSetCapture( HWND_DESKTOP, (HWND)NULL);
}
 
 
/******************************************************************************/
/*                                                                            */
/* mark a whole island							      */
/*                                                                            */
/******************************************************************************/
VOID
MarkIsland( pslMark, fMark)

PSEGLIST  pslMark;
BOOL	  fMark;
{
  PSEGLIST  psl;
  BOOL	    fFirst;

  for( psl = pslMark, fFirst = TRUE
     ; (psl != pslMark) || fFirst
     ; psl = psl->pslNextIsland, fFirst = FALSE )
      psl->fIslandMark = fMark; 	      /* mark as island member	      */
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
  POINTL    aptlClient[3];
  HRGN	    hrgn;
  MATRIXLF  matlf;
 
  if( ptsScrollPos.x > ptsScrollMax.x)	   /* clip to range of scroll param   */
      ptsScrollPos.x = ptsScrollMax.x;
  if( ptsScrollPos.x < 0)
      ptsScrollPos.x = 0;
 
  if( ptsOldScrollPos.x != ptsScrollPos.x) /* only process change in position */
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
  WinQueryWindowRect( hwndClient, (PRECTL)aptlClient);
  if( abs( ptsScrollPos.x - ptsOldScrollPos.x) <= ptsScrollMax.x)
  {
      WinScrollWindow( hwndClient
		     , ptsOldScrollPos.x - ptsScrollPos.x
		     , 0
		     , NULL
		     , NULL
		     , hrgn
		     , NULL
		     , 0);
  } else
  {
      GpiSetRegion( hpsClient, hrgn, 1L, (PRECTL)aptlClient);
  }
  /****************************************************************************/
  /* adjust the default view matrix					      */
  /****************************************************************************/
  GpiQueryDefaultViewMatrix( hpsClient, 9L, &matlf );
  matlf.lM31 -= ptsScrollPos.x - ptsOldScrollPos.x;
  GpiSetDefaultViewMatrix( hpsClient, 9L, &matlf, TRANSFORM_REPLACE);

  DoDraw( hpsClient, hrgn, TRUE);     /* paint into the client area	      */
  ptsOldScrollPos.x = ptsScrollPos.x;
  GpiDestroyRegion( hpsClient, hrgn);

  aptlClient[2] = aptlClient[0];
  GpiBitBlt( hpsBitmapBuff	      /* update the off-screen client image   */
	   , hpsClient
	   , 3L
	   , aptlClient
	   , ROP_SRCCOPY
	   , BBO_IGNORE );
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
  POINTL    aptlClient[3];
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
  WinQueryWindowRect( hwndClient, (PRECTL)aptlClient);
  if( abs( ptsScrollPos.y - ptsOldScrollPos.y) <= ptsScrollMax.y)
  {
      WinScrollWindow( hwndClient
		     , 0
		     , ptsScrollPos.y - ptsOldScrollPos.y
		     , NULL
		     , NULL
		     , hrgn
		     , NULL
		     , 0);
  } else
  {
      GpiSetRegion( hpsClient, hrgn, 1L, (PRECTL)aptlClient);
  }
  GpiQueryDefaultViewMatrix( hpsClient, 9L, &matlf );
  matlf.lM32 += ptsScrollPos.y - ptsOldScrollPos.y;
  GpiSetDefaultViewMatrix( hpsClient, 9L, &matlf, TRANSFORM_REPLACE);

  DoDraw( hpsClient, hrgn, TRUE);
  ptsOldScrollPos.y = ptsScrollPos.y;
  GpiDestroyRegion( hpsClient, hrgn);

  aptlClient[2] = aptlClient[0];
  GpiBitBlt( hpsBitmapBuff
	   , hpsClient
	   , 3L
	   , aptlClient
	   , ROP_SRCCOPY
	   , BBO_IGNORE );
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
  POINTL  aptlUpdtNew[3];
 
  WinQueryWindowRect( hwndClient, &rclInvalid);
  hrgnUpdt = GpiCreateRegion( hpsBitmapBuff, 1L, &rclInvalid);
  DoDraw( hpsBitmapBuff, hrgnUpdt, TRUE);
  GpiDestroyRegion( hpsBitmapBuff, hrgnUpdt);
  aptlUpdtNew[0].x = rclInvalid.xLeft;
  aptlUpdtNew[0].y = rclInvalid.yBottom;
  aptlUpdtNew[1].x = rclInvalid.xRight;
  aptlUpdtNew[1].y = rclInvalid.yTop;
  ROUND_DOWN_MOD( aptlUpdtNew[0].x, lByteAlignX);	  /* round down       */
  ROUND_DOWN_MOD( aptlUpdtNew[0].y, lByteAlignY);	  /* round down       */
  ROUND_UP_MOD(   aptlUpdtNew[1].x, lByteAlignX);	  /* round up	      */
  ROUND_UP_MOD(   aptlUpdtNew[1].y, lByteAlignY);	  /* round up	      */
  aptlUpdtNew[2] = aptlUpdtNew[0];
  GpiBitBlt( hpsClient
	   , hpsBitmapBuff
	   , 3L
	   , aptlUpdtNew
	   , ROP_SRCCOPY
	   , BBO_IGNORE );
}


/******************************************************************************/
/*                                                                            */
/* toggle a flag and update the menu check-box				      */
/*                                                                            */
/******************************************************************************/
VOID
ToggleMenuItem( usMenuMajor, usMenuMinor, pfFlag)

USHORT	usMenuMajor, usMenuMinor;
PBOOL	pfFlag;
{
  MENUITEM mi;

  WinSendMsg( WinWindowFromID( hwndFrame, FID_MENU)
	    , MM_QUERYITEM
	    , MPFROM2SHORT( usMenuMajor, FALSE)
	    , MPFROMP( (PMENUITEM)&mi));

  if( *pfFlag)
  {
    *pfFlag = FALSE;
    WinSendMsg( mi.hwndSubMenu
	      , MM_SETITEMATTR
	      , MPFROM2SHORT( usMenuMinor, TRUE)
	      , MPFROM2SHORT( MIA_CHECKED, ~MIA_CHECKED) );
  }
  else
  {
    *pfFlag = TRUE;
    WinSendMsg( mi.hwndSubMenu
	      , MM_SETITEMATTR
	      , MPFROM2SHORT( usMenuMinor, TRUE)
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
  if( lScale > 0)
    lScale = 0;
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
 
  if( lScale >= 0)
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
/* Use special knowledge that the only non-identity part of the segment       */
/* transform is the translation part.					      */
/******************************************************************************/
VOID
SetRect( psl)

PSEGLIST  psl;
{
  MATRIXLF  matlf;

  GpiQuerySegmentTransformMatrix( hpsClient	/* get model xform of segment */
				, psl->lSegId
				, 9L
				, &matlf );
  psl->rclCurrent = psl->rclBitBlt;		/* world space bounding rect  */
  psl->rclCurrent.xLeft   += matlf.lM31;	/* apply translation	      */
  psl->rclCurrent.yBottom += matlf.lM32;
  psl->rclCurrent.xRight  += matlf.lM31;
  psl->rclCurrent.yTop	  += matlf.lM32;
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
  POINTL    ptlCenter, ptlTrans, ptlScale, aptl[4], ptlOrg, aptlSides[12];
  PSEGLIST  psl;
  LONG	    l;
 
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
  /* scale down to 1:1 of bitmap in file				      */
  /****************************************************************************/
  ptlScale.x = UNITY * bmpBitmapFile.cx /(ptlTopRight.x - ptlBotLeft.x);
  ptlScale.y = UNITY * bmpBitmapFile.cy /(ptlTopRight.y - ptlBotLeft.y);
 
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
  /* create bitmaps for pieces						      */
  /****************************************************************************/

  ptlOffset = ptlBotLeft;		   /* BottomLeft corner in dev coord  */
  GpiConvert( hpsClient, CVTC_WORLD, CVTC_DEVICE, 1L, &ptlOffset);
  if( (ptlScale.x != ptlScaleRef.x) || (ptlScale.y != ptlScaleRef.y))
  {
    ptlScaleRef = ptlScale;

    /**************************************************************************/
    /* create a shadow bitmap of the original, sized to current output size   */
    /**************************************************************************/
    aptl[0] = ptlBotLeft;		   /* current output rect, dev coord  */
    aptl[1] = ptlTopRight;
    GpiConvert( hpsClient, CVTC_WORLD, CVTC_DEVICE, 2L, aptl);

    aptl[0].x -= ptlOffset.x;		   /* put bottom left at (0,0)	      */
    aptl[0].y -= ptlOffset.y;
    aptl[1].x -= ptlOffset.x - 1;	   /* correct for coordinate convert  */
    aptl[1].y -= ptlOffset.y - 1;
    aptl[2].x = 0L;
    aptl[2].y = 0L;
    aptl[3].x = bmpBitmapFile.cx + 1;	   /* bitmap dimensions 	      */
    aptl[3].y = bmpBitmapFile.cy + 1;
    GpiSetBitmap( hpsBitmapSize, hbmBitmapSize);
    GpiBitBlt( hpsBitmapSize		   /* copy the bitmap		      */
	     , hpsBitmapFile
	     , 4L
	     , aptl
	     , ROP_SRCCOPY
	     , BBO_IGNORE);

    for( psl = pslHead; psl != NULL; psl = psl->pslNext)
    {
      if( DosSemWait( hsemTerminate, SEM_IMMEDIATE_RETURN)) /* exit if quit   */
	break;
      aptl[0].x = psl->rclBitBlt.xLeft;     /* bounding rect in world space   */
      aptl[0].y = psl->rclBitBlt.yBottom;
      aptl[1].x = psl->rclBitBlt.xRight;
      aptl[1].y = psl->rclBitBlt.yTop;
      aptl[2] = aptl[0];
      aptl[3] = aptl[1];
      GpiConvert( hpsClient, CVTC_WORLD, CVTC_DEVICE, 2L, &aptl[2]);
      ptlOrg = aptl[2];
      aptl[3].x -= ptlOrg.x - 1;	    /* bitmap rect of piece	      */
      aptl[3].y -= ptlOrg.y - 1;
      aptl[2].x = 0L;
      aptl[2].y = 0L;
      psl->aptlBitBlt[0] = aptl[0];
      psl->aptlBitBlt[1] = aptl[1];
      psl->aptlBitBlt[2] = aptl[2];
      psl->aptlBitBlt[3] = aptl[3];

      /************************************************************************/
      /* compute the piece control points				      */
      /************************************************************************/
      for( l=0; l<12; l++)
	aptlSides[l] = psl->aptlSides[l];
      GpiConvert( hpsClient, CVTC_WORLD, CVTC_DEVICE, 12L, aptlSides);
      for( l=0; l<12; l++)
      {
	aptlSides[l].x -= ptlOrg.x;
	aptlSides[l].y -= ptlOrg.y;
      }

      /************************************************************************/
      /* prepare the mask to punch a hole in the output bitmap		      */
      /************************************************************************/
      GpiSetBitmap( psl->hpsHole, psl->hbmHole);
      GpiSetClipPath( psl->hpsHole, 0L, SCP_RESET);  /* no clip path	      */
      GpiBitBlt( psl->hpsHole			     /* fill with 1's         */
	       , NULL
	       , 2L
	       , &psl->aptlBitBlt[2]
	       , ROP_ONE
	       , BBO_IGNORE);

      GpiBeginPath( psl->hpsHole, 1L);		     /* define a clip path    */
      GpiMove( psl->hpsHole, &aptlSides[11]);
      GpiPolySpline( psl->hpsHole, 12L, aptlSides);
      GpiEndPath( psl->hpsHole);
      GpiSetClipPath( psl->hpsHole, 1L, SCP_AND);
      GpiBitBlt( psl->hpsHole			     /* fill with 0's         */
	       , NULL
	       , 2L
	       , &psl->aptlBitBlt[2]
	       , ROP_ZERO
	       , BBO_IGNORE);
      GpiSetClipPath( psl->hpsHole, 0L, SCP_RESET);  /* clear the clip path   */

      /************************************************************************/
      /* prepare the mask to fill the hole in the output bitmap 	      */
      /************************************************************************/
      GpiSetBitmap( psl->hpsFill, psl->hbmFill);
      GpiSetClipPath( psl->hpsFill, 0L, SCP_RESET);  /* no clip path	      */
      GpiBitBlt( psl->hpsFill			     /* fill with 1's         */
	       , NULL
	       , 2L
	       , &psl->aptlBitBlt[2]
	       , ROP_ZERO
	       , BBO_IGNORE);

      GpiBeginPath( psl->hpsFill, 1L);		     /* define a clip path    */
      GpiMove( psl->hpsFill, &aptlSides[11]);
      GpiPolySpline( psl->hpsFill, 12L, aptlSides);
      GpiEndPath( psl->hpsFill);
      GpiSetClipPath( psl->hpsFill, 1L, SCP_AND);
      aptl[0] = psl->aptlBitBlt[2];
      aptl[1] = psl->aptlBitBlt[3];
      aptl[2].x = ptlOrg.x - ptlOffset.x;	     /* pick the right part   */
      aptl[2].y = ptlOrg.y - ptlOffset.y;	     /* of the sized bitmap   */
      GpiBitBlt( psl->hpsFill			     /* fill with data	      */
	       , hpsBitmapSize
	       , 3L
	       , aptl
	       , ROP_SRCCOPY
	       , BBO_IGNORE);
      GpiSetClipPath( psl->hpsFill, 0L, SCP_RESET);  /* clear the clip path   */

      GpiSetColor( psl->hpsFill, CLR_RED);	     /* draw the outline      */
      GpiMove( psl->hpsFill, &aptlSides[11]);
      GpiPolySpline( psl->hpsFill, 12L, aptlSides);
    }
  }
}
 
 
/******************************************************************************/
/*                                                                            */
/* Draw one piece.							      */
/*                                                                            */
/******************************************************************************/
VOID
DrawPiece( hps, psl, fFill)

HPS	  hps;
PSEGLIST  psl;
BOOL	  fFill;
{
    POINTL  aptl[4];

    if( GpiQuerySegmentAttrs( hpsClient, psl->lSegId, ATTR_VISIBLE) == ATTR_ON)
    {
      aptl[2].x = aptl[2].y = 0L;
      aptl[3] = psl->aptlBitBlt[3];
      aptl[0].x = psl->rclBitBlt.xLeft + psl->ptlModelXlate.x;
      aptl[0].y = psl->rclBitBlt.yBottom + psl->ptlModelXlate.y;
      GpiConvert( hpsClient, CVTC_MODEL, CVTC_DEVICE, 1L, aptl);
      aptl[1].x = aptl[0].x + aptl[3].x;
      aptl[1].y = aptl[0].y + aptl[3].y;
      GpiBitBlt( hps			    /* punch a hole		      */
	       , psl->hpsHole
	       , 4L
	       , aptl
	       , ROP_SRCAND
	       , BBO_IGNORE );
      if( fFill)
	GpiBitBlt( hps			    /* fill the hole		      */
		 , psl->hpsFill
		 , 4L
		 , aptl
		 , ROP_SRCPAINT
		 , BBO_IGNORE );
    }
}

/******************************************************************************/
/*                                                                            */
/* Draw the picture, using the passed region for clipping.		      */
/* Intersect the bounding box used for the clip region with the client rect.  */
/* Test each segment to see if its bounding box intersects the bounding box   */
/* of the clipping region.  Draw only if there is an intersection.	      */
/*                                                                            */
/******************************************************************************/
BOOL
DoDraw( hps, hrgn, fPaint)

HPS	hps;
HRGN	hrgn;
BOOL	fPaint;
{
  HRGN	    hrgnOld;
  RECTL     rcl, rclRegion, rclDst, rclClient;
  PSEGLIST  psl;

  if( fPaint)
  {
    GpiSetColor( hps, CLR_BACKGROUND);
    GpiPaintRegion( hps, hrgn); 		/* erase region 	      */
  }
  GpiQueryRegionBox( hps, hrgn, &rclRegion);
  WinQueryWindowRect( hwndClient, &rclClient);
  if( !WinIntersectRect( habAsync, &rclRegion, &rclRegion, &rclClient))
    return( FALSE);				/* not in client window       */
  GpiSetClipRegion( hps, hrgn, &hrgnOld);	/* make the clip region       */
  for( psl = pslHead; psl != NULL; psl = psl->pslNext) /* scan all pieces     */
  {
    /**************************************************************************/
    /* get the piece bounding box in device coordinates 		      */
    /**************************************************************************/
    rcl = psl->rclCurrent;
    GpiConvert( hpsClient, CVTC_MODEL, CVTC_DEVICE, 2L, (PPOINTL)&rcl);
    rcl.xRight++;
    rcl.yTop++;
    /**************************************************************************/
    /* if the piece might be visible, and drawing allowed, draw the piece     */
    /**************************************************************************/
    if( WinIntersectRect( habAsync, &rclDst, &rcl, &rclRegion))
	if( DosSemWait( hsemDrawOn, SEM_IMMEDIATE_RETURN))
	  DrawPiece( hps, psl, TRUE);
	else
	  break;
  }
  GpiSetClipRegion( hps, NULL, &hrgnOld);
  if( DosSemWait( hsemDrawOn, SEM_IMMEDIATE_RETURN))
    GpiSetRegion( hpsClient, hrgnInvalid, 0L, NULL);

  return( TRUE);
}
 
/******************************************************************************/
/*                                                                            */
/* Return a pointer to a segment list member, based on segment id.	      */
/*                                                                            */
/******************************************************************************/
PSEGLIST
SegListGet( lSeg)

LONG  lSeg;
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
PSEGLIST
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
	  return( NULL);
	*pslHead = *pslUpdate;
	pslHead->pslPrev = NULL;
	pslHead->pslNext = NULL;
	pslTail = pslHead;
      } else
      {
	psl = MAKEP( sel, 0);
	if( psl == NULL)
	  return( NULL);
	*psl = *pslUpdate;
	pslHead->pslPrev = psl;
	psl->pslNext = pslHead;
	psl->pslPrev = NULL;
	pslHead = psl;
      }
      return( pslHead);
      break;

    case ADD_TAIL_SEG:
      DosAllocSeg( sizeof( SEGLIST), &sel, 0);
      if( pslTail == NULL)
      {
	pslHead = MAKEP( sel, 0);
	if( pslHead == NULL)
	  return( NULL);
	*pslHead = *pslUpdate;
	pslHead->pslPrev = NULL;
	pslHead->pslNext = NULL;
	pslTail = pslHead;
      } else
      {
	psl = MAKEP( sel, 0);
	if( psl == NULL)
	  return( NULL);
	*psl = *pslUpdate;
	pslTail->pslNext = psl;
	psl->pslPrev = pslTail;
	psl->pslNext = NULL;
	pslTail = psl;
      }
      return( pslTail);
      break;

    case MAKE_TAIL_SEG:
      if( pslUpdate == pslTail)
	return( pslTail);
      if( pslUpdate == pslHead)
      {
	pslHead = pslHead->pslNext;
	pslHead->pslPrev = NULL;
      } else
      {
	pslUpdate->pslPrev->pslNext = pslUpdate->pslNext;
	pslUpdate->pslNext->pslPrev = pslUpdate->pslPrev;
      }
      pslTail->pslNext = pslUpdate;
      pslUpdate->pslPrev = pslTail;
      pslTail = pslUpdate;
      pslTail->pslNext = NULL;
      return( pslTail);
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
	  return( psl);
	  break;
	}
      }
      return( NULL);
      break;

    default:
      return( NULL);
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
  {
    GpiSetBitmap( pslHead->hpsFill, NULL);
    GpiDeleteBitmap( pslHead->hbmFill);
    GpiDestroyPS( pslHead->hpsFill);
    DevCloseDC( pslHead->hdcFill);

    GpiSetBitmap( pslHead->hpsHole, NULL);
    GpiDeleteBitmap( pslHead->hbmHole);
    GpiDestroyPS( pslHead->hpsHole);
    DevCloseDC( pslHead->hdcHole);

    SegListUpdate( DEL_SEG, pslHead);
  }
  GpiDeleteSegments( hpsClient, 1L, lLastSegId);

  if( hbmBitmapFile)
  {
      GpiSetBitmap( hpsBitmapFile, NULL);
      GpiDeleteBitmap( hbmBitmapFile);
  }
  if( hbmBitmapSize)
  {
      GpiSetBitmap( hpsBitmapSize, NULL);
      GpiDeleteBitmap( hbmBitmapSize);
  }
  if( hbmBitmapBuff)
  {
      GpiSetBitmap( hpsBitmapBuff, NULL);
      GpiDeleteBitmap( hbmBitmapBuff);
  }
  if( hbmBitmapSave)
  {
      GpiSetBitmap( hpsBitmapSave, NULL);
      GpiDeleteBitmap( hbmBitmapSave);
  }

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
  PSEGLIST  psl;
  LONG	    l, lMinor, lNeighbor, alFuzz[36][4];
  SIZEL     sizl;
  BITMAPINFOHEADER bmp;
  DATETIME  date;

  /****************************************************************************/
  /* compute some fuzz for the control points				      */
  /****************************************************************************/
  DosGetDateTime( &date);
  srand( (USHORT)date.hundredths);
  for( l=0; l<36; l++)
    for( lMinor=0; lMinor<4; lMinor++)
      alFuzz[l][lMinor] = 50 * (rand() % 10);

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
  GpiSetInitialSegmentAttrs( hpsClient, ATTR_FASTCHAIN, ATTR_OFF);
  GpiSetInitialSegmentAttrs( hpsClient, ATTR_DETECTABLE, ATTR_OFF);
 
  /****************************************************************************/
  /* draw the pieces							      */
  /****************************************************************************/
  lLastSegId = 0;
  for( ptl.x = ptlBotLeft.x; ptl.x < ptlTopRight.x; ptl.x += 500)
  {
    if( DosSemWait( hsemTerminate, SEM_IMMEDIATE_RETURN))
      break;
    for( ptl.y = ptlBotLeft.y; ptl.y < ptlTopRight.y; ptl.y += 500)
    {
      if( DosSemWait( hsemTerminate, SEM_IMMEDIATE_RETURN))
	break;
      lLastSegId++;

      /************************************************************************/
      /* compute the piece outline control points			      */
      /************************************************************************/
      aptlControl[0].x = 250L;
      aptlControl[0].y = 500L;
      aptlControl[1].x = 250;
      aptlControl[1].y = -500L;
      aptlControl[2].x = 500L;
      aptlControl[2].y = 0L;

      aptlControl[3].x = 0L;
      aptlControl[3].y = 250L;
      aptlControl[4].x = 1000L;
      aptlControl[4].y = 250L;
      aptlControl[5].x = 500L;
      aptlControl[5].y = 500L;

      aptlControl[6].x = 250L;
      aptlControl[6].y = 0L;
      aptlControl[7].x = 250L;
      aptlControl[7].y = 1000L;
      aptlControl[8].x = 0L;
      aptlControl[8].y = 500L;

      aptlControl[9].x	= 500L;
      aptlControl[9].y	= 250L;
      aptlControl[10].x = -500L;
      aptlControl[10].y = 250L;
      aptlControl[11].x = 0L;
      aptlControl[11].y = 0L;

      if( ptl.y == ptlBotLeft.y)
      {
	aptlControl[0].y = 0L;
	aptlControl[1].y = 0L;
      }

      if( (ptl.x + 500) == ptlTopRight.x)
      {
	aptlControl[3].x = 500L;
	aptlControl[4].x = 500L;
      }

      if( (ptl.y + 500) == ptlTopRight.y)
      {
	aptlControl[6].y = 500L;
	aptlControl[7].y = 500L;
      }

      if( ptl.x == ptlBotLeft.x)
      {
	aptlControl[ 9].x = 0L;
	aptlControl[10].x = 0L;
      }

      /************************************************************************/
      /* compute the adjacent segments					      */
      /************************************************************************/
      sl.lAdjacent[0] = lLastSegId - 7;
      sl.lAdjacent[1] = lLastSegId - 6;
      sl.lAdjacent[2] = lLastSegId - 5;
      sl.lAdjacent[3] = lLastSegId - 1;
      sl.lAdjacent[4] = lLastSegId + 1;
      sl.lAdjacent[5] = lLastSegId + 5;
      sl.lAdjacent[6] = lLastSegId + 6;
      sl.lAdjacent[7] = lLastSegId + 7;
      if( ptl.x == ptlBotLeft.x)
      {
	sl.lAdjacent[0] = 0;
	sl.lAdjacent[1] = 0;
	sl.lAdjacent[2] = 0;
      }
      if( ptl.y == ptlBotLeft.y)
      {
	sl.lAdjacent[0] = 0;
	sl.lAdjacent[3] = 0;
	sl.lAdjacent[5] = 0;
      }
      if( (ptl.x + 500) == ptlTopRight.x)
      {
	sl.lAdjacent[5] = 0;
	sl.lAdjacent[6] = 0;
	sl.lAdjacent[7] = 0;
      }
      if( (ptl.y + 500) == ptlTopRight.y)
      {
	sl.lAdjacent[2] = 0;
	sl.lAdjacent[4] = 0;
	sl.lAdjacent[7] = 0;
      }

      /************************************************************************/
      /* throw in some fuzz						      */
      /************************************************************************/
      if( sl.lAdjacent[3])
      {
	aptlControl[0].y  -= alFuzz[lLastSegId - 1][0];
	aptlControl[1].y  += alFuzz[lLastSegId - 1][1];
      }

      if( sl.lAdjacent[1])
      {
	aptlControl[9].x  -= alFuzz[lLastSegId - 1][2];
	aptlControl[10].x += alFuzz[lLastSegId - 1][3];
      }

      if( lNeighbor = sl.lAdjacent[4])
      {
	aptlControl[7].y  -= alFuzz[lNeighbor - 1][0];
	aptlControl[6].y  += alFuzz[lNeighbor - 1][1];
      }

      if( lNeighbor = sl.lAdjacent[6])
      {
	aptlControl[4].x  -= alFuzz[lNeighbor - 1][2];
	aptlControl[3].x  += alFuzz[lNeighbor - 1][3];
      }

      /************************************************************************/
      /* compute the piece control points in world coordinates		      */
      /************************************************************************/
      for( l=0; l<12; l++)
      {
	aptlSides[l].x = ptl.x + aptlControl[l].x;
	aptlSides[l].y = ptl.y + aptlControl[l].y;
	sl.aptlSides[l] = aptlSides[l];
      }

      /************************************************************************/
      /* compute the dimensions of the matching rects for BitBlt	      */
      /************************************************************************/
      sl.rclBitBlt.xLeft   = ptl.x - 250;
      sl.rclBitBlt.yBottom = ptl.y - 250;
      sl.rclBitBlt.xRight  = ptl.x + 750;
      sl.rclBitBlt.yTop    = ptl.y + 750;
      if( ptl.x == ptlBotLeft.x)
	sl.rclBitBlt.xLeft += 250;
      if( ptl.y == ptlBotLeft.y)
	sl.rclBitBlt.yBottom += 250;
      if( (ptl.x + 500) == ptlTopRight.x)
	sl.rclBitBlt.xRight -= 250;
      if( (ptl.y + 500) == ptlTopRight.y)
	sl.rclBitBlt.yTop -= 250;

      /************************************************************************/
      /* store the piece location					      */
      /************************************************************************/
      sl.ptlLocation = ptl;

      /************************************************************************/
      /* draw one piece 						      */
      /************************************************************************/
      GpiOpenSegment( hpsClient, lLastSegId);
      GpiSetTag( hpsClient, lLastSegId);
      GpiBeginPath( hpsClient, 1L);
      GpiMove( hpsClient, &ptl);
      GpiPolySpline( hpsClient, 12L, aptlSides);
      GpiEndPath( hpsClient);
      GpiSetColor( hpsClient, CLR_BLACK);
      GpiFillPath( hpsClient, 1L, 0L);
      GpiCloseSegment( hpsClient);


      /************************************************************************/
      /* create the masks						      */
      /************************************************************************/
      sizl.cx = 2 + ((bmpBitmapFile.cx
		     * (sl.rclBitBlt.xRight - sl.rclBitBlt.xLeft))
		     / (ptlTopRight.x - ptlBotLeft.x));
      sizl.cy = 2 + ((bmpBitmapFile.cy
		     * (sl.rclBitBlt.yTop - sl.rclBitBlt.yBottom))
		     / (ptlTopRight.y - ptlBotLeft.y));

      bmp    = bmpBitmapFile;
      bmp.cx = LOUSHORT( sizl.cx);
      bmp.cy = LOUSHORT( sizl.cy);

      sl.hdcFill = DevOpenDC( habMain
			    , OD_MEMORY
			    , "*"
			    , 3L
			    , (PDEVOPENDATA)&dop
			    , NULL);
      sl.hpsFill = GpiCreatePS( habMain
			      , sl.hdcFill
			      , &sizl
			      , PU_PELS | GPIA_ASSOC | GPIT_MICRO );
      sl.hbmFill = GpiCreateBitmap( sl.hpsFill
				  , &bmp
				  , 0L
				  , NULL
				  , NULL);


      sl.hdcHole = DevOpenDC( habMain
			    , OD_MEMORY
			    , "*"
			    , 3L
			    , (PDEVOPENDATA)&dop
			    , NULL);
      sl.hpsHole = GpiCreatePS( habMain
			      , sl.hdcHole
			      , &sizl
			      , PU_PELS | GPIA_ASSOC | GPIT_MICRO );
      sl.hbmHole = GpiCreateBitmap( sl.hpsHole
				  , &bmp
				  , 0L
				  , NULL
				  , NULL);



      GpiSetSegmentAttrs( hpsClient, lLastSegId, ATTR_DETECTABLE, ATTR_ON);

      sl.lSegId = lLastSegId;
      sl.pslNext = NULL;
      sl.pslPrev = NULL;
      sl.fIslandMark = FALSE;
      sl.ptlModelXlate.x = sl.ptlModelXlate.y = 0L;
      SetRect( &sl);
      psl = SegListUpdate( ADD_TAIL_SEG, &sl);
      psl->pslNextIsland = psl; 	/* point to self ==> island of one    */
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
  hbmBitmapSize    = GpiCreateBitmap( hpsBitmapSize
				    , &bmpBitmapFile
				    , 0L
				    , NULL
				    , NULL);
  if( !hbmBitmapSize)
    return( FALSE);

  bmpBitmapSave    = bmpBitmapFile;
  bmpBitmapSave.cx = LOUSHORT( sizlMaxClient.cx);
  bmpBitmapSave.cy = LOUSHORT( sizlMaxClient.cy);
  hbmBitmapSave    = GpiCreateBitmap( hpsBitmapSave
				    , &bmpBitmapSave
				    , 0L
				    , NULL
				    , NULL);
  if( !hbmBitmapSave)
    return( FALSE);
  GpiSetBitmap( hpsBitmapSave, hbmBitmapSave);

  hbmBitmapBuff    = GpiCreateBitmap( hpsBitmapBuff
				    , &bmpBitmapSave
				    , 0L
				    , NULL
				    , NULL);
  if( !hbmBitmapBuff)
    return( FALSE);
  GpiSetBitmap( hpsBitmapBuff, hbmBitmapBuff);

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
		   , PU_PELS | GPIA_ASSOC | GPIT_MICRO );
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
