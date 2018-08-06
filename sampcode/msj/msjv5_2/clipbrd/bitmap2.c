/*
 * WINDOWS CLIPBOARD VIEWER - BITMAP FORMAT SOURCE CODE
 *
 * LANGUAGE      : Microsoft C5.1
 * MODEL         : small
 * ENVIRONMENT   : Microsoft Windows 2.1 SDK
 * STATUS        : operational
 *
 */

#define  NOCOMM

#include <windows.h>

#define  HORZ_BORDER       2
#define  VERT_BORDER       2

#define  HORZ_STEPSIZE     8
#define  VERT_STEPSIZE     8

typedef struct {
 HANDLE  hBmp;
 HANDLE  hData;
 POINT   ptOrg;
 RECT    rcWnd;
 RECT    rcBmp;
 HDC     hScrDC;
 HDC     hMemDC;
} DISPINFO;

typedef DISPINFO FAR *   LPDISPINFO;

BOOL PASCAL          BitmapInit( HANDLE, WORD, WORD, LPSTR );

HANDLE FAR PASCAL    BitmapCreate( HWND, HANDLE );
HANDLE FAR PASCAL    BitmapSize( HWND, HANDLE, WORD, LONG );
HANDLE FAR PASCAL    BitmapHScroll( HWND, HANDLE, WORD, LONG );
HANDLE FAR PASCAL    BitmapVScroll( HWND, HANDLE, WORD, LONG );
HANDLE FAR PASCAL    BitmapPaint( HWND, HANDLE, WORD, LONG );
HANDLE FAR PASCAL    BitmapDestroy( HWND, HANDLE );



/* BOOL
 * BitmapInit( hInstance, wDataSegment, wHeapSize, lpszCmdLine )
 *
 *    hInstance      library instance handle
 *    wDataSegment   library data segment
 *    wHeapSize      default heap size
 *    lpszCmdLine    command line arguments
 *
 * This function performs all the initialization necessary to use
 * the bitmap viewer display dynamic library.  It is assumed that no
 * local heap is used, hence no call to LocalInit.  A non-zero value
 * is returned if the initialization was sucessful.
 *
 */

BOOL PASCAL BitmapInit(hInstance,wDataSegment,wHeapSize,lpszCmdLine)
   HANDLE      hInstance;
   WORD        wDataSegment;
   WORD        wHeapSize;
   LPSTR       lpszCmdLine;
{

 /* warning level 3 compatibility */
 hInstance;
 wDataSegment;
 wHeapSize;
 lpszCmdLine;

 /* sucessful return */
   return( TRUE );

}



/*
 * BitmapCreate( hWnd, hClipData ) : HANDLE;
 *
 *    hWnd           handle to display window
 *    hClipData      handle to current clipboard data
 *
 * This function performs all the initialization necessary in order
 * to view a bitmap clipboard format.  A handle to a display infor-
 * mation data block (the internal format of which is only known
 * inside this module) which the owner is responsible for saving.
 *
 */

HANDLE FAR PASCAL BitmapCreate(
 HWND        hWnd,
 HANDLE      hClipData )
{
 BITMAP      Bitmap;
 HANDLE      hDispInfo;
 WORD        wWndWidth;
 WORD        wWndHeight;
 WORD        wScrollWidth;
 WORD        wScrollHeight;
 LPDISPINFO  lpDispInfo;

 /* reset scroll bars */
 SetScrollPos( hWnd, SB_HORZ, 0, TRUE );
 SetScrollPos( hWnd, SB_VERT, 0, TRUE );

 /* attempt to allocate data */
 hDispInfo = GlobalAlloc( GHND, (DWORD)sizeof(DISPINFO) );
 if ( hDispInfo ) {

   /* lock it down */
   lpDispInfo = (LPDISPINFO)GlobalLock( hDispInfo );
   if ( lpDispInfo ) {

     /* define bitmap data */
     lpDispInfo->hData = hClipData;

     /* define bitmap dimensions */
     GetObject( hClipData, sizeof(BITMAP), (LPSTR)&Bitmap );

     lpDispInfo->rcBmp.top = 0;
     lpDispInfo->rcBmp.left = 0;
     lpDispInfo->rcBmp.right  = Bitmap.bmWidth;
     lpDispInfo->rcBmp.bottom = Bitmap.bmHeight;

     /* define window origin in bitmap coordinates */
     lpDispInfo->ptOrg.x = -HORZ_BORDER;
     lpDispInfo->ptOrg.y = -VERT_BORDER;

     /* define window dimensions */
     GetClientRect( hWnd, &lpDispInfo->rcWnd );

     lpDispInfo->rcWnd.top = VERT_BORDER;
     lpDispInfo->rcWnd.left = HORZ_BORDER;
     lpDispInfo->rcWnd.right -= HORZ_BORDER;
     lpDispInfo->rcWnd.bottom -= VERT_BORDER;

     OffsetRect( &lpDispInfo->rcWnd, -HORZ_BORDER, -VERT_BORDER );
     IntersectRect( &lpDispInfo->rcWnd, &lpDispInfo->rcWnd,
                    &lpDispInfo->rcBmp );
     OffsetRect( &lpDispInfo->rcWnd, HORZ_BORDER, VERT_BORDER );

     wWndWidth =  lpDispInfo->rcWnd.right -  lpDispInfo->rcWnd.left;
     wWndHeight = lpDispInfo->rcWnd.bottom - lpDispInfo->rcWnd.top;

     /* define scrollbar ranges */
     wScrollWidth =  ( Bitmap.bmWidth  > wWndWidth ) ?
                       Bitmap.bmWidth  - wWndWidth : 1;
     wScrollHeight = ( Bitmap.bmHeight > wWndHeight ) ?
                       Bitmap.bmHeight - wWndHeight : 1;

     SetScrollRange( hWnd, SB_HORZ, 0, wScrollWidth, FALSE );
     SetScrollRange( hWnd, SB_VERT, 0, wScrollHeight, FALSE );

     /* unlock data */
     GlobalUnlock( hDispInfo );

   } else {
     GlobalFree( hDispInfo );
     hDispInfo = NULL;
   }

 }

 /* return display info handle */
 return( hDispInfo );

}

/*
 * BitmapSize( hWnd, hDispInfo, wParam, lParam ) : HANDLE;
 *
 *   hWnd        handle to display window
 *   hDispInfo   handle to display information block
 *   wParam      word parameter of WM_SIZE message
 *   lParam      long parameter of WM_SIZE message
 *
 * This function resets the display information data block whenever
 * the size of the display region is changed.  If successful, a
 * handle to the new display information data block is returned.
 * Failure to call this function whenever the size of the display
 * region is changed will cause unusual display results.
 *
 */

HANDLE FAR PASCAL BitmapSize(
 HWND      hWnd,
 HANDLE    hDispInfo,
 WORD      wParam,
 LONG      lParam )
{
 WORD        wWndWidth;
 WORD        wBmpWidth;
 WORD        wWndHeight;
 WORD        wBmpHeight;
 WORD        wScrollWidth;
 WORD        wScrollHeight;
 LPDISPINFO  lpDispInfo;

 /* warning level 3 compatibility */
 wParam;
 lParam;

 /* reset scroll bars */
 SetScrollPos( hWnd, SB_HORZ, 0, TRUE );
 SetScrollPos( hWnd, SB_VERT, 0, TRUE );

 /* lock it down display information block */
 lpDispInfo = (LPDISPINFO)GlobalLock( hDispInfo );
 if ( lpDispInfo ) {

   /* define window origin in bitmap coordinates */
   lpDispInfo->ptOrg.x = -HORZ_BORDER;
   lpDispInfo->ptOrg.y = -VERT_BORDER;

   /* define bitmap dimensions */
   wBmpWidth = lpDispInfo->rcBmp.right - lpDispInfo->rcBmp.left;
   wBmpHeight = lpDispInfo->rcBmp.bottom - lpDispInfo->rcBmp.top;

   /* define window dimensions */
   GetClientRect( hWnd, &lpDispInfo->rcWnd );

   lpDispInfo->rcWnd.top = VERT_BORDER;
   lpDispInfo->rcWnd.left = HORZ_BORDER;
   lpDispInfo->rcWnd.right -= HORZ_BORDER;
   lpDispInfo->rcWnd.bottom -= VERT_BORDER;

   OffsetRect( &lpDispInfo->rcWnd, -HORZ_BORDER, -VERT_BORDER );
   IntersectRect( &lpDispInfo->rcWnd, &lpDispInfo->rcWnd,
                  &lpDispInfo->rcBmp );
   OffsetRect( &lpDispInfo->rcWnd, HORZ_BORDER, VERT_BORDER );

   wWndWidth = lpDispInfo->rcWnd.right - lpDispInfo->rcWnd.left;
   wWndHeight = lpDispInfo->rcWnd.bottom - lpDispInfo->rcWnd.top;

   /* define scrollbar ranges */
   wScrollWidth=(wBmpWidth>wWndWidth) ? wBmpWidth-wWndWidth : 1;
   wScrollHeight=(wBmpHeight>wWndHeight) ? wBmpHeight-wWndHeight : 1;

   SetScrollRange( hWnd, SB_HORZ, 0, wScrollWidth, FALSE );
   SetScrollRange( hWnd, SB_VERT, 0, wScrollHeight, FALSE );

   /* unlock data */
   GlobalUnlock( hDispInfo );

 } else
   hDispInfo = NULL;

 /* return final result */
 return( hDispInfo );

}

/*
 * BitmapHScroll( hWnd, hDispInfo, wParam, lParam ) : HANDLE;
 *
 *   hWnd        handle to display window
 *   hDispInfo   handle to display information block
 *   wParam      current scroll code
 *   lParam      current scroll parameter
 *
 * This function is responsible for handling all the horizontal
 * scroll messages received when viewing a bitmap clipboard format.
 * If necessary, changes to the display information block can be
 * made.  As currently implemented, no action is taken.  The value
 * returned is the handle to the updated display information block.
 */

HANDLE FAR PASCAL BitmapHScroll(
 HWND      hWnd,
 HANDLE    hDispInfo,
 WORD      wParam,
 LONG      lParam )
{
 WORD        wWndWidth;
 WORD        wBmpWidth;
 WORD        wOldScrollPos;
 WORD        wNewScrollPos;
 WORD        wOldScrollRange;
 LPDISPINFO  lpDispInfo;

 /* access display information block */
 lpDispInfo = (LPDISPINFO)GlobalLock( hDispInfo );
 if ( lpDispInfo ) {

   /* initialization */
   wWndWidth = lpDispInfo->rcWnd.right - lpDispInfo->rcWnd.left;
   wBmpWidth = lpDispInfo->rcBmp.right - lpDispInfo->rcBmp.left;

   wOldScrollPos = lpDispInfo->ptOrg.x + HORZ_BORDER;
   wOldScrollRange=(wBmpWidth > wWndWidth) ? wBmpWidth-wWndWidth : 0;

   /* define display contexts (if necessary) */
   if ( lpDispInfo->hScrDC == NULL ) {
     lpDispInfo->hScrDC = GetDC( hWnd );
     lpDispInfo->hMemDC = CreateCompatibleDC( lpDispInfo->hScrDC );
     lpDispInfo->hBmp = SelectObject( lpDispInfo->hMemDC,
                                      lpDispInfo->hData );
   }

   /* process scroll message */
   switch( wParam )
     {
   case SB_LINEUP : /* move left one line */
     wNewScrollPos = (wOldScrollPos > HORZ_STEPSIZE) ?
                      wOldScrollPos-HORZ_STEPSIZE : 0;
     break;
   case SB_LINEDOWN : /* move right one line */
     wNewScrollPos = (wOldScrollPos+HORZ_STEPSIZE<=wOldScrollRange) ?
                      wOldScrollPos+HORZ_STEPSIZE : wOldScrollRange;
     break;
   case SB_PAGEUP :  /* move left one page */
     wNewScrollPos = (wOldScrollPos > wWndWidth) ?
                      wOldScrollPos-wWndWidth : 0;
     break;
   case SB_PAGEDOWN : /* move right one page */
     wNewScrollPos = (wOldScrollPos+wWndWidth <= wOldScrollRange) ?
                      wOldScrollPos+wWndWidth : wOldScrollRange;
     break;
   case SB_THUMBPOSITION : /* move to an absolute position */
   case SB_THUMBTRACK : /* track the current thumb position */
     wNewScrollPos = (wOldScrollRange > 1) ? LOWORD(lParam) : 0;
     break;
   case SB_TOP : /* move to the first line */
     wNewScrollPos = 0;
     break;
   case SB_BOTTOM : /* move to the last line */
     wNewScrollPos = wOldScrollRange;
     break;
   case SB_ENDSCROLL : /* end scrolling */
     wNewScrollPos = wOldScrollPos;
     break;
   }

   /* perform scroll and update (if necessary) */
   if ( wNewScrollPos != wOldScrollPos ) {

     /* update window */
     BitBlt(
            lpDispInfo->hScrDC,
            HORZ_BORDER,
            VERT_BORDER,
            wWndWidth,
            lpDispInfo->rcWnd.bottom - lpDispInfo->rcWnd.top,
            lpDispInfo->hMemDC,
            wNewScrollPos,
            lpDispInfo->ptOrg.y + VERT_BORDER,
            SRCCOPY
         );

     /* update origin & horizontal scrollbar */
     lpDispInfo->ptOrg.x = wNewScrollPos - HORZ_BORDER;
     SetScrollPos( hWnd, SB_HORZ, wNewScrollPos, TRUE );

   }

   /* release display context (if necessary) */
   if ( (wParam == SB_ENDSCROLL)||(wParam == SB_THUMBPOSITION) ) {
     SelectObject( lpDispInfo->hMemDC, lpDispInfo->hBmp );
     DeleteDC( lpDispInfo->hMemDC );
     ReleaseDC( hWnd, lpDispInfo->hScrDC );
     lpDispInfo->hScrDC = NULL;
   }

   /* unlock data */
   GlobalUnlock( hDispInfo );

 } else
   hDispInfo = NULL;

 /* return result */
 return( hDispInfo );

}



/*
 * BitmapVScroll( hWnd, hDispInfo, wParam, lParam ) : HANDLE;
 *
 *    hWnd           handle to display window
 *    hDispInfo      handle to display information block
 *    wParam       current scroll code
 *   lParam      current scroll parameter
 *
 * This function is responsible for handling all the vertical scroll
 * messages received when viewing a bitmap clipboard format.  If
 * necessary, changes to the display information block can be made.
 * As currently implemented, no action is taken.  The value returned
 * is the handle to the updated display information block.
 */

HANDLE FAR PASCAL BitmapVScroll(
 HWND        hWnd,
 HANDLE      hDispInfo,
 WORD        wParam,
 LONG        lParam )
{
 WORD        wWndHeight;
 WORD        wBmpHeight;
 WORD        wOldScrollPos;
 WORD        wNewScrollPos;
 WORD        wOldScrollRange;
 LPDISPINFO  lpDispInfo;

 /* access display information block */
 lpDispInfo = (LPDISPINFO)GlobalLock( hDispInfo );
 if ( lpDispInfo ) {

   /* initialization */
   wWndHeight = lpDispInfo->rcWnd.bottom - lpDispInfo->rcWnd.top;
   wBmpHeight = lpDispInfo->rcBmp.bottom - lpDispInfo->rcBmp.top;

   wOldScrollPos = lpDispInfo->ptOrg.y + VERT_BORDER;
   wOldScrollRange = (wBmpHeight > wWndHeight) ?
                      wBmpHeight-wWndHeight : 0;

   /* define display contexts (if necessary) */
   if ( lpDispInfo->hScrDC == NULL ) {
     lpDispInfo->hScrDC = GetDC( hWnd );
     lpDispInfo->hMemDC = CreateCompatibleDC( lpDispInfo->hScrDC );
     lpDispInfo->hBmp = SelectObject( lpDispInfo->hMemDC,
                                      lpDispInfo->hData );
   }

   /* process scroll message */
   switch( wParam )
     {
   case SB_LINEUP : /* move up one line */
     wNewScrollPos = (wOldScrollPos > VERT_STEPSIZE) ?
                      wOldScrollPos-VERT_STEPSIZE : 0;
     break;
   case SB_LINEDOWN : /* move down one line */
     wNewScrollPos=(wOldScrollPos+VERT_STEPSIZE<=wOldScrollRange) ?
                    wOldScrollPos+VERT_STEPSIZE : wOldScrollRange;
     break;
   case SB_PAGEUP :  /* move up one page */
     wNewScrollPos=(wOldScrollPos > wWndHeight) ?
                    wOldScrollPos-wWndHeight : 0;
     break;
   case SB_PAGEDOWN : /* move down one page */
     wNewScrollPos=(wOldScrollPos+wWndHeight<=wOldScrollRange) ?
                    wOldScrollPos+wWndHeight : wOldScrollRange;
     break;
   case SB_THUMBPOSITION : /* move to an absolute position */
   case SB_THUMBTRACK : /* track the current thumb position */
     wNewScrollPos = (wOldScrollRange > 1) ? LOWORD(lParam) : 0;
     break;
   case SB_TOP : /* move to the first line */
     wNewScrollPos = 0;
     break;
   case SB_BOTTOM : /* move to the last line */
     wNewScrollPos = wOldScrollRange;
     break;
   case SB_ENDSCROLL : /* end scrolling */
     wNewScrollPos = wOldScrollPos;
     break;
   }

   /* perform scroll and update (if necessary) */
   if ( wNewScrollPos != wOldScrollPos ) {

     /* update window */
     BitBlt(
            lpDispInfo->hScrDC,
            HORZ_BORDER,
            VERT_BORDER,
            lpDispInfo->rcWnd.right - lpDispInfo->rcWnd.left,
            wWndHeight,
            lpDispInfo->hMemDC,
            lpDispInfo->ptOrg.x + HORZ_BORDER,
            wNewScrollPos,
            SRCCOPY
         );

     /* update origin & horizontal scrollbar */
     lpDispInfo->ptOrg.y = wNewScrollPos - VERT_BORDER;
     SetScrollPos( hWnd, SB_VERT, wNewScrollPos, TRUE );

   }

   /* release display context (if necessary) */
   if ( (wParam == SB_ENDSCROLL)||(wParam == SB_THUMBPOSITION) ) {
     SelectObject( lpDispInfo->hMemDC, lpDispInfo->hBmp );
     DeleteDC( lpDispInfo->hMemDC );
     ReleaseDC( hWnd, lpDispInfo->hScrDC );
     lpDispInfo->hScrDC = NULL;
   }

   /* unlock data */
   GlobalUnlock( hDispInfo );

 } else
   hDispInfo = NULL;

 /* return result */
 return( hDispInfo );

}



/*
 * BitmapPaint( hWnd, hDispInfo, wParam, lParam ) : HANDLE;
 *
 *   hWnd        handle to display window
 *   hDispInfo   handle to display information block
 *   wParam      WM_PAINT word parameter
 *   lParam      WM_PAINT long parameter
 *
 * This function is responsible for handling all the paint related
 * aspects for the bitmap clipboard format.  This function calculates
 * the required update portion of the window and BitBlts the bitmap
 * into the region.  The handle returned by this function is to the
 * update display information block.
 *
 */

HANDLE FAR PASCAL BitmapPaint(
 HWND        hWnd,
 HANDLE      hDispInfo,
 WORD        wParam,
 LONG        lParam )
{
 PAINTSTRUCT Ps;
 HDC         hMemDC;
 HANDLE      hOldData;
 LPDISPINFO  lpDispInfo;

 /* warning level 3 compatibility */
 wParam;
 lParam;

 /* access display information block */
 lpDispInfo = (LPDISPINFO)GlobalLock( hDispInfo );
 if ( lpDispInfo ) {

   /* start paint operation */
   BeginPaint( hWnd, &Ps );

   /* define update region */
IntersectRect(&Ps.rcPaint,&Ps.rcPaint,&lpDispInfo->rcWnd );
   OffsetRect(&Ps.rcPaint,lpDispInfo->ptOrg.x,lpDispInfo->ptOrg.y );
IntersectRect(&Ps.rcPaint,&Ps.rcPaint,&lpDispInfo->rcBmp );
   OffsetRect(&Ps.rcPaint,-lpDispInfo->ptOrg.x,-lpDispInfo->ptOrg.y);

   /* perform BitBlt operation */
   hMemDC = CreateCompatibleDC( Ps.hdc );
   if ( hMemDC ) {
     hOldData = SelectObject( hMemDC, lpDispInfo->hData );
     BitBlt(
            Ps.hdc,
            Ps.rcPaint.left,
            Ps.rcPaint.top,
            Ps.rcPaint.right - Ps.rcPaint.left,
            Ps.rcPaint.bottom - Ps.rcPaint.top,
            hMemDC,
            Ps.rcPaint.left + lpDispInfo->ptOrg.x,
            Ps.rcPaint.top + lpDispInfo->ptOrg.y,
            SRCCOPY
           );
     SelectObject( hMemDC, hOldData );
     DeleteDC( hMemDC );
   }

   /* unlock data & end paint operation */
   GlobalUnlock( hDispInfo );
   EndPaint( hWnd, &Ps );

 } else
   hDispInfo = NULL;

 /* return final result */
 return( hDispInfo );

}

/*
 * BitmapDestroy( hWnd, hDispInfo ) : HANDLE;
 *
 *   hWnd        handle to display window
 *   hDispInfo   handle to display information block
 *
 * This function is to be called whenever the display region is being
 * destroyed.  It is responsible for restoring the system to it's
 * original state and for releasing any memory or resources defined.
 * The value returned is the handle to the OLD display information
 * block.  This handle should NEVER be used after this function is
 * called.  If an error occurs a value of NULL is returned.
 *
 */

HANDLE FAR PASCAL BitmapDestroy(
 HWND      hWnd,
 HANDLE    hDispInfo )
{

 /* warning level 3 compatibility */
 hWnd;

 /* free allocated memory block & return old handle */
 GlobalFree( hDispInfo );
 return( hDispInfo );

}
