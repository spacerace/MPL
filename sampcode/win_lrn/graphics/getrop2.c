/*

Windows Version 2.03 function demonstration application

Function(s) demonstrated in this program: GetROP2

Compiler version: 5.01


Description:

The GetROP2 function returns the current drawing mode.  The drawing mode
specifies how the pen or interior color and the color already on the
display surface are combined to yield a new color.  This program
simply calls GetROP2 and displays the current drawing mode based on
the function return value.


Additional Comments:

Note that the drawing modes define how GDI combines source and destination
colors when drawing with the current pen.

****************************************************************************/

#include <windows.h>

long FAR PASCAL WndProc (HWND, unsigned, WORD, LONG) ;

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
     HANDLE      hInstance, hPrevInstance ;
     LPSTR       lpszCmdLine ;
     int         nCmdShow ;
     {
     static char szAppName [] = "GetROP2" ;
     HWND        hWnd ;
     WNDCLASS    wndclass ;
     MSG msg;
     HDC             hDC;                 /* For the GetROP2 demonstration */
     short           iROP2;               /* For the GetROP2 demonstration */

     if (!hPrevInstance) 
          {
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
                    "GetROP2",                  /* window caption          */
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

/***************************************************************************/
/* Begin the GetROP2 demonstration */

     hDC = GetDC (hWnd);
     iROP2 = GetROP2 (hDC);                /* Get the current drawing mode */

     switch (iROP2)
        {
        case R2_BLACK:
           {
           MessageBox (hWnd,
                       (LPSTR)"The current drawing mode returned by GetROP2 \
is R2_BLACK.",
                       (LPSTR)"GetROP2",
                       MB_OK
                      );
           break;
           }
        case R2_NOTMERGEPEN:
           {
           MessageBox (hWnd,
                       (LPSTR)"The current drawing mode returned by GetROP2 \
is R2_NOTMERGEPEN.",  
                       (LPSTR)"GetROP2",
                       MB_OK
                      );
           break;
           }
        case R2_MASKNOTPEN:
           {
           MessageBox (hWnd,
                       (LPSTR)"The current drawing mode returned by GetROP2 \
is R2_MASKNOTPEN.", 
                       (LPSTR)"GetROP2",
                       MB_OK
                      );
           break;
           }
        case R2_NOTCOPYPEN:
           {
           MessageBox (hWnd,
                       (LPSTR)"The current drawing mode returned by GetROP2 \
is R2_NOTCOPYPEN.", 
                       (LPSTR)"GetROP2",
                       MB_OK
                      );
           break;
           }
        case R2_MASKPENNOT:
           {
           MessageBox (hWnd,
                       (LPSTR)"The current drawing mode returned by GetROP2 \
is R2_MASKPENNOT.",   
                       (LPSTR)"GetROP2",
                       MB_OK
                      );
           break;
           }
        case R2_NOT:
           {
           MessageBox (hWnd,
                       (LPSTR)"The current drawing mode returned by GetROP2 \
is R2_NOT .",   
                       (LPSTR)"GetROP2",
                       MB_OK
                      );
           break;
           }
        case R2_XORPEN:
           {
           MessageBox (hWnd,
                       (LPSTR)"The current drawing mode returned by GetROP2 \
is R2_XORPEN.",    
                       (LPSTR)"GetROP2",
                       MB_OK
                      );
           break;
           }
        case R2_NOTMASKPEN:
           {
           MessageBox (hWnd,
                       (LPSTR)" The current drawing mode returned by GetROP2 \
is R2_NOTMASKPEN.",    
                       (LPSTR)"GetROP2",
                       MB_OK
                      );
           break;
           }
        case R2_MASKPEN:
           {
           MessageBox (hWnd,
                       (LPSTR)"The current drawing mode returned by GetROP2 \
is R2_MASKPEN.",      
                       (LPSTR)"GetROP2",
                       MB_OK
                      );
           break;
           }
        case R2_NOTXORPEN:
           {
           MessageBox (hWnd,
                       (LPSTR)"The current drawing mode returned by GetROP2 \
is R2_NOTXORPEN.",        
                       (LPSTR)"GetROP2",
                       MB_OK
                      );
           break;
           }
        case R2_NOP:
           {
           MessageBox (hWnd,
                       (LPSTR)"The current drawing mode returned by GetROP2 \
is R2_NOP.",          
                       (LPSTR)"GetROP2",
                       MB_OK
                      );
           break;
           }
        case R2_MERGENOTPEN:
           {
           MessageBox (hWnd,
                       (LPSTR)"The current drawing mode returned by GetROP2 \
is R2_MERGENOTPEN.",          
                       (LPSTR)"GetROP2",
                       MB_OK
                      );
           break;
           }
        case R2_COPYPEN:
           {
           MessageBox (hWnd,
                       (LPSTR)"The current drawing mode returned by GetROP2 \
is R2_COPYPEN.",           
                       (LPSTR)"GetROP2",
                       MB_OK
                      );
           break;
           }
        case R2_MERGEPENNOT:
           {
           MessageBox (hWnd,
                       (LPSTR)"The current drawing mode returned by GetROP2 \
is R2_MERGEPENNOT.",
                       (LPSTR)"GetROP2",
                       MB_OK
                      );
           break;
           }
        case R2_MERGEPEN:
           {
           MessageBox (hWnd,
                       (LPSTR)"The current drawing mode returned by GetROP2 \
is R2_MERGEPEN.",  
                       (LPSTR)"GetROP2",
                       MB_OK
                      );
           break;
           }
        case R2_WHITE:
           {
           MessageBox (hWnd,
                       (LPSTR)"The current drawing mode returned by GetROP2 \
is R2_WHITE.",
                       (LPSTR)"GetROP2",
                       MB_OK
                      );
           break;
           }
        default:
           MessageBox (hWnd,
                       (LPSTR)"Something is wrong.  GetROP2 is supposed to \
return the current drawing mode but instead returns an unknown value.",
                       (LPSTR)"GetROP2",
                       NULL
                      );
           break;
        } /* end switch */

     ReleaseDC (hWnd, hDC);

/* End the GetROP2 demonstration */
/***************************************************************************/

     while (GetMessage(&msg, NULL, 0, 0))
     {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
     } 
     return (msg.wParam) ;     
     }

long FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd ;
unsigned iMessage ;
WORD     wParam ;
LONG     lParam ;
{

 switch(iMessage)
 {
  case WM_DESTROY:
  {
   PostQuitMessage(0);
   break;
  }
  default:
  {
   return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
  }
 }

 return (0L); 
}
