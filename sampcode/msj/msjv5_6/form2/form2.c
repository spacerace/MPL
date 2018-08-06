/*
 * FORM LIBRARY - EDIT FORM MODULE
 *
 * LANGUAGE      : Microsoft C 5.1
 * MODEL         : medium
 * ENVIRONMENT   : Microsoft Windows 3.0 SDK
 * STATUS        : operational
 *
 *    Eikon Systems, Inc.
 *    989 East Hillsdale Blvd, Suite 260
 *    Foster City, California 94404
 *    415-349-4664
 *
 * 07/12/90 1.00 - Kevin P. Welch - initial creation.
 *
 */

#define  NOCOMM
 
#include <windows.h>
#include <control.h>

#include "page.h"

#include "form.h"
#include "form.d"

/* local macro definitions */
#define  xRCtoDEV(x,y)        (((x)*LOWORD(y))/4)
#define  yRCtoDEV(x,y)        (((x)*HIWORD(y))/8)
#define  xDEVtoRC(x,y)        (((x)*4)/LOWORD(y))
#define  yDEVtoRC(x,y)        (((x)*8)/HIWORD(y))

/* local function definitions */
VOID FAR PASCAL   EditDisplay( HWND, WORD );
BOOL FAR PASCAL   EditInit( HWND, WORD, LONG );
BOOL FAR PASCAL   EditControl( HWND, WORD, LONG );
BOOL FAR PASCAL   EditCommand( HWND, WORD, LONG );
BOOL FAR PASCAL   EditDestroy( HWND, WORD, LONG );

/**/

/*
 * FormEdit( hWndParent, hData ) : HANDLE
 *
 *    hWndParent     handle to parent window
 *    hData          handle to form data block
 *
 * This function displays a dialog box, enabling the user to enter
 * data using a series of linked dialog boxes.  The value returned by
 * this function is a global handle with the new data block.  If a NULL
 * handle is returned then it can be assumed that the data entry was
 * cancelled.
 *
 */

HANDLE FAR PASCAL FormEdit(
   HWND           hWndParent,
   HANDLE         hData )
{
   DLGBOX         DlgBox;
   HANDLE         hResult;
   HANDLE         hResInfo;
   HANDLE         hResData;
   HANDLE         hDlgData;
   LPSTR          lpResData;
   LPDLGBOX       lpDlgData;

   /* initialization */
   hResult = NULL;
   FormInfo.hTempData = hData;

   /* find first dialog box */
   hResInfo = FindResource( FormInfo.hInstance, "Page1", RT_DIALOG );
   if ( hResInfo ) {

      /* load first dialog box */
      hResData = LoadResource( FormInfo.hInstance, hResInfo );
      if ( hResData ) {

         /* access first dialog box */
         lpResData = LockResource( hResData );
         if ( lpResData ) {
            
            /* retrieve dialog box */
            GetDlg( lpResData, &DlgBox );
            UnlockResource( hResData );
      
            /* allocate dialog template */
            hDlgData = GlobalAlloc( GMEM_MOVEABLE|GMEM_ZEROINIT, (DWORD)sizeof(DLGBOX) );
            if ( hDlgData ) {

               /* define the contents */
               lpDlgData = (LPDLGBOX)GlobalLock( hDlgData );
               if ( lpDlgData ) {

                  lpDlgData->lStyle = WS_BORDER | WS_CAPTION | WS_DLGFRAME | WS_SYSMENU | WS_POPUP;
                  lpDlgData->wControls = 0;
                  lpDlgData->wX = 0;
                  lpDlgData->wY = 0;
                  lpDlgData->wCX = DlgBox.wCX + 48;
                  lpDlgData->wCY = DlgBox.wCY + 20;

                  lpDlgData->szMenu[0] = 0;
                  lpDlgData->szClass[0] = 0;

                  StringCopy( 
                     lpDlgData->szCaption, 
                     FormInfo.szTitle,
                     sizeof(lpDlgData->szCaption)
                  );

                  /* unlock template */
                  GlobalUnlock( hDlgData );

                  /* install keyboard hook & edit form */
                  FormInfo.fnOldMsgHook = SetWindowsHook( WH_MSGFILTER, FormMsgHookFn );
                  hResult = DialogBoxIndirect( FormInfo.hInstance, hDlgData, hWndParent, FormEditDlgFn );
                  UnhookWindowsHook( WH_MSGFILTER, FormMsgHookFn );

                  /* remove keyboard hook */
                  FormInfo.fnOldMsgHook = NULL;
   
               }

               /* free template */
               GlobalFree( hDlgData );

            }

         }

         FreeResource( hResData );

      }
   }

   /* return final result */
   return( hResult );

}

/**/

/*
 * FormEditDlgFn( hDlg, wMessage, wParam, lParam ) : BOOL;
 *
 *    hDlg           dialog box window handle
 *    wMessage       dialog box message
 *    wParam         word parameter
 *    lParam         long parameter
 *
 * This function is responsible for processsing all the messages relating
 * to the edit form dialog box.
 *
 */

BOOL FAR PASCAL FormEditDlgFn(
   HWND     hDlg,
   WORD     wMessage,
   WORD     wParam,
   LONG     lParam )
{
   BOOL     bResult;

   /* process message */
   switch ( wMessage )
      {
   case WM_INITDIALOG : /* initialize dialog box */
      bResult = EditInit( hDlg, wParam, lParam );
      break;
   case WM_CTLCOLOR : /* window control color */
      bResult = EditControl( hDlg, wParam, lParam );
      break;
   case WM_USER : /* user defined message */
      bResult = EditControl( hDlg, 0, MAKELONG(GetFocus(),0) );
      break;
   case WM_COMMAND: /* dialog command */
      bResult = EditCommand( hDlg, wParam, lParam );
      break;
   case WM_DESTROY : /* destroy dialog box */
      bResult = EditDestroy( hDlg, wParam, lParam );
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
 * EditInit( hDlg, wParam, lParam ) : BOOL;
 *
 *    hDlg           handle to dialog box
 *    wParam         word parameter
 *    lParam         long parameter
 *
 * The function defines each of the control pages and initializes the
 * dialog box.  A value of TRUE is returned if the process was successful
 * and the dialog box manager is to set the window with the initial
 * input focus.  A value of FALSE is returned if the process was
 * successful and the dialog box manager is not to set the window with
 * the initial input focus.
 *
 */

BOOL FAR PASCAL EditInit(
   HWND        hDlg,
   WORD        wParam,
   LONG        lParam )
{
   WORD        wPage;
   WORD        wWidth;
   WORD        wHeight;
   WORD        wControl;
   LONG        lDlgUnits;
   HWND        hWndCtl;
   HANDLE      hDlgInfo;
   HANDLE      hDlgData;
   HANDLE      hCtlList;
   RECT        rcClient;
   LPSTR       lpDlgData;
   HWND FAR *  lpCtlList;

   char        szCtlName[MAX_NAME];
   char        szDlgName[MAX_NAME];
   char        szCtlData[MAX_DATA];

   DLGBOX      DlgBox;
   DLGCTL      DlgCtl;

   /* warning level 3 compatibility */
   wParam;
   lParam;
   
   /* define property lists */
   SET_COMMENT( hDlg, NULL );
   SET_CRNTPAGE( hDlg, NULL );
   SET_DATA( hDlg, FormInfo.hTempData );
   
   /* retrieve dialog base units */
   lDlgUnits = GetDialogBaseUnits();

   /* calculate dialog box dimensions in RC coordinates */
   GetClientRect( hDlg, &rcClient );
   
   wWidth = xDEVtoRC( rcClient.right - rcClient.left, lDlgUnits );
   wHeight = yDEVtoRC( rcClient.bottom - rcClient.top, lDlgUnits );
   
   /* create all controls associated with the base page */
   CreateWindow(
      "button",
      "&Save",
      BS_DEFPUSHBUTTON | WS_GROUP | WS_TABSTOP | WS_CHILD | WS_VISIBLE,
      xRCtoDEV(wWidth-36,lDlgUnits),
      yRCtoDEV(4,lDlgUnits),
      xRCtoDEV(32,lDlgUnits),
      yRCtoDEV(12,lDlgUnits),
      hDlg,
      ID0_SAVE,
      INSTANCE(PARENT(hDlg)),
      NULL
   );
     
   CreateWindow(
      "button",
      "&Print",
      BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE,
      xRCtoDEV(wWidth-36,lDlgUnits),
      yRCtoDEV(18,lDlgUnits),
      xRCtoDEV(32,lDlgUnits),
      yRCtoDEV(12,lDlgUnits),
      hDlg,
      ID0_PRINT,
      INSTANCE(PARENT(hDlg)),
      NULL
   );

   CreateWindow(
      "button",
      "Cancel",
      BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE, 
      xRCtoDEV(wWidth-36,lDlgUnits), 
      yRCtoDEV(32,lDlgUnits), 
      xRCtoDEV(32,lDlgUnits), 
      yRCtoDEV(12,lDlgUnits),
      hDlg,
      ID0_CANCEL,
      INSTANCE(PARENT(hDlg)),
      NULL
   );

   CreateWindow(
      "page",
      "",
      WS_GROUP | WS_TABSTOP | WS_CHILD | WS_VISIBLE,
      xRCtoDEV(wWidth-36,lDlgUnits),
      yRCtoDEV(48,lDlgUnits),
      xRCtoDEV(32,lDlgUnits),
      yRCtoDEV(wHeight-52,lDlgUnits),
      hDlg,
      ID0_PAGE,
      INSTANCE(PARENT(hDlg)),
      NULL
   );
   
   CreateWindow(
      "static",
      "Help text associated with control.",
      SS_LEFT | WS_GROUP | WS_CHILD | WS_VISIBLE,
      xRCtoDEV(4,lDlgUnits),
      yRCtoDEV(wHeight-12,lDlgUnits),
      xRCtoDEV(wWidth-48,lDlgUnits),
      yRCtoDEV(8,lDlgUnits),
      hDlg,
      ID0_COMMENT,
      INSTANCE(PARENT(hDlg)),
      NULL
   ); 

   CreateWindow(
      "static",
      "",
      SS_BLACKFRAME | WS_CHILD | WS_VISIBLE,
      xRCtoDEV(wWidth-40,lDlgUnits),
      yRCtoDEV(0,lDlgUnits),
      xRCtoDEV(1,lDlgUnits),
      yRCtoDEV(wHeight,lDlgUnits),
      hDlg,
      (WORD)-1,
      INSTANCE(PARENT(hDlg)),
      NULL
   );
   
   /* create all controls associated with each page */
   for ( wPage=1; wPage<=FormInfo.wPages; wPage++ ) {

      /* define dialog box name */
      wsprintf( szDlgName, "Page%u", wPage );

      /* load dialog page */
      hDlgInfo = FindResource( FormInfo.hInstance, szDlgName, RT_DIALOG );
      if ( hDlgInfo ) {

         /* load resource information */
         hDlgData = LoadResource( FormInfo.hInstance, hDlgInfo );
         if ( hDlgData ) {

            /* lock resource data */
            lpDlgData = LockResource( hDlgData );
            if ( lpDlgData ) {

               /* extract dialog box */
               GetDlg( lpDlgData, &DlgBox );

               /* allocate space for window handle list */
               hCtlList = GlobalAlloc( GMEM_MOVEABLE|GMEM_ZEROINIT, (DWORD)((DlgBox.wControls+4)*sizeof(HWND)) );
               if ( hCtlList ) {
                  
                  /* lock the data down */
                  lpCtlList = (HWND FAR *)GlobalLock( hCtlList );
                  if ( lpCtlList ) {

                     lpCtlList[0] = (HWND)DlgBox.wControls;

                     /* extract each control */
                     for ( wControl=0; wControl<DlgBox.wControls; wControl++ ) {

                        /* extract control (albeit inefficiently) */
                        GetCtl( lpDlgData, wControl, &DlgCtl );
                        
                        /* create window */
                        hWndCtl = CreateWindow (
                                       DlgCtl.szClass,
                                       DlgCtl.szText,
                                       DlgCtl.lStyle,
                                       xRCtoDEV(DlgCtl.wX+4,lDlgUnits),
                                       yRCtoDEV(DlgCtl.wY+4,lDlgUnits),
                                       xRCtoDEV(DlgCtl.wCX,lDlgUnits),
                                       yRCtoDEV(DlgCtl.wCY,lDlgUnits),
                                       hDlg,
                                       DlgCtl.wID,
                                       INSTANCE(PARENT(hDlg)),
                                       NULL
                                    );  

                        /* continue if successful */
                        if ( hWndCtl ) {

                           /* hide window & save window handle */
                           ShowWindow( hWndCtl, SW_HIDE );
                           lpCtlList[wControl+1] = hWndCtl;

                           /* define control data */
                           if ( GetCtlName(FormInfo.hInstance,DlgCtl.wID,szCtlName,sizeof(szCtlName)) ) 
                              if ( GetCtlData(GET_DATA(hDlg),szCtlName,szCtlData,sizeof(szCtlData)) ) 
                                 SetCtlData( hDlg, hWndCtl, &DlgCtl, szCtlData );

                        } else {
                           lpCtlList[wControl+1] = NULL;
                           WARNING( NULL, "Unable to create control!" );
                        }
               
                     }

                     /* unlock the data & store */
                     GlobalUnlock( hCtlList );
                     SET_CTLLIST( hDlg, wPage, hCtlList );

                  } else
                     WARNING( NULL, "Unable to lock control list!" );
               
               } else
                  WARNING( NULL, "Unable to allocate control list!" );

               /* unlock resource */
               UnlockResource( hDlgData );

            } else
               WARNING( NULL, "Unable to lock page information!" );

            /* free resource */
            FreeResource( hDlgData );

         } else
            WARNING( NULL, "Unable to load page information!" );

      } else
         WARNING( NULL, "Unable to find page information!" );

   }

   /* center dialog box within parent window */
   CenterPopup( hDlg, GetParent(hDlg) );

   /* define page selector range */
   SendMessage( 
      GetDlgItem(hDlg,ID0_PAGE), 
      PM_SETRANGE, 
      FormInfo.wPages, 
      0L
   );

   /* display first page */
   EditDisplay( hDlg, 1 );

   /* return result */
   return( FALSE );

}

/**/

/*
 * EditControl( hDlg, wParam, lParam ) : BOOL;
 *
 *    hDlg           handle to dialog box
 *    wParam         word parameter
 *    lParam         long parameter
 *
 * The function is responsible for handling the WM_CTLCOLOR message
 * and displaying the related help text in the comment field.  A value
 * of TRUE is returned if the WM_CTLCOLOR message was processed.
 *
 */

BOOL FAR PASCAL EditControl(
   HWND        hDlg,
   WORD        wParam,
   LONG        lParam )
{
   char        szComment[256];

   /* warning level 3 compatibility */
   wParam;
   
   /* display appropriate help text comment */
   if ( (LOWORD(lParam)!=GET_COMMENT(hDlg))&&(IsWindowEnabled(hDlg))&&(LOWORD(lParam)==GetFocus()) ) {

      /* update instance variable */
      SET_COMMENT( hDlg, LOWORD(lParam) );
   
      /* define new comment */
      switch( ID(LOWORD(lParam)) )
         {
      case ID0_SAVE :
         SetWindowText(
            GetDlgItem(hDlg,ID0_COMMENT), 
            "Save data entry form contents."
         );
         break;
      case ID0_PRINT :
         SetWindowText( 
            GetDlgItem(hDlg,ID0_COMMENT), 
            "Print data entry form contents."
         );
         break;
      case ID0_CANCEL :
         SetWindowText( 
            GetDlgItem(hDlg,ID0_COMMENT), 
            "Close data entry form without saving."
         );
         break;
      case ID0_PAGE :
         SetWindowText( 
            GetDlgItem(hDlg,ID0_COMMENT), 
            "Data entry form page selector."
         );
         break;
      default :
         GetCtlComment( INSTANCE(hDlg), ID(LOWORD(lParam)), szComment, sizeof(szComment) );
         SetWindowText( GetDlgItem(hDlg,ID0_COMMENT), szComment );
         break;
      }

   }

   /* return result */
   return( FALSE );

}

/**/

/*
 * EditCommand( hDlg, wParam, lParam ) : BOOL;
 *
 *    hDlg           handle to dialog box
 *    wParam         word parameter
 *    lParam         long parameter
 *
 * The function is responsible for handling the WM_COMMAND messages
 * received by the main dialog box window function.  A value of TRUE
 * is returned if the process was successful.
 *
 */

BOOL FAR PASCAL EditCommand(
   HWND        hDlg,
   WORD        wParam,
   LONG        lParam )
{
   
   /* make sure control description field is updated */
   EditControl( hDlg, 0, MAKELONG(GetFocus(),0) );

   /* process commands */
   switch( wParam )
      {
   case ID0_SAVE : /* save updated form */
   case IDOK :
      {
         HANDLE      hData;

         hData = GetDlgData( hDlg );
         if ( hData )
            EndDialog( hDlg, hData );
         else
            WARNING( hDlg, "Unable to save form!" );

      }
      break;
   case ID0_PRINT : /* print current form */
      {
         HANDLE      hData;

         hData = GetDlgData( hDlg );
         if ( hData ) {
            FormPrint( hDlg, hData );
            GlobalFree( hData );
         } else
            WARNING( hDlg, "Unable to print form!" );

      }
      break;
   case ID0_CANCEL : /* cancel data entry */
   case IDCANCEL :
      
      /* verify user intentions */
      if ( MessageBox(hDlg,"Close form without saving changes?",FormInfo.szTitle,MB_YESNO|MB_ICONQUESTION) == IDYES )
         EndDialog( hDlg, NULL );

      break;
   case ID0_PAGE : /* new dialog page selected */

      /* display appropriate page */
      if ( HIWORD(lParam) == PN_SELCHANGE ) 
         EditDisplay( hDlg, (WORD)SendMessage(LOWORD(lParam),PM_GETSELECT,0,0L)+1 );

      break;
   default : /* something else - ignore */
      break;
   }

   /* return result */
   return( TRUE );

}

/**/

/*
 * EditDisplay( hDlg, wNewPage ) : VOID;
 *
 *    hDlg           handle to a dialog box
 *    wNewPage       number of page to display
 *
 * This function hides the controls relating to the old dialog box
 * page and displays those associated with the new one.  If a NULL
 * page number is specified then it is assumed not to be present.
 *
 */

VOID FAR PASCAL EditDisplay(
   HWND        hDlg,
   WORD        wNewPage )
{
   WORD        wOldPage;
   WORD        wControl;
   WORD        wControls;
   HWND FAR *  lpCtlList;

   /* retrieve old page */
   wOldPage = GET_CRNTPAGE(hDlg);

   /* hide old page */
   if ( wOldPage && GET_CTLLIST(hDlg,wOldPage) ) {

      /* access controls associated with old page */
      lpCtlList = (HWND FAR *)GlobalLock( GET_CTLLIST(hDlg,wOldPage) );
      if ( lpCtlList ) {

         /* retrieve number of controls */
         wControls = (WORD)lpCtlList[0];

         /* hide each control */
         for ( wControl=1; wControl<=wControls; wControl++ ) {
            ShowWindow( lpCtlList[wControl], SW_HIDE );
            UpdateWindow( lpCtlList[wControl] );
         }

         /* unlock list */
         GlobalUnlock( GET_CTLLIST(hDlg,wOldPage) );

      } else
         WARNING( NULL, "Unable to lock control list!" );

   }

   /* display new page */
   if ( wNewPage && GET_CTLLIST(hDlg,wNewPage) ) {

      /* access controls associated with new page */
      lpCtlList = (HWND FAR *)GlobalLock( GET_CTLLIST(hDlg,wNewPage) );
      if ( lpCtlList ) {

         /* retrieve number of controls */
         wControls = (WORD)lpCtlList[0];

         /* display each control (in reverse order) */
         for ( wControl=wControls; wControl>0; wControl-- ) {
            ShowWindow( lpCtlList[wControl], SW_SHOW );
            UpdateWindow( lpCtlList[wControl] );
         } 

         /* set focus to first in list (if desired) */
         if ( GetFocus() != GetDlgItem(hDlg,ID0_PAGE) )
            SetFocus( lpCtlList[1] );
         
         /* unlock list */
         GlobalUnlock( GET_CTLLIST(hDlg,wNewPage) );

      } else
         WARNING( NULL, "Unable to lock control list!" );

   }

   /* update property list */
   SET_CRNTPAGE( hDlg, wNewPage );

}

/**/

/*
 * EditDestroy( hDlg, wParam, lParam ) : BOOL;
 *
 *    hDlg           handle to dialog box
 *    wParam         word parameter
 *    lParam         long parameter
 *
 * The function is responsible for handling the case when the
 * WM_DESTROY message is received by the main dialog box window
 * function.  A value of TRUE is returned if the cleanup process
 * was successful.
 *
 */

BOOL FAR PASCAL EditDestroy(
   HWND        hDlg,
   WORD        wParam,
   LONG        lParam )
{
   WORD        wPage;
   HANDLE      hCtlList;

   /* warning level 3 compatibility */
   wParam;
   lParam;
   
   /* remove property lists */
   REM_DATA( hDlg );
   REM_COMMENT( hDlg );
   REM_CRNTPAGE( hDlg );

   for ( wPage=1; wPage<=FormInfo.wPages; wPage++ ) {
      hCtlList = REM_CTLLIST(hDlg,wPage);
      if ( hCtlList )
         GlobalFree( hCtlList );
   }
   
   /* return result */
   return( TRUE );

}




