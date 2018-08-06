/*
 * FORM VIEWER - SOURCE CODE
 *
 * LANGUAGE      : Microsoft C 5.1
 * MODEL         : medium
 * ENVIRONMENT   : Microsoft Windows 3.00 SDK
 * STATUS        : operational
 *
 *    Eikon Systems, Inc.
 *    989 East Hillsdale Blvd, Suite 260
 *    Foster City, California 94404
 *
 * 07/12/90 1.00 - Kevin P. Welch - initial creation.
 *
 */

#define NOCOMM

#include <windows.h>
#include <memory.h>
#include "viewer.h"

/**/

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

   /* register window class (if necessary) */
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
      
         /* display main window */
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

/**/

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
         SET_EDIT( hWnd, NULL );
         SET_FORM( hWnd, NULL );

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
         if ( hWndEdit ) {
            SET_EDIT( hWnd, hWndEdit );
            EnableWindow( hWndEdit, FALSE );
         } else {
            WARNING( hWnd, "Unable to create edit window!" );
            PostMessage( hWnd, WM_CLOSE, NULL, (LONG)NULL );
         } 

      }
      break;
   case WM_SIZE : /* window being resized */
      
      /* resize edit window (if present) */
      if ( GET_EDIT(hWnd) )
         MoveWindow( 
            GET_EDIT(hWnd),
            -1, 
            -1, 
            LOWORD(lParam)+2,
            HIWORD(lParam)+2,
            TRUE
         );

      break;
   case WM_INITMENU : /* initialize menu */

      /* enable edit & print menus */
      if ( GET_FORM(hWnd) ) {
         EnableMenuItem( wParam, IDM_EDIT, MF_ENABLED );
         EnableMenuItem( wParam, IDM_PRINT, MF_ENABLED );
      } else {
         EnableMenuItem( wParam, IDM_EDIT, MF_DISABLED|MF_GRAYED );
         EnableMenuItem( wParam, IDM_PRINT, MF_DISABLED|MF_GRAYED );
      }

      break;
   case WM_COMMAND : /* window command */

      /* process message */
      switch( wParam )
         {
      case IDM_OPEN :
         
         /* display open dialog box */
         if ( Dialog(hWnd,"ViewerOpen",OpenDlgFn) )
            SetFocus( GET_EDIT(hWnd) );

         break;
      case IDM_EDIT :
         {
            HANDLE      hData;
            HANDLE      hResult;
            LPEDITFN    lpEditFn;

            /* retrieve text block */
            hData = GetText( GET_EDIT(hWnd) );
            if ( hData ) {
   
               /* pass data to form edit library function */
               lpEditFn = GetProcAddress( GET_FORM(hWnd), FORM_EDIT );
               hResult = (*lpEditFn)( hWnd, hData );

               /* redisplay updated information */
               if ( hResult ) {
                  SetText( GET_EDIT(hWnd), hResult );
                  GlobalFree( hResult );
               } 

               /* release initial data block */
               GlobalFree( hData );

            } else
               WARNING( hWnd, "Unable to retrieve text block!" );

         } 
         break;
      case IDM_PRINT :
         {
            HANDLE      hData;
            HANDLE      hResult;
            LPPRINTFN   lpPrintFn;

            /* retrieve edit data */
            hData = GetText( GET_EDIT(hWnd) );
            if ( hData ) {
   
               /* pass data to form print library function */
               lpPrintFn = GetProcAddress( GET_FORM(hWnd), FORM_PRINT );
               hResult = (*lpPrintFn)( hWnd, hData );

               /* release edit data */
               GlobalFree( (hResult)?hResult:hData );

            } else
               WARNING( hWnd, "Unable to retrieve text block!" );

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
      if ( GET_FORM(hWnd) )
         FreeLibrary( GET_FORM(hWnd) );
   
      /* remove property lists */
      REM_EDIT( hWnd );
      REM_FORM( hWnd );
   
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

/**/

/*
 * OpenDlgFn( hDlg, wMsg, wParam, lParam ) : BOOL ;
 *
 *    hDlg           handle to dialog box
 *    wMsg           message or event
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
   /* local variables */
   BOOL        bResult;

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

               /* update main window caption */
               lstrcpy( szCaption, "Form Viewer - " );
               lstrcat( szCaption, szLib );

               SetWindowText( PARENT(hDlg), szCaption );

               /* release old library */
               if ( GET_FORM(PARENT(hDlg)) ) 
                  FreeLibrary( GET_FORM(PARENT(hDlg)) );

               /* save library handle & activate edit window */
               SET_FORM( PARENT(hDlg), hLib );
               EnableWindow( GET_EDIT(PARENT(hDlg)), TRUE );
               
               /* close dialog box */
               EndDialog( hDlg, TRUE );

            } 

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

/**/

/*
 * AboutDlgFn( hDlg, wMsg, wParam, lParam ) : BOOL ;
 *
 *    hDlg           handle to dialog box
 *    wMsg           message or event
 *    wParam         word portion of message
 *    lParam         long portion of message
 *
 * This function is responsible for processing all the messages
 * that relate to the Viewer about dialog box. 
 *
 */

BOOL FAR PASCAL AboutDlgFn(
   HWND        hDlg,
   WORD        wMsg,
   WORD        wParam,
   LONG        lParam )
{
   /* local variables */
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

/**/

/*
 * GetText( hWnd ) : HANDLE;
 *
 *    hWnd           handle to edit control window
 *
 * This function allocates and retrieve a block of text data
 * from a multi-line edit control.  Each text line is terminated
 * with a single NULL character, and the last by two NULL characters.
 *
 * If a value of NULL is returned, it can be assumed that insufficient
 * memory was available or an invalid window handle was provided.
 *
 * Note - this function assumes that the maximum text line length
 * is 256 characters.
 *
 */

HANDLE FAR PASCAL GetText(
   HWND        hWnd )
{
   /* local variables */
   WORD        wLen;
   WORD        wMax;
   WORD        wLine;
   HANDLE      hData;
   DWORD       dwOld;
   DWORD       dwNew;
   LPSTR       lpData;
   char        szData[256];

   /* initialization */
   dwOld = 0;

   /* allocate space for data */
   hData = GlobalAlloc( GMEM_MOVEABLE|GMEM_ZEROINIT, dwOld+32 );
   if ( hData ) {
   
      /* retrieve number of text lines */
      wMax = (WORD)SendMessage( hWnd, EM_GETLINECOUNT, 0, (LONG)0 );

      /* process each text line */
      for ( wLine=0; wLine<wMax; wLine++ ) {
   
         /* retrieve next text line (make null terminated) */
         szData[0] = sizeof(szData) - 2;

         wLen = (WORD)SendMessage(
                        hWnd, 
                        EM_GETLINE, 
                        wLine, 
                        (LONG)(LPSTR)szData
                     );

         szData[wLen] = NULL;

         /* continue if line not empty */
         if ( wLen ) {

            /* calculate new memory block sizes */
            dwNew = dwOld + wLen + 1;

            /* expand global buffer */
            hData = GlobalReAlloc( hData, dwNew+32, NULL );
            if ( hData ) {

               /* copy text string into global buffer */
               lpData = GlobalLock( hData );
               if ( lpData ) {
                  lstrcpy( lpData+(WORD)dwOld, szData );
                  GlobalUnlock( hData );
                  dwOld = dwNew;
               } 

            } 

         }
         
      }

      /* append final NULL character */
      hData = GlobalReAlloc( hData, dwOld+32, NULL );
      if ( hData ) {
         lpData = GlobalLock( hData );
         if ( lpData ) {
            *(lpData+(WORD)dwOld) = NULL;
            GlobalUnlock( hData );
         }
      }

   }

   /* return result */
   return( hData );

}

/**/

/*
 * SetText( hWnd, hData ) : HANDLE;
 *
 *    hWnd           handle to edit control
 *    hData          handle to formatted data block
 *
 * This function erases the current contents of the edit control and
 * displays the new text data block.  It is assumed that each line
 * in the text block is terminated with a single NULL character and
 * the last line by two NULL characters.
 *
 * If the function was successful, the original data block handle is
 * returned.  If unsuccessful, a NULL handle is returned.
 *
 */

HANDLE FAR PASCAL SetText(
   HWND        hWnd,
   HANDLE      hData )
{
   /* local variables */
   WORD        wLen;
   HANDLE      hTemp;
   DWORD       dwOld;
   DWORD       dwNew;
   LPSTR       lpData;
   LPSTR       lpTemp;

   /* initialization */
   dwOld = 0;
   
   /* allocate space for formatted data block */
   hTemp = GlobalAlloc( GMEM_MOVEABLE|GMEM_ZEROINIT, dwOld+32 );
   if ( hTemp ) {

      /* copy text string into global buffer */
      lpData = GlobalLock( hData );
      if ( lpData ) {

         /* process each line in data block */
         while ( *lpData ) {

            /* determine length of next string */
            wLen = lstrlen(lpData);

            /* calculate new memory block sizes */
            dwNew = dwOld + wLen + 2;

            /* expand temporary data block */
            hTemp = GlobalReAlloc( hTemp, dwNew+32, NULL );
            if ( hTemp ) {

               /* copy text string into global buffer */
               lpTemp = GlobalLock( hTemp );
               if ( lpTemp ) {
                  lstrcpy( &lpTemp[(WORD)dwOld], lpData );
                  lstrcpy( &lpTemp[(WORD)dwOld+wLen], "\r\n" );
                  GlobalUnlock( hTemp );
                  dwOld = dwNew;
               } 

            } 

            /* move to next line */
            lpData += wLen + 1;

         }

         /* unlock data block */
         GlobalUnlock( hData );

      } 
   
      /* append final NULL character */
      hTemp = GlobalReAlloc( hTemp, dwOld+32, NULL );
      if ( hTemp ) {
         lpTemp = GlobalLock( hTemp );
         if ( lpTemp ) {
            *(lpTemp+(WORD)dwOld) = NULL;
            GlobalUnlock( hTemp );
         }
      }

      /* update edit control */
      lpTemp = GlobalLock( hTemp );
      if ( lpTemp ) {
         SendMessage( hWnd, WM_SETTEXT, 0, (LONG)lpTemp );
         GlobalUnlock( hTemp );
      } 

      /* release temporary memory block */
      GlobalFree( hTemp );

   }

   /* return result */
   return( hData );

}

/**/

/*
 * Dialog( hParentWnd, lpszTemplate, lpfnDlgProc ) : BOOL
 *
 *    hParentWnd        handle to parent window
 *    lpszTemplate      dialog box template
 *    lpfnDlgProc       dialog window function
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
   bResult = DialogBox( INSTANCE(hParentWnd), lpszTemplate, hParentWnd, lpProc );
   FreeProcInstance( lpProc );

   /* return result */
   return( bResult );

}

/**/

/*
 * CenterPopup( hWnd, hParentWnd ) : BOOL
 *
 *    hWnd              window handle
 *    hParentWnd        parent window handle
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
   int      xPopup;   
   int      yPopup;   
   int      cxPopup;  
   int      cyPopup;  
   int      cxScreen; 
   int      cyScreen; 
   int      cxParent; 
   int      cyParent; 
   RECT     rcWindow; 

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


