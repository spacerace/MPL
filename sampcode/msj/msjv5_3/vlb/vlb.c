/*
**  vlb.c     Virtual List Box Source Code file
**
**  Author: Robert A. Wood
**          Executive Micro Systems
**          1716 Azurite Trail
**          Plano, TX 75075
**
**  Microsoft C version 5.1 / medium memory model
**  Microsoft Windows SDK version 2.1
**  Runtime: Windows 286 version 2.1
**
**    Date: 03/19/90
*/


#include <windows.h>
#undef   min
#undef   max
#include <stdlib.h>
#include <stdio.h>
#include "vlb.h"
#include "lmem.h"

static   char     StringBuffer[ VLBSTRLEN + 1 ];
static   char     szVLBPropName[]   =  "VLB";
static   FARPROC  lpfnVLBProc =  NULL;

LPSTR FAR PASCAL lstrcpy (LPSTR, LPSTR) ;
WORD FAR PASCAL Rpad( LPSTR str, WORD length );

/*
**  Virtual List Box initialization function
********************************************************************/
BOOL FAR PASCAL InitVLB( HANDLE hInstance, HWND hDlg,
                         WORD ListBoxId, VLBPROC VLBCallBack )
{
   int         x;
   HANDLE      hCtl;
   HANDLE      hVLB;
   LPVLB       lpVLB;
   HDC         hDC;
   TEXTMETRIC  tm;            // need info about character sizes
   LONG  FAR * lpStringIds;

   // check that a call back function address was passed in
   if( ! VLBCallBack )
   {
      MessageBeep( 0 );
      MessageBox( hDlg, "Invalid VLBCallBack", NULL,
                  MB_ICONHAND | MB_OK );
      return( FALSE );
   }

   // check for existence of ListBoxId in specified dialog box
   if( ( hCtl = GetDlgItem( hDlg, ListBoxId ) ) == NULL )
   {
      MessageBeep( 0 );
      MessageBox( hDlg, "Invalid List Box Id", NULL,
                  MB_ICONHAND | MB_OK );
      return( FALSE );
   }

   // allocate VLB control information structure
   if( hVLB = GlobalAlloc( GHND, (LONG)sizeof( VLB ) ) )
      lpVLB = (LPVLB)GlobalLock( hVLB );
   else
   {
      SendMessage( hDlg, WM_COMMAND, ListBoxId,
                   MAKELONG( hCtl, VLBN_ERRSPACE ) );
      return( FALSE );
   }

   hDC = GetDC( hCtl );
   GetTextMetrics( hDC, &tm );
   ReleaseDC( hCtl, hDC );

   lpVLB->ListBoxId = ListBoxId;
   lpVLB->MultiSelection = (BOOL)
                (GetWindowLong( hCtl, GWL_STYLE ) & LBS_MULTIPLESEL);
   GetClientRect( hCtl, &lpVLB->ClientRect );
   lpVLB->CharWidth = tm.tmAveCharWidth;
   lpVLB->CharHeight = tm.tmHeight + tm.tmExternalLeading;
   lpVLB->DisplayStrings = (BYTE)
                        lpVLB->ClientRect.bottom / lpVLB->CharHeight;
   lpVLB->DisplayChars = (BYTE)
                          lpVLB->ClientRect.right / lpVLB->CharWidth;
   lpVLB->FocusString = 0;
   lpVLB->FirstDisplayString = 0;
   lpVLB->TotalSelectedStrings = 0;
   lpVLB->MaxSelectedStrings = lpVLB->DisplayStrings;
   VLB_CALLBACK = VLBCallBack;
   lpVLB->SelectedStringId = -1;

   // allocate buffer for displayed strings
   if( !(lpVLB->hDisplayBuffer = GlobalAlloc( GHND, (LONG)
           (lpVLB->DisplayStrings * (lpVLB->DisplayChars + 1) ) ) ) )
   {
      GlobalUnlock( hVLB );
      GlobalFree( hVLB );
      SendMessage( hDlg, WM_COMMAND, ListBoxId,
                   MAKELONG( hCtl, VLBN_ERRSPACE ) );
      return( FALSE );
   }

   // allocate array of longs for the string Ids of displayed strings
   if( !(lpVLB->hStringIds = GlobalAlloc( GHND, (LONG)
                      (lpVLB->DisplayStrings * sizeof( LONG ) ) ) ) )
   {
      GlobalUnlock( lpVLB->hDisplayBuffer );
      GlobalFree( lpVLB->hDisplayBuffer );
      GlobalUnlock( hVLB );
      GlobalFree( hVLB );
      SendMessage( hDlg, WM_COMMAND, ListBoxId,
                   MAKELONG( hCtl, VLBN_ERRSPACE ) );
      return( FALSE );
   }

   // initialize string Ids to -1
   lpStringIds = (LONG FAR *) GlobalLock( lpVLB->hStringIds );
   for( x = 0; x < lpVLB->DisplayStrings; x++, lpStringIds[x] = -1L )
      ;
   GlobalUnlock( lpVLB->hStringIds );

   // setup for a multiselection list box
   if( lpVLB->MultiSelection )
   {
      LONG  FAR * lpSelectedStringIds;

      // allocate array of longs for the selected string Id's
      if( !(lpVLB->hSelectedStringIds = GlobalAlloc( GHND, (LONG)
                  (lpVLB->MaxSelectedStrings * sizeof( LONG ) ) ) ) )
      {
         GlobalUnlock( lpVLB->hStringIds );
         GlobalFree( lpVLB->hStringIds );
         GlobalUnlock( lpVLB->hDisplayBuffer );
         GlobalFree( lpVLB->hDisplayBuffer );
         GlobalUnlock( hVLB );
         GlobalFree( hVLB );
         SendMessage( hDlg, WM_COMMAND, ListBoxId,
                                   MAKELONG( hCtl, VLBN_ERRSPACE ) );
         return( FALSE );
      }

      // initialize selected string Ids to -1
      lpSelectedStringIds = (LONG FAR *)
                             GlobalLock( lpVLB->hSelectedStringIds );
      for( x = 0; x < lpVLB->DisplayStrings;
           x++, lpSelectedStringIds[x] = -1L )
         ;
      GlobalUnlock( lpVLB->hSelectedStringIds );
   }

   if( !lpfnVLBProc )
      lpfnVLBProc = MakeProcInstance( (FARPROC) VLBProc, hInstance );

   // subclass VLB
   SetWindowLong( hCtl, GWL_WNDPROC, (LONG) lpfnVLBProc );

   // store handle to VLB structure in the control's property list
   SetProp( hCtl, szVLBPropName, hVLB );

   // send create messages to control and to the VLB callback func
   SendMessage( hCtl, WM_CREATE, 0, 0L );
   VLB_CALLBACK( ListBoxId, VCB_CREATE, 0, 0L );

   // get the vitual list box length and set the scroll range
   lpVLB->TotalStrings = VLB_CALLBACK( ListBoxId, VCB_LENGTH, 0, 0L);
   SetScrollRange( hCtl, SB_VERT, 0, (int)
             lpVLB->TotalStrings - lpVLB->DisplayStrings - 1, TRUE );

   // check for horizontally scrolling
   if( GetWindowLong( hCtl, GWL_STYLE ) & WS_HSCROLL )
   {
      // get the vitual list box width and set the scroll range
      lpVLB->TotalWidth = VLB_CALLBACK( ListBoxId, VCB_WIDTH, 0, 0L);
      SetScrollRange( hCtl, SB_HORZ, 0, (int)
                                       lpVLB->TotalWidth - 1, TRUE );
   }

   // load list box
   VLB_CALLBACK( ListBoxId, VCB_SETFOCUS, 0, 0L );
   SendMessage( hCtl, VLB_RELOAD, RELOAD_STRINGPOS, 0L );
   VLB_CALLBACK( ListBoxId, VCB_KILLFOCUS, 0, 0L );

   // I'm out of here
   GlobalUnlock( hVLB );
   return( TRUE );
}

/*
**  Virtual List Box Procedure
********************************************************************/
LONG FAR PASCAL VLBProc( HWND hCtl, unsigned message,
                         WORD wParam, LONG lParam )
{
   HANDLE   hVLB;
   LPVLB    lpVLB;
   LONG     lThumb;

   switch ( message )
   {
   case  WM_CHAR:
      break;

   case  WM_SETCURSOR:            // override setting cursor
      break;

   case WM_GETDLGCODE:            // don't let dialog manager have
      return( DLGC_WANTARROWS );  // his way with the arrow keys

   case WM_CREATE:                // sent just after being subclassed
      return( 1L );

   case VLB_RELOAD:               // reload the list box
      LoadVLB( hCtl, wParam, lParam );
      return( 1L );

   case WM_PAINT:                 // this is where it all takes shape
      hVLB = GetProp( hCtl, szVLBPropName );
      lpVLB = (LPVLB) GlobalLock( hVLB );

      PaintVLB( hCtl, lpVLB );

      GlobalUnlock( hVLB );
      break;

   case WM_SETFOCUS:
      HideCaret(hCtl);

      hVLB = GetProp( hCtl, szVLBPropName );
      lpVLB = (LPVLB) GlobalLock( hVLB );

      VLB_CALLBACK( lpVLB->ListBoxId, VCB_SETFOCUS, 0, 0L );
      FrameFocusString( hCtl, lpVLB, TRUE );

      GlobalUnlock( hVLB );
      break;

   case WM_KILLFOCUS:
      hVLB = GetProp( hCtl, szVLBPropName );
      lpVLB = (LPVLB) GlobalLock( hVLB );

      FrameFocusString( hCtl, lpVLB, FALSE );
      InvertSelectedStrings( hCtl, lpVLB, (int)( lpVLB->FocusString -
                                       lpVLB->FirstDisplayString ) );

      VLB_CALLBACK( lpVLB->ListBoxId, VCB_KILLFOCUS, 0, 0L );

      GlobalUnlock( hVLB );
      break;

   case WM_VSCROLL:
      switch( wParam )
      {
      case SB_THUMBPOSITION:
         LoadVLB( hCtl, RELOAD_STRINGPOS, (LONG)LOWORD( lParam ) );
         break;

      case SB_LINEDOWN:
      case SB_LINEUP:
      case SB_PAGEDOWN:
      case SB_PAGEUP:
         ScrollVLB( hCtl, wParam, NULL );
         break;
      }
      break;

   case WM_HSCROLL:
      hVLB = GetProp( hCtl, szVLBPropName );
      lpVLB = (LPVLB) GlobalLock( hVLB );

      lThumb = -1;

      switch( wParam )
      {
      case SB_THUMBPOSITION:
         lThumb = (LONG)LOWORD( lParam );
         lThumb = VLB_CALLBACK( lpVLB->ListBoxId, VCB_HTHUMB,
                                &lThumb, 0L );
         LoadVLB( hCtl, RELOAD_STRINGS, 0L );
         break;

      case SB_LINEDOWN:
         lThumb = VLB_CALLBACK( lpVLB->ListBoxId, VCB_RIGHT, 0, 0L );
         LoadVLB( hCtl, RELOAD_STRINGS, 0L );
         break;

      case SB_LINEUP:
         lThumb = VLB_CALLBACK( lpVLB->ListBoxId, VCB_LEFT, 0, 0L );
         LoadVLB( hCtl, RELOAD_STRINGS, 0L );
         break;

      case SB_PAGEDOWN:
         lThumb = VLB_CALLBACK( lpVLB->ListBoxId,VCB_PAGERIGHT,0,0L);
         LoadVLB( hCtl, RELOAD_STRINGS, 0L );
         break;

      case SB_PAGEUP:
         lThumb = VLB_CALLBACK( lpVLB->ListBoxId, VCB_PAGELEFT,0,0L);
         LoadVLB( hCtl, RELOAD_STRINGS, 0L );
         break;
      }

      if( lThumb >= 0 )
      {
         lThumb = (LONG)min( (int)lpVLB->TotalWidth - 1,
                        max( 0, (int)lThumb ) );
         SetScrollPos( hCtl, SB_HORZ, (int)lThumb, TRUE );
      }

      GlobalUnlock( hVLB );
      break;

   case WM_MOUSEMOVE:
      break;

   case WM_LBUTTONDBLCLK:
         SendMessage( GetParent( hCtl ), WM_COMMAND,
             GetWindowWord( hCtl, GWW_ID ),
             MAKELONG( hCtl, VLBN_DBLCLK ) );
      break;

   case WM_LBUTTONDOWN:
   {
      LONG lFocusString;

      hVLB = GetProp( hCtl, szVLBPropName );
      lpVLB = (LPVLB) GlobalLock( hVLB );

      lpVLB->FocusString = lpVLB->FirstDisplayString +
                     ( HIWORD( lParam ) / lpVLB->CharHeight );

      SetSelectedString( hCtl, NULL, lpVLB );

      InvalidateRect( hCtl, NULL, TRUE );

      GlobalUnlock( hVLB );
      break;
   }

   case WM_KEYDOWN:
   {
      BOOL  CtrlKey = HIBYTE( GetKeyState( VK_CONTROL ) );
      hVLB = GetProp( hCtl, szVLBPropName );
      lpVLB = (LPVLB) GlobalLock( hVLB );

      switch( wParam )
      {
      case ' ':
      {
         SetSelectedString( hCtl, wParam, lpVLB );

         InvalidateRect( hCtl, NULL, TRUE );
         break;
      }

      case VK_DOWN:
         if( lpVLB->FocusString == lpVLB->FirstDisplayString +
                                    lpVLB->DisplayStrings - 1 )
            ScrollVLB( hCtl, SB_LINEDOWN, NULL );

         SetFocusString( wParam, lpVLB );

         if( ! CtrlKey )
            SetSelectedString( hCtl, wParam, lpVLB );

         InvalidateRect( hCtl, NULL, TRUE );
         PaintVLB( hCtl, lpVLB );
         break;

      case VK_UP:
         if( lpVLB->FocusString == lpVLB->FirstDisplayString )
            ScrollVLB( hCtl, SB_LINEUP, NULL );

         SetFocusString( wParam, lpVLB );

         if( ! CtrlKey )
            SetSelectedString( hCtl, wParam, lpVLB );

         InvalidateRect( hCtl, NULL, TRUE );
         PaintVLB( hCtl, lpVLB );
         break;

      case VK_NEXT:
         if( lpVLB->FocusString == lpVLB->FirstDisplayString +
                                    lpVLB->DisplayStrings - 1 )
            ScrollVLB( hCtl, SB_PAGEDOWN, NULL );

         SetFocusString( wParam, lpVLB );

         if( ! CtrlKey )
            SetSelectedString( hCtl, wParam, lpVLB );

         InvalidateRect( hCtl, NULL, TRUE );
         PaintVLB( hCtl, lpVLB );
         break;

      case VK_PRIOR:
         if( lpVLB->FocusString == lpVLB->FirstDisplayString )
            ScrollVLB( hCtl, SB_PAGEUP, NULL );

         SetFocusString( wParam, lpVLB );

         if( ! CtrlKey )
            SetSelectedString( hCtl, wParam, lpVLB );

         InvalidateRect( hCtl, NULL, TRUE );
         PaintVLB( hCtl, lpVLB );
         break;

      case VK_HOME:
         LoadVLB( hCtl, RELOAD_STRINGPOS, 0L );

         SetFocusString( wParam, lpVLB );

         if( ! CtrlKey )
            SetSelectedString( hCtl, wParam, lpVLB );

         InvalidateRect( hCtl, NULL, TRUE );
         break;

      case VK_END:
         LoadVLB( hCtl, RELOAD_STRINGPOS, lpVLB->TotalStrings - 1 );

         SetFocusString( wParam, lpVLB );

         if( ! CtrlKey )
            SetSelectedString( hCtl, wParam, lpVLB );

         InvalidateRect( hCtl, NULL, TRUE );
         break;

      case VK_RIGHT:
      case VK_LEFT:
      {
         int message;

         if( wParam == VK_RIGHT )
         {
            if( CtrlKey )
               message = VCB_PAGERIGHT;
            else
               message = VCB_RIGHT;
         }
         else
         {
            if( CtrlKey )
               message = VCB_PAGELEFT;
            else
               message = VCB_LEFT;
         }

         lThumb = VLB_CALLBACK( lpVLB->ListBoxId, message, 0, 0L );
         lThumb = (LONG)min( (int)lpVLB->TotalWidth - 1,
                        max( 0, (int)lThumb ) );
         SetScrollPos( hCtl, SB_HORZ, (int)lThumb, TRUE );
         LoadVLB( hCtl, RELOAD_STRINGS, 0L );

         InvalidateRect( hCtl, NULL, TRUE );
         break;
      }
      }
      GlobalUnlock( hVLB );
      break;
   }

   case VLB_SETCURSEL:     // WM_USER+6   select specified StringId
   case VLB_SETSEL:        // WM_USER+7   sets selection of StringId
      break;

   case VLB_GETCOUNT:      // WM_USER+1   get total virtual strings
   case VLB_GETSELCOUNT:   // WM_USER+2   get total selected strings
   case VLB_GETSTRLEN:     // WM_USER+3   get total string length
   case VLB_GETSELSTR:     // WM_USER+4   get the selected String
   case VLB_GETSELID:      // WM_USER+5   get selected StringId(s)
  {
      LONG lret = 0;
      hVLB = GetProp( hCtl, szVLBPropName );
      lpVLB = (LPVLB) GlobalLock( hVLB );

      if( message == VLB_GETCOUNT )
         lret = lpVLB->TotalStrings;
      else if( message == VLB_GETSELCOUNT )
         lret = lpVLB->TotalSelectedStrings;
      else if( message == VLB_GETSTRLEN )
         lret = lpVLB->CharWidth;
      else if( message == VLB_GETSELSTR )
      {
         if( lpVLB->TotalSelectedStrings )
         {
            if( lpVLB->MultiSelection )
            {
               LONG FAR *lpSelectedStringIds =
                  (LONG FAR *)GlobalLock( lpVLB->hSelectedStringIds);

               VLB_CALLBACK( lpVLB->ListBoxId, VCB_FULLSTRING,
                       lpSelectedStringIds + wParam, StringBuffer );

               GlobalUnlock( lpVLB->hSelectedStringIds );
            }
            else
               VLB_CALLBACK( lpVLB->ListBoxId, VCB_FULLSTRING,
                       &lpVLB->SelectedStringId, StringBuffer );
         }
         else
            *StringBuffer = 0;

         lret = (LONG)(LPSTR)StringBuffer;
      }
      else if( message == VLB_GETSELID )
      {
         if( lpVLB->MultiSelection )
            lret = lpVLB->hSelectedStringIds;
         else
            lret = lpVLB->SelectedStringId;
      }

      GlobalUnlock( hVLB );
      return( lret );
   }

   case WM_DESTROY:
      hVLB = GetProp( hCtl, szVLBPropName );
      lpVLB = (LPVLB) GlobalLock( hVLB );

      VLB_CALLBACK( lpVLB->ListBoxId, VCB_DESTROY, 0, 0L );

      GlobalFree( lpVLB->hDisplayBuffer );
      GlobalFree( lpVLB->hStringIds );
      if( lpVLB->MultiSelection )
         GlobalFree( lpVLB->hSelectedStringIds );
      GlobalUnlock( hVLB );
      GlobalFree( hVLB );

      RemoveProp( hCtl, szVLBPropName );
      break;

   default:
       return( DefWindowProc( hCtl, message, wParam, lParam ) );
   }

   return( 0L );
}

//*******************************************************************
VOID FrameFocusString( HWND hCtl, LPVLB lpVLB, BOOL draw )
{
   RECT  Rect;
   HDC   hDC;

   // is the focus string visible
   if( lpVLB->FocusString >= lpVLB->FirstDisplayString  &&
       lpVLB->FocusString < lpVLB->FirstDisplayString +
                           lpVLB->DisplayStrings )
   {
      hDC = GetDC( hCtl );

      Rect.left  = lpVLB->ClientRect.left + 1;
      Rect.right = lpVLB->ClientRect.right;
      Rect.top = (WORD) ( (lpVLB->FocusString -
                 lpVLB->FirstDisplayString) * lpVLB->CharHeight) + 1;
      Rect.bottom = Rect.top + lpVLB->CharHeight;

      if( draw )
         FrameRect( hDC, &Rect, GetStockObject( GRAY_BRUSH ) );
      else  // redraw string without frame
      {
         LPSTR lpString = (LPSTR)GlobalLock( lpVLB->hDisplayBuffer );
         WORD nLine = (WORD)
                  ( lpVLB->FocusString - lpVLB->FirstDisplayString );

         // move display buffer pointer to focus string
         lpString += nLine * ( lpVLB->DisplayChars + 1 );

         TextOut ( hDC, 1, ( nLine * lpVLB->CharHeight ) + 1,
                   lpString, lpVLB->DisplayChars );

         GlobalUnlock( lpVLB->hDisplayBuffer );
      }
      ReleaseDC( hCtl, hDC );
   }
}

//*******************************************************************
VOID InvertSelectedStrings( HDC hCtl, LPVLB lpVLB, int StringPos )
{
   int  x;
   LONG  y;
   RECT  Rect;
   int FirstString, LastString;
   LONG FAR *lpStringIds;
   HDC   hDC;

   hDC = GetDC( hCtl );

   lpStringIds = (LONG FAR *)GlobalLock( lpVLB->hStringIds );

   Rect.left  = lpVLB->ClientRect.left + 1;
   Rect.right = lpVLB->ClientRect.right;

   if( StringPos < 0 )      // process all displayed strings
   {
      FirstString = 0;
      LastString = (int)lpVLB->DisplayStrings - 1;
   }
   else
      FirstString = LastString = StringPos;

   if( lpVLB->MultiSelection )
   {
      LONG FAR *lpSelectedStringIds =
               (LONG FAR *)GlobalLock( lpVLB->hSelectedStringIds );

      for( x = FirstString; x <= LastString; x++ )
      {
         for( y = 0; y < lpVLB->TotalSelectedStrings; y++ )
         {
            if( lpStringIds[x] == lpSelectedStringIds[ y ] )
            {
               Rect.top = ( x * lpVLB->CharHeight ) + 1;
               Rect.bottom = Rect.top + lpVLB->CharHeight;
               InvertRect( hDC, &Rect );
               break;
            }
         }
      }
      GlobalUnlock( lpVLB->hSelectedStringIds );
   }
   else  // single selection
   {
      for( x = FirstString; x <= LastString; x++ )
      {
         if( lpStringIds[x] == lpVLB->SelectedStringId )
         {
            Rect.top = ( x * lpVLB->CharHeight ) + 1;
            Rect.bottom = Rect.top + lpVLB->CharHeight;
            InvertRect( hDC, &Rect );
            break;
         }
      }
   }
   ReleaseDC( hCtl, hDC );
   GlobalUnlock( lpVLB->hStringIds );
}

//*******************************************************************
WORD FAR PASCAL Rpad( LPSTR str, WORD length )
{
   LPSTR cp = str;                  // pointer to string
   int x;                           // current string position
   for( x = 0; *cp; x++, cp++ )     // skip to end of string
      ;
   for( ; x < length; x++, *cp++ = ' ' )  // pad string with spaces
      ;
   *cp = 0;                         // NULL terminate string
   return( length );                // return new string length
}

//*******************************************************************
BOOL FAR PASCAL LoadVLB( HWND hCtl, WORD wParam, LONG lParam )
{
   int x;
   HANDLE hVLB = GetProp( hCtl, szVLBPropName );
   LPVLB lpVLB = (LPVLB)GlobalLock( hVLB );
   LPSTR lpString = (LPSTR)GlobalLock( lpVLB->hDisplayBuffer );
   LONG FAR *lpStringIds = (LONG FAR *)GlobalLock(lpVLB->hStringIds);
   LONG l;

   // initialize the DisplayBuffer and StringIds
   lmemset( lpString, 0,
                 lpVLB->DisplayStrings * (lpVLB->DisplayChars + 1) );
   for( x = 0; x < lpVLB->DisplayStrings; x++, lpStringIds[x] = -1L )
      ;

   if( wParam == RELOAD_STRINGS )
      VLB_CALLBACK( lpVLB->ListBoxId, VCB_STRING,
                    &lpStringIds[0], StringBuffer );
   else if( wParam == RELOAD_STRINGPOS )
   {
      lpStringIds[0] = min( lParam,
                       lpVLB->TotalStrings - lpVLB->DisplayStrings );
      lpVLB->FirstDisplayString = lpStringIds[0];
      VLB_CALLBACK( lpVLB->ListBoxId, VCB_VTHUMB,
                    &lpStringIds[0], StringBuffer );
   }
   else if( wParam == RELOAD_STRINGID )
   {
      lpStringIds[0] = lParam;
      VLB_CALLBACK( lpVLB->ListBoxId, VCB_STRING,
                    &lpStringIds[0], StringBuffer );
   }

   // load first DisplayString
   StringBuffer[ lpVLB->DisplayChars ] = 0; // Null @ Display width
   lstrcpy( lpString, StringBuffer );       // advance to next string
   Rpad( lpString, lpVLB->DisplayChars );   // pad with spaces

   for( x = 1; x < lpVLB->DisplayStrings; x++ )
   {
      l = lpStringIds[x - 1];
      if( VLB_CALLBACK( lpVLB->ListBoxId, VCB_NEXT,
                        &l, StringBuffer ) )
      {
         StringBuffer[ lpVLB->DisplayChars ] = 0;  // Null Terminate
         lpString += lpVLB->DisplayChars + 1;      // go to next str
         lstrcpy( lpString, StringBuffer );        // copy buf to str
         Rpad( lpString, lpVLB->DisplayChars );    // pad with spaces
         lpStringIds[x] = l;
      }
      else
         break;
   }

   InvalidateRect( hCtl, NULL, TRUE );      // Force WM_PAINT message

   // set new scroll bar thumb postion
   SetScrollPos( hCtl, SB_VERT, (int)lpVLB->FirstDisplayString,TRUE);

   GlobalUnlock( lpVLB->hDisplayBuffer );
   GlobalUnlock( lpVLB->hStringIds );
   GlobalUnlock( hVLB );
   return( TRUE );
}

//*******************************************************************
BOOL FAR PASCAL ScrollVLB( HWND hCtl, WORD wParam, int Scroll )
{
   int x, scroll, ret = TRUE;
   HANDLE hVLB = GetProp( hCtl, szVLBPropName );
   LPVLB lpVLB = (LPVLB)GlobalLock( hVLB );
   LPSTR lpString = (LPSTR)GlobalLock( lpVLB->hDisplayBuffer );
   LONG FAR *lpStringIds = (LONG FAR *)GlobalLock(lpVLB->hStringIds);
   LONG l;
   LPSTR lpGoodStrings;
   int nGoodStrings;

   // Check if scroll request is possible
   if( wParam == SB_LINEDOWN )
   {  // at the end of the total strings
      if( lpVLB->FirstDisplayString + lpVLB->DisplayStrings >
                                            lpVLB->TotalStrings - 1 )
         ret = FALSE;
      else
         scroll = 1;
      lpVLB->ScrollWindow = 1;
   }
   else if( wParam == SB_LINEUP )
   {
      if( lpVLB->FirstDisplayString == 0 )
         ret = FALSE;
      else
         scroll = 1;
      lpVLB->ScrollWindow = -1;
   }
   else if( wParam == SB_PAGEDOWN )
   {
      if( lpVLB->FirstDisplayString + lpVLB->DisplayStrings >
                                            lpVLB->TotalStrings - 1 )
         ret = FALSE;
      else
         scroll = min( (int)lpVLB->DisplayStrings - 1,
            (int)(lpVLB->TotalStrings - ( lpVLB->FirstDisplayString +
                                         lpVLB->DisplayStrings ) ) );
      lpVLB->ScrollWindow = scroll;
   }
   else if( wParam == SB_PAGEUP )
   {
      if( lpVLB->FirstDisplayString == 0 )
         ret = FALSE;
      else
         scroll = min( (int)lpVLB->DisplayStrings - 1,
                       (int)lpVLB->FirstDisplayString );
      lpVLB->ScrollWindow = scroll * -1;
   }
   else         // scroll number of the Scroll 3rd paramater
   {
      lpVLB->ScrollWindow = Scroll;
      if( Scroll < 0 )
      {
         Scroll = abs( Scroll );         // absolute scroll amount
         wParam = SB_LINEUP;             // scroll up
      }
      else if( Scroll > 0 )
         wParam = SB_LINEDOWN;           // scroll down
      else //  if( Scroll == 0 )         // no scroll amt specified
         ret = FALSE;

      if( Scroll >= lpVLB->DisplayStrings )  // scroll less than #
         ret = FALSE;                        // of displayed strings
      else
         scroll = Scroll;
   }

   if( ret == FALSE )
   {
      lpVLB->ScrollWindow = FALSE;
      GlobalUnlock( lpVLB->hDisplayBuffer );
      GlobalUnlock( lpVLB->hStringIds );
      GlobalUnlock( hVLB );
      return( FALSE );
   }

   // pointer to strings that will still be displayed
   lpGoodStrings = lpString + ( (lpVLB->DisplayChars + 1) * scroll );
   // number of strings that will still be displayed
   nGoodStrings = ( lpVLB->DisplayStrings - scroll );

   // adjust strings that will still be displayed and get new strings
   if( wParam == SB_LINEUP  ||  wParam == SB_PAGEUP )
   {
      lpVLB->FirstDisplayString -= scroll;

      // push good strings & StringIds down in their buffers
      lmemmove( lpGoodStrings, lpString,
                          nGoodStrings * (lpVLB->DisplayChars + 1) );
      lmemmove( lpStringIds + scroll, lpStringIds,
                                     nGoodStrings * sizeof( LONG ) );

      // move Display string pointer to last string to get
      lpString += ( scroll - 1 ) * (lpVLB->DisplayChars + 1);

      // get the previous scroll number of strings
      for( x = 0; x < scroll; x++ )
      {
         // get the StringId of the last string read
         l = lpStringIds[ scroll - x  ];

         if( VLB_CALLBACK( lpVLB->ListBoxId, VCB_PREV,
                           &l, StringBuffer ) )
         {
            StringBuffer[ lpVLB->DisplayChars ] = 0;// Null Terminate
            lstrcpy( lpString, StringBuffer );     // copy buf to str
            Rpad( lpString, lpVLB->DisplayChars ); // pad with spaces
            lpString -= lpVLB->DisplayChars + 1;   // go to next str
            lpStringIds[ ( scroll - x ) - 1 ] = l; // save StringId
         }
         else
            break;
      }
   }
   else     // if( wParam == SB_LINEDOWN  ||  wParam == SB_PAGEDOWN )
   {
      lpVLB->FirstDisplayString += scroll;

      // move good strings & StringIds to the beginning of their bufs
      lmemmove( lpString, lpGoodStrings,
                          nGoodStrings * (lpVLB->DisplayChars + 1) );
      lmemmove( lpStringIds, lpStringIds + scroll,
                                     nGoodStrings * sizeof( LONG ) );

      // move Display string pointer to first string to get
      lpString += nGoodStrings * (lpVLB->DisplayChars + 1);

      // get the next scroll number of strings
      for( x = 0; x < scroll; x++ )
      {
         // get the StringId of the last string read
         l = lpStringIds[ ( nGoodStrings + x ) - 1 ];

         if( VLB_CALLBACK( lpVLB->ListBoxId, VCB_NEXT,
                           &l, StringBuffer ) )
         {
            StringBuffer[ lpVLB->DisplayChars ] = 0;// Null Terminate
            lstrcpy( lpString, StringBuffer );     // copy buf to str
            Rpad( lpString, lpVLB->DisplayChars ); // pad with spaces
            lpString += lpVLB->DisplayChars + 1;   // go to next str
            lpStringIds[ nGoodStrings + x ] = l;   // save StringId
         }
         else
            break;
      }
   }

   InvalidateRect( hCtl, NULL, TRUE );      // Force WM_PAINT message

   // set new scroll bar thumb postion
   SetScrollPos( hCtl, SB_VERT, (int)lpVLB->FirstDisplayString,TRUE);

   GlobalUnlock( lpVLB->hDisplayBuffer );
   GlobalUnlock( lpVLB->hStringIds );
   GlobalUnlock( hVLB );
   return( TRUE );
}

//*******************************************************************
VOID FAR PASCAL SetSelectedString( HWND hCtl, WORD wParam,
                                   LPVLB lpVLB )
{
   LONG  FAR *lpStringIds;
   LONG  FAR *lpSelectedStringId;

   lpStringIds = (LONG FAR *)GlobalLock( lpVLB->hStringIds );

   if( lpVLB->MultiSelection )
   {
      LONG  FAR *lpSelectedStringIds;

      if( ! HIBYTE( GetKeyState( VK_SHIFT ) ) )
         lpVLB->TotalSelectedStrings = 0;
      else
      {
         if(lpVLB->TotalSelectedStrings == lpVLB->MaxSelectedStrings)
         {
            lpVLB->MaxSelectedStrings += lpVLB->DisplayStrings;
            if( ! ( lpVLB->hSelectedStringIds =
               GlobalReAlloc( lpVLB->hSelectedStringIds,
               lpVLB->MaxSelectedStrings * sizeof( LONG ), GHND ) ) )
            {
               lpVLB->MaxSelectedStrings -= lpVLB->DisplayStrings;
               SendMessage( GetParent( hCtl ), WM_COMMAND,
                 lpVLB->ListBoxId, MAKELONG( hCtl, VLBN_ERRSPACE ) );
               return;
            }
         }
      }
      lpSelectedStringIds = (LONG FAR *)
                             GlobalLock( lpVLB->hSelectedStringIds );
      lpSelectedStringId =
                 lpSelectedStringIds + lpVLB->TotalSelectedStrings++;
   }
   else
   {
      lpVLB->TotalSelectedStrings = 1;
      lpSelectedStringId = &lpVLB->SelectedStringId;
   }

   switch( wParam )
   {
   case NULL:              // mouse left button click
   case ' ':               // space bar select focus string
   case VK_DOWN:
   case VK_UP:
      *lpSelectedStringId =
         lpStringIds[lpVLB->FocusString - lpVLB->FirstDisplayString];
      break;

   case VK_NEXT:
   case VK_END:
      *lpSelectedStringId = lpStringIds[lpVLB->DisplayStrings-1];
      break;

   case VK_PRIOR:
   case VK_HOME:
      *lpSelectedStringId = lpStringIds[0];
      break;
   }

      SendMessage( GetParent( hCtl ), WM_COMMAND, lpVLB->ListBoxId,
                                  MAKELONG( hCtl, VLBN_SELCHANGE ) );

   if( lpVLB->MultiSelection )
      GlobalUnlock( lpVLB->hSelectedStringIds );
   GlobalUnlock( lpVLB->hStringIds );
}

//*******************************************************************
VOID FAR PASCAL SetFocusString( WORD wParam, LPVLB lpVLB )
{
   switch( wParam )
   {
   case VK_DOWN:
   case VK_UP:
      if( lpVLB->FocusString < lpVLB->FirstDisplayString )
         lpVLB->FocusString = lpVLB->FirstDisplayString;
      else if( lpVLB->FocusString >
               lpVLB->FirstDisplayString +lpVLB->DisplayStrings - 1 )
         lpVLB->FocusString =
               lpVLB->FirstDisplayString + lpVLB->DisplayStrings - 1;
      else
      {
         if( wParam == VK_DOWN )
         {
            if( lpVLB->FocusString < lpVLB->TotalStrings - 1 )
              lpVLB->FocusString++;
         }
         else
            if( lpVLB->FocusString )
               lpVLB->FocusString--;
      }
      break;

   case VK_NEXT:
      lpVLB->FocusString =
               lpVLB->FirstDisplayString + lpVLB->DisplayStrings - 1;
      break;

   case VK_PRIOR:
      lpVLB->FocusString = lpVLB->FirstDisplayString;
      break;

   case VK_HOME:
      lpVLB->FocusString = 0;
      break;

   case VK_END:
      lpVLB->FocusString = lpVLB->TotalStrings - 1;
      break;
   }
}

//*******************************************************************
VOID FAR PASCAL PaintVLB( HWND hCtl, LPVLB lpVLB )
{
   int         x, y, first, last;
   HDC         hDC;
   PAINTSTRUCT ps;
   LPSTR       lpString;
   RECT        Rect;
   LONG  FAR * lpStringIds;

   // let's get the display string buffer and each string's Id
   lpString = (LPSTR)GlobalLock( lpVLB->hDisplayBuffer );
   lpStringIds = (LONG FAR *)GlobalLock( lpVLB->hStringIds );

   hDC = BeginPaint( hCtl, &ps );

   GetVLBColors( hCtl, hDC );

   first = 0;
   last = lpVLB->DisplayStrings - 1;

   for( x = first, y = 1; x <= last; x++, y += lpVLB->CharHeight )
   {
      if( lpStringIds[x] >= 0 )
      {
         TextOut ( hDC, 1, y, lpString, lpVLB->DisplayChars );
         lpString += lpVLB->DisplayChars + 1;
      }
      else
      {
         lmemset( lpString, ' ', lpVLB->DisplayChars );
         TextOut ( hDC, 1, y, lpString, lpVLB->DisplayChars );
      }
   }

   EndPaint( hCtl, &ps );

   if( lpVLB->TotalSelectedStrings )
      InvertSelectedStrings( hCtl, lpVLB, -1 );

   if( hCtl == GetFocus() )
      FrameFocusString( hCtl, lpVLB, TRUE );

   lpVLB->ScrollWindow = FALSE;
   GlobalUnlock( lpVLB->hDisplayBuffer );
   GlobalUnlock( lpVLB->hStringIds );
}

//*******************************************************************
VOID FAR PASCAL GetVLBColors( HWND hCtl, HANDLE hDC )
{
   HANDLE hBrush;

   SetBkColor( hDC, GetSysColor( COLOR_WINDOW ) );
   SetTextColor( hDC, GetSysColor( COLOR_WINDOWTEXT ) );

   if( hBrush = (HANDLE)SendMessage( GetParent( hCtl ), WM_CTLCOLOR,
                          hDC, MAKELONG( hCtl, CTLCOLOR_LISTBOX ) ) )
      SelectObject( hDC, hBrush );
}

//*** END OF VLB.C **************************************************
