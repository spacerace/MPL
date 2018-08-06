/*
 * Form Library - UTILITY FUNCTIONS MODULE
 *
 * LANGUAGE      : Microsoft C 5.1
 * MODEL         : small
 * ENVIRONMENT   : Microsoft Windows 2.1 SDK
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

#include "form.h"
#include "form.d"

/* local dialog box data structures */
#define  CT_MASK        0x80
#define  CT_BUTTON      0x80
#define  CT_EDIT        0x81
#define  CT_STATIC      0x82
#define  CT_LIST        0x83
#define  CT_SCROLL      0x84
#define  CT_COMBOBOX    0x85

typedef struct {
   LONG        lStyle;
   BYTE        bItems;
   WORD        wX;
   WORD        wY;
   WORD        wCX;
   WORD        wCY;
   char        szText[1];
} PACKEDDLGBOX;

typedef struct {
   WORD        wX;
   WORD        wY;
   WORD        wCX;
   WORD        wCY;
   WORD        wID;
   LONG        lStyle;
   char        szClass[1];
   char        szWndText[1];
   BYTE        bInfo;
} PACKEDDLGCTL;

typedef PACKEDDLGBOX FAR *    LPPACKEDDLGBOX;
typedef PACKEDDLGCTL FAR *    LPPACKEDDLGCTL;

/**/

/* DIALOG BOX HANDLING FUNCTIONS */

/*
 * GetDlg( lpResData, lpDlgBox ) : BOOL;
 *
 *    lpResData      long pointer to packed dialog box structure
 *    lpDlgBox       long pointer to unpacked dialog box structure
 *
 * This function unpacks the dialog box data structure defined by
 * the specified resource and fills in the appropriate fields in the
 * structure provided.  A value of TRUE is returned if the process
 * was successful.
 *
 * Note that this function assumes that no menu name, class name, or
 * font information is specified with the dialog box template.
 *
 */

BOOL FAR PASCAL GetDlg(
   LPSTR             lpResData,  
   LPDLGBOX          lpDlgBox )
{
   LPSTR             lpResource;
   LPPACKEDDLGBOX    lpPackedDlgBox;

   /* coerce pointers */
   lpPackedDlgBox = (LPPACKEDDLGBOX)lpResData;

   /* extract fixed portions */
   lpDlgBox->lStyle = lpPackedDlgBox->lStyle;
   lpDlgBox->wControls = lpPackedDlgBox->bItems;
   lpDlgBox->wX = lpPackedDlgBox->wX;
   lpDlgBox->wY = lpPackedDlgBox->wY;
   lpDlgBox->wCX = lpPackedDlgBox->wCX;
   lpDlgBox->wCY = lpPackedDlgBox->wCY;

   /* extract variable portion */
   lpResource = (LPSTR)lpPackedDlgBox->szText;
   lpResource = (*lpResource == -1) ? lpResource+3 : lpResource+lstrlen(lpResource)+1;
   lpResource += lstrlen(lpResource) + 1;

   StringCopy( lpDlgBox->szCaption, lpResource, MAX_CAPTION );

   /* return result */
   return( TRUE );

}

/**/

/*
 * GetDlgData( hDlg ) : HANDLE;
 *
 *    hDlg           handle to dialog box 
 *
 * This function retrieves information from the data entry form and
 * returns a handle referencing a global block of memory.  A NULL
 * value is returned if the process was unsuccessful.
 *
 * Note that this function will not retrieve a combined field name
 * and data value larger than MAX_DATA.
 *
 */

HANDLE FAR PASCAL GetDlgData(
   HWND        hDlg )
{
   WORD        wPage;
   WORD        wControl;
   WORD        wValueSize;
   WORD        wDlgDataSize;
   HANDLE      hDlgData;
   HWND FAR *  lpCtlList;
   LPSTR       lpDlgData;

   char        szCtlData[MAX_DATA];

   /* initialization */
   wDlgDataSize = 0;

   /* allocate memory block */
   hDlgData = GlobalAlloc( GMEM_MOVEABLE|GMEM_ZEROINIT, (DWORD)32 );
   if ( hDlgData ) {
      
      /* loop through each dialog box page */
      for ( wPage=1; wPage<=FormInfo.wPages; wPage++ ) {

         /* access list of dialog page controls */
         lpCtlList = (HWND FAR *)GlobalLock( GET_CTLLIST(hDlg,wPage) );
         if ( lpCtlList ) {

            /* loop through each control */
            for ( wControl=1; wControl<(WORD)lpCtlList[0]; wControl++ ) {

               /* retrieve data associated with control */
               if ( GetDlgCtlData(hDlg,lpCtlList[wControl],szCtlData,sizeof(szCtlData)) ) {

                  /* calculate data value size */
                  wValueSize = lstrlen(szCtlData) + 1;

                  /* update global data block */
                  hDlgData = GlobalReAlloc( hDlgData, (DWORD)(wDlgDataSize+wValueSize+32), NULL );
                  if ( hDlgData ) {
                     lpDlgData = GlobalLock( hDlgData );
                     if ( lpDlgData ) {
                        lstrcpy( lpDlgData+wDlgDataSize, szCtlData );
                        wDlgDataSize = wDlgDataSize + wValueSize;
                        GlobalUnlock( hDlgData );
                     } else
                        WARNING( hDlg, "Unable to access data block!" );
                  } else
                     WARNING( hDlg, "Unable to reallocate data block!" );
               
               } 
            
            }
         
            /* unlock control list */
            GlobalUnlock( GET_CTLLIST(hDlg,wPage) );
         
         }
      
      }
      
      /* append final NULL */
      hDlgData = GlobalReAlloc( hDlgData, (DWORD)(wDlgDataSize+32), NULL );
      if ( hDlgData ) {
         lpDlgData = GlobalLock( hDlgData );
         if ( lpDlgData ) {
            *(lpDlgData+wDlgDataSize) = NULL;
            GlobalUnlock( hDlgData );
         } else
            WARNING( hDlg, "Unable to access data block! (null)" );
      } else
         WARNING( hDlg, "Unable to reallocate data block! (null)" );

   }

   /* return final result */
   return( hDlgData );

}

/**/

/*
 * GetDlgCtlData( hDlg, hWndCtl, lpszData, wMaxData ) : BOOL;
 *
 *    hDlg           handle to dialog box containing control
 *    hWndCtl        handle to control window
 *    lpszData       combined field name and data value
 *    wMaxData       maximum field name and data size
 *
 * This function retrieves the field name and data associated with the
 * control window provided.  If the control is unlisted then it is
 * assumed to be unimportant and a value of FALSE is returned.
 *
 * If the control is listed, but no data is defined then a VALUE of
 * TRUE is returned.  The resulting data value consists of the field
 * name followed by the appropriate default data value (in most cases,
 * this is nothing).
 *
 * Note that the following control types are NOT supported:
 *
 *    Scrollbars
 *    ComboBoxes
 *    ListBoxes
 *
 */

BOOL FAR PASCAL GetDlgCtlData(
   HWND        hDlg,
   HWND        hWndCtl,
   LPSTR       lpszData,
   WORD        wMaxData )
{
   BOOL        bResult;
   WORD        wCtlStyle;
   char        szCtlName[MAX_NAME];
   char        szCtlData[MAX_DATA];
   char        szClassName[MAX_NAME];

   /* initialization */
   bResult = FALSE;
   *lpszData = NULL;

   /* retrieve control name */
   if ( GetCtlName(INSTANCE(hDlg),ID(hWndCtl),szCtlName,sizeof(szCtlName)) ) {
 
      /* normal return */
      bResult = TRUE;
      szCtlData[0] = NULL;

      /* retrieve control class name & style flags */
      GetClassName( hWndCtl, szClassName, sizeof(szClassName) );
      wCtlStyle = LOWORD( GetWindowLong(hWndCtl,GWL_STYLE) );
   
      /* extract control data */
      if ( !lstrcmp(szClassName,"Button") ) {

         /* extract button control data */
         switch( wCtlStyle&0x000F )
            {
         case BS_CHECKBOX :
         case BS_AUTOCHECKBOX :
         case BS_RADIOBUTTON :
         case BS_3STATE :
         case BS_AUTO3STATE :
         case BS_AUTORADIOBUTTON :

            /* define TRUE-FALSE result */
            if ( SendMessage(hWndCtl,BM_GETCHECK,0,0L) )
               StringCopy( szCtlData, "TRUE", sizeof(szCtlData) );
            else
               StringCopy( szCtlData, "FALSE", sizeof(szCtlData) );

            break;
         case BS_PUSHBUTTON :
         case BS_DEFPUSHBUTTON :
         case BS_GROUPBOX :
         case BS_USERBUTTON :
         case BS_PUSHBOX :
         case BS_OWNERDRAW :
         default:
            GetWindowText( hWndCtl, szCtlData, sizeof(szCtlData) );
            break;
         }


      } else
         GetWindowText( hWndCtl, szCtlData, sizeof(szCtlData) );
      
      /* combine control name & data value */
      StringCopy( lpszData, szCtlName, wMaxData );
      StringCat( lpszData, ":", wMaxData );
      StringCat( lpszData, szCtlData, wMaxData );

   }

   /* return result */
   return( bResult );

}

/**/

/* CONTROL HANDLING FUNCTIONS */

/*
 * GetCtl( lpDlgData, wControl, lpDlgCtl ) : BOOL;
 *
 *    lpDlgData      long pointer to packed dialog box structure
 *    wControl       index to desired control
 *    lpDlgCtl       long pointer to unpacked control structure
 *
 * This function retrieves and unpacks a the specified dialog box
 * control data structure, returning the resulting data in the
 * unpacked control structure provided.  A value of TRUE is returned
 * if the process was successful.
 *                                                         
 */

BOOL FAR PASCAL GetCtl(
   LPSTR             lpDlgData,
   WORD              wControl,
   LPDLGCTL          lpDlgCtl )
{
   BYTE              bType;
   WORD              wEntry;
   BOOL              bResult;
   LPPACKEDDLGBOX    lpPackedDlgBox;
   LPPACKEDDLGCTL    lpPackedDlgCtl;

   /* initialization */
   bResult = FALSE;

   /* coerce pointers */
   lpPackedDlgBox = (LPPACKEDDLGBOX)lpDlgData;
   
   /* skip to control section */
   lpDlgData = (LPSTR)lpPackedDlgBox->szText;
   lpDlgData = (*lpDlgData == -1) ? lpDlgData+3 : lpDlgData+lstrlen(lpDlgData)+1;

   lpDlgData += lstrlen(lpDlgData) + 1;
   lpDlgData += lstrlen(lpDlgData) + 1;

   lpPackedDlgCtl = (LPPACKEDDLGCTL)lpDlgData;

   /* loop through controls */
   for ( wEntry=0; (wEntry<wControl)&&(wEntry<lpPackedDlgBox->bItems); wEntry++ ) {

      /* skip through data */
      lpDlgData = lpPackedDlgCtl->szClass;
      lpDlgData = (*lpDlgData & CT_MASK) ? lpDlgData+1 : lpDlgData+lstrlen(lpDlgData)+1;
      lpDlgData += lstrlen(lpDlgData) + 1;
      lpDlgData += sizeof( lpPackedDlgCtl->bInfo );

      /* define pointer to next control */
      lpPackedDlgCtl = (LPPACKEDDLGCTL)lpDlgData;

   }

   /* extract desired control */
   if ( wEntry == wControl ) {

      /* define return value */
      bResult = TRUE;

      /* extract fixed portions */
      lpDlgCtl->wX = lpPackedDlgCtl->wX;
      lpDlgCtl->wY = lpPackedDlgCtl->wY;
      lpDlgCtl->wCX = lpPackedDlgCtl->wCX;
      lpDlgCtl->wCY = lpPackedDlgCtl->wCY;
      lpDlgCtl->wID = lpPackedDlgCtl->wID;
      lpDlgCtl->lStyle = lpPackedDlgCtl->lStyle;

      /* define control class name */
      bType = (unsigned char) *lpPackedDlgCtl->szClass;
      if ( !(bType&CT_MASK) ) 
         if ( lstrcmp(lpPackedDlgCtl->szClass,"scrollbar") == 0 )
            bType = CT_SCROLL;

      switch( bType )
         {
      case CT_BUTTON :
         StringCopy( lpDlgCtl->szClass, "Button", MAX_CLASS );
         break;
      case CT_EDIT :
         StringCopy( lpDlgCtl->szClass, "Edit", MAX_CLASS );
         break;
      case CT_STATIC :
         StringCopy( lpDlgCtl->szClass, "Static", MAX_CLASS );
         break;
      case CT_LIST :
         StringCopy( lpDlgCtl->szClass, "ListBox", MAX_CLASS );
         break;
      case CT_SCROLL :
         StringCopy( lpDlgCtl->szClass, "ScrollBar", MAX_CLASS );
         break;
      case CT_COMBOBOX :
         StringCopy( lpDlgCtl->szClass, "ComboBox", MAX_CLASS );
         break;
      default :
         StringCopy( lpDlgCtl->szClass, "?", MAX_CLASS );
         break;
      }

      /* define control text */
      lpDlgData = lpPackedDlgCtl->szClass;
      lpDlgData = (*lpDlgData & CT_MASK) ? lpDlgData+1 : lpDlgData+lstrlen(lpDlgData)+1;

      StringCopy( lpDlgCtl->szText, lpDlgData, MAX_TEXT );

   }

   /* return result */
   return( bResult );

}

/**/

/*
 * GetCtlName( hInstance, wCtlID, lpszCtlName, wMaxCtlName ) : BOOL;
 *
 *    hInstance      instance handle of module
 *    wCtlID         child window control ID
 *    szCtlName      long pointer to control name
 *    wMaxCtlName    maximum size of control name
 *
 * Retrieves the control name from the applications string table
 * that references a particular control based on the child window
 * identifier provided.  A value of TRUE is returned if the search
 * was successful.
 *
 */

BOOL FAR PASCAL GetCtlName(
   HANDLE      hInstance,
   WORD        wCtlID,
   LPSTR       lpszCtlName,
   WORD        wMaxCtlName )
{
   WORD        w;
   BOOL        bResult;
   char        szString[MAX_TITLE];

   /* initialization */
   bResult = TRUE;
   *lpszCtlName = NULL;
   
   /* attempt to load string */
   if ( LoadString(hInstance,wCtlID,szString,sizeof(szString)) ) {
      for (w=0; (szString[w])&&(szString[w]!=':'); w++);
      szString[w] = NULL;
      StringCopy( lpszCtlName, szString, wMaxCtlName );
   } else
      bResult = FALSE;

   /* return result */
   return( bResult );
   
}

/**/

/*
 * GetCtlData( hData, lpszCtlName, lpszData, wMaxData ) : BOOL;
 *
 *    hData          handle to global data block
 *    lpszCtlName    long pointer to control name
 *    lpszData       long pointer to data block to define
 *    wMaxData       maximum size of data block
 *
 * This function retrieve a data value from the global data block
 * using the control name provided.  A value of TRUE is returned if the
 * search was successful. 
 *
 * This function is typically called when the initial dialog box
 * control data values are being defined.
 *
 * Also note that this function assumes that the data is correctly
 * formatted as follows:
 *
 *    <FIELDNAME>:<FIELDDATA> NULL
 *    <FIELDNAME>:<FIELDDATA> NULL
 *    NULL
 *
 */

BOOL FAR PASCAL GetCtlData(
   HANDLE      hData,
   LPSTR       lpszCtlName,
   LPSTR       lpszData,
   WORD        wMaxData )
{
   WORD        w;
   BOOL        bResult;
   LPSTR       lpszDataBlock;

   char        szCrntCtlName[MAX_NAME];

   /* initialization */
   bResult = FALSE;

   /* attempt to lock global data block */
   lpszDataBlock = GlobalLock( hData );
   if ( lpszData ) {

      /* search for desired entry */
      while ( (*lpszDataBlock)&&(bResult==FALSE) ) {

         /* extract field identifier */
         for ( w=0; (*lpszDataBlock)&&(*lpszDataBlock!=':'); lpszDataBlock++ )
            if ( w < MAX_NAME-1 )
               szCrntCtlName[w++] = *lpszDataBlock;

         szCrntCtlName[w] = NULL;
         lpszDataBlock++;

         /* compare to desired field name */
         if ( !lstrcmp(lpszCtlName,szCrntCtlName) ) {
            bResult = TRUE;
            StringCopy( lpszData, lpszDataBlock, wMaxData );
         } 

         /* skip to next record (plus one more character) */
         while ( *lpszDataBlock++ )
            ;

      }

      /* unlock data */
      GlobalUnlock( hData );

   }
   
   /* return value */
   return( bResult );

}

/**/

/*
 * GetCtlComment( hInstance, wCtlID, lpszComment, wMaxComment ) : BOOL;
 *
 *    hInstance      instance handle of module
 *    wCtlID         child window control ID
 *    lpszComment    long pointer to string variable
 *    wMaxComment    maximum size of string
 *
 * Retrieve string comment describing a particular control based on the
 * child window identifier provided.  A value of TRUE is returned if
 * the search was successful.
 *
 */

BOOL FAR PASCAL GetCtlComment(
   HANDLE      hInstance,
   WORD        wCtlID,
   LPSTR       lpszComment,
   WORD        wMaxComment )
{
   WORD        w;
   BOOL        bResult;
   char        szComment[MAX_TITLE];

   /* initialization */
   bResult = TRUE;
   *lpszComment = NULL;
   
   /* attempt to load string */
   if ( LoadString(hInstance,wCtlID,szComment,sizeof(szComment)) ) {
      for (w=0; (szComment[w])&&(szComment[w]!=':'); w++);
      StringCopy( lpszComment, &szComment[w+1], wMaxComment );
   } else
      bResult = FALSE;

   /* return result */
   return( bResult );
   
}

/**/

/*
 * SetCtlData( hDlg, hWndCtl, lpDlgCtl, lpszData ) : BOOL;
 *
 *    hDlg           handle to dialog box
 *    hWndCtl        handle to control
 *    lpDlgCtl       pointer to control template
 *    lpszData       data associated with the control
 *
 * This function is responsible for defining the information
 * associated with a particular control when the dialog box
 * is initially displayed.
 *
 * This function does not support control classes:
 *
 *    Scrollbars
 *    ComboBoxes
 *    ListBoxes
 *
 * A value of TRUE is returned if the operation was successful.
 *
 */

BOOL FAR PASCAL SetCtlData(
   HWND        hDlg,
   HWND        hWndCtl,
   LPDLGCTL    lpDlgCtl,
   LPSTR       lpszData )
{
   
   /* output data based on control type */
   if ( !lstrcmp(lpDlgCtl->szClass,"Button") ) {
   
      /* process different types of buttons */
      switch( LOWORD(lpDlgCtl->lStyle)&0x000F )
         {
      case BS_CHECKBOX :
      case BS_AUTOCHECKBOX :
      case BS_3STATE :
      case BS_AUTO3STATE :
         CheckDlgButton( hDlg, lpDlgCtl->wID, (lpszData[0]=='T')?TRUE:FALSE );
         break;
      case BS_RADIOBUTTON :
      case BS_AUTORADIOBUTTON :
         if ( lpszData[0] == 'T' )
            CheckRadioButton( hDlg, lpDlgCtl->wID, lpDlgCtl->wID, lpDlgCtl->wID );
         break;
      case BS_PUSHBUTTON :
      case BS_DEFPUSHBUTTON :
      case BS_GROUPBOX :
      case BS_USERBUTTON :
      case BS_PUSHBOX :
         SetWindowText( hWndCtl, lpszData );
         break;
      }
   
   } else
      SetWindowText( hWndCtl, lpszData );

   /* return result */
   return( TRUE );

}

/**/

/* STRING HANDLING UTILITY FUNCTIONS */

/*
 * StringMatch( lpszString, lpszPattern ) : BOOL;
 *
 *    lpszString        base string
 *    lpszPattern       pattern string
 *
 * This function returns TRUE if the pattern provided is replicated
 * in the first set of characters in the base string.
 *
 */

BOOL FAR PASCAL StringMatch(
   LPSTR       lpszString,
   LPSTR       lpszPattern )
{

   /* find first difference between strings */
   while ( (*lpszPattern)&&(*lpszPattern==*lpszString) ) {
      lpszString++;
      lpszPattern++;
   }

   /* return comparison */
   return( (*lpszPattern) ? FALSE : TRUE );

}

/**/

/*
 * StringCat( lpszDest, lpszSrce, wMaxDest ) : VOID;
 *
 *    lpszDest       string to append characters
 *    lpszSrce       string to copy from
 *    wMaxDest       maximum length of destination string
 *
 * This function appends the source string onto the end of the
 * destination string, while checking that the maximum length of
 * the destination string is not exceeded.
 *
 */

VOID FAR PASCAL StringCat(
   LPSTR       lpszDest,
   LPSTR       lpszSrce,
   WORD        wMaxDest )
{

   /* move to end of string */
   while ( *lpszDest ) {
      lpszDest++;
      wMaxDest--;
   }

   /* copy string & append null */
   while ( (--wMaxDest)&&(*lpszDest++=*lpszSrce++) );
   *lpszDest = NULL;

}

/**/

/*
 * StringCopy( lpszDest, lpszSrce, wMax ) : VOID;
 *
 *    lpszDest       string to copy characters
 *    lpszSrce       string to copy from
 *    wMaxDest       maximum number of characters to copy
 *
 * This function copies the source string to the destination
 * string while checking that less than the maximum number of
 * characters are transfered.  In an overflow case, a NULL is
 * automatically appended.
 *
 */

VOID FAR PASCAL StringCopy(
   LPSTR    lpszDest,
   LPSTR    lpszSrce,
   WORD     wMaxDest )
{

   /* copy string until null encountered */
   while ( (--wMaxDest)&&(*lpszDest++=*lpszSrce++) );
   *lpszDest = NULL;

}

/**/

/*
 * StringJoin( lpszDest, lpszSrce, wMaxDest ) : VOID;
 *
 *    lpszDest       string to append characters
 *    lpszSrce       string to copy from
 *    wMaxDest       maximum length of destination string
 *
 * This function appends the source string onto the end of the
 * destination string, including terminating null characters.  During
 * this process, the maximum size of the destination string is
 * checked to ensure that an overflow does not occur.
 *
 */

VOID FAR PASCAL StringJoin(
   LPSTR       lpszDest,
   LPSTR       lpsSrce,
   WORD        wMaxDest )
{

   /* search for double null characters */
   while ( *lpszDest )
      while ( (--wMaxDest)&&(*lpszDest++) );

   /* copy string & append null */
   while ( (--wMaxDest)&&(*lpszDest++=*lpsSrce++) );
   *lpszDest = NULL;

}

/**/

/* MISC UTILITY FUNCTIONS */

/*
 * GetPrinterDC() : HDC;
 *
 *
 * This function creates a printer display context for the current
 * default printer and returns it to the calling routine. 
 *
 */

HDC FAR PASCAL GetPrinterDC()
{
   LPSTR    lpszDevice;
   LPSTR    lpszDriver;
   LPSTR    lpszOutput;
   char     szPrinter[64];

   /* retrieve profile string */
   GetProfileString( "windows", "device", "", szPrinter, sizeof(szPrinter) );

   /* extract device name */
   lpszDevice = szPrinter;
   lpszOutput = szPrinter;

   while ( *lpszOutput != ',' )
      lpszOutput++;
   *lpszOutput++ = NULL;

   /* extract driver name */
   lpszDriver = lpszOutput;

   while ( *lpszOutput != ',' )
      lpszOutput++;
   *lpszOutput++ = NULL;

   /* return value */
   return( CreateDC(lpszDriver,lpszDevice,lpszOutput,NULL) );

}

/**/

/*
 * CenterPopup( hWnd, hWndParent ) : VOID;
 *
 *    hWnd           window handle
 *    hWndParent     parent window handle
 *
 * This routine centers the popup window in the screen or display
 * using the window handles provided.  The window is centered over
 * the parent if the parent window is valid.  Special provision
 * is made for the case when the popup would be centered outside
 * the screen - in this case it is positioned at the appropriate
 * border.
 *
 */

VOID FAR PASCAL CenterPopup(
   HWND        hWnd,
   HWND        hWndParent )
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
   if ( hWndParent ) {

      /* retrieve parent rectangle */
      GetWindowRect( hWndParent, (LPRECT)&rcWindow );

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

}

/**/

/*
 * FormMsgHookFn( wContext, wCode, lData ) : WORD;
 *
 *    wContext       message context
 *    wCode          message code
 *    lData          supplemental information
 *
 * This function intercepts the following set of function keys and
 * responds with the appropriate action:
 *
 *    PgUp           move to previous form page
 *    PgDn           move to next form page
 *
 */

WORD FAR PASCAL FormMsgHookFn(
   WORD        wContext,
   WORD        wCode,
   LONG        lData )
{
   WORD        wResult;

   /* initialization */
   wResult = TRUE;

   /* check if message is of interest */
   if ( (wContext==MSGF_DIALOGBOX)&&(wCode==HC_ACTION) ) {

      if ( ((MSG FAR *)lData)->message==WM_KEYDOWN ) {
      
         /* process virtual key code */
         switch( ((MSG FAR *)lData)->wParam )
            {
         case VK_PRIOR :
            SendMessage( GetDlgItem(PARENT(((MSG FAR *)lData)->hwnd),ID0_PAGE), WM_KEYDOWN, VK_PRIOR, 0L );
            break;
         case VK_NEXT :
            SendMessage( GetDlgItem(PARENT(((MSG FAR *)lData)->hwnd),ID0_PAGE), WM_KEYDOWN, VK_NEXT, 0L );
            break;
         default :
            wResult = (WORD)DefHookProc( wContext, wCode, lData, (FARPROC FAR *)&FormInfo.fnOldMsgHook );
            break;
         }
   
      } else 
         if ( ((MSG FAR *)lData)->message!=WM_KEYUP ) 
            wResult = (WORD)DefHookProc( wContext, wCode, lData, (FARPROC FAR *)&FormInfo.fnOldMsgHook );

   } else
      wResult = (WORD)DefHookProc( wContext, wCode, lData, (FARPROC FAR *)&FormInfo.fnOldMsgHook );
   
   /* pass message to default hook function */
   return( wResult );

}

/**/

/*
 * Warning( hWndParent, lpszMessage, lpszFile, wLine ) : VOID;
 *
 *    hWndParent     parent window handle
 *    lpszMessage    pointer to error message
 *    lpszFile       pointer to current file
 *    wLine          line number of error
 *
 * This function displays a message box that describes the error
 * encountered.  Included with the error is an identifier that can
 * be used to track down the file and line-number in question.
 *
 */

VOID FAR PASCAL Warning(
   HWND        hWndParent,
   LPSTR       lpszMessage,
   LPSTR       lpszFile,
   WORD        wLine )
{
   char        szWarning[256];

   /* create error message & display in a message box */
   wsprintf( szWarning, "%s  [%s:%u]", lpszFile, lpszMessage, wLine );
   MessageBox( hWndParent, szWarning, FormInfo.szTitle, MB_OK|MB_ICONHAND );

}

