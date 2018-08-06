/*
**  demo.c     demonstration of VLB  (virtual list box)
**
**  Author: Robert A. Wood
**
**  Microsoft C version 5.1 / medium memory model
**  Microsoft Windows SDK version 2.1
**  Runtime: Windows 286 version 2.1
**
**    Date: 03/19/90
*/

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include "demo.h"
#include "vlb.h"
#include "lmem.h"

HWND hWnd;
HANDLE hInst;
char szAppName[] = "Demo";

WORD FAR PASCAL WinMain( HANDLE hInstance, HANDLE hPrevInstance,
                         LPSTR lpCmdLine, WORD nCmdShow )
{
   MSG msg;

   if( ! hPrevInstance )
      MainInit( hInstance );

   hWnd = CreateWindow( szAppName,          // window class
               szAppName,                   // window name
               WS_OVERLAPPEDWINDOW,         // window style
               0,                           // x position
               0,                           // y position
               CW_USEDEFAULT,               // width
               0,                           // height
               NULL,                        // parent handle
               NULL,                        // menu or child ID
               hInstance,                   // instance
               NULL);                       // additional info

   hInst = hInstance;

   ShowWindow( hWnd, nCmdShow );
   UpdateWindow( hWnd );

   while( GetMessage( &msg, NULL, 0, 0 ) )
   {
      TranslateMessage( &msg );
      DispatchMessage( &msg );
   }
   return( msg.wParam );
}

/*
** Initializes window data and registers window class
********************************************************************/
BOOL MainInit( HANDLE hInstance )
{
   WNDCLASS Class;

   Class.hCursor       = LoadCursor(NULL,IDC_ARROW);
   Class.hIcon         = NULL;
   Class.cbClsExtra    = 0;
   Class.cbWndExtra    = 0;
   Class.lpszMenuName  = szAppName;
   Class.lpszClassName = szAppName;
   Class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);;
   Class.hInstance     = hInstance;
   Class.style         = CS_VREDRAW | CS_HREDRAW;
   Class.lpfnWndProc   = MainWndProc;
   Class.style = NULL;

  return( RegisterClass( &Class ) );
}

/*
** Main Window Procedure
********************************************************************/
LONG FAR PASCAL MainWndProc( HWND hWnd, unsigned message,
                             WORD wParam, LONG lParam )
{
   FARPROC  lpfnDlgProc;

   switch( message )
   {
      case WM_COMMAND:
         if( wParam == IDM_DIALOG )
         {
            lpfnDlgProc = MakeProcInstance( DlgProc, hInst );
            DialogBox( hInst, "DEMO", hWnd, lpfnDlgProc );
            FreeProcInstance(lpfnDlgProc);
         }
         break;

      case WM_DESTROY:
         PostQuitMessage( FALSE );
         break;

      default:
         return( DefWindowProc( hWnd, message, wParam, lParam ) );
   }
   return( FALSE );
}

/*
** Dialog Procedure
********************************************************************/
BOOL FAR PASCAL DlgProc( HWND hDlg, unsigned message,
                         WORD wParam, LONG lParam )
{
   switch ( message )
   {
      case WM_INITDIALOG:
         InitVLB( hInst, hDlg, IDD_LISTBOX, VLBfile );
         break;

      case WM_COMMAND:
         if ( wParam == IDOK )
            EndDialog( hDlg, TRUE );
         if ( wParam == IDCANCEL )
            EndDialog( hDlg, FALSE );
         if( wParam == IDD_LISTBOX )
         {
            switch( HIWORD( lParam ) )
            {
            case VLBN_DBLCLK:
            case VLBN_SELCHANGE:
               SendMessage( GetDlgItem( hDlg, IDD_TEXT ),
                  WM_SETTEXT, 0, SendMessage( LOWORD( lParam ),
                  VLB_GETSELSTR, 0, 0L ) );
               break;
            }
         }

         break;

      default:
         return( FALSE );
   }
   return( TRUE );
}

/*
** demo VLB CallBack function for ASCII an file
********************************************************************/
LONG FAR PASCAL VLBfile( WORD wListBoxId,        // control id
                         unsigned message,      // message
                         LONG FAR *lplStringID, // string ID
                         LPSTR lpszString )     // string
{
   static int hFile = 0;                 // file handle
   static LONG TotalStrings = 0;         // total records in file
   static Tabs[] = { 0, 9, 15, 23, 33 }; // tabs for pageleft & right
   static int TotalTabs = 5;             // total tabs
   static int Column = 0;                // 1st col of display string
   static int StrLength = 40;            // total string length
   static int RecLength = 42;            // total record len (CR/LF)
   static char filename[] = "vlb.txt";   // VLB filename
   char buf[60];                         // temp buffer to read to

   switch( message )
   {
      case VCB_LENGTH:
         return( TotalStrings );

      case VCB_WIDTH:
         return( Tabs[ TotalTabs-1 ] );

      case VCB_CREATE:
      {
         long length;
         if( ( hFile = open( filename, O_RDONLY ) ) == NULL )
            return( FALSE );

         length = lseek( hFile, 0L, SEEK_END );
         lseek( hFile, 0L, SEEK_SET );
         TotalStrings = length / RecLength;
         Column = 0;                // 1st column of displayed string
         return( TRUE );
      }

      case VCB_SETFOCUS:
      case VCB_KILLFOCUS:
         return( TRUE );

      case VCB_DESTROY:
         close( hFile );
         return( TRUE );

      case VCB_NEXT:
         if( *lplStringID >= TotalStrings - 1 )
            return( FALSE );
         (*lplStringID)++;
         break;

      case VCB_PREV:
         if( *lplStringID <= 0 )
            return( FALSE );
         (*lplStringID)--;
         break;

      case VCB_FIRST:
         *lplStringID = 0;
         break;

      case VCB_LAST:
         *lplStringID = TotalStrings-1;
         break;

      case VCB_VTHUMB:
      case VCB_FULLSTRING:
      case VCB_STRING:
         *lplStringID = (LONG)min( (int)(TotalStrings-1),
                              max( 0, (int)*lplStringID ) );
         break;

      case VCB_LEFT:          // move string start one char left
         return( Column = max( 0, Column - 1 ) );

      case VCB_RIGHT:         // move string start one char right
         return( Column = min( Tabs[ TotalTabs-1 ], Column + 1 ) );

      case VCB_PAGELEFT:      // move string start one column left
      {
         int x;
         if( Column <= 0 )
            return( Column = Tabs[0] );

         for( x = 0; Column > Tabs[x]; x++ )
            ;
         Column = Tabs[x-1];
         return( Column = max( 0, Column ) );
      }

      case VCB_PAGERIGHT:     // move string start one column right
      {
         int x;
         if( Column >= Tabs[ TotalTabs - 1 ] )
            return( Column = Tabs[ TotalTabs - 1 ] );

         for( x = 0; Column >= Tabs[x]; x++ )
            ;
         Column = Tabs[x];
         return( Column = min( Tabs[ TotalTabs - 1 ], Column ) );
      }

      case VCB_HTHUMB:        // move string start specified by thumb
         return( Column = min( StrLength-1,
                          max( 0, (int)*lplStringID ) ) );

      default:
         return( FALSE );
   }

   if( message == VCB_NEXT   || message == VCB_PREV   ||
       message == VCB_VTHUMB || message == VCB_STRING ||
       message == VCB_FIRST  || message == VCB_LAST   ||
       message == VCB_FULLSTRING )
   {
      if( hFile < 0 )           // file open failed
         return( FALSE );

      lseek( hFile, *lplStringID * RecLength, SEEK_SET );
      read( hFile, buf, RecLength );

      if( message == VCB_FULLSTRING )
      {
         lmemmove( lpszString, buf, StrLength );
         *( lpszString + StrLength ) = 0;
      }
      else
      {
         lmemmove( lpszString, buf + Column, StrLength - Column );
         *( lpszString + StrLength - Column ) = 0;
      }
   }
   return( TRUE );
}

//*** END OF DEMO.C *************************************************
