/*
 * COLORS.C - Colorful MDI Children
 *
 * LANGUAGE      : Microsoft C5.1
 * MODEL         : medium
 * ENVIRONMENT   : Microsoft Windows 2.1 SDK
 * STATUS        : operational
 *
 * This module contains all the application code for the COLORS
 * application.  This application handles an MDI desktop on which
 * any number of colored documents reside.  Documents are either
 * red, green, or blue and can be shaded from 0% of that color to
 * 100% of that color (in increments of 25%).  In order to demonstrate
 * acclerators, only the blue documents have accelerator keys.
 *
 * Developed by:
 *   Geoffrey Nicholls
 *   Kevin P. Welch
 *
 * (C) Copyright 1988
 * Eikon Systems, Inc.
 * 989 E. Hillsdale Blvd, Suite 260
 * Foster City  CA  94404
 *
 */

#include <string.h>
#include <stdio.h>
#include <windows.h>

#include "colors.h"
#include "mdi.h"

/**/

/*
 *
 * Static variables
 *
 */

/* Text for client area */
static char    *szShadings[5] = {"0 %", "25 %", "50 %", "75 %", "100 %"};

/* Titles of documents */
static char    *szTitles[3] = { "Red", "Green", "Blue" };

/* Count of each document (for titles) */
static int     wCounts[3] = { 0, 0, 0 };

/* Color & Shading table */
static DWORD   rgbColors[3][5] = {
                  {  /* RED */
                  RGB(255,255,255),    /*   0 % */
                  RGB(255,192,192),    /*  25 % */
                  RGB(255,128,128),    /*  50 % */
                  RGB(255,64,64),      /*  75 % */
                  RGB(255,0,0)         /* 100 % */
                  },
                  {  /* GREEN */
                  RGB(255,255,255),    /*   0 % */
                  RGB(192,255,192),    /*  25 % */
                  RGB(128,255,128),    /*  50 % */
                  RGB(64,255,64),      /*  75 % */
                  RGB(0,255,0)         /* 100 % */
                  },
                  {  /* BLUE */
                  RGB(255,255,255),    /*   0 % */
                  RGB(192,192,255),    /*  25 % */
                  RGB(128,128,255),    /*  50 % */
                  RGB(64,64,255),      /*  75 % */
                  RGB(0,0,255)         /* 100 % */
                  } };

/**/

/*
 * PASCAL WinMain( hPrevInst, hInst, lpszCommand, nCmdShow ) : int;
 *
 *    hPrevInst      Previous instance handle
 *    hInst          Current instance handle
 *    lpszCmdLine    Pointer to command line arguments
 *    nCmdShow       ShowWindow() parameter
 *
 * First routine called by windows.  Calls the initialization routine
 * and contains the message loop.
 *
 */

int PASCAL WinMain(
   HANDLE      hInst,
   HANDLE      hPrevInst,
   LPSTR       lpszCmdLine,
   int         nCmdShow )
{
   HWND        hwndColors;       /* Handle to our MDI desktop */
   MSG         msg;              /* Current message */

   /* Initialize things needed for this application */
   hwndColors = MainInit( hPrevInst, hInst, nCmdShow );
   if ( !hwndColors )
   {
      /* Failure to initialize */
      return NULL;
   }

   /* Process messages */
   while ( MdiGetMessage( hwndColors, &msg, NULL, NULL, NULL ) )
   {
      /* Normal message processing */
      if ( !MdiTranslateAccelerators( hwndColors, &msg ) )
      {
         TranslateMessage( &msg );
         DispatchMessage( &msg );
      }
   }

   /* Done */
   return msg.wParam;
}

/**/

/*
 * MainInit( hPrevInst, hInst, nCmdShow ) : HWND;
 *
 *    hPrevInst      Previous instance handle
 *    hInst          Current instance handle
 *    nCmdShow       ShowWindow() parameter
 *
 * First, initialize the MDI desktop and the color document windows.
 * Second, create the MDI desktop and then create one red document.
 *
 */

HWND MainInit(
   HANDLE      hPrevInst,
   HANDLE      hInst,
   int         nCmdShow )
{
   char        szTitle[80];      /* Title of our MDI desktop */
   char        szClass[80];      /* Class name of our MDI desktop */
   HWND        hwndColors;       /* Handle to our MDI desktop */
   WNDCLASS    WndClass;         /* Class structure */

   /* Window classes */
   if ( !hPrevInst )
   {
      /* Main window */
      LoadString( hInst, IDS_MAINCLASS, szClass, sizeof( szClass ) );

      /* Prepare registation */
      memset( &WndClass, 0, sizeof( WndClass ) );
      WndClass.style          = CS_HREDRAW | CS_VREDRAW;
      WndClass.lpfnWndProc    = MainWndProc;
      WndClass.hInstance      = hInst;
      WndClass.hIcon          = LoadIcon( hInst, "MainIcon" );
      WndClass.hCursor        = LoadCursor( NULL, IDC_ARROW );
      WndClass.hbrBackground  = COLOR_APPWORKSPACE + 1;
      WndClass.lpszMenuName   = "MainMenu";
      WndClass.lpszClassName  = szClass;

      /* Register main class */
      if ( !RegisterClass( &WndClass ) )
         return NULL;

      /* Allow each of the MDI children to do their own initialize */
      if ( !ColorInit(hInst) )
         return NULL;
   }

   /* Create our main overlapped window */
   LoadString( hInst, IDS_TITLE, szTitle, sizeof( szTitle ) );
   hwndColors = MdiMainCreateWindow( szClass,
      szTitle, 
      WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
      NULL,
      NULL,
      hInst,
      NULL );

   /* Did we create successfully? */
   if ( !hwndColors )
      return NULL;

   /* Give us one red child to begin with */
   if ( !ColorCreate( hwndColors, COLOR_RED ) )
   {
      DestroyWindow( hwndColors );
      return NULL;
   }

   /* Ready */
   ShowWindow( hwndColors, nCmdShow );
   UpdateWindow( hwndColors );

   /* Done */
   return hwndColors;
}

/**/

/*
 * MainWndProc( hwndMain, message, wParam, lParam ) : LONG;
 *
 *    hwndColors     Handle to our MDI desktop
 *    message        Current message
 *    wParam         Word parameter to the message
 *    lParam         Long parameter to the message
 *
 * Handle messages for our MDI desktop.  This includes any WM_COMMAND
 * messages that are received when NO document is visible on the
 * desktop.
 *
 */

long FAR PASCAL MainWndProc(
   HWND        hwndColors,
   unsigned    message,
   WORD        wParam,
   LONG        lParam )
{
   FARPROC     lpProc;           /* Procedure instance for dialogs */
   HANDLE      hInst;            /* Current instance handle */

   switch ( message )
   {
   case WM_COMMAND:
      hInst = GetWindowWord( hwndColors, GWW_HINSTANCE );
      switch( wParam )
      {
      case IDM_NEW:
         /* New dialog box */
         lpProc = MakeProcInstance( MainDlgNew, hInst );
         switch( DialogBox( hInst, "MainNew", hwndColors, lpProc ) )
         {
         case DLGNEW_RED:
            ColorCreate( hwndColors, COLOR_RED );
            break;

         case DLGNEW_GREEN:
            ColorCreate( hwndColors, COLOR_GREEN );
            break;

         case DLGNEW_BLUE:
            ColorCreate( hwndColors, COLOR_BLUE );
            break;
         }
         FreeProcInstance( lpProc );
         break;

      case IDM_OPEN:
         break;

      case IDM_ABOUT:
         /* About dialog box */
         lpProc = MakeProcInstance( MainDlgAbout, hInst );
         DialogBox( hInst, "MainAbout", hwndColors, lpProc );
         FreeProcInstance( lpProc );
         break;

      case IDM_EXIT:
         /* Tell application to shut down */
         PostMessage( hwndColors, WM_SYSCOMMAND, SC_CLOSE, 0L );
         break;
      }
      break;

   case WM_DESTROY:
      PostQuitMessage( 0 );
      break;
   }
   return MdiMainDefWindowProc( hwndColors, message, wParam, lParam );
}

/**/

/*
 * ColorInit( hInst ) : BOOL;
 *
 *    hInst          Current instance handle
 *
 * Register the document class.
 *
 */

BOOL ColorInit(
   HANDLE      hInst )
{
   char        szClass[80];      /* Class name */
   WNDCLASS    WndClass;         /* Class structure */

   /* Get class name */
   LoadString( hInst, IDS_COLORCLASS, szClass, sizeof( szClass ) );

   /* Prepare registation */
   memset( &WndClass, 0, sizeof( WndClass ) );
   WndClass.style          = CS_HREDRAW | CS_VREDRAW;
   WndClass.lpfnWndProc    = ColorWndProc;
   WndClass.cbWndExtra     = WE_EXTRA;
   WndClass.hInstance      = hInst;
   WndClass.hCursor        = LoadCursor( NULL, IDC_ARROW );
   WndClass.hbrBackground  = GetStockObject( GRAY_BRUSH );
   WndClass.lpszClassName  = szClass;

   /* Register */
   return RegisterClass( &WndClass );
}

/**/

/*
 * ColorCreate( hwndParent, wType ) : HWND;
 *
 *    hwndParent     Handle to our MDI desktop
 *    wType          Document color
 *
 * Create a document window of a given color on the MDI desktop.
 * It loads the appropriate menu, and accelerator table if the
 * color is BLUE.  It initializes color and shading in the window
 * extra words.
 *
 */

HWND ColorCreate(
   HWND        hwndParent,
   int         wType )
{
   char        szClass[80];      /* Class name for documents */
   char        szTitle[80];      /* Title for this document */
   HANDLE      hAccel = NULL;    /* Accelerator for blue doc only */
   HANDLE      hInst;            /* Current instance handle */
   HMENU       hmenuChild;       /* Handle to document's menu */
   HWND        hwndChild;        /* Handle to document */

   /* Get important info */
   hInst = GetWindowWord( hwndParent, GWW_HINSTANCE );
   LoadString( hInst, IDS_COLORCLASS, szClass, sizeof( szClass ) );
   sprintf( szTitle, "%s%d", szTitles[wType], ++wCounts[wType] );

   switch( wType )
   {
   case COLOR_RED:
      hmenuChild = LoadMenu( hInst, "RedMenu" );
      break;

   case COLOR_GREEN:
      hmenuChild = LoadMenu( hInst, "GreenMenu" );
      break;

   case COLOR_BLUE:
      hmenuChild = LoadMenu( hInst, "BlueMenu" );
      hAccel = LoadAccelerators( hInst, "BlueAccel" );
      break;
   }

   /* Create */
   hwndChild = MdiChildCreateWindow( szClass,
      szTitle,
      WS_MDICHILD,
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
      hwndParent,
      hmenuChild,
      hInst,
      0L );

   /* Success? */
   if ( hwndChild )
   {
      SetWindowWord( hwndChild, WE_SHADE, IDM_100 );
      SetWindowWord( hwndChild, WE_COLOR, wType );
      MdiSetAccel( hwndChild, hAccel );
   }

   return hwndChild;
}

/**/

/*
 * ColorWndProc( hwndColor, message, wParam, lParam ) : LONG;
 *
 *    hwndChild      Handle to the current document
 *    message        Current message
 *    wParam         Word parameter to the message
 *    lParam         Long parameter to the message
 *
 * Handle messages for our documents.  WM_COMMAND messages arrive at
 * this procedure just as if the menu were attached to this window.
 *
 */

long FAR PASCAL ColorWndProc(
   HWND        hwndChild,
   unsigned    message,
   WORD        wParam,
   LONG        lParam )
{
   char        szText[20];       /* Client area text */
   HBRUSH      hBrush;           /* Brush for filling document */
   PAINTSTRUCT Paint;            /* Paint structure */
   int         wColor;           /* Color of current document */
   int         wShade;           /* Shading of current document */

   switch ( message )
   {
   case WM_COMMAND:
      switch( wParam )
      {
      /* File menu */
      case IDM_SAVE:
      case IDM_SAVEAS:
      case IDM_PRINT:
         break;

      case IDM_CLOSE:
         PostMessage( hwndChild, WM_SYSCOMMAND, SC_CLOSE, lParam );
         break;

      case IDM_0:
      case IDM_25:
      case IDM_50:
      case IDM_75:
      case IDM_100:
         CheckMenuItem( MdiGetMenu( hwndChild ),
            SetWindowWord( hwndChild, WE_SHADE, wParam ),
            MF_UNCHECKED );
         CheckMenuItem( MdiGetMenu( hwndChild ),
            GetWindowWord( hwndChild, WE_SHADE ),
            MF_CHECKED );
         InvalidateRect( hwndChild, (LPRECT) NULL, TRUE );
         break;
      }
      break;

   case WM_ERASEBKGND:
      return TRUE;

   case WM_PAINT:
      wColor = GetWindowWord( hwndChild, WE_COLOR );
      wShade = GetWindowWord( hwndChild, WE_SHADE );
      BeginPaint( hwndChild, &Paint );
      hBrush = CreateSolidBrush( rgbColors[wColor][wShade - IDM_0] );
      FillRect( Paint.hdc, &Paint.rcPaint, hBrush );
      DeleteObject( hBrush );
      strcpy( szText, szShadings[wShade - IDM_0] );
      TextOut( Paint.hdc, 0, 0, szText, strlen( szText ) );
      EndPaint( hwndChild, &Paint );
      break;
   }
   return MdiChildDefWindowProc( hwndChild, message, wParam, lParam );
}

/**/

/*
 * MainDlgNew( hDlg, message, wParam, lParam ) : int;
 *
 *    hDlg           Handle to dialog box
 *    message        Current message
 *    wParam         Word parameter to the message
 *    lParam         Long parameter to the message
 *
 * Handle the NEW dialog box.
 *
 */

int FAR PASCAL MainDlgNew(
   HWND        hDlg,
   unsigned    message,
   WORD        wParam,
   LONG        lParam )
{
   static int  iButton;          /* Keep track of radio buttons */
   int         iReturn = FALSE;  /* Return value */

   switch ( message )
   {
   case WM_INITDIALOG:
      SendMessage( hDlg, WM_COMMAND, DLGNEW_RED, 0L );
      iReturn = TRUE;
      break;
   
   case WM_COMMAND:
      switch( wParam )
      {
      case DLGNEW_RED:
      case DLGNEW_GREEN:
      case DLGNEW_BLUE:
         iButton = wParam;
         CheckRadioButton( hDlg, DLGNEW_RED, DLGNEW_BLUE, iButton );
         if ( HIWORD( lParam ) == BN_DOUBLECLICKED )
         {
            SendMessage( hDlg, WM_COMMAND, IDOK, 0L );
         }
         break;

      case IDOK:
         EndDialog( hDlg, iButton );
         break;
      
      case IDCANCEL:
         EndDialog( hDlg, 0 );
         break;
      }
      break;
   }
   return iReturn;
}

/**/

/*
 * MainDlgAbout( hDlg, message, wParam, lParam ) : int;
 *
 *    hDlg           Handle to dialog box
 *    message        Current message
 *    wParam         Word parameter to the message
 *    lParam         Long parameter to the message
 *
 * Handle the ABOUT dialog box.
 *
 */

int FAR PASCAL MainDlgAbout(
   HWND        hDlg,
   unsigned    message,
   WORD        wParam,
   LONG        lParam )
{
   int         iReturn = FALSE;  /* Return value */

   switch( message )
   {
   case WM_INITDIALOG:
      iReturn = TRUE;
      break;

   case WM_COMMAND:
      EndDialog( hDlg, TRUE );
      break;
   }
   return iReturn;
}
