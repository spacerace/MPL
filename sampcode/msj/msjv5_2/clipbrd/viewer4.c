/*
 * WINDOWS CLIPBOARD VIEWER - UTILITY SOURCE CODE
 *
 * LANGUAGE      : Microsoft C 5.1
 * MODEL         : medium
 * ENVIRONMENT   : Microsoft Windows 2.1 SDK
 * STATUS        : operational
 *
 * 1.01- Kevin P. Welch - add param to GetClipboardFmtName.
 *
 */

#define  NOCOMM

#include <windows.h>
#include "viewer.h"


/*
 * CenterPopup( hWnd, hParentWnd ) : BOOL
 *
 *   hWnd          window handle
 *   hParentWnd      parent window handle
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
   HWND    hWnd,
   HWND    hParentWnd
 )
{
 /* local variables */
 int   xPopup;           /* popup x position */
 int   yPopup;           /* popup y position */
 int   cxPopup;          /* width of popup window */
 int   cyPopup;          /* height of popup window */
 int   cxScreen;         /* width of main display */
 int   cyScreen;         /* height of main display */
 int   cxParent;         /* width of parent window */
 int   cyParent;         /* height of parent window */
 RECT  rcWindow;         /* temporary window rect */

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
   cxParent = rcWindow.right  - rcWindow.left;
   cyParent = rcWindow.bottom - rcWindow.top;

   /* center within parent window */
   xPopup = rcWindow.left + ((cxParent - cxPopup)/2);
   yPopup = rcWindow.top  + ((cyParent - cyPopup)/2);

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


/*
 * GetClipboardFmtName( wFmt, lpszFmt, wMax, bInquire ) : WORD;
 *
 *    wFmt           clipboard format number
 *    lpszFmt        name of clipboard format
 *    wMax           maximum name size
 *   bInquire      inquire full name if owner-display
 *
 * This utility function is identical to GetClipboardFormatName, but
 * is capable of defining the name of ANY clipboard format, including
 * predefined and owner-display ones.  Value returned is the number
 * of bytes copied to the clipboard format name.  If this value is
 * zero then the clipboard format number specified is undefined.
 *
 * Note that if the bInquire flag is TRUE, this function will attempt
 * to ask the clipboard owner for the full name of the owner display
 * data.  This may result in misleading information if this function
 * is called with an owmer display format number when the real owner
 * is not currently present!
 *
 */

WORD FAR PASCAL GetClipboardFmtName(
 WORD      wFmt,
 LPSTR     lpszFmt,
 WORD      wMax,
 BOOL      bInquire )
{
 HANDLE    hTemp;
 LPSTR     lpszTemp;

 /* initialization */
 lpszFmt[0] = 0;

 /* define format name */
 switch( wFmt )
   {
 case NULL : /* empty */
   lstrcpy( lpszFmt, "(Clipboard Empty)" );
   break;
 case CF_TEXT : /* standard text */
   lstrcpy( lpszFmt, "Text" );
   break;
 case CF_BITMAP : /* standard GDI bitmap */
   lstrcpy( lpszFmt, "Bitmap" );
   break;
 case CF_METAFILEPICT : /* standard GDI metafile */
   lstrcpy( lpszFmt, "Picture" );
   break;
 case CF_SYLK : /* standard SYLK text */
   lstrcpy( lpszFmt, "SYLK" );
   break;
 case CF_DIF : /* standard DIF text */
   lstrcpy( lpszFmt, "DIF" );
   break;
 case CF_TIFF : /* standard binary TIFF data */
   lstrcpy( lpszFmt, "TIFF" );
   break;
 case CF_OEMTEXT : /* standard OEM text */
   lstrcpy( lpszFmt, "OEM Text" );
   break;
 case CF_OWNERDISPLAY : /* owner display */
   lstrcpy( lpszFmt, "Owner Display" );
   if ( bInquire && GetClipboardOwner() ) {
     hTemp = GlobalAlloc( GHND, (DWORD)64 );
     if ( hTemp ) {
       lpszTemp = GlobalLock( hTemp );
       if ( lpszTemp ) {
         SendMessage( GetClipboardOwner(), WM_ASKCBFORMATNAME,
                      63, (LONG)lpszTemp );
         lstrcpy( lpszFmt, lpszTemp );
         GlobalUnlock( hTemp );
       }
       GlobalFree( hTemp );
     }
   }
   break;
 case CF_DSPTEXT : /* display text */
   lstrcpy( lpszFmt, "Display Text" );
   break;
 case CF_DSPBITMAP : /* display bitmap */
   lstrcpy( lpszFmt, "Display Bitmap" );
   break;
 case CF_DSPMETAFILEPICT : /* display picture */
   lstrcpy( lpszFmt, "Display Picture" );
   break;
 default : /* something else */
   if ( GetClipboardFormatName(wFmt,lpszFmt,wMax) == 0 )
     lstrcpy( lpszFmt, "(Undefined)" );
   break;
 }

 /* return size of string */
 return( (WORD)lstrlen(lpszFmt) );

}

/*
 * GetClipboardFmtNumber( lpszFmt ) : WORD;
 *
 *    lpszFmt        name of clipboard format
 *
 * This function retrieves (and if necessary, defines) the internal
 * clipboard format number for the specified string.  Before checking
 * registered clipboard formats, this function checks to see if one
 * of the predefined formats is referenced.
 *
 */

WORD FAR PASCAL GetClipboardFmtNumber(
 LPSTR       lpszFmt )
{
 WORD      wFmt;

 /* check predefined formats */
 if ( lstrcmp(lpszFmt,"Text") == 0 )
   wFmt = CF_TEXT;
 else
   if ( lstrcmp(lpszFmt,"Bitmap") == 0 )
     wFmt = CF_BITMAP;
   else
     if ( lstrcmp(lpszFmt,"Picture") == 0 )
       wFmt = CF_METAFILEPICT;
     else
       if ( lstrcmp(lpszFmt,"SYLK") == 0 )
         wFmt = CF_SYLK;
       else
         if ( lstrcmp(lpszFmt,"DIF") == 0 )
           wFmt = CF_DIF;
         else
           if ( lstrcmp(lpszFmt,"TIFF") == 0 )
             wFmt = CF_TIFF;
           else
             if ( lstrcmp(lpszFmt,"OEM Text") == 0 )
               wFmt = CF_OEMTEXT;
             else
               if ( lstrcmp(lpszFmt,"Owner Display") == 0 )
                 wFmt = CF_OWNERDISPLAY;
               else
                 if ( lstrcmp(lpszFmt,"Display Text") == 0 )
                   wFmt = CF_DSPTEXT;
                 else
                   if ( lstrcmp(lpszFmt,"Display Bitmap") == 0 )
                     wFmt = CF_DSPBITMAP;
                   else
                     if ( lstrcmp(lpszFmt,"Display Picture") == 0 )
                       wFmt = CF_DSPMETAFILEPICT;
                     else
                       wFmt = RegisterClipboardFormat( lpszFmt );

 /* return format */
 return( wFmt );

}

/*
 * GetClipboardModule( wCrntFmt, hLibrary ) : HANDLE;
 *
 *    wCrntFmt       current clipboard foramt
 *    hLibrary       handle to clipboard support library
 *
 * This function searches the clipboard module library for one which
 * can support the format in question.  If none is present a value of
 * NULL is returned.  This should be interpreted that the current
 * format is not supported by the system.
 *
 */

HANDLE FAR PASCAL GetClipboardModule(
 WORD        wCrntFmt,
 HANDLE      hLibrary )
{
 WORD      wEntry;
 HANDLE    hModule;
 LPLIBRARY lpLibrary;

 /* initialization */
 hModule = NULL;

 lpLibrary = (LPLIBRARY)GlobalLock( hLibrary );
 if ( lpLibrary ) {

   /* search library for module */
   for ( wEntry=0;(wEntry<lpLibrary->wModules)&&(hModule==NULL);
         wEntry++ )
     if ( lpLibrary->Module[wEntry].wFormat == wCrntFmt )
       hModule = lpLibrary->Module[wEntry].hModule;

   /* unlock library */
   GlobalUnlock( hLibrary );

 }

 /* return module handle */
 return( hModule );

}
