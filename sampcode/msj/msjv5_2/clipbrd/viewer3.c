/*
 * WINDOWS CLIPBOARD VIEWER - DIALOG SOURCE CODE
 *
 * LANGUAGE      : Microsoft C 5.1
 * MODEL         : medium
 * ENVIRONMENT   : Microsoft Windows 2.1 SDK
 * STATUS        : operational
 *
 */

#define  NOCOMM

#include <windows.h>
#include <string.h>

#include "viewer.h"

/*
 * Dialog( hParentWnd, lpszTemplate, lpfnDlgProc ) : BOOL
 *
 *   hParentWnd      handle to parent window
 *   lpszTemplate    dialog box template
 *   lpfnDlgProc     dialog window function
 *
 * This utility function displays the specified dialog box, using the
 * template provided.  It automatically makes a new instance of the
 * dialog box function.  Note that this function will NOT work
 * correctly if an invalid or NULL parent window handle is provided.
 *
 */

BOOL FAR PASCAL Dialog( hParentWnd, lpszTemplate, lpfnDlgProc )
 HWND      hParentWnd;
 LPSTR     lpszTemplate;
 FARPROC   lpfnDlgProc;
{
 /* local variables */
 BOOL        bResult;
 FARPROC     lpProc;

 /* display palette dialog box */
 lpProc = MakeProcInstance( lpfnDlgProc, INSTANCE(hParentWnd) );
 bResult = DialogBox( INSTANCE(hParentWnd), lpszTemplate,
                      hParentWnd, lpProc );
 FreeProcInstance( lpProc );

 /* return result */
 return( bResult );

}

/*
 * AddFormatDlgFn( hDlg, wMsg, wParam, lParam ) : BOOL ;
 *
 *    hDlg           handle to dialog box
 *    wMsg          message or event
 *    wParam         word portion of message
 *    lParam         long portion of message
 *
 * This function is responsible for adding a new dynamic library
 * to the list of supported formats.  While doing so it checks to
 * make sure that the specified format is not referenced twice.
 * After adding a new format, this function also updates WIN.INI to
 * reference the dynamic library.
 *
 */

BOOL FAR PASCAL AddFormatDlgFn(
 HWND        hDlg,
 WORD        wMsg,
 WORD        wParam,
 LONG        lParam )
{
 BOOL      bResult;

 /* initialization */
 bResult = TRUE;

 /* process message */
 switch( wMsg )
   {
 case WM_INITDIALOG :
   CenterPopup( hDlg, GetParent(hDlg) );
   EnableWindow( GetDlgItem(hDlg,IDADD), FALSE );
   break;
 case WM_COMMAND :

   /* process sub-message */
   switch( wParam )
     {
   case IDCANCEL :
     EndDialog( hDlg, FALSE );
     break;
   case IDADD :
     {
       char    szFmt[32];
       char    szLib[64];

       /* retrieve format & library names */
       GetDlgItemText( hDlg, IDFORMAT, szFmt, sizeof(szFmt) );
       GetDlgItemText( hDlg, IDLIBRARY, szLib, sizeof(szLib) );

       /* upshift library name */
       strupr( szLib );

       /* end dialog & add library to list */
       EndDialog( hDlg, TRUE );
       SendMessage( GetParent(hDlg), WM_ADDFMT,
                    GetClipboardFmtNumber(szFmt),
                    (LONG)(LPSTR)szLib );

     }
     break;
   case IDFORMAT :
   case IDLIBRARY :

     /* enable or disable add button */
     if ( HIWORD(lParam) == EN_CHANGE )
EnableWindow(GetDlgItem(hDlg,IDADD),
             ( SendMessage(GetDlgItem(hDlg,IDFORMAT),
                           WM_GETTEXTLENGTH,0,0L)      &&
               SendMessage(GetDlgItem(hDlg,IDLIBRARY),
                           WM_GETTEXTLENGTH,0,0L)
             ) ? TRUE : FALSE
            );

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

/*
 * RemFormatDlgFn( hDlg, wMsg, wParam, lParam ) : BOOL ;
 *
 *    hDlg          handle to dialog box
 *    wMsg          message or event
 *    wParam        word portion of message
 *    lParam        long portion of message
 *
 * This function is responsible for removing the display dynamic
 * library support for a particular clipboard format.  While doing
 * so is automatically updates WIN.INI and the parent window
 * instance variables.
 *
 */

BOOL FAR PASCAL RemFormatDlgFn(
 HWND        hDlg,
 WORD        wMsg,
 WORD        wParam,
 LONG        lParam )
{
 BOOL      bResult;

 /* initialization */
 bResult = TRUE;

 /* process message */
 switch( wMsg )
   {
 case WM_INITDIALOG :
   {
     WORD      wEntry;
     WORD      wModules;
     HWND      hWndLibList;
     char *    pszEntry;
     char      szList[512];

     /* center window */
     CenterPopup( hDlg, GetParent(hDlg) );
     EnableWindow( GetDlgItem(hDlg,IDREMOVE), FALSE );

     /* initialize dialog box */
     wModules = (WORD)SendMessage( GetParent(hDlg), WM_GETFMT, 0,
                                   (LONG)(LPSTR)szList );
     if ( wModules > 0 ) {

       /* initialize listbox */
       hWndLibList = GetDlgItem( hDlg, IDLIBLIST );
       SendMessage(hWndLibList,WM_SETREDRAW, (WORD)FALSE, (LONG)0 );
       SendMessage(hWndLibList,LB_RESETCONTENT, (WORD)0, (LONG)0 );

       /* retrieve and display each listed library module */
       pszEntry = &szList[0];
       for ( wEntry=0; wEntry<wModules; wEntry++ ) {
         SendMessage( hWndLibList, LB_ADDSTRING, (WORD)0,
                      (LONG)(LPSTR)pszEntry );
         pszEntry += strlen(pszEntry) + 1;
       }

       /* display listbox */
         SendMessage(hWndLibList,WM_SETREDRAW,(WORD)TRUE,(LONG)0);
       InvalidateRect( hWndLibList, (LPRECT)NULL, TRUE );

     }

   }
   break;
 case WM_COMMAND :

   /* process sub-message */
   switch( wParam )
     {
   case IDLIBLIST :

     /* enable remove button if library selected */
     if (HIWORD(lParam) == LBN_SELCHANGE)
       EnableWindow( GetDlgItem(hDlg,IDREMOVE), TRUE );

     break;
   case IDREMOVE :
     {
       WORD    wEntry;

       /* retrieve selected library index & notify parent */
       wEntry = (WORD)SendMessage( GetDlgItem(hDlg,IDLIBLIST),
                                   LB_GETCURSEL, 0, 0L );
       if ( SendMessage(GetParent(hDlg),WM_REMFMT,wEntry,0L) )
         EndDialog( hDlg, TRUE );

     }

     break;
   case IDCANCEL :
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

/*
 * AboutDlgFn( hDlg, wMsg, wParam, lParam ) : BOOL ;
 *
 *    hDlg          handle to dialog box
 *    wMsg          message or event
 *    wParam        word portion of message
 *    lParam        long portion of message
 *
 * This function is responsible for processing all the messages
 * that relate to the Viewer about dialog box.  About the only
 * acts this function performs is to center the dialog box and
 * wait for the Ok button to be pressed.
 *
 */

BOOL FAR PASCAL AboutDlgFn(
 HWND      hDlg,
 WORD      wMsg,
 WORD      wParam,
 LONG      lParam )
{
 BOOL      bResult;

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
   case IDOK :
     EndDialog( hDlg, TRUE );
     break;
   case IDCANCEL :
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
