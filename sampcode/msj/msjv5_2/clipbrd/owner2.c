/*
 * WINDOWS CLIPBOARD VIEWER - OWNER DISPLAY FORMAT SOURCE CODE
 *
 * LANGUAGE      : Microsoft C5.1
 * MODEL         : small
 * ENVIRONMENT   : Microsoft Windows 2.1 SDK
 * STATUS        : operational
 *
 * Kevin P. Welch
 */

#define  NOCOMM
#include <windows.h>

BOOL PASCAL        OwnerInit( HANDLE, WORD, WORD, LPSTR );

HANDLE FAR PASCAL    OwnerCreate( HWND, HANDLE );
HANDLE FAR PASCAL    OwnerSize( HWND, HANDLE, WORD, LONG );
HANDLE FAR PASCAL    OwnerHScroll( HWND, HANDLE, WORD, LONG );
HANDLE FAR PASCAL    OwnerVScroll( HWND, HANDLE, WORD, LONG );
HANDLE FAR PASCAL    OwnerPaint( HWND, HANDLE, WORD, LONG );
HANDLE FAR PASCAL    OwnerDestroy( HWND, HANDLE );

/* BOOL
 * OwnerInit( hInstance, wDataSegment, wHeapSize, lpszCmdLine )
 *
 *    hInstance      library instance handle
 *    wDataSegment   library data segment
 *    wHeapSize      default heap size
 *    lpszCmdLine    command line arguments
 *
 * This function performs all the initialization necessary to use
 * the owner display dynamic library.  It is assumed that no local
 * heap is used, hence no call to LocalInit.  A non-zero value is
 * returned if the initialization was sucessful.
 *
 */

BOOL PASCAL OwnerInit( hInstance,wDataSegment,wHeapSize,lpszCmdLine )
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
 * OwnerCreate( hWnd, hClipData ) : HANDLE;
 *
 *    hWnd           handle to display window
 *    hClipData      handle to current clipboard data
 *
 * This function performs all the initialization necessary in order
 * to view an owner display clipboard format.  A handle to a display
 * information data block (the internal format of which is only known
 * inside this module) is returned which the owner is responsible
 * for saving.  In the owner display case the handle to the current
 * clipboard data is sufficient information to maintain the display.
 */

HANDLE FAR PASCAL OwnerCreate(
 HWND      hWnd,
 HANDLE    hClipData )
{
 HANDLE    hRect;
 LPRECT    lpRect;
 HWND      hWndOwner;

 /* reset scroll bars */
 SetScrollPos( hWnd, SB_HORZ, 0, TRUE );
 SetScrollPos( hWnd, SB_VERT, 0, TRUE );

 /* notify clipboard owner of change */
 hWndOwner = GetClipboardOwner();
 if ( hWndOwner ) {

   /* allocate space for update rectangle */
   hRect = GlobalAlloc( GHND, (DWORD)sizeof(RECT) );
   if ( hRect ) {

     /* lock update rectangle */
     lpRect = (LPRECT)GlobalLock( hRect );
     if ( lpRect ) {

       /* define the update rectangle & notify clipboard owner */
       GetClientRect( hWnd, lpRect );
       SendMessage( hWndOwner, WM_SIZECLIPBOARD, hWnd,
                    MAKELONG(hRect,0) );

       /* unlock the rectangle */
       GlobalUnlock( hRect );

     }

     /* release the rectangle */
     GlobalFree( hRect );

   }

 }

 /* return clipboard data handle */
 return( hClipData );

}

/*
 * OwnerSize( hWnd, hDispInfo, wParam, lParam ) : HANDLE;
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
 */

HANDLE FAR PASCAL OwnerSize(
 HWND      hWnd,
 HANDLE    hDispInfo,
 WORD      wParam,
 LONG      lParam )
{
 HANDLE    hRect;
 LPRECT    lpRect;
 HWND      hWndOwner;

 /* warning level 3 compatibility */
 wParam;

 /* reset scroll bars */
 SetScrollPos( hWnd, SB_HORZ, 0, TRUE );
 SetScrollPos( hWnd, SB_VERT, 0, TRUE );

 /* notify clipboard owner of change */
 hWndOwner = GetClipboardOwner();
 if ( hWndOwner ) {

   /* allocate space for update rectangle */
   hRect = GlobalAlloc( GHND, (DWORD)sizeof(RECT) );
   if ( hRect ) {

     /* lock update rectangle */
     lpRect = (LPRECT)GlobalLock( hRect );
     if ( lpRect ) {

       /* define the update rectangle */
       lpRect->top = 0;
       lpRect->left = 0;
       lpRect->right = LOWORD(lParam);
       lpRect->bottom = HIWORD(lParam);

       /* notify the clipboard owner */
       SendMessage( hWndOwner, WM_SIZECLIPBOARD, hWnd,
                    MAKELONG(hRect,0) );

       /* unlock the rectangle */
       GlobalUnlock( hRect );

     }

     /* release the rectangle */
     GlobalFree( hRect );

   }

 }

 /* return final result */
 return( hDispInfo );

}

/*
 * OwnerHScroll( hWnd, hDispInfo, wParam, lParam ) : HANDLE;
 *
 *   hWnd        handle to display window
 *   hDispInfo   handle to display information block
 *   wParam      word parameter of WM_HSCROLL message
 *   lParam      long parameter of WM_HSCROLL message
 *
 * This function is responsible for handling all the horizontal
 * scroll messages received when viewing a bitmap clipboard format.
 * If necessary, changes to the display info block can be made.
 * As currently implemented, no action is taken.  The value returned
 * is the handle to the updated display information block.
 */

HANDLE FAR PASCAL OwnerHScroll(
 HWND      hWnd,
 HANDLE    hDispInfo,
 WORD      wParam,
 LONG      lParam )
{
 HWND      hWndOwner;

 /* notify clipboard owner if present */
 hWndOwner = GetClipboardOwner();
 if ( hWndOwner )
   SendMessage( hWndOwner, WM_HSCROLLCLIPBOARD, hWnd,
                MAKELONG(wParam,LOWORD(lParam)) );

 /* return result */
 return( hDispInfo );

}

/*
 * OwnerVScroll( hWnd, hDispInfo, wParam, lParam ) : HANDLE;
 *
 *   hWnd        handle to display window
 *   hDispInfo   handle to display information block
 *   wParam      word parameter of WM_VSCROLL message
 *   lParam      long parameter of WM_VSCROLL message
 *
 * This function is responsible for handling all the vertical scroll
 * messages received when viewing a bitmap clipboard format.  If
 * necessary, changes to the display info block can be made.
 * As currently implemented, no action is taken.  The value returned
 * is the handle to the updated display information block.
 */

HANDLE FAR PASCAL OwnerVScroll(
 HWND      hWnd,
 HANDLE    hDispInfo,
 WORD      wParam,
 LONG      lParam )
{
 HWND      hWndOwner;

 /* notify clipboard owner if present */
 hWndOwner = GetClipboardOwner();
 if ( hWndOwner )
   SendMessage( hWndOwner, WM_VSCROLLCLIPBOARD, hWnd,
                MAKELONG(wParam,LOWORD(lParam)) );

 /* return result */
 return( hDispInfo );

}

/*
 * OwnerPaint( hWnd, hDispInfo, wParam, lParam ) : HANDLE;
 *
 *   hWnd        handle to display window
 *   hDispInfo   handle to display information block
 *   wParam      word parameter of WM_PAINT message
 *   lParam      long parameter of WM_PAINT message
 *
 * This function is responsible for handling all the paint related
 * aspects for the bitmap clipboard format.  This function calculates
 * the required update portion of the window and BitBlts the bitmap
 * into the region.  The handle returned by this function is to the
 * update display information block.
 */

HANDLE FAR PASCAL OwnerPaint(
 HWND          hWnd,
 HANDLE        hDispInfo,
 WORD          wParam,
 LONG          lParam )
{
 HANDLE        hPs;
 LPPAINTSTRUCT lpPs;
 HWND          hWndOwner;

 /* warning level 3 compatibility */
 wParam;
 lParam;

 /* allocate space for paint structure */
 hPs = GlobalAlloc( GHND, (DWORD)sizeof(PAINTSTRUCT) );
 if ( hPs ) {

   /* lock it down */
   lpPs = (LPPAINTSTRUCT)GlobalLock( hPs );
   if ( lpPs ) {

     /* start the paint operation */
     BeginPaint( hWnd, lpPs );

     /* notify the clipboard owner if present */
     hWndOwner = GetClipboardOwner();
     if ( hWndOwner )
       SendMessage( hWndOwner, WM_PAINTCLIPBOARD, hWnd,
                    MAKELONG(hPs,0) );

     /* end the paint operation & unlock the paint structure */
     EndPaint( hWnd, lpPs );
     GlobalUnlock( hPs );

   }

   /* release the paint structure */
   GlobalFree( hPs );

 }

 /* return final result */
 return( hDispInfo );

}

/*
 * OwnerDestroy( hDispInfo ) : HANDLE;
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
 * Note that is this function we are passing a NULL rectangle to the
 * clipboard owner.  This informs it that it can release any/all
 * resources used to support the display of Owner Display data.
 */

HANDLE FAR PASCAL OwnerDestroy(
 HWND      hWnd,
 HANDLE    hDispInfo )
{
 HANDLE    hRect;
 HWND      hWndOwner;

 /* check if clipboard owner still around */
 hWndOwner = GetClipboardOwner();
 if ( hWndOwner ) {

   /* notify clipboard owner */
   hRect = GlobalAlloc( GHND, (DWORD)sizeof(RECT) );
   SendMessage(hWndOwner,WM_SIZECLIPBOARD,hWnd,MAKELONG(hRect,0) );
   GlobalFree( hRect );

 }

 /* return old handle */
 return( hDispInfo );

}
