/*
Function(s) demonstrated in this program: SetTextJustification
Compiler version: 5.1
Description: This program removes leading spaces from the given string,
             counts the number of spaces left, removes and subtracts the
             trailing spaces, and then justifies the remaining string to
             zero and eighty.
*/

#include <windows.h>

long FAR PASCAL WndProc ( HWND, unsigned, WORD, LONG );
extern LPSTR FAR PASCAL lstrcpy ( LPSTR, LPSTR );
extern int   FAR PASCAL lstrlen ( LPSTR );
void Justify ( HDC );

static char szAppName [] = "STJUST" ;
static char szFuncName [] = "SetTextJustification" ;

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE      hInstance;
HANDLE      hPrevInstance;
LPSTR       lpszCmdLine;
int         nCmdShow;
{
     HWND        hWnd;
     WNDCLASS    wndclass;
     MSG         msg;

     if (!hPrevInstance) {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
          wndclass.lpfnWndProc   = WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = NULL ;
          wndclass.lpszClassName = szAppName ;

          if (!RegisterClass (&wndclass))
               return FALSE ;
          }

     hWnd = CreateWindow (szAppName,            /* window class name       */
                    szFuncName,                 /* window caption          */
                    WS_OVERLAPPEDWINDOW,        /* window style            */
                    CW_USEDEFAULT,              /* initial x position      */
                    0,                          /* initial y position      */
                    CW_USEDEFAULT,              /* initial x size          */
                    0,                          /* initial y size          */
                    NULL,                       /* parent window handle    */
                    NULL,                       /* window menu handle      */
                    hInstance,                  /* program instance handle */
                    NULL) ;                     /* create parameters       */

     ShowWindow (hWnd, nCmdShow) ;
     UpdateWindow (hWnd) ;

     while (GetMessage(&msg, NULL, 0, 0))
     {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
     } 
     return (msg.wParam) ;     
}

long FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd;
unsigned iMessage;
WORD     wParam;
LONG     lParam;
{
   HDC         hDC;
   PAINTSTRUCT ps;

   switch(iMessage)
      {
      case WM_PAINT:
         hDC = BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);
         Justify ( hDC );
         EndPaint(hWnd, (LPPAINTSTRUCT)&ps);
         break;
      case WM_DESTROY:
         PostQuitMessage(0);
         break;
      default:
         return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
      }
   return (0L); 
}

void Justify ( hDC )
HDC   hDC;
{
   TEXTMETRIC  tmText;
   DWORD       dwExtent;
   char        szJustifyString [80];
   int         nBreakCount;
   int         nElement;
   int         nIndex;

   lstrcpy ( (LPSTR)szJustifyString,
      (LPSTR)"    This string is justified between 0 and 80.   \0" );

   GetTextMetrics ( hDC, (LPTEXTMETRIC)&tmText );

   nBreakCount = 0;
   nElement = 0;

                                                 /* Removing leading spaces */
   while ( szJustifyString [ nElement ]  == ' ' &&
      szJustifyString [ nElement ] != '\0' )
      {
      for ( nIndex = 0; nIndex < lstrlen ( (LPSTR)szJustifyString );
       nIndex++ )
         szJustifyString [ nIndex ] = szJustifyString [ nIndex + 1 ];
      }

                                               /* Counting remaining spaces */
   do
      {
      while ( szJustifyString [ nElement ] != '\0' &&
         szJustifyString [ nElement++ ] != ' ');
      if ( szJustifyString [ nElement ] == '\0' )
         break;

      nBreakCount++;
                                          /* Clearing error out of dwExtent */
      SetTextJustification ( hDC, 0, 0 );
      dwExtent = GetTextExtent ( hDC, (LPSTR)szJustifyString,
         lstrlen ( (LPSTR)szJustifyString ) - 1 );
      }
   while ( LOWORD ( dwExtent ) < tmText.tmAveCharWidth * 80 );

                                                /* Removing trailing spaces */
   nBreakCount--;
   nElement = lstrlen ( (LPSTR)szJustifyString );
   while ( szJustifyString [ nElement ]  == ' ' && nElement != 0 )
      {
      nElement--;
      nBreakCount--;
      }
                                          /* Clearing error out of dwExtent */
   SetTextJustification ( hDC, 0, 0 );    
                                        /* Justifing string szJustifyString */
   dwExtent = GetTextExtent ( hDC, (LPSTR)szJustifyString,
      lstrlen ( (LPSTR)szJustifyString ) - 1 );
   SetTextJustification ( hDC, tmText.tmAveCharWidth * 80 -
      (int)LOWORD ( dwExtent ), nBreakCount );

   TextOut ( hDC, 0 , 0 + (int)HIWORD ( dwExtent ), (LPSTR)szJustifyString,
      lstrlen ( (LPSTR)szJustifyString ) );
}
