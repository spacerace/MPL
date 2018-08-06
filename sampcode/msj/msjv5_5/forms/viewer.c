/*
 * FORM VIEWER - SOURCE CODE
 *
 * LANGUAGE      : Microsoft C 5.1
 * MODEL         : medium
 * ENVIRONMENT   : Microsoft Windows 3.00 SDK
 * STATUS        : operational
 *
 * 07/12/90 1.00 - Kevin P. Welch - initial creation.
 *
 */

#define NOCOMM

#include <windows.h>
#include <memory.h>
#include "viewer.h"

/**/

/*
 * WinMain( hCrntInst, hPrevInst, lpszCmd, wCmdShow ) : VOID
 *
 *    hCrntInst      current instance handle
 *    hPrevInst      previous instance handle
 *    lpszCmd        current command line
 *    wCmdShow       initial show window parameter
 *
 * This function is responsible for registering and creating the form
 * viewer window.  Once the window has been created, messages
 * are retrieved until the window is destroyed.
 *
 */

int PASCAL WinMain(
   HANDLE      hCrntInst,
   HANDLE      hPrevInst,
   LPSTR       lpszCmd,
   WORD        wCmdShow )
{
   /* local variables */
   MSG         Msg;
   HWND        hWnd;
   WNDCLASS    WndClass;

   /* warning level 3 compatibility */
   lpszCmd;

   /* define special window class */
   memset( &WndClass, 0, sizeof(WNDCLASS) );

   WndClass.lpszClassName =   "Viewer";
   WndClass.hInstance =       hCrntInst;
   WndClass.lpfnWndProc =     ViewerWndFn;
   WndClass.lpszMenuName =    "ViewerMenu";
   WndClass.hbrBackground =   NULL;
   WndClass.style =           CS_HREDRAW | CS_VREDRAW;
   WndClass.hCursor =         LoadCursor( NULL, IDC_ARROW );
   WndClass.hIcon =           LoadIcon( hCrntInst, "ViewerIcon" );

   if ( (hPrevInst) || (RegisterClass(&WndClass)) ) {

      /* create main window */
      hWnd = CreateWindow(
            "Viewer",
            "Form Viewer - (untitled)",
            VIEWER_STYLE,
            VIEWER_XPOS,
            VIEWER_YPOS,
            VIEWER_WIDTH,
            VIEWER_HEIGHT,
            (HWND)NULL,
            (HMENU)NULL,
            hCrntInst,
            (LPSTR)&WndClass
         );

      /* continue if successful */
      if ( hWnd ) {

         /* display window */
         ShowWindow( hWnd, wCmdShow );

         /* process messages */
         while ( GetMessage(&Msg,NULL,0,0) ) {
            TranslateMessage( &Msg );
            DispatchMessage( &Msg );
         }

         /* normal exit */
         return( Msg.wParam );

      } else
         WARNING( NULL, "Unable to create window!" );

   } else
      WARNING( NULL, "Unable to register window!" );

   /* abnormal exit */
   return( NULL );

}

/**/

/*
 * ViewerWndFn( hWnd, wMessage, wParam, lParam ) : LONG
 *
 *    hWnd        window handle
 *    wMessage    message number
 *    wParam      additional message information
 *    lParam      additional message information
 *
 * This window function processes all the messages related to the
 * form viewer window.  With the menu a new form can be loaded, and
 * existing form edited or printed.
 *
 */

LONG FAR PASCAL ViewerWndFn(
   HWND        hWnd,
   WORD        wMsg,
   WORD        wParam,
   LONG        lParam )
{
   /* local variables */
   LONG        lResult;

   /* initialization */
   lResult = FALSE;

   /* process each message */
   switch( wMsg )
      {
  case WM_CREATE : /* window being created */
     {
        HWND     hWndEdit;
        RECT     rcClient;

        /* retrieve client rectangle */
        GetClientRect( hWnd, &rcClient );

        /* define property lists */
        SetProp( hWnd, VIEWER_HWNDEDIT, NULL );
        SetProp( hWnd, VIEWER_HFORMLIB, NULL );

        /* create edit child window */
        hWndEdit = CreateWindow(
              "Edit",
              "",
              WS_CHILD|WS_VISIBLE|WS_HSCROLL|WS_VSCROLL|
                 ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL,
              -1,
              -1,
              rcClient.right+2,
              rcClient.bottom+2,
              hWnd,
              1,
              INSTANCE(hWnd),
              NULL
           );

        /* continue if successful */
        if ( !hWndEdit ) {
           WARNING( hWnd, "Unable to create edit window!" );
           PostMessage( hWnd, WM_CLOSE, NULL, (LONG)NULL );
        } else
           SetProp( hWnd, VIEWER_HWNDEDIT, hWndEdit );

     }
     break;
  case WM_SIZE : /* window being resized */

     /* resize edit window (if present) */
     if ( GetProp(hWnd,VIEWER_HWNDEDIT) )
        MoveWindow(
           GetProp(hWnd,VIEWER_HWNDEDIT),
           -1,
           -1,
           LOWORD(lParam)+2,
           HIWORD(lParam)+2,
           TRUE
        );

     break;
  case WM_INITMENU : /* initialize menu */

     /* enable edit & print menus */
     if ( GetProp(hWnd,VIEWER_HFORMLIB) ) {
        EnableMenuItem( wParam, IDM_EDIT, MF_ENABLED );
        EnableMenuItem( wParam, IDM_PRINT, MF_ENABLED );
     }

     break;
  case WM_COMMAND : /* window command */

      /* process message */
      switch( wParam )
         {
      case IDM_OPEN :
        Dialog( hWnd, "ViewerOpen", OpenDlgFn );
         break;
      case IDM_EDIT :
        {
           HANDLE      hForm;
           LPEDITFN    lpEditFn;

           /* retrieve edit data */

           /* call edit function dynamically */
           hForm = GetProp( hWnd, VIEWER_HFORMLIB );
           if ( hForm ) {
              lpEditFn = GetProcAddress( hForm, FORM_EDIT );
              (*lpEditFn)( hWnd, NULL );
           }

        }
         break;
      case IDM_PRINT :
        {
           HANDLE      hForm;
           LPPRINTFN   lpPrintFn;

           /* retrieve edit data */

           /* call print function dynamically */
           hForm = GetProp( hWnd, VIEWER_HFORMLIB );
           if ( hForm ) {
              lpPrintFn = GetProcAddress( hForm, FORM_PRINT );
              (*lpPrintFn)( hWnd, NULL );
           }

        }
         break;
      case IDM_EXIT :
         PostMessage( hWnd, WM_CLOSE, NULL, (LONG)NULL );
         break;
      case IDM_ABOUT :
        Dialog( hWnd, "ViewerAbout", AboutDlgFn );
         break;
      }

      break;
   case WM_DESTROY : /* destroy window */

     /* free form library (if present) */
     if ( GetProp(hWnd,VIEWER_HFORMLIB) )
        FreeLibrary( GetProp(hWnd,VIEWER_HFORMLIB) );

     /* remove property lists */
     RemoveProp( hWnd, VIEWER_HWNDEDIT );
     RemoveProp( hWnd, VIEWER_HFORMLIB );

     /* die gracefully */
     PostQuitMessage( 0 );

     break;
   default : /* send to default */
      lResult = DefWindowProc( hWnd, wMsg, wParam, lParam );
      break;
   }

   /* return normal result */
   return( lResult );

}

/**/

/*
 * OpenDlgFn( hDlg, wMsg, wParam, lParam ) : BOOL ;
 *
 *    hDlg           handle to dialog box
 *    wMsg          message or event
 *    wParam         word portion of message
 *    lParam         long portion of message
 *
 * This function is responsible for enabling the user to
 * open a new form library.
 *
 */

BOOL FAR PASCAL OpenDlgFn(
  HWND        hDlg,
  WORD        wMsg,
  WORD        wParam,
  LONG        lParam )
{
  BOOL        bResult;

   /* warning level 3 compatibility */
  lParam;

  /* initialization */
  bResult = TRUE;

  /* process message */
  switch( wMsg )
     {
  case WM_INITDIALOG :
     CenterPopup( hDlg, GetParent(hDlg) );
     break;
  case WM_COMMAND :

     /* process sub-message */
     switch( wParam )
        {
     case ID_FORM :

        /* enable or disable OK button */
        if ( HIWORD(lParam) == EN_CHANGE )
           EnableWindow(
              GetDlgItem(hDlg,ID_OK),
              SendMessage(
                 GetDlgItem(hDlg,ID_FORM),
                 WM_GETTEXTLENGTH,
                 0,
                 0L
              ) ? TRUE : FALSE
           );

        break;
     case ID_OK :
        {
           HANDLE   hLib;
           char     szLib[128];
           char     szCaption[128];

           /* retrieve library name */
           GetDlgItemText( hDlg, ID_FORM, szLib, sizeof(szLib) );

           /* attempt to load new library */
           hLib = LoadLibrary( szLib );
           if ( hLib >= 32 ) {

              /* free old form library */
              if ( GetProp(PARENT(hDlg),VIEWER_HFORMLIB) )
                 FreeLibrary(GetProp(PARENT(hDlg),VIEWER_HFORMLIB) );

              /* update main window caption */
              lstrcpy( szCaption, "Form Viewer - " );
              lstrcat( szCaption, szLib );

              SetWindowText( PARENT(hDlg), szCaption );

              /* save new library handle & close dialog box */
              SetProp( PARENT(hDlg), VIEWER_HFORMLIB, hLib );
              EndDialog( hDlg, TRUE );

           } else
              WARNING( hDlg, "Unable to load library!" );

        }
        break;
     case ID_CANCEL :

        /* return false */
        EndDialog( hDlg, FALSE );

        break;
     default :
        bResult = FALSE;
        break;
     }

     break;
  default :
     bResult = FALSE;
     break;
  }

  /* return final result */
  return( bResult );

}

/**/

/*
 * AboutDlgFn( hDlg, wMsg, wParam, lParam ) : BOOL ;
 *
 *    hDlg           handle to dialog box
 *    wMsg          message or event
 *    wParam         word portion of message
 *    lParam         long portion of message
 *
 *   This function is responsible for processing all the messages
 * that relate to the Viewer about dialog box.
 *
 */

BOOL FAR PASCAL AboutDlgFn(
  HWND        hDlg,
  WORD        wMsg,
  WORD        wParam,
  LONG        lParam )
{
  BOOL        bResult;

  /* warning level 3 compatibility */
  lParam;

  /* initialization */
  bResult = TRUE;

  /* process message */
  switch( wMsg )
     {
  case WM_INITDIALOG :
     CenterPopup( hDlg, GetParent(hDlg) );
     break;
  case WM_COMMAND :

     /* process sub-message */
     if ( wParam == ID_OK )
        EndDialog( hDlg, TRUE );
     else
        bResult = FALSE;

     break;
  default :
     bResult = FALSE;
     break;
  }

  /* return final result */
  return( bResult );

}

/**/

/*
 * Dialog( hParentWnd, lpszTemplate, lpfnDlgProc ) : BOOL
 *
 *      hParentWnd        handle to parent window
 *      lpszTemplate      dialog box template
 *      lpfnDlgProc       dialog window function
 *
 * This utility function displays the specified dialog box, using the
 * template provided.  It automatically makes a new instance of the
 * dialog box function.  Note that this function will NOT work
 * correctly if an invalid or NULL parent window handle is provided.
 *
 */

BOOL FAR PASCAL Dialog( hParentWnd, lpszTemplate, lpfnDlgProc )
  HWND        hParentWnd;
  LPSTR       lpszTemplate;
  FARPROC     lpfnDlgProc;
{
  /* local variables */
  BOOL           bResult;
  FARPROC        lpProc;

  /* display palette dialog box */
  lpProc = MakeProcInstance( lpfnDlgProc, INSTANCE(hParentWnd) );
  bResult = DialogBox( INSTANCE(hParentWnd), lpszTemplate,
                       hParentWnd, lpProc );
  FreeProcInstance( lpProc );

  /* return result */
  return( bResult );

}

/**/

/*
 * CenterPopup( hWnd, hParentWnd ) : BOOL
 *
 *      hWnd              window handle
 *      hParentWnd        parent window handle
 *
 * This routine centers the popup window in the screen or display
 * using the window handles provided.  The window is centered over
 * the parent if the parent window is valid.  Special provision
 * is made for the case when the popup would be centered outside
 * the screen - in this case it is positioned at the appropriate
 * border.
 *
 */

BOOL FAR PASCAL CenterPopup(
     HWND     hWnd,
     HWND     hParentWnd
  )
{
  /* local variables */
  int      xPopup;                 /* popup x position */
  int      yPopup;                 /* popup y position */
  int      cxPopup;                /* width of popup window */
  int      cyPopup;                /* height of popup window */
  int      cxScreen;               /* width of main display */
  int      cyScreen;               /* height of main display */
  int      cxParent;               /* width of parent window */
  int      cyParent;               /* height of parent window */
  RECT     rcWindow;               /* temporary window rect */

  /* retrieve main display dimensions */
  cxScreen = GetSystemMetrics( SM_CXSCREEN );
  cyScreen = GetSystemMetrics( SM_CYSCREEN );

  /* retrieve popup rectangle  */
  GetWindowRect( hWnd, (LPRECT)&rcWindow );

  /* calculate popup extents */
  cxPopup = rcWindow.right - rcWindow.left;
  cyPopup = rcWindow.bottom - rcWindow.top;

  /* calculate bounding rectangle */
  if ( hParentWnd ) {

     /* retrieve parent rectangle */
     GetWindowRect( hParentWnd, (LPRECT)&rcWindow );

     /* calculate parent extents */
     cxParent = rcWindow.right - rcWindow.left;
     cyParent = rcWindow.bottom - rcWindow.top;

     /* center within parent window */
     xPopup = rcWindow.left + ((cxParent - cxPopup)/2);
     yPopup = rcWindow.top + ((cyParent - cyPopup)/2);

     /* adjust popup x-location for screen size */
     if ( xPopup+cxPopup > cxScreen )
        xPopup = cxScreen - cxPopup;

     /* adjust popup y-location for screen size */
     if ( yPopup+cyPopup > cyScreen )
        yPopup = cyScreen - cyPopup;

  } else {

     /* center within entire screen */
     xPopup = (cxScreen - cxPopup) / 2;
     yPopup = (cyScreen - cyPopup) / 2;

  }

  /* move window to new location & display */
  MoveWindow(
     hWnd,
     ( xPopup > 0 ) ? xPopup : 0,
     ( yPopup > 0 ) ? yPopup : 0,
     cxPopup,
     cyPopup,
     TRUE
  );

  /* normal return */
  return( TRUE );

}
