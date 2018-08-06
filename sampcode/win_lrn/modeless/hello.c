/*  Hello.c
    Hello Application
    Windows Version 1.01
    Copyright (c) Microsoft 1985

This application has been  revised  to  demonstrate  the  coding 
differences between the two Dialog boxes: 
   MODAL, created  using DialogBox and 
   MODELESS, created using CreateDialog.  
The  application  starts  out by having the user select the "GO" 
menu item which creates the "First Dialog"  and  then  the  DEMO 
button  then  creates  the "Second Dialog".  The "Second Dialog" 
then allows you to push some buttons and process them.  

To point out the differences in coding,  there are  "#ifdef"  to 
allow  you  to  compile using the desired method and to visually 
see what needs to be done when using one method over the  other.  
This  information  is  an  expanded implementation of the sample 
code on pages 169-173 of the Programming Guide.  The default  is 
set to compile using MODAL dialog boxes.  If you want to compile 
using the MODELESS dialog boxes, change "MODAL" to "XMODAL".*/ 

#define XMODAL /* default */

#include "windows.h"
#include "hello.h"

char szAppName[10];
char szAbout[10];
char szMessage[15];
int  MessageLength;

#ifdef MODAL
#else
HWND  hDlgWnd1;
HWND  hDlgWnd2;
#endif
static  HANDLE hInst;
FARPROC lpprocAbout;
FARPROC lpprocFirstDlg;
FARPROC lpprocSecondDlg;

long FAR PASCAL HelloWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL     FirstDlg(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL    SecondDlg(HWND, unsigned, WORD, LONG);

BOOL FAR PASCAL About( hDlg, message, wParam, lParam )
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    if (message == WM_COMMAND) {
        EndDialog( hDlg, TRUE );
        return TRUE;
        }
    else if (message == WM_INITDIALOG)
        return TRUE;
    else return FALSE;
}


void HelloPaint( hDC )
HDC hDC;
{
    TextOut( hDC, (short)10, (short)10,
#ifdef MODAL
           (LPSTR)"DialogBox = Modal",(short)17 );
#else
           (LPSTR)"CreateDialog = Modeless",(short)23 );
#endif
}

/* Procedure called when the application is loaded for the first time */
BOOL HelloInit( hInstance )
HANDLE hInstance;
{
    PWNDCLASS   pHelloClass;

    /* Load strings from resource */
    LoadString( hInstance, IDSNAME, (LPSTR)szAppName, 10 );
    LoadString( hInstance, IDSABOUT, (LPSTR)szAbout, 10 );
    MessageLength = LoadString( hInstance, IDSTITLE, (LPSTR)szMessage, 15 );

    pHelloClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

    pHelloClass->hCursor        = LoadCursor( NULL, IDC_ARROW );
    pHelloClass->hIcon          = LoadIcon( hInstance, (LPSTR)szAppName );
    pHelloClass->lpszMenuName   = (LPSTR)szAppName;
    pHelloClass->lpszClassName  = (LPSTR)szAppName;
    pHelloClass->hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    pHelloClass->hInstance      = hInstance;
    pHelloClass->style          = CS_HREDRAW | CS_VREDRAW;
    pHelloClass->lpfnWndProc    = HelloWndProc;

    if (!RegisterClass( (LPWNDCLASS)pHelloClass ) )
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
        return FALSE;

    LocalFree( (HANDLE)pHelloClass );
    return TRUE;        /* Initialization succeeded */
}


int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    BOOL  bResult;
    MSG   msg;
    HWND  hWnd;
    HMENU hMenu;

    if (!hPrevInstance) {
        /* Call initialization procedure if this is the first instance */
        if (!HelloInit( hInstance ))
            return FALSE;
        }
    else {
        /* Copy data from previous instance */
        GetInstanceData( hPrevInstance, (PSTR)szAppName, 10 );
        GetInstanceData( hPrevInstance, (PSTR)szAbout, 10 );
        GetInstanceData( hPrevInstance, (PSTR)szMessage, 15 );
        GetInstanceData( hPrevInstance, (PSTR)&MessageLength, sizeof(int) );
        }

    hWnd = CreateWindow((LPSTR)szAppName,
                        (LPSTR)szMessage,
                        WS_TILEDWINDOW,
                        0,    /*  x - ignored for tiled windows */
                        0,    /*  y - ignored for tiled windows */
                        0,    /* cx - ignored for tiled windows */
                        0,    /* cy - ignored for tiled windows */
                        (HWND)NULL,        /* no parent */
                        (HMENU)NULL,       /* use class menu */
                        (HANDLE)hInstance, /* handle to window instance */
                        (LPSTR)NULL        /* no params to pass on */
                        );

    /* Save instance handle for DialogBox */
    hInst = hInstance;

    /* Bind callback function with module instance */
        lpprocAbout = MakeProcInstance( (FARPROC)About,     hInstance );
     lpprocFirstDlg = MakeProcInstance( (FARPROC)FirstDlg,  hInstance );
    lpprocSecondDlg = MakeProcInstance( (FARPROC)SecondDlg, hInstance );

    /* Insert "About..." into system menu */
    hMenu = GetSystemMenu(hWnd, FALSE);
    ChangeMenu(hMenu, 0, NULL, 999, MF_APPEND | MF_SEPARATOR);
    ChangeMenu(hMenu, 0, (LPSTR)szAbout, IDSABOUT, MF_APPEND | MF_STRING);

    /* Make window visible according to the way the app is activated */
    ShowWindow( hWnd, cmdShow );
    UpdateWindow( hWnd );

    /* Polling messages from event queue */
#ifdef MODAL
    while (GetMessage((LPMSG)&msg, NULL, 0, 0)) {
        TranslateMessage((LPMSG)&msg);
        DispatchMessage((LPMSG)&msg);
        }
#else
    /* The following can be rewritten in many ways, but was made 
       for clairity and for easy modification.
       Since this program does not use an Accelerator Table the 
       code has been commented out.  I thought I should put it 
       in for completness.  */ 

    while (GetMessage((LPMSG)&msg, NULL, 0, 0)) {
        bResult = NULL; /* default is that the message has not been processed */
        if (hDlgWnd1 != NULL)
           bResult = IsDialogMessage(hDlgWnd1, (LPMSG)&msg);
        if (hDlgWnd2 != NULL)
           bResult = IsDialogMessage(hDlgWnd2, (LPMSG)&msg);
     /* if (!bResult)  /* message not processed by above dialog(s) */
     /*    bResult = TranslateAccelerator(hWnd, hAccelTable, (LPMSG)&msg );
     */
        if (!bResult) {  /* the parent will get the message */
             TranslateMessage((LPMSG)&msg);
             DispatchMessage((LPMSG)&msg);
        } /* end if */
    } /* end while */
#endif
    return (int)msg.wParam;
}


long FAR PASCAL HelloWndProc( hWnd, message, wParam, lParam )
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    PAINTSTRUCT ps;

    switch (message)
    {
    case WM_SYSCOMMAND:
        switch (wParam)
        {
        case IDSABOUT:
            DialogBox( hInst, MAKEINTRESOURCE(ABOUTBOX), hWnd, lpprocAbout );
            break;
        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
        }
        break;

    case WM_CREATE:
#ifdef MODAL
#else
    hDlgWnd1 = NULL;  /* initialize the first time through */
    hDlgWnd2 = NULL;  /* initialize the first time through */
#endif
        break;

    case WM_COMMAND:
#ifdef MODAL
        DialogBox( hInst, MAKEINTRESOURCE(FIRSTDIALOG),
                   hWnd, lpprocFirstDlg);
#else
        hDlgWnd1 = CreateDialog( hInst, MAKEINTRESOURCE(FIRSTDIALOG),
                                 hWnd, lpprocFirstDlg);
#endif
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    case WM_PAINT:
        BeginPaint( hWnd, (LPPAINTSTRUCT)&ps );
        HelloPaint( ps.hdc );
        EndPaint( hWnd, (LPPAINTSTRUCT)&ps );
        break;

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
        break;
    }
    return(0L);
}

/**********************************************************
                  First  Dialog
***********************************************************/
BOOL FAR PASCAL FirstDlg(hDlg, message, wParam, lParam)
HWND	   hDlg;
unsigned   message;
WORD       wParam;
LONG       lParam;
{
    switch (message){
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        switch (wParam)
        {
        case IDDINSERT:
#ifdef MODAL
             DialogBox(hInst, MAKEINTRESOURCE(SECONDDIALOG),
		       hDlg, lpprocSecondDlg);
             /* please note that the Dialog in the RC file does 
                NOT have the WS_VISIBLE attribute set which will
                allow for keyboard processing BEFORE the dialog 
                is displayed.                                   */
#else
	     hDlgWnd2 = CreateDialog(hInst, MAKEINTRESOURCE(SECONDDIALOG),
				     hDlg, lpprocSecondDlg);
             /* ShowWindow is needed here because the dialog 
                style doesn't have WM_VISIBLE bit set.  */
             ShowWindow(hDlgWnd2,SHOW_OPENWINDOW);
#endif
            break;
        case IDDCANCEL:
#ifdef MODAL
	    EndDialog(hDlg, TRUE);
#else
	    DestroyWindow(hDlg);
	    hDlgWnd1 = NULL; /* clear because this is used in the input loop */
#endif
	    break;

        default:
            return FALSE;
        }
        break;

    default:
        return FALSE;
    }
    return TRUE;
}


/**********************************************************
                  Second Dialog
***********************************************************/
BOOL FAR PASCAL SecondDlg(hDlg, message, wParam, lParam)
HWND	   hDlg;
unsigned   message;
WORD       wParam;
LONG       lParam;
{

  switch (message){
     case WM_INITDIALOG:
      /* Modify the text strings. */
	 SetDlgItemText( hDlg, IDDTXT1,
             (LPSTR)"Use the 'TAB' key to move around."); 
	 SetDlgItemText( hDlg, IDDTXT2,
             (LPSTR)"Select the button desired."); 

      /* Modify the button text. */
	 SetDlgItemText( hDlg, IDDABORT, (LPSTR)"ONE");
	 SetDlgItemText( hDlg, IDDRETRY, (LPSTR)"TWO");
	 SetDlgItemText( hDlg, IDDIGNORE,(LPSTR)"THREE");
	 SetDlgItemText( hDlg, IDDOK,	 (LPSTR)"FOUR");

      /* Disable the 2nd button */
	 EnableWindow( GetDlgItem(hDlg, IDDRETRY), FALSE );

         break;

     case WM_COMMAND:
         switch (wParam)
         {
           case IDDABORT:
	     MessageBox( hDlg,(LPSTR)"One",(LPSTR)"Button Selected",MB_OK);
             break;
           case IDDRETRY:
	     MessageBox( hDlg,(LPSTR)"Two",(LPSTR)"Button Selected",MB_OK);
             break;
           case IDDIGNORE:
	     MessageBox( hDlg,(LPSTR)"Three",(LPSTR)"Button Selected",MB_OK);
             break;
           case IDDOK:
	     MessageBox( hDlg,(LPSTR)"Four",(LPSTR)"Button Selected",MB_OK);
             break;
	   default: ;  /* nothing */
		break;
	 }  /* end switch */

    case WM_CLOSE:

#ifdef MODAL
	 EndDialog(hDlg, TRUE);
#else
	 DestroyWindow(hDlg);
	 hDlgWnd2 = NULL; /* clear because this is used in the input loop */
#endif

	break;

     default:
         return FALSE;
   }
   return TRUE;
}
