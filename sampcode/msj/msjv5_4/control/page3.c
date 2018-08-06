/*
 * Page Selection Control - STYLE MODULE
 *
 * LANGUAGE      : Microsoft C5.1
 * MODEL         : small
 * ENVIRONMENT   : Microsoft Windows 3.0 SDK
 * STATUS        : operational
 *
 * This module defines the function responsible for displaying the
 * style dialog box, through which the user can define the window
 * text and page range.
 *
 *    Eikon Systems, Inc.
 *    989 East Hillsdale Blvd, Suite 260
 *    Foster City, California 94404
 *    415-349-4664
 *
 * 11/30/89 1.00 - Kevin P. Welch - initial creation.
 *
 */

#define  NOCOMM
 
#include <windows.h>
#include <control.h>

#include "page.h"
#include "page.d"

#define  HCTLSTYLE      MAKEINTRESOURCE(1)
#define  LPFNSTRTOIDLO  MAKEINTRESOURCE(2)
#define  LPFNSTRTOIDHI  MAKEINTRESOURCE(3)

/* global variables for data exchange */
static HANDLE           hCtlStyleTemp;
static LPFNSTRTOID      lpfnStrToIdTemp;
static LPFNIDTOSTR      lpfnIdToStrTemp;

/* local function definitions */
BOOL FAR PASCAL   CenterPopup( HWND, HWND );
BOOL FAR PASCAL   PageDlgFn( HWND, WORD, WORD, LONG );

/**/

/*
 * PageStyle( hWnd, hCtlStyle, lpfnStrToId, lpfnIdToStr ) : LONG;
 *
 *    hWnd           handle to parent window
 *    hCtlStyle      handle to control style block
 *    lpfnStrToId    pointer to Verify id function.
 *    lpfnIdToStr    pointer to GetIdStr function.
 *
 * This function displays a dialog box, enabling the user to define
 * a new control style.  A value of TRUE is returned if the process
 * was successful, FALSE otherwise.
 *
 * The lpfnStrToId and lpfnIdToStr functions can be optionally used
 * by the calling program to verify the id value entered for the
 * control.  If these two values are NULL then no verification
 * is performed.
 *
 */

BOOL FAR PASCAL PageStyle(
   HWND           hWnd,
   HANDLE         hCtlStyle,
   LPFNSTRTOID    lpfnStrToId,
   LPFNIDTOSTR    lpfnIdToStr )
{
   BOOL        bResult;
   FARPROC     lpProc;

   /* check for valid style block */
   if ( hCtlStyle ) {

      /* initialization */
      hCtlStyleTemp = hCtlStyle;
      lpfnStrToIdTemp = lpfnStrToId;
      lpfnIdToStrTemp = lpfnIdToStr;

      /* display dialog box */
      lpProc = MakeProcInstance( PageDlgFn, hPageInst );
      bResult = DialogBox( hPageInst, "PageStyle", hWnd, lpProc );
      FreeProcInstance( lpProc );

   } else
      bResult = FALSE;

   /* return final result */
   return( bResult );

}

/**/

/*
 * PageDlgFn( hDlg, wMessage, wParam, lParam ) : LONG;
 *
 *    hDlg           dialog box window handle
 *    wMessage       dialog box message
 *    wParam         word parameter
 *    lParam         long parameter
 *
 * This function is responsible for managing the style dialog
 * displayed to the user.  All responses are validated before
 * the dialog box is closed.  All values are passed using some
 * temporary global variables.
 *
 */

BOOL FAR PASCAL PageDlgFn(
   HWND     hDlg,
   WORD     wMessage,
   WORD     wParam,
   LONG     lParam )
{
   BOOL     bResult;

   /* warning level 3 compatibility */
   lParam;
   
   /* initialization */
   bResult = TRUE;

   /* process message */
   switch ( wMessage )
      {
   case WM_INITDIALOG : /* initialize dialog box */

      /* initialize property list */
      SetProp( hDlg, HCTLSTYLE, hCtlStyleTemp );
      SetProp( hDlg, LPFNSTRTOIDLO, LOWORD(lpfnStrToIdTemp) );
      SetProp( hDlg, LPFNSTRTOIDHI, HIWORD(lpfnStrToIdTemp) );

      /* center within parent window */
      CenterPopup( hDlg, GetParent( hDlg ) );

      /* define dialog box contents */
      if ( hCtlStyleTemp ) {

         char        szId[32];
         LPCTLSTYLE  lpCtlStyle;

         /* lock the data down */
         lpCtlStyle = (LPCTLSTYLE)GlobalLock( hCtlStyleTemp );
         if ( lpCtlStyle ) {

            /* define page id field */
            if ( lpfnIdToStrTemp && lpfnStrToIdTemp ) {
               (*lpfnIdToStrTemp)( lpCtlStyle->wId, (LPSTR)szId,
                  sizeof(szId) );
               SetDlgItemText( hDlg, ID_IDENTIFIER, szId );
            }

            /* unlock the data */
            GlobalUnlock( GetProp(hDlg,HCTLSTYLE) );

         } else
            MessageBox( hDlg, "Unable to retrieve style inforation!",
               "Page Control", MB_OK );

      } else
         MessageBox( hDlg, "Control style information undefined!",
            "Page Control", MB_OK );

      break;
   case WM_COMMAND:

      /* process commands */
      switch( wParam )
         {
      case IDOK : /* OK button */
         {
            WORD           wSize;
            BOOL           bClose;
            HANDLE         hCtlStyle;   
            DWORD          dwResult;    
            LPCTLSTYLE     lpCtlStyle;  
            LPFNSTRTOID    lpfnStrToId;
            char           szId[CTLTITLE];

            /* initialization */
            bClose = FALSE;
            hCtlStyle = GetProp( hDlg, HCTLSTYLE );

            /* lock data down */
            lpCtlStyle = hCtlStyle ?
               (LPCTLSTYLE)GlobalLock(hCtlStyle) : NULL;
            if (lpCtlStyle) {

               /* define new window id */
               lpfnStrToId = (LPFNSTRTOID)MAKELONG(
                     GetProp(hDlg,LPFNSTRTOIDLO),
                     GetProp(hDlg,LPFNSTRTOIDHI)
                  );
               if ( lpfnStrToId ) {

                  /* retrieve identifier string */
                  wSize = GetDlgItemText( hDlg, ID_IDENTIFIER, szId,
                     sizeof(szId) );
                  szId[wSize] = NULL;

                  /* translate indentifier */
                  dwResult = (*lpfnStrToId)((LPSTR)szId);
                  if ( LOWORD(dwResult) ) {
                     bClose = TRUE;
                     lpCtlStyle->wId = HIWORD( dwResult );
                  }

               }

               /* unlock the data */
               GlobalUnlock( GetProp(hDlg,HCTLSTYLE) );
  
            }

            /* close dialog box */
            if ( bClose )
               EndDialog( hDlg, TRUE );

         }
         break;
      case IDCANCEL : /* Cancel button */
         EndDialog( hDlg, FALSE );
         break;
      default : /* something else - ignore */
         break;
      }

      break;
   case WM_DESTROY : /* window being destroyed */

      /* clean up property lists */
      RemoveProp( hDlg, HCTLSTYLE );
      RemoveProp( hDlg, LPFNSTRTOIDLO );
      RemoveProp( hDlg, LPFNSTRTOIDHI );

      break;
   default : /* some other message */
      bResult = FALSE;
      break;
   }

   /* return final result */
   return bResult;

}

/**/

/*
 * CenterPopup( hWnd, hParentWnd ) : BOOL;
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
   HWND        hWnd,
   HWND        hParentWnd )
{
   int         xPopup;
   int         yPopup;
   int         cxPopup;
   int         cyPopup;
   int         cxScreen;
   int         cyScreen;
   int         cxParent;
   int         cyParent;
   RECT        rcWindow;

   /* retrieve main display dimensions */
   cxScreen = GetSystemMetrics( SM_CXSCREEN );
   cyScreen = GetSystemMetrics( SM_CYSCREEN );

   /* retrieve popup rectangle  */
   GetWindowRect( hWnd, (LPRECT)&rcWindow );

   /* calculate popup size */
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
