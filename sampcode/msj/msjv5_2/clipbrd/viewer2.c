/*
 * WINDOWS CLIPBOARD VIEWER - STATUS & CLIENT WINDOW SOURCE CODE
 *
 * LANGUAGE      : Microsoft C 5.1
 * MODEL         : medium
 * ENVIRONMENT   : Microsoft Windows 2.1 SDK
 * STATUS        : operational
 *
 * Note that both the client and status windows save the current
 * clipboard format and data handle.  In general this is NOT good
 * programming practice.  In this situation we assume that we will
 * always be notifed when ANY change occurs to the clipboard.
 *
 * 1.01 - Kevin P. Welch - add param to GetClipboardFmtName.
 *
 */

#define  NOCOMM

#include <windows.h>
#include <string.h>
#include <stdio.h>

#include "viewer.h"

/* client window properties */
#define  CLIENT_FORMAT     MAKEINTRESOURCE(1)
#define  CLIENT_MODULE     MAKEINTRESOURCE(2)
#define  CLIENT_DISPINFO   MAKEINTRESOURCE(3)

/* status window properties */
#define  STATUS_DATA       MAKEINTRESOURCE(1)
#define  STATUS_FORMAT     MAKEINTRESOURCE(2)
#define  STATUS_DISPFONT   MAKEINTRESOURCE(3)

/*
 * StatusWndFn( hWnd, wMsg, wParam, lParam ) : LONG
 *
 *    hWnd       window handle
 *    wMsg       message number
 *    wParam     additional message information
 *    lParam     additional message information
 *
 * This window function processes all the messages related to
 * the status child window of the clipboard viewer.
 *
 */

LONG FAR PASCAL StatusWndFn( hWnd, wMsg, wParam, lParam )
   HWND        hWnd;
   WORD        wMsg;
   WORD        wParam;
   LONG        lParam;
{
   LONG        lResult;

   /* initialization */
   lResult = FALSE;

   /* process each message */
   switch( wMsg )
      {
 case WM_CREATE : /* window being created */
   {
     HDC         hDC;
     LOGFONT     LogFont;
     TEXTMETRIC  TextMetric;

     /* retrieve system font metrics */
     hDC = GetDC( hWnd );
     GetTextMetrics( hDC, &TextMetric );
     ReleaseDC( hWnd, hDC );

     /* define display font */
     LogFont.lfHeight = TextMetric.tmHeight;
     LogFont.lfWidth = TextMetric.tmAveCharWidth;
     LogFont.lfEscapement = 0;
     LogFont.lfOrientation = 0;
     LogFont.lfWeight = FW_MEDIUM;
     LogFont.lfItalic = FALSE;
     LogFont.lfUnderline = FALSE;
     LogFont.lfStrikeOut = FALSE;
     LogFont.lfCharSet = ANSI_CHARSET;
     LogFont.lfOutPrecision = OUT_STROKE_PRECIS;
     LogFont.lfClipPrecision = CLIP_STROKE_PRECIS;
     LogFont.lfQuality = PROOF_QUALITY;
     LogFont.lfPitchAndFamily = DEFAULT_PITCH|FF_MODERN;

     strcpy( LogFont.lfFaceName, "Helv" );

     /* define property lists */
     SetProp( hWnd, STATUS_DATA, NULL );
     SetProp( hWnd, STATUS_FORMAT, NULL );
     SetProp( hWnd, STATUS_DISPFONT, CreateFontIndirect(&LogFont) );

   }
   break;
 case WM_UPDATE : /* update command from parent */

   /* update property lists */
   SetProp( hWnd, STATUS_DATA, LOWORD(lParam) );
   SetProp( hWnd, STATUS_FORMAT, wParam );

   /* force update of window */
   InvalidateRect( hWnd, NULL, TRUE );

   break;
 case WM_PAINT : /* paint window */
   {
     PAINTSTRUCT   Ps;
     WORD          wCrntFmt;
     HFONT         hOldFont;
     RECT          rcClient;
     HANDLE        hCrntData;
     TEXTMETRIC    TextMetric;
     char          szFmtSize[32];
     char          szFmtName[80];
     char          szFmtOwner[80];

     /* retrieve property list data */
     hCrntData = GetProp( hWnd, STATUS_DATA );
     wCrntFmt = GetProp( hWnd, STATUS_FORMAT );

     /* define clipboard format name */
     GetClipboardFmtName(wCrntFmt,szFmtName,sizeof(szFmtName),TRUE);

     /* define clipboard format owner */
     if ( wCrntFmt && GetClipboardOwner() ) {
       GetModuleFileName(
         GetClassWord( GetClipboardOwner(), GCW_HMODULE ),
         szFmtSize,
         sizeof(szFmtSize)
       );
       sprintf( szFmtOwner, "%s from %s", szFmtName,
                strrchr(szFmtSize,'\\')+1 );
       strcpy( szFmtName, szFmtOwner );
     }

     /* define clipboard format size */
     switch( wCrntFmt )
       {
     case NULL : /* empty */
       strcpy( szFmtSize, "0 bytes" );
       break;
     case CF_BITMAP : /* standard GDI bitmap */
     case CF_DSPBITMAP : /* display bitmap */
       {
         BITMAP      bmStruct;

         /* retieve bitmap object */
         if (GetObject(hCrntData,sizeof(BITMAP),(LPSTR)&bmStruct)==
             sizeof(BITMAP) ) {
           sprintf(
             szFmtSize,
             " - %u x %u%s",
             bmStruct.bmWidth,
             bmStruct.bmHeight,
             ((bmStruct.bmPlanes==1)&&(bmStruct.bmBitsPixel==1)) ?
              " - mono" : ""
           );
           strcat( szFmtName, szFmtSize );
           sprintf(
             szFmtSize,
             "%ld bytes",
             sizeof(BITMAP) + ((LONG)bmStruct.bmWidthBytes *
                    bmStruct.bmPlanes * bmStruct.bmHeight)
           );
         } else
           strcpy( szFmtSize, "(size unknown)" );

       }
       break;
     case CF_METAFILEPICT : /* standard GDI metafile */
     case CF_DSPMETAFILEPICT : /* display metafile */
       {
         LPMETAFILEPICT    lpmfStruct;
         char          szMapMode[32];

         /* retieve bitmap object */
         lpmfStruct = (LPMETAFILEPICT)GlobalLock( hCrntData );
         if ( lpmfStruct ) {
           switch( lpmfStruct->mm )
             {
              case MM_TEXT :
                sprintf( szFmtSize, " - %u x %u - text",
                         lpmfStruct->xExt, lpmfStruct->yExt );
                break;
              case MM_LOMETRIC :
                sprintf( szFmtSize, " - %u x %u - low metric",
                         lpmfStruct->xExt, lpmfStruct->yExt );
                break;
              case MM_HIMETRIC :
                sprintf( szFmtSize, " - %u x %u - high metric",
                         lpmfStruct->xExt, lpmfStruct->yExt );
                break;
              case MM_LOENGLISH :
                sprintf( szFmtSize, " - %u x %u - low english",
                         lpmfStruct->xExt, lpmfStruct->yExt );
                break;
              case MM_HIENGLISH :
                sprintf( szFmtSize, " - %u x %u - high english",
                         lpmfStruct->xExt, lpmfStruct->yExt );
                break;
              case MM_TWIPS :
                sprintf( szFmtSize, " - %u x %u - twips",
                         lpmfStruct->xExt, lpmfStruct->yExt );
                break;
              case MM_ISOTROPIC :
                sprintf( szFmtSize, " - isotropic" );
                break;
              case MM_ANISOTROPIC :
                sprintf( szFmtSize, " - anisotropic" );
                break;
              default :
                strcpy( szMapMode, "" );
                break;
           }
           strcat( szFmtName, szFmtSize );
           sprintf(
             szFmtSize,
             "%ld bytes",
             GlobalSize(hCrntData)+GlobalSize(lpmfStruct->hMF)
           );
           UnlockData( hCrntData );
         } else
           strcpy( szFmtSize, "(size unknown)" );

       }
       break;
     case CF_TEXT : /* standard text */
     case CF_SYLK : /* standard SYLK text */
     case CF_DIF : /* standard DIF text */
     case CF_TIFF : /* standard binary TIFF data */
     case CF_OEMTEXT : /* standard OEM text */
     case CF_OWNERDISPLAY : /* owner display */
     case CF_DSPTEXT : /* display text */
     default : /* something else */
       sprintf( szFmtSize, "%ld bytes", GlobalSize(hCrntData) );
       break;
     }

     /* make sure entire window is updated */
     InvalidateRect( hWnd, NULL, TRUE );

     /* start painting */
     BeginPaint( hWnd, &Ps );
     GetTextMetrics( Ps.hdc, &TextMetric );
     hOldFont = SelectObject( Ps.hdc, GetProp(hWnd,
                                              STATUS_DISPFONT) );

     /* retrieve & adjust client rectangle for drawing */
     GetClientRect( hWnd, &rcClient );
     rcClient.left += TextMetric.tmAveCharWidth;
     rcClient.right -= TextMetric.tmAveCharWidth;

     /* update clipboard format name & size */
     DrawText( Ps.hdc, szFmtName, -1, &rcClient,
               DT_LEFT|DT_VCENTER|DT_SINGLELINE );
     DrawText( Ps.hdc, szFmtSize, -1, &rcClient,
               DT_RIGHT|DT_VCENTER|DT_SINGLELINE );

     /* end painting */
     SelectObject( Ps.hdc, hOldFont );
     EndPaint( hWnd, &Ps );

   }
   break;
 case WM_CLOSE : /* window being closed */

   /* delete display font */
   DeleteObject( GetProp(hWnd,STATUS_DISPFONT) );

   /* remove property lists */
   RemoveProp( hWnd, STATUS_DATA );
   RemoveProp( hWnd, STATUS_FORMAT );
   RemoveProp( hWnd, STATUS_DISPFONT );

   /* end it all */
   DestroyWindow( hWnd );

   break;
 default : /* send to default */
      lResult = DefWindowProc( hWnd, wMsg, wParam, lParam );
      break;
   }

   /* return normal result */
   return( lResult );

}

/*
 * ClientWndFn( hWnd, wMsg, wParam, lParam ) : LONG
 *
 *    hWnd        window handle
 *    wMsg       message number
 *    wParam      additional message information
 *    lParam      additional message information
 *
 * This window function processes all the messages related to
 * the client area of the clipboard viewer.
 *
 */

LONG FAR PASCAL ClientWndFn( hWnd, wMsg, wParam, lParam )
   HWND        hWnd;
   WORD        wMsg;
   WORD        wParam;
   LONG        lParam;
{
 FARPROC   lpFn;
 LONG        lResult;

   /* initialization */
   lResult = FALSE;

   /* process each message */
   switch( wMsg )
      {
 case WM_CREATE : /* window being created */

   /* define property lists */
   SetProp( hWnd, CLIENT_FORMAT, NULL );
   SetProp( hWnd, CLIENT_MODULE, NULL );
   SetProp( hWnd, CLIENT_DISPINFO, NULL );

     break;
 case WM_UPDATE : /* update command from parent */

   /* check if clipboard data present */
   if (GetProp(hWnd,CLIENT_FORMAT) && GetProp(hWnd,CLIENT_MODULE)) {
     lpFn=GetProcAddress(GetProp(hWnd,CLIENT_MODULE),LIB_DESTROY );
     (*lpFn)( hWnd, GetProp(hWnd,CLIENT_DISPINFO) );
   }

   /* update property lists */
   SetProp( hWnd, CLIENT_FORMAT, wParam );
   SetProp( hWnd, CLIENT_MODULE, HIWORD(lParam) );

   if ( wParam && HIWORD(lParam) ) {
     lpFn = GetProcAddress( GetProp(hWnd,CLIENT_MODULE), LIB_CREATE);
     SetProp( hWnd, CLIENT_DISPINFO, (*lpFn)(hWnd,LOWORD(lParam)) );
   } else {
     SetProp( hWnd, CLIENT_DISPINFO, NULL );
     SetScrollPos( hWnd, SB_HORZ, 0, TRUE );
     SetScrollPos( hWnd, SB_VERT, 0, TRUE );
   }

   /* force update of window */
   InvalidateRect( hWnd, NULL, TRUE );

   break;
 case WM_SIZE : /* window being sized */
 case WM_HSCROLL : /* horizontal scroll request */
 case WM_VSCROLL : /* vertical scroll request */
 case WM_PAINT : /* window being painted */

   /* determine if a responsible library function available */
   if (GetProp(hWnd,CLIENT_FORMAT) && GetProp(hWnd,CLIENT_MODULE)) {

     LPSTR     lpszLibrary;

     /* determine which library is responsible */
     switch( wMsg )
       {
     case WM_SIZE :
       lpszLibrary = LIB_SIZE;
       break;
     case WM_HSCROLL :
       lpszLibrary = LIB_HSCROLL;
       break;
     case WM_VSCROLL :
       lpszLibrary = LIB_VSCROLL;
       break;
     case WM_PAINT :
       lpszLibrary = LIB_PAINT;
       break;
     }

     /* call display library function */
     lpFn = GetProcAddress(GetProp(hWnd,CLIENT_MODULE),lpszLibrary);
     (*lpFn)( hWnd, GetProp(hWnd,CLIENT_DISPINFO), wParam, lParam );

   } else
       lResult = DefWindowProc( hWnd, wMsg, wParam, lParam );

   break;
 case WM_CLOSE : /* window being closed */

   /* pass to responsible destroy library function */
   if (GetProp(hWnd,CLIENT_FORMAT) && GetProp(hWnd,CLIENT_MODULE)) {
     lpFn = GetProcAddress(GetProp(hWnd,CLIENT_MODULE),LIB_DESTROY );
     (*lpFn)( hWnd, GetProp(hWnd,CLIENT_DISPINFO) );
   }

   /* remove properties */
   RemoveProp( hWnd, CLIENT_FORMAT );
   RemoveProp( hWnd, CLIENT_MODULE );
   RemoveProp( hWnd, CLIENT_DISPINFO );

   /* end it all */
   DestroyWindow( hWnd );

   break;
 default : /* send to default */
      lResult = DefWindowProc( hWnd, wMsg, wParam, lParam );
      break;
   }

   /* return normal result */
   return( lResult );

}


