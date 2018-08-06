/*
 * WINDOWS SPYGLASS - SOURCE CODE
 *
 * LANGUAGE      : Microsoft C5.1
 * MODEL         : small
 * ENVIRONMENT   : Microsoft Windows 2.1 SDK
 * STATUS        : operational
 *
 * This module contains the window mainline and all other
 * related source code for the sypglass application.
 *
 *    (C) Copyright 1988
 *
 *    Kevin P. Welch
 *    Eikon Systems, Inc
 *    989 East Hillsdale Blvd, Suite 260
 *    Foster City, California 94404
 *    415-349-4664
 *
 * 08/31/88 1.00 - Kevin P. Welch - initial creation.
 *
 */

#include <windows.h>
#include <memory.h>
#include "spyglass.h"
 
/* global data definitions */
HANDLE   hCursor;          
BOOL     bCapture;
HWND     hWndNext;
HDC      hScreenDC;
RECT     rMagGlass;
RECT     rSpyGlass;
WORD     wEnlargement;

/**/

/*
 * WinMain( hInstance, hPrevInstance, lpszCmd, wCmdShow ) : VOID
 *
 *    hInstance      current instance handle
 *    hPrevInstance  previous instance handle
 *    lpszCmd        current command line
 *    wCmdShow       initial show window parameter
 *
 * This function is responsible for registering and creating
 * the spyglass window.  Once the window has been created, it is
 * also responsible for retrieving and dispatching all the
 * messages related to the spyglass window.
 *
 */

VOID PASCAL WinMain(
   HANDLE      hInstance,
   HANDLE      hPrevInstance,
   LPSTR       lpszCmd,
   WORD        wCmdShow )
{
   /* local variables */
   MSG         Msg;
   HWND        hWnd;
   WNDCLASS    WndClass;

   /* define desktop class */
   memset( &WndClass, 0, sizeof(WNDCLASS) );
   WndClass.lpszClassName =   (LPSTR)"SpyGlass";
   WndClass.hCursor =         LoadCursor( NULL, IDC_ARROW );
   WndClass.lpszMenuName =    (LPSTR)NULL;
   WndClass.style =           CS_HREDRAW | CS_VREDRAW;
   WndClass.lpfnWndProc =     SpyGlassWndFn;
   WndClass.hInstance =       hInstance;
   WndClass.hIcon =           LoadIcon( hInstance, "SpyGlassIcon" );
   WndClass.hbrBackground =   (HBRUSH)(COLOR_BACKGROUND + 1);

   if ( (hPrevInstance) || (RegisterClass(&WndClass)) ) {

      /* create main window */
      hWnd = CreateWindow(
            "SpyGlass",              /* class name */
            "SpyGlass",              /* caption */
            SPYGLASS_STYLE,          /* style */
            SPYGLASS_XPOS,           /* x position */
            SPYGLASS_YPOS,           /* y position */
            SPYGLASS_WIDTH,          /* width */
            SPYGLASS_HEIGHT,         /* height */
            (HWND)NULL,              /* parent window */
            (HMENU)NULL,             /* menu */
            hInstance,               /* application */
            (LPSTR)NULL              /* other data */
         );

      /* continue if successful */
      if ( hWnd ) {
      
         /* display window */
         ShowWindow( hWnd, wCmdShow );                  

         /* process messages */
         while ( GetMessage((LPMSG)&Msg,NULL,0,0) ) {
            TranslateMessage( (LPMSG)&Msg );
            DispatchMessage( (LPMSG)&Msg );
         }

         /* normal exit */
         exit( Msg.wParam );  

      } else
         WARNING( NULL, "Unable to create SpyGlass window!" );

   } else
      WARNING( NULL, "Unable to register SpyGlass window!" );

}

/**/

/*
 * SpyGlassWndFn( hWnd, wMessage, wParam, lParam ) : LONG
 *
 *    hWnd        window handle
 *    wMessage    message number
 *    wParam      additional message information
 *    lParam      additional message information
 *
 * This window function processes all the messages related to
 * the SpyGlass window.  Note that most of the operations
 * performed by this function are done in screen coordinates.
 *
 */
 
LONG FAR PASCAL SpyGlassWndFn( hWnd, wMessage, wParam, lParam )
   HWND        hWnd;
   WORD        wMessage;
   WORD        wParam;
   LONG        lParam;
{
   /* local variables */
   LONG        lResult;    

   /* initialization */
   lResult = FALSE;
   
   /* process each message */
   switch( wMessage )
      {
   case WM_CREATE : /* window being created */
      {
         HMENU    hSysMenu;
      
         /* intialization */
         bCapture = FALSE;
         wEnlargement = SC_8X;
      
         /* put ourselves in the viewer chain */
         hWndNext = SetClipboardViewer( hWnd );

         /* add enlargement items to system menu */
         hSysMenu = GetSystemMenu( hWnd, FALSE );

         ChangeMenu( hSysMenu, NULL, NULL, NULL, MF_SEPARATOR|MF_APPEND );
         ChangeMenu( hSysMenu, NULL, "&1x Zoom", SC_1X, MF_APPEND );
         ChangeMenu( hSysMenu, NULL, "&2x Zoom", SC_2X, MF_APPEND );
         ChangeMenu( hSysMenu, NULL, "&4x Zoom", SC_4X, MF_APPEND );
         ChangeMenu( hSysMenu, NULL, "&8x Zoom", SC_8X, MF_APPEND );
         ChangeMenu( hSysMenu, NULL, "1&6x Zoom", SC_16X, MF_APPEND );

         CheckMenuItem( hSysMenu, wEnlargement, MF_CHECKED );
      
      }
      break;
   case WM_SYSCOMMAND : /* system command */

      /* process sub-message */
      switch( wParam )
         {
      case SC_1X : /* change enlargement factor */
      case SC_2X :
      case SC_4X :
      case SC_8X :
      case SC_16X :
         {
            HMENU    hSysMenu;
         
            hSysMenu = GetSystemMenu( hWnd, FALSE );
            CheckMenuItem( hSysMenu, wEnlargement, MF_UNCHECKED );
            wEnlargement = wParam;
            CheckMenuItem( hSysMenu, wEnlargement, MF_CHECKED );
         }
         break;
      default :
         lResult = DefWindowProc( hWnd, wMessage, wParam, lParam );
         break;
      }     
      
      break;
   case WM_LBUTTONDOWN : /* mouse button down */
   
      /* start tracking if capture is off */
      if ( bCapture == FALSE ) {

         POINT    ptWindow;
         
         /* start tracking mouse & hide cursor */
         bCapture = TRUE;

         SetCapture( hWnd );
         hCursor = SetCursor( NULL );

         /* retrieve viewport coordinates */
         GetClientRect( hWnd, &rSpyGlass );

         /* redefine viewport in screen coordinates */
         ptWindow.x = rSpyGlass.left;
         ptWindow.y = rSpyGlass.top;
         ClientToScreen( hWnd, &ptWindow );
         rSpyGlass.left = ptWindow.x;
         rSpyGlass.top = ptWindow.y;
         rSpyGlass.right += rSpyGlass.left;
         rSpyGlass.bottom += rSpyGlass.top;

         /* define magnifying glass - origin at mouse location */
         ClientToScreen( hWnd, (LPPOINT)&lParam );

         rMagGlass.top = HIWORD(lParam);
         rMagGlass.left = LOWORD(lParam);
         rMagGlass.right = rMagGlass.left +
               (WIDTH(rSpyGlass) / wEnlargement);
         rMagGlass.bottom = rMagGlass.top +
               (HEIGHT(rSpyGlass) / wEnlargement);

         /* display initial magnifying glass */
         hScreenDC = CreateDC( "DISPLAY", NULL, NULL, NULL );

         SetROP2( hScreenDC, R2_NOT );
         SetBkMode( hScreenDC, TRANSPARENT );

         Rectangle(
            hScreenDC,
            rMagGlass.left - 1,
            rMagGlass.top - 1,
            rMagGlass.right + 1,
            rMagGlass.bottom + 1
         );

      }

      break;
   case WM_RBUTTONDOWN : /* right button clicked */

      /* grab screen image if capture is on */
      if ( bCapture ) {
      
         BOOL     bIntersect;
         RECT     rIntersect;

         /* determine viewport & magnifying glass intersect */
         bIntersect = (BOOL)IntersectRect(
                                 &rIntersect,
                                 &rMagGlass,
                                 &rSpyGlass
                              );

         /* erase magnifying glass if intersection */
         if ( bIntersect )
            Rectangle(
               hScreenDC,
               rMagGlass.left - 1,
               rMagGlass.top - 1,
               rMagGlass.right + 1,
               rMagGlass.bottom  + 1
            );

         /* perform stretch or bit-blt - depending on enlargement */
         if ( wEnlargement > 1 ) 
            StretchBlt(
               hScreenDC,
               rSpyGlass.left,
               rSpyGlass.top,
               WIDTH(rSpyGlass),
               HEIGHT(rSpyGlass),
               hScreenDC,
               rMagGlass.left,
               rMagGlass.top,
               WIDTH(rMagGlass),
               HEIGHT(rMagGlass),
               SRCCOPY
            );
         else
            BitBlt(
               hScreenDC,
               rSpyGlass.left,
               rSpyGlass.top,
               WIDTH(rSpyGlass),
               HEIGHT(rSpyGlass),
               hScreenDC,
               rMagGlass.left,
               rMagGlass.top,
               SRCCOPY
            );

         /* restore magnifying glass if intersection */
         if ( bIntersect )
            Rectangle(
               hScreenDC,
               rMagGlass.left - 1,
               rMagGlass.top - 1,
               rMagGlass.right + 1,
               rMagGlass.bottom  + 1
            );

      } 

      break;
   case WM_MOUSEMOVE : /* mouse being moved */
      
      /* move magnifying glass if capture is on */
      if ( bCapture ) {
      
         BOOL     bIntersect;
         RECT     rIntersect;

         /* convert mouse location to screen coordinates */
         ClientToScreen( hWnd, (LPPOINT)&lParam );
         
         /* determine if viewport & magnifying glass intersect */
         bIntersect = (BOOL)IntersectRect( 
                                 &rIntersect,
                                 &rMagGlass,
                                 &rSpyGlass
                              );

         /* erase old magnifying glass */
         Rectangle(
            hScreenDC,
            rMagGlass.left - 1,
            rMagGlass.top - 1,
            rMagGlass.right + 1,
            rMagGlass.bottom  + 1
         );
      
         /* calculate new magnifying glass position */
         rMagGlass.right -= rMagGlass.left;
         rMagGlass.bottom -= rMagGlass.top;

         rMagGlass.top = HIWORD(lParam);
         rMagGlass.left = LOWORD(lParam);

         rMagGlass.right += rMagGlass.left;
         rMagGlass.bottom += rMagGlass.top;

         /* display new magnifying glass if no intersection */
         if ( !bIntersect )
            Rectangle(
               hScreenDC,
               rMagGlass.left - 1,
               rMagGlass.top - 1,
               rMagGlass.right + 1,
               rMagGlass.bottom  + 1
            );

         /* capture screen image if right button depressed */
         if ( wParam&MK_RBUTTON ) {
            if ( wEnlargement > 1 ) 
               StretchBlt(
                  hScreenDC,
                  rSpyGlass.left,
                  rSpyGlass.top,
                  WIDTH(rSpyGlass),
                  HEIGHT(rSpyGlass),
                  hScreenDC,
                  rMagGlass.left,
                  rMagGlass.top,
                  WIDTH(rMagGlass),
                  HEIGHT(rMagGlass),
                  SRCCOPY
               );
            else
               BitBlt(
                  hScreenDC,
                  rSpyGlass.left,
                  rSpyGlass.top,
                  WIDTH(rSpyGlass),
                  HEIGHT(rSpyGlass),
                  hScreenDC,
                  rMagGlass.left,
                  rMagGlass.top,
                  SRCCOPY
               );

         }

         /* restore magnifying glass if intersection */
         if ( bIntersect )
            Rectangle(
               hScreenDC,
               rMagGlass.left - 1,
               rMagGlass.top - 1,
               rMagGlass.right + 1,
               rMagGlass.bottom  + 1
            );

      }

      break;
   case WM_LBUTTONUP : /* mouse button up */

      /* stop tracking if capture is on */
      if ( bCapture ) {

         HDC         hMemoryDC;
         HBITMAP     hOldBitmap;
         HBITMAP     hMemoryBitmap;
      
         /* turn off capture & restore cursor */
         ReleaseCapture();
         SetCursor( hCursor );

         /* erase magnifying glass */
         Rectangle(
            hScreenDC,
            rMagGlass.left - 1,
            rMagGlass.top - 1,
            rMagGlass.right + 1,
            rMagGlass.bottom + 1
         );

         /* copy viewport image to clipboard */
         hMemoryDC = CreateCompatibleDC( hScreenDC );
         if ( hMemoryDC ) {

            hMemoryBitmap = CreateCompatibleBitmap(
                                 hScreenDC,
                                 WIDTH(rSpyGlass), 
                                 HEIGHT(rSpyGlass)
                              );

            if ( hMemoryBitmap ) {
               
               hOldBitmap = SelectObject( hMemoryDC, hMemoryBitmap );
               BitBlt(
                  hMemoryDC,
                  0,
                  0,
                  WIDTH(rSpyGlass),
                  HEIGHT(rSpyGlass),
                  hScreenDC,
                  rSpyGlass.left,
                  rSpyGlass.top,
                  SRCCOPY
               );
               
               SelectObject( hMemoryDC, hOldBitmap );
               
               if ( OpenClipboard(hWnd) ) {
                  EmptyClipboard();
                  SetClipboardData( CF_BITMAP, hMemoryBitmap );
                  CloseClipboard();
               } else {
                  DeleteObject( hMemoryBitmap );
                  WARNING( hWnd, "Unable to open clipboard!" );
               }

            } else
               WARNING( hWnd, "Insufficient memory for bitmap!" );

            /* mark capture off & delete DC */
            bCapture = FALSE;
            DeleteDC( hMemoryDC );

         } else
            WARNING( hWnd, "Insufficient memory to copy bitmap!" );

         /* delete screen display context */
         DeleteDC( hScreenDC );

      } 
      
      break;
   case WM_PAINT : /* paint the display window */

      {
         PAINTSTRUCT    Ps;
         BITMAP         Bm;
         HBITMAP        hBitmap;
         HDC            hMemoryDC;
         HDC            hDisplayDC;

         hDisplayDC = BeginPaint( hWnd, &Ps );
         if ( OpenClipboard(hWnd) ) {
            if ( hBitmap = GetClipboardData(CF_BITMAP) ) {
               hMemoryDC = CreateCompatibleDC( hDisplayDC );
               SelectObject( hMemoryDC, hBitmap );
               GetObject( hBitmap, sizeof(BITMAP), (LPSTR)&Bm );
               BitBlt(
                  hDisplayDC,
                  0,
                  0,
                  Bm.bmWidth,
                  Bm.bmHeight,
                  hMemoryDC,
                  0,
                  0,
                  SRCCOPY
               );
               DeleteDC( hMemoryDC );
            }
            CloseClipboard();
         }
         EndPaint( hWnd, &Ps );
      }
      
      break;
   case WM_DRAWCLIPBOARD : /* clipboard contents changing */
      
      /* pass the message down the chain */
      if ( hWndNext )
         SendMessage( hWndNext, wMessage, wParam, lParam );

      /* force repaint of display */
      if ( bCapture == FALSE )
         InvalidateRect( hWnd, NULL, TRUE );

      break;
   case WM_CHANGECBCHAIN : /* clipboard chain changing */

      /* re-link viewer chain */
      if ( wParam == hWndNext )
         hWndNext = LOWORD(lParam);
      else
         if ( hWndNext )
            SendMessage( hWndNext, wMessage, wParam, lParam );

      break;
   case WM_DESTROY : /* destroy window */
      ChangeClipboardChain( hWnd, hWndNext );
      PostQuitMessage( 0 );
      break;
   default : /* send to default */
      lResult = DefWindowProc( hWnd, wMessage, wParam, lParam );
      break;
   }
   
   /* return normal result */
   return( lResult );

}
