/*

Function(s) demonstrated in this program: Int21Function4B, LockSegment, 
   UnlockSegment

Windows version:  2.03

Windows SDK version:  2.00

Compiler version:  C 5.10

Description:  Int21Function4B is a Function created using Masm, and it 
   corresponds with the MS-DOS interrupt 21 function 4BH.  The function
   LockSegment locks the segment whose segment address is specified
	by the parameter.  If the parameter is -1 the function locks the 
   current data segment.  The function UnlockSegment unlocks the segment
   whose address is specified by the parameter, -1 indicates current
	segment.

Additional Comments:  Int21Function4B can be used to spawn windows
   applications, as well as old MS-DOS applications from within a 
   windows application.  A method of sending command line arguments
   to the spawnded windows application is also discussed.

*/

#define NOMINMAX
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "WinSpawn.h"

HWND     hWndMain;
HANDLE   hInstMain;
char     szOutputBuffer1 [70];
char     szOutputBuffer2 [500];
char     szSpawnAppName  [40];
char     szCommandLine   [40];

typedef struct {WORD   environment;
                LPSTR  commandline;
                LPSTR  FCB1;
                LPSTR  FCB2;
               } EXECBLOCK;
EXECBLOCK      exec;
WORD           w[2];


/****************************************************************************/
/************************    Message Structure      *************************/
/****************************************************************************/

struct { char *szMessage; }
       Messages [] = {
"About",
"     This is a sample application to demonstrate the\n\
use of the LockSegment and UnlockSegment Windows\n\
Functions, as well as Interrupt 21 function 4B in\n\
spawning Windows applications, and MS-DOS applications\n\
from a Windows Windows application.",

"Help Message",
"     This program demonstrates the use of the\n\
Windows Function LockSegment, UnlockSegment, and\n\
Int21Function4B.  Use the menu to select either a\n\
MS-DOS application or a Windows application to be\n\
spawned.  The program will also send command line\n\
arguments to the spawned application.",

"LockSegment",
"    The current data segment has been locked.",

"UnlockSegment",
"    The current data segment has been unlocked.",

};	

/****************************************************************************/

void ProcessMessage (HWND, int); 

void ProcessMessage (hWnd, MessageNumber) 
     HWND     hWnd;
     int      MessageNumber;
{
     sprintf (szOutputBuffer1, "%s", Messages [MessageNumber]);
     sprintf (szOutputBuffer2, "%s", Messages [MessageNumber + 1]);
     MessageBox (hWnd, szOutputBuffer2, szOutputBuffer1, MB_OK);
}       

/****************************************************************************/

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
     HANDLE      hInstance, hPrevInstance ;
     LPSTR       lpszCmdLine ;
     int         nCmdShow ;
     {
     static char szAppName [] = "WinSpawn" ;
     static char szChildClass [] = "WinSpawnChild" ;
     HWND        hWnd ;
     WNDCLASS    wndclass ;
     MSG msg;

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
          wndclass.lpszMenuName  = szAppName ;
          wndclass.lpszClassName = szAppName ;

          if (!RegisterClass (&wndclass))
               return FALSE ;

          }

     hWnd = CreateWindow (szAppName,            /* window class name       */
                    "Windows Spawn",            /* window caption          */
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

     hWndMain  = hWnd;
     hInstMain = hInstance;

     while (GetMessage(&msg, NULL, 0, 0))
     {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
     } 
     return (msg.wParam) ;     
     }

/****************************************************************************/

long FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd ;
unsigned iMessage ;
WORD     wParam ;
LONG     lParam ;
{
 HMENU       hMenu;
 PAINTSTRUCT ps;

 switch(iMessage)
 {
  case WM_CREATE:
       hMenu = GetSystemMenu (hWnd, FALSE);

       ChangeMenu (hMenu, NULL, "&About", IDM_ABOUT, 
                   MF_APPEND | MF_STRING);
       break;

  case WM_SYSCOMMAND:
       switch (wParam) {
          case IDM_ABOUT:
               ProcessMessage (hWnd, 0);
               break;
          default:
               return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
       }
       break;

  case WM_COMMAND:
       switch (wParam) {

          case IDM_SPAWNIT:
               GlobalCompact(-1L);
               LockSegment (-1);
               ProcessMessage (hWnd, 4);

               exec.environment = 0;
               exec.commandline = szCommandLine;

               w[0] = 2;
               w[1] = SW_SHOWNORMAL;
               exec.FCB1 = (LPSTR)w;
               exec.FCB2 = (LPSTR)NULL;

               Int21Function4B(0, (LPSTR)szSpawnAppName,
                               (LPSTR)&exec);

               UnlockSegment (-1);
               ProcessMessage (hWnd, 6);
               break;

          case IDM_CLOCK:
               sprintf (szSpawnAppName,"CLOCK.EXE");
               sprintf (szCommandLine, "\0\r");
               SendMessage (hWnd, WM_COMMAND, IDM_SPAWNIT, 0L);
               break;

          case IDM_DOSDIRSORT:			
               sprintf (szSpawnAppName,"COMMAND.COM");
               sprintf (szCommandLine, "%c /c dir | sort\0\r", 14);
               SendMessage (hWnd, WM_COMMAND, IDM_SPAWNIT, 0L);
               break;

          case IDM_DOSTYPE:			
               sprintf (szSpawnAppName,"COMMAND.COM");
               sprintf (szCommandLine, "%c /c type winspawn.c\0\r", 0x13);
               SendMessage (hWnd, WM_COMMAND, IDM_SPAWNIT, 0L);
               break;

          case IDM_NOTEPAD:
               sprintf (szSpawnAppName,"NOTEPAD.EXE");
               sprintf (szCommandLine, " WINSPAWN.C\0\r");
               SendMessage (hWnd, WM_COMMAND, IDM_SPAWNIT, 0L);
               break;


          case IDM_HELP:
               ProcessMessage (hWnd, 2);
               break;
       }
       break;

  case WM_PAINT:
       BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);
       EndPaint(hWnd, (LPPAINTSTRUCT)&ps);
       break;

  case WM_DESTROY:
       PostQuitMessage(0);
       break;

  default:
  {
   return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
  }
 }
 return (0L); 
}
