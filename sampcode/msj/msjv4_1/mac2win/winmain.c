/****************************************************************************
		WINMAIN.C
**********************************************************************®*****/

#ifdef AS
#include	<memory.h>
#endif

#include	"config.h"

#define NOPOLYGON
#define NOMINMAX
#define NOSOUND
#define NOCOMM
#define NOKANJI

#include	"windows.h"
#include	"wgenapp.h"

#include	"stddefs.h"

#include 	"applcatn.h"
#include	"envrnmt.h"

/****************************************************************/

PUBLIC int  PASCAL	WinMain(HANDLE, HANDLE, LPSTR, int);

PUBLIC LONG WCALLBACK	win_ChildWndProc(HWND,int,WORD,LONG);
PUBLIC LONG WCALLBACK	win_MainWndProc(HWND,int,WORD,LONG);

PUBLIC BOOL WCALLBACK	dlg_AboutBoxProc(HWND,unsigned,WORD,LONG);
PUBLIC VOID		win_UpdateAppWndSize(Rect *);

/****************************************************************/

PUBLIC	VOID		win_ScreenToAppl		(POINT *);

LOCAL	BOOL		win_FirstTimeInitialization	(HANDLE);
LOCAL	BOOL		win_EveryTimeInitialization	(HANDLE);
LOCAL	VOID		win_DoCommandMsg		(HANDLE,WORD);

LOCAL	VOID		win_MakeCmdEvent		(HWND,int,int);
LOCAL	VOID		win_MakeCharEvent		(HWND,int);
LOCAL	VOID		win_MakeMouseEvent		(HWND,int,int,int);
LOCAL	VOID		win_MakeAppEvent		(HWND,int);


/****************************************************************/

LOCAL	HANDLE		hAppInstance;
LOCAL	HWND		hAppWnd;

LOCAL	HCURSOR		hArrowCursor;
LOCAL	HCURSOR		hIBeamCursor;     
LOCAL	HCURSOR		hWaitCursor;      

LOCAL	BOOL		bIsFirstTime = TRUE;
LOCAL	BOOL		bIsMouseDown = FALSE;
LOCAL	BOOL		bIsAppEvent = FALSE;

LOCAL	RECT		appWndRect;	/* in CLIENT coords */
LOCAL	RECT		appScrRect;	/* in SCREEN coords */

/****************************************************************
    QUERY FUNCTIONS provide private data to other modules
****************************************************************/

PUBLIC HANDLE win_GethAppInstance() { return hAppInstance; }
PUBLIC HWND   win_GethAppWnd     () { return hAppWnd     ; }
PUBLIC VOID   win_GetAppWndRect(r) RECT *r; { *r = appWndRect; }

/****************************************************************
    EVENT PROCESSING build application-level event structure
*****************************************************************/

LOCAL APPEVENT * theAppEvent;

PUBLIC VOID win_GetAppEvent(the_event) /*called from application*/
    APPEVENT *		the_event;
{
    MSG			msg;

    bIsAppEvent = FALSE;
    theAppEvent = the_event;/* store ptr so MakeAppEvent can find it*/

    /* this is the internal event loop, which runs until an
     * event of interest to the application-level is encountered.
     */
    while (GetMessage((LPMSG) &msg, NULL, 0, 0) && !bIsAppEvent)
    {
	    TranslateMessage((LPMSG) &msg);
	    DispatchMessage((LPMSG)  &msg);
    }
    ut_PrintShort("RV|winGetAppEvent: returns type#",the_event->event_type);
}

/****************************************************************
    These little functions build different types of app events.
    (Could have been done with in-line code. I placed them here
    for consistency.)
*****************************************************************/

LOCAL VOID win_MakeCmdEvent(hWnd,category,item)
    HWND	hWnd;
    int		category,item;
{
    theAppEvent->event_type	= CMD_EVENT;
    theAppEvent->cmd_category	= category;
    theAppEvent->cmd_item	= item;
    theAppEvent->canvas		= GetWindowWord(hWnd,0);
    bIsAppEvent = TRUE;
}

LOCAL VOID win_MakeCharEvent(hWnd,the_char)
    HWND	hWnd;
    int		the_char;
{
    theAppEvent->event_type	= CHAR_EVENT;
    theAppEvent->the_char	= the_char;
    theAppEvent->canvas		= GetWindowWord(hWnd,0);
    bIsAppEvent = TRUE;
}

LOCAL VOID win_MakeMouseEvent(hWnd,type,x,y)
    HWND	hWnd;
    int		type,x,y;
{
    theAppEvent->event_type	= type;
    theAppEvent->where.h	= x;
    theAppEvent->where.v	= y;    
    theAppEvent->canvas		= GetWindowWord(hWnd,0);
    bIsAppEvent = TRUE;
}

LOCAL VOID win_MakeAppEvent(hWnd,type)
    HWND	hWnd;
    int		type;
{
    ut_PrintShort("RV|MakeAppEvent: type#",type);
    theAppEvent->event_type	= type;
    theAppEvent->canvas		= GetWindowWord(hWnd,0);
    bIsAppEvent = TRUE;
}

/****************************************************************
    Handle windows command events. Translate to application level.
*****************************************************************/
LOCAL VOID win_DoCommandMsg(hWnd, cmd)
    HWND hWnd;	
    WORD cmd;
{
#define DEBUG_MENU 9000

    ut_PrintShort("RV|win_DoCommandMsg: cmd",cmd);
#ifdef DBG
    if(ut_DebugMenuFilter(cmd - DEBUG_MENU))	return;	
#endif
    win_MakeCmdEvent(hWnd, (cmd/100)*100 , cmd%100);
    return;
}



/******************************************************************
	ABOUT BOX DIALOG HANDLER
*******************************************************************/	

#define SIZEOF_ABOUTSTR 10
LOCAL CHAR 	AboutString[SIZEOF_ABOUTSTR];/* about str in sys menu */
LOCAL FARPROC	lpprocAbout;
LOCAL WORD	AboutDlgFlag;

/****************************************************************/
PUBLIC BOOL WCALLBACK dlg_AboutBoxProc( hDlg, message, wParam, lParam )
    HWND		hDlg;
    unsigned		message;
    WORD		wParam;
    LONG		lParam;
{
    if (message == WM_COMMAND) 
    {
	EndDialog( hDlg, AboutDlgFlag );
	return TRUE;
    }
    else if (message == WM_INITDIALOG)
	return TRUE;
    else return FALSE;
}

/****************************************************************/
LOCAL BOOL dlg_InitAboutBox(hInstance,hAppWnd)

    HANDLE	hInstance;
    HWND	hAppWnd;
{
    HMENU	hMenu;
    lpprocAbout = MakeProcInstance( (FARPROC)dlg_AboutBoxProc, hInstance );

    LoadString(hInstance,	        /* handle to the instance */
           IDS_ABOUT,          /* unsigned short defined in design.h */
           (LPSTR)AboutString, /* lp to buffer where string is loaded */
           SIZEOF_ABOUTSTR);  /* max no. of chars to go into the buffer */

    hMenu = GetSystemMenu(hAppWnd,FALSE); /* get handle to the system menu */
    if(hMenu==NULL) return FALSE;
    ChangeMenu(hMenu, 0, NULL, 999, MF_APPEND | MF_SEPARATOR);
    ChangeMenu(hMenu, 0, (LPSTR)AboutString, ABOUT_SELECT, 
				MF_APPEND | MF_STRING);
    return TRUE;
}


/****************************************************************
    WINMAIN
*****************************************************************/

PUBLIC int PASCAL WinMain(hThisInstance, hPrevInstance, lpszCmdLine, cmdShow)

    HANDLE	hThisInstance;
    HANDLE	hPrevInstance;
    LPSTR	lpszCmdLine;
    int		cmdShow;
{
#define EXTRA_BYTES	sizeof(int)	/*extrabytes in window class */

    /* If there is no previous instance of this app, do initialization */
    if (!hPrevInstance)
    {
	if (!win_FirstTimeInitialization(hThisInstance))	return FALSE;
    }
    else
    {
	/* Initialization for subsequent instances only */
	/* Copy data from previous instance */
	GetInstanceData( hPrevInstance, AboutString, SIZEOF_ABOUTSTR);
    }

    if(!win_EveryTimeInitialization(hThisInstance))		return FALSE;
    /* above has set up hAppWnd */
    ShowWindow(  hAppWnd, cmdShow);
    app_MainProcedure();
    exit(0);
}

/****************************************************************/

PUBLIC BOOL within_exit_sequence = FALSE; /*for displaying debug msgs*/
    

PUBLIC LONG WCALLBACK win_MainWndProc(hWnd, message, wParam, lParam) 

    HWND	hWnd;
    int		message;
    WORD	wParam;
    LONG	lParam;
{
    if (message != WM_NCHITTEST 	/*don't trace these frequent msgs*/
	&& message != WM_MOUSEMOVE
	&& message != WM_NCMOUSEMOVE 
	&& message != WM_DESTROY 
	&& message != WM_NCDESTROY
	&& !within_exit_sequence)
    {
	ut_Print2Hex("RV|MainWndProc: hAppWnd hAppInstance",
		    (long)hAppWnd,(long)hAppInstance);
	ut_Print4Hex("RV|MainWndProc: hWnd message w l",
		    (long)hWnd,(long)message,(long)wParam,(long)lParam);
    }

    hAppWnd = hWnd;

    switch (message)
    {
    case WM_SYSCOMMAND:
        {
           switch (wParam & 0xfff0)
           {
           case ABOUT_SELECT:
		 AboutDlgFlag = DialogBox(GetWindowWord(hWnd, GWW_HINSTANCE),
					 MAKEINTRESOURCE(IDD_ABOUT_BOX),
					 hWnd,lpprocAbout);
		 break;

           default:   return DefWindowProc(hWnd, message, wParam, lParam);
           } 
        } break;

    case WM_CREATE:   
	{
	    if (bIsFirstTime == TRUE)
	    {
		win_UpdateAppWndSize((Rect *)NULL);
		
#ifdef DBG
		if(!ut_CreateDBugWindow(hWnd, 0)) FatalExit(0x111);
		    /*FatalError("ML|MainWndProc: CreateDBG failed!");*/
#endif
		bIsFirstTime = FALSE;

	    }
	    ut_PutLine("RV|MainWndProc: WM_CREATE");
	} break;
	    

    case WM_CHAR:
	    {
		ut_Print2Hex("RV|MainWndProc: WM_CHAR w l",
				(long)wParam,(long)lParam);
		win_MakeCharEvent(hWnd, wParam);
	    }  break;	

    case WM_DESTROY:
	{
	    within_exit_sequence = TRUE;
	    PostQuitMessage(0);	    
	    win_MakeAppEvent(hWnd, QUIT_EVENT);	    
	} break;

    case WM_COMMAND: /* check if its not a menu selection */
	{
	    ut_Print2Hex("RV|MainWndProc: WM_COMMAND: w l",
		(long)wParam,(long)lParam);
	    if (lParam != 0)  
		return DefWindowProc(hWnd, message, wParam, lParam);
	    /*else*/
	    win_DoCommandMsg(hWnd, wParam);
	} break;

    case WM_SIZE:
    case WM_MOVE:
	{
		win_UpdateAppWndSize((Rect *)NULL);
	} break;

    case WM_PAINT:
	{
	    PAINTSTRUCT		ps;
	    HDC			hDC;
	    
	    BeginPaint(hWnd, (LPPAINTSTRUCT) & ps);
#if 0 /*for testing*/
	    hDC = ps.hdc;
	    TextOut(hDC,40,20,"WITHIN THE PAINT MSG",
		strlen("WITHIN THE PAINT MSG"));
	    ut_PutLine("RV|MainWndProc: WM_PAINT");
	    /*ValidateRect(hWnd,(LPRECT)NULL);<<<necessary?<<<*/
#endif
	    EndPaint(hWnd, (LPPAINTSTRUCT) & ps);
	}
	break;

    case WM_LBUTTONDOWN:
	{
	    int x = LOWORD(lParam);
	    int y = HIWORD(lParam);

	    ut_Print2Shorts("RV|MainWndProc: LBUTTONDOWN",x,y);
	} break;

    default:
		/* Anything else is thrown away. */
	return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return (long)FALSE;
}

/****************************************************************/

PUBLIC LONG WCALLBACK win_ChildWndProc(hWnd, message, wParam, lParam)

    HWND		hWnd;
    int			message;
    WORD		wParam;
    long		lParam;
{
    extern BOOL window_is_being_created;
    extern long env_GetCanvasDev();
    static int prev_canvas = -1;
    POINT		pt;
    int			canvas;
    
    canvas = GetWindowWord(hWnd,0); /* get extra bytes in ms-window struc */

    if (message != WM_NCHITTEST /*0x84*/ 
	&& message != WM_MOUSEMOVE /*0x200*/
	&& message != WM_NCMOUSEMOVE /*0xa0*/)
    {
	ut_Print2Hex("RV|ChildWndProc: hAppInst hAppWnd",
		    (long)hAppInstance,(long)hAppWnd);
	ut_Print4Hex("RV|ChildWndProc: message w l hWnd",
		    (long)message,(long)wParam,(long)lParam,(long)hWnd);
	ut_PrintShort("RV|ChildWndProc: canvas#",canvas);
    }

    if( window_is_being_created)
	    return DefWindowProc(hWnd, message, wParam, lParam);

    pt = MAKEPOINT(lParam);
    ClientToScreen(hWnd, (LPPOINT) &pt);
    win_ScreenToAppl(&pt); /* point is now in "mac global" coordinates */

		    
    switch (message)	/* we can now switch on message type */
    {
	case WM_LBUTTONDOWN:
	{
	    bIsMouseDown = TRUE;

	    ut_Print2Shorts("RV|ChildWndProc: PAGE mse DOWN",
		    (short) LOWORD(lParam), (short) HIWORD(lParam));

	    ut_Print2Shorts("RV|ChildWndProc: canvas, prev_canvas",
			    canvas,prev_canvas);

	    if (canvas != prev_canvas)	/* they have to click on it */
	    {
		ut_PrintShort("RV|ChildWndProc: ActivateVIEW canvas#",canvas);
		prev_canvas = canvas;
		/*return DefWindowProc(hWnd, message, wParam, lParam);*/
		win_MakeAppEvent(hWnd,ACTIVATEVIEW_EVENT);
	    }
	    else
	    {
		/*SetCapture(hWnd);*/
		win_MakeMouseEvent(hWnd, MOUSEDOWN_EVENT,pt.x,pt.y);
	    }
	    return (long)TRUE;
	}   break;

	case WM_SIZE:
	{
	    win_MakeAppEvent(hWnd, RESIZEVIEW_EVENT);
	}   break;


	case WM_LBUTTONUP:
	{
	    bIsMouseDown = FALSE;
	    /*ReleaseCapture();*/
	    win_MakeMouseEvent(hWnd, MOUSEUP_EVENT,pt.x,pt.y);
	    return (long)TRUE;
	}   break;

	case WM_PAINT:
	{
	    ut_PrintShort("RV|ChildWndProc: WM_PAINT on canvas#",canvas);
	    win_MakeAppEvent(hWnd, UPDATEVIEW_EVENT);
	    return (long)TRUE;
	}   break;
#if 0 /*---- other cases to handle in the future---*/
	case WM_CHAR:	/* Key Hit */
	{
	    win_MakeCharEvent(hWnd, wParam);
	} break;
	
	case WM_CLOSE:
	{
	    win_MakeAppEvent(hWnd, CLOSEVIEW_EVENT);
	    ShowWindow(hWnd, HIDE_WINDOW);
	    return (long)TRUE;
	} break;

	case WM_SIZE:
	{
	    win_UpdatePageWindowSize(argument needed)
	    win_MakeAppEvent(hWnd, RESIZEVIEW_EVENT);
	}   break;

	case WM_MOUSEMOVE:
	{
	    win_MakeMouseEvent(hWnd, NULL_EVENT,pt.x,pt.y);
	}   break;
#endif
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}


/****************************************************************/
LOCAL BOOL  win_FirstTimeInitialization(hInstance)
    HANDLE hInstance;
{
    /* register the three types of windows: main, page and debug */
    WNDCLASS wcMainWindow, wcChildWindow;

    memset(&wcMainWindow,0,sizeof(WNDCLASS));
    memset(&wcChildWindow,0,sizeof(WNDCLASS));    
    
    wcMainWindow.hCursor	= (HANDLE) NULL;
    wcMainWindow.hCursor	= LoadCursor(NULL, 
				    MAKEINTRESOURCE(IDC_ARROW));
    wcMainWindow.hIcon		= LoadIcon(hInstance, (LPSTR) "GENAPP");
    wcMainWindow.lpszMenuName	= (LPSTR) "GENAPP";
    wcMainWindow.lpszClassName	= (LPSTR) "GENAPP";
    wcMainWindow.hbrBackground	= GetStockObject(WHITE_BRUSH);
    wcMainWindow.hInstance	= hInstance;
    wcMainWindow.style		= CS_VREDRAW | CS_HREDRAW;
    wcMainWindow.lpfnWndProc	= win_MainWndProc;
    wcMainWindow.cbWndExtra	= EXTRA_BYTES;

    if (!RegisterClass((LPWNDCLASS) &wcMainWindow))	return FALSE;

    wcChildWindow.hCursor	= (HANDLE) NULL;
    wcChildWindow.hIcon		= (HANDLE) NULL;
    wcChildWindow.lpszMenuName	= (LPSTR) NULL;
    wcChildWindow.lpszClassName	= (LPSTR) "Child";
    wcChildWindow.hbrBackground	= GetStockObject(WHITE_BRUSH);
    wcChildWindow.hInstance	= hInstance;
    wcChildWindow.style		= CS_VREDRAW | CS_HREDRAW 
				    | CS_DBLCLKS;
    wcChildWindow.lpfnWndProc	= win_ChildWndProc;
    wcChildWindow.cbWndExtra	= EXTRA_BYTES;

    if (!RegisterClass((LPWNDCLASS) &wcChildWindow))	return FALSE;

#ifdef DBG
    if(!ut_RegisterDBugWindow(hInstance,EXTRA_BYTES))	return FALSE;
#endif
    /* else */
    return TRUE;
}

/****************************************************************/
LOCAL BOOL win_EveryTimeInitialization(hInstance)
    HANDLE hInstance;
{
    hAppInstance = hInstance;

    hAppWnd = CreateWindow(
		(LPSTR) "GENAPP",	/* The class name   */
		(LPSTR) "GENAPP",	/* The window instance name */
		WS_TILEDWINDOW | WS_CLIPCHILDREN,/* Window type = tiled. */
#ifdef AS
		CW_USEDEFAULT,0,CW_USEDEFAULT,0,
#else
		0, 0, 0, 0,	/* These dont apply for tiled windows. */
#endif			
		(HANDLE) NULL,	/* NULL for tiled. */
		(HANDLE) NULL,	/* NULL means use the class menu. */
		(HANDLE) hAppInstance,	/* Instance handle passedto WinMain */
		NULL	/* Param passed thru WM_CREATE message, not needed. */
	);

    if (hAppWnd == NULL) return FALSE;

    SetWindowWord(hAppWnd, 0, (WORD) -1);

    /* bring in cursors */
    hArrowCursor      = LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW));
    hIBeamCursor      = LoadCursor(NULL,MAKEINTRESOURCE(IDC_IBEAM));
    hWaitCursor       = LoadCursor(NULL,MAKEINTRESOURCE(IDC_WAIT));

    if(!dlg_InitAboutBox(hAppInstance,hAppWnd)) return FALSE;
    return TRUE;
}


/***************************************************************
	    SCREEN TO APPLICATION COORDINATE MAPPING
 ***************************************************************/	    

/****************************************************************/
PUBLIC VOID win_ScreenToAppl(pt)
    POINT *pt;
{
    pt->y -= appScrRect.top;
    pt->x -= appScrRect.left;
}

/****************************************************************/
PUBLIC VOID		win_UpdateAppWndSize(mac_rect)
    Rect *		mac_rect;
{
    POINT		p1,p2;
    
    GetClientRect(hAppWnd, (LPRECT) & appWndRect);

    p1.x = appWndRect.left;	p1.y = appWndRect.top;
    p2.x = appWndRect.right;	p2.y = appWndRect.bottom;

    ClientToScreen(hAppWnd, (LPPOINT) &p1);
    ClientToScreen(hAppWnd, (LPPOINT) &p2);    

    appScrRect.left  = p1.x;	appScrRect.top    = p1.y;
    appScrRect.right = p2.x;	appScrRect.bottom = p2.y;
    
    ut_Print4Shorts("RV|win_UpdateAppWndSz: SCR rect", 
	    appScrRect.left,  appScrRect.top,
	    appScrRect.right, appScrRect.bottom);

    ut_Print4Shorts("RV|win_UpdateAppWndSz: WND rect", 
	    appWndRect.left,  appWndRect.top,
	    appWndRect.right, appWndRect.bottom);	

    if(mac_rect != (Rect *)NULL)
    {
	mac_rect->left   = appWndRect.left;
	mac_rect->top    = appWndRect.top;
	mac_rect->right  = appWndRect.right;
	mac_rect->bottom = appWndRect.bottom;
    }
}

#ifndef DBG
PUBLIC VOID env_FatalError(){FatalExit(0x111);}
PUBLIC VOID env_NotImplemented() {;}
#endif



