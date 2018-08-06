/*--------------------------------------------------------
   MDIDEMO.C -- Multiple Document Interface Demonstration
                (c) Charles Petzold, 1990

   Windows SDK V. 3.0
   Microsoft C V. 6.0
   Windows     V. 3.0

  --------------------------------------------------------*/

#include <windows.h>
#include <stdlib.h>
#include "mdidemo.h"

long FAR PASCAL FrameWndProc  (HWND, WORD, WORD, LONG) ;
BOOL FAR PASCAL CloseEnumProc (HWND, LONG) ;
long FAR PASCAL HelloWndProc  (HWND, WORD, WORD, LONG) ;
long FAR PASCAL RectWndProc   (HWND, WORD, WORD, LONG) ;

 // structure for storing data unique to each Hello child window

typedef struct
     {
     short    nColor ;
     COLORREF clrText ;
     }
     HELLODATA ;

typedef HELLODATA NEAR *NPHELLODATA ;

 // structure for storing data unique to each Rect child window

typedef struct
     {
     short cxClient ;
     short cyClient ;
     }
     RECTDATA ;

typedef RECTDATA NEAR *NPRECTDATA ;

 // global variables

char   szFrameClass [] = "MdiFrame" ;
char   szHelloClass [] = "MdiHelloChild" ;
char   szRectClass  [] = "MdiRectChild" ;
HANDLE hInst ;
HMENU  hMenuInit, hMenuHello, hMenuRect ;
HMENU  hMenuInitWindow, hMenuHelloWindow, hMenuRectWindow ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
   {
   HANDLE   hAccel ;
   HWND     hwndFrame, hwndClient ;
   MSG      msg ;
   WNDCLASS wndclass ;

   hInst = hInstance ;

   if (!hPrevInstance)
      {
                // Register the frame window class

      wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
      wndclass.lpfnWndProc   = FrameWndProc ;
      wndclass.cbClsExtra    = 0 ;
      wndclass.cbWndExtra    = 0 ;
      wndclass.hInstance     = hInstance ;
      wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
      wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
      wndclass.hbrBackground = COLOR_APPWORKSPACE + 1 ;
      wndclass.lpszMenuName  = NULL ;
      wndclass.lpszClassName = szFrameClass ;

      RegisterClass (&wndclass) ;

                // Register the Hello child window class

      wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
      wndclass.lpfnWndProc   = HelloWndProc ;
      wndclass.cbClsExtra    = 0 ;
      wndclass.cbWndExtra    = sizeof (LOCALHANDLE) ;
      wndclass.hInstance     = hInstance ;
      wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
      wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
      wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
      wndclass.lpszMenuName  = NULL ;
      wndclass.lpszClassName = szHelloClass ;

      RegisterClass (&wndclass) ;

                // Register the Rect child window class

      wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
      wndclass.lpfnWndProc   = RectWndProc ;
      wndclass.cbClsExtra    = 0 ;
      wndclass.cbWndExtra    = sizeof (LOCALHANDLE) ;
      wndclass.hInstance     = hInstance ;
      wndclass.hIcon         = NULL ;
      wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
      wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
      wndclass.lpszMenuName  = NULL ;
      wndclass.lpszClassName = szRectClass ;

      RegisterClass (&wndclass) ;
      }
           // Obtain handles to three possible menus & submenus

   hMenuInit  = LoadMenu (hInst, "MdiMenuInit") ;
   hMenuHello = LoadMenu (hInst, "MdiMenuHello") ;
   hMenuRect  = LoadMenu (hInst, "MdiMenuRect") ;

   hMenuInitWindow  = GetSubMenu (hMenuInit,   INIT_MENU_POS) ;
   hMenuHelloWindow = GetSubMenu (hMenuHello, HELLO_MENU_POS) ;
   hMenuRectWindow  = GetSubMenu (hMenuRect,   RECT_MENU_POS) ;

             // Load accelerator table

   hAccel = LoadAccelerators (hInst, "MdiAccel") ;

             // Create the frame window

   hwndFrame = CreateWindow (szFrameClass, "MDI Demonstration",
                             WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             NULL, hMenuInit, hInstance, NULL) ;

   hwndClient = GetWindow (hwndFrame, GW_CHILD) ;

   ShowWindow (hwndFrame, nCmdShow) ;
   UpdateWindow (hwndFrame) ;

             // Enter the modified message loop

   while (GetMessage (&msg, NULL, 0, 0))
        {
        if (!TranslateMDISysAccel (hwndClient, &msg) &&
            !TranslateAccelerator (hwndFrame, hAccel, &msg))
             {
             TranslateMessage (&msg) ;
             DispatchMessage (&msg) ;
             }
        }
   return msg.wParam ;
   }

long FAR PASCAL FrameWndProc (HWND hwnd, WORD message,
                              WORD wParam, LONG lParam)
   {
   static HWND        hwndClient ;
   CLIENTCREATESTRUCT clientcreate ;
   FARPROC            lpfnEnum ;
   HWND               hwndChild, hwndNext ;
   MDICREATESTRUCT    mdicreate ;

   switch (message)
      {
      case WM_CREATE:          // Create the client window

         clientcreate.hWindowMenu  = hMenuInitWindow ;
         clientcreate.idFirstChild = IDM_FIRSTCHILD ;

         hwndClient = CreateWindow ("MDICLIENT", NULL,
                        WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
                        0, 0, 0, 0, hwnd, 1, hInst,
                        (LPSTR) &clientcreate) ;
         return 0 ;

      case WM_COMMAND:
         switch (wParam)
           {
           case IDM_NEWHELLO:       // Create a Hello child window

              mdicreate.szClass = szHelloClass ;
              mdicreate.szTitle = "Hello" ;
              mdicreate.hOwner  = hInst ;
              mdicreate.x       = CW_USEDEFAULT ;
              mdicreate.y       = CW_USEDEFAULT ;
              mdicreate.cx      = CW_USEDEFAULT ;
              mdicreate.cy      = CW_USEDEFAULT ;
              mdicreate.style   = 0 ;
              mdicreate.lParam  = NULL ;

              hwndChild = SendMessage (hwndClient, WM_MDICREATE, 0,
                             (LONG) (LPMDICREATESTRUCT) &mdicreate) ;
              return 0 ;

           case IDM_NEWRECT:        // Create a Rect child window

              mdicreate.szClass = szRectClass ;
              mdicreate.szTitle = "Rectangles" ;
              mdicreate.hOwner  = hInst ;
              mdicreate.x       = CW_USEDEFAULT ;
              mdicreate.y       = CW_USEDEFAULT ;
              mdicreate.cx      = CW_USEDEFAULT ;
              mdicreate.cy      = CW_USEDEFAULT ;
              mdicreate.style   = 0 ;
              mdicreate.lParam  = NULL ;

              hwndChild = SendMessage (hwndClient,  WM_MDICREATE, 0,
                             (LONG) (LPMDICREATESTRUCT) &mdicreate) ;
              return 0 ;

           case IDM_CLOSE:          // Close the active window

              hwndChild = LOWORD (SendMessage (hwndClient,
                                       WM_MDIGETACTIVE, 0, 0L)) ;

              if (SendMessage (hwndChild, WM_QUERYENDSESSION, 0, 0L))
                   SendMessage (hwndClient, WM_MDIDESTROY,
                                hwndChild, 0L) ;
              return 0 ;

           case IDM_EXIT:           // Exit the program

              SendMessage (hwnd, WM_CLOSE, 0, 0L) ;
              return 0 ;

                        // Messages for arranging windows
           case IDM_TILE:
              SendMessage (hwndClient, WM_MDITILE, 0, 0L) ;
              return 0 ;

           case IDM_CASCADE:
              SendMessage (hwndClient, WM_MDICASCADE, 0, 0L) ;
              return 0 ;

           case IDM_ARRANGE:
              SendMessage (hwndClient, WM_MDIICONARRANGE, 0, 0L) ;
              return 0 ;

           case IDM_CLOSEALL:       // Attempt to close all children

              lpfnEnum = MakeProcInstance (CloseEnumProc, hInst) ;
              EnumChildWindows (hwndClient, lpfnEnum, 0L) ;
              FreeProcInstance (lpfnEnum) ;
              return 0 ;

           default:            // Pass to active child

              hwndChild = LOWORD (SendMessage (hwndClient,
                                  WM_MDIGETACTIVE, 0, 0L)) ;

             if (IsWindow (hwndChild))
                  SendMessage (hwndChild, WM_COMMAND,
                               wParam, lParam) ;

             break ;        // and then to DefFrameProc
           }
         break ;

      case WM_QUERYENDSESSION:
      case WM_CLOSE:       // Attempt to close all children

         SendMessage (hwnd, WM_COMMAND, IDM_CLOSEALL, 0L) ;

         if (NULL != GetWindow (hwndClient, GW_CHILD))
              return 0 ;

         break ;   // ie, call DefFrameProc ;

      case WM_DESTROY :
         PostQuitMessage (0) ;
         return 0 ;
      }
        // Pass unprocessed msgs to DefFrameProc (not DefWindowProc)

   return DefFrameProc (hwnd, hwndClient, message, wParam, lParam) ;
   }

BOOL FAR PASCAL CloseEnumProc (HWND hwnd, LONG lParam)
     {
     if (GetWindow (hwnd, GW_OWNER))         // check for icon title
          return 1 ;

     SendMessage (GetParent (hwnd), WM_MDIRESTORE, hwnd, 0L) ;

     if (!SendMessage (hwnd, WM_QUERYENDSESSION, 0, 0L))
          return 1 ;

     SendMessage (GetParent (hwnd), WM_MDIDESTROY, hwnd, 0L) ;
          return 1 ;
     }

long FAR PASCAL HelloWndProc (HWND hwnd, WORD message,
                            WORD wParam, LONG lParam)
   {
   static COLORREF clrTextArray [] = { RGB (  0,   0,   0),
                                       RGB (255,   0,   0),
                                       RGB (  0, 255,   0),
                                       RGB (  0,   0, 255),
                                       RGB (255, 255, 255) } ;
   static HWND     hwndClient, hwndFrame ;
   HDC             hdc ;
   HMENU           hMenu ;
   LOCALHANDLE     hHelloData ;
   NPHELLODATA     npHelloData ;
   PAINTSTRUCT     ps ;
   RECT            rect ;

   switch (message)
      {
      case WM_CREATE:
                   // Allocate memory for window private data

         hHelloData = LocalAlloc (LMEM_MOVEABLE | LMEM_ZEROINIT,
                                  sizeof (HELLODATA)) ;

         npHelloData = (NPHELLODATA) LocalLock (hHelloData) ;
         npHelloData->nColor  = IDM_BLACK ;
         npHelloData->clrText = RGB (0, 0, 0) ;
         LocalUnlock (hHelloData) ;
         SetWindowWord (hwnd, 0, hHelloData) ;

                   // Save some window handles

         hwndClient = GetParent (hwnd) ;
         hwndFrame  = GetParent (hwndClient) ;
         return 0 ;

      case WM_COMMAND:
         switch (wParam)
            {
            case IDM_BLACK:
            case IDM_RED:
            case IDM_GREEN:
            case IDM_BLUE:
            case IDM_WHITE:
                         // Change the text color

               hHelloData  = GetWindowWord (hwnd, 0) ;
               npHelloData = (NPHELLODATA) LocalLock (hHelloData) ;

               hMenu = GetMenu (hwndFrame) ;

               CheckMenuItem (hMenu, npHelloData->nColor,
                                     MF_UNCHECKED) ;
               npHelloData->nColor = wParam ;
               CheckMenuItem (hMenu, npHelloData->nColor,
                                     MF_CHECKED) ;

               npHelloData->clrText =
                     clrTextArray [wParam - IDM_BLACK] ;

               LocalUnlock (hHelloData) ;
               InvalidateRect (hwnd, NULL, FALSE) ;
            }
         return 0 ;

      case WM_PAINT:
                   // Paint the window

         hdc = BeginPaint (hwnd, &ps) ;

         hHelloData  = GetWindowWord (hwnd, 0) ;
         npHelloData = (NPHELLODATA) LocalLock (hHelloData) ;
         SetTextColor (hdc, npHelloData->clrText) ;
         LocalUnlock (hHelloData) ;

         GetClientRect (hwnd, &rect) ;

         DrawText (hdc, "Hello, World!", -1, &rect,
                   DT_SINGLELINE | DT_CENTER | DT_VCENTER) ;

         EndPaint (hwnd, &ps) ;
         return 0 ;

      case WM_MDIACTIVATE:

                   // Set the Hello menu if gaining focus

         if (wParam == TRUE)
              SendMessage (hwndClient, WM_MDISETMENU, 0,
                           MAKELONG (hMenuHello, hMenuHelloWindow)) ;

                   // check or uncheck menu item

         hHelloData  = GetWindowWord (hwnd, 0) ;
         npHelloData = (NPHELLODATA) LocalLock (hHelloData) ;
         CheckMenuItem (hMenuHello, npHelloData->nColor,
                        wParam ? MF_CHECKED : MF_UNCHECKED) ;
         LocalUnlock (hHelloData) ;

                   // Set the Init menu if losing focus

         if (wParam == FALSE)
              SendMessage (hwndClient, WM_MDISETMENU, 0,
                           MAKELONG (hMenuInit, hMenuInitWindow)) ;

         DrawMenuBar (hwndFrame) ;
         return 0 ;

      case WM_QUERYENDSESSION:
      case WM_CLOSE:
         if (IDOK != MessageBox (hwnd,"OK to close window?","Hello",
                                 MB_ICONQUESTION | MB_OKCANCEL))
              return 0 ;

         break ;   // ie, call DefMDIChildProc

      case WM_DESTROY:
         hHelloData = GetWindowWord (hwnd, 0) ;
         LocalFree (hHelloData) ;
         return 0 ;
      }
         // Pass unprocessed message to DefMDIChildProc

   return DefMDIChildProc (hwnd, message, wParam, lParam) ;
   }

long FAR PASCAL RectWndProc (HWND hwnd, WORD message,
                           WORD wParam, LONG lParam)
   {
   static HWND hwndClient, hwndFrame ;
   HPEN        hBrush ;
   HDC         hdc ;
   LOCALHANDLE hRectData ;
   NPRECTDATA  npRectData ;
   PAINTSTRUCT ps ;
   short       xLeft, xRight, yTop, yBottom, nRed, nGreen, nBlue ;

   switch (message)
        {
      case WM_CREATE:
                   // Allocate memory for window private data

         hRectData = LocalAlloc (LMEM_MOVEABLE | LMEM_ZEROINIT,
                                 sizeof (RECTDATA)) ;

         SetWindowWord (hwnd, 0, hRectData) ;

                   // Start the timer going

         SetTimer (hwnd, 1, 250, NULL) ;

                   // Save some window handles

         hwndClient = GetParent (hwnd) ;
         hwndFrame  = GetParent (hwndClient) ;
         return 0 ;

      case WM_SIZE:            // Save the window size

         hRectData  = GetWindowWord (hwnd, 0) ;
         npRectData = (NPRECTDATA) LocalLock (hRectData) ;

         npRectData->cxClient = LOWORD (lParam) ;
         npRectData->cyClient = HIWORD (lParam) ;

         LocalUnlock (hRectData) ;

         break ; //WM_SIZE must be processed by DefMDIChildProc

      case WM_TIMER:           // Display a random rectangle

         hRectData  = GetWindowWord (hwnd, 0) ;
         npRectData = (NPRECTDATA) LocalLock (hRectData) ;

         xLeft   = rand () % npRectData->cxClient ;
         xRight  = rand () % npRectData->cxClient ;
         yTop    = rand () % npRectData->cyClient ;
         yBottom = rand () % npRectData->cyClient ;
         nRed    = rand () & 255 ;
         nGreen  = rand () & 255 ;
         nBlue   = rand () & 255 ;

         hdc = GetDC (hwnd) ;
         hBrush = CreateSolidBrush (RGB (nRed, nGreen, nBlue)) ;
         SelectObject (hdc, hBrush) ;

         Rectangle (hdc, min (xLeft, xRight), min (yTop, yBottom),
                         max (xLeft, xRight), max (yTop, yBottom)) ;

         ReleaseDC (hwnd, hdc) ;
         DeleteObject (hBrush) ;
         LocalUnlock (hRectData) ;
         return 0 ;

      case WM_PAINT:           // Clear the window

         InvalidateRect (hwnd, NULL, TRUE) ;
         hdc = BeginPaint (hwnd, &ps) ;
         EndPaint (hwnd, &ps) ;
         return 0 ;

      case WM_MDIACTIVATE:     // Set the appropriate menu
         if (wParam == TRUE)
              SendMessage (hwndClient, WM_MDISETMENU, 0,
                           MAKELONG (hMenuRect, hMenuRectWindow)) ;
         else
              SendMessage (hwndClient, WM_MDISETMENU, 0,
                           MAKELONG (hMenuInit, hMenuInitWindow)) ;

         DrawMenuBar (hwndFrame) ;
         return 0 ;

      case WM_DESTROY:
         hRectData = GetWindowWord (hwnd, 0) ;
         LocalFree (hRectData) ;
         KillTimer (hwnd, 1) ;
         return 0 ;
      }
         // Pass unprocessed message to DefMDIChildProc

   return DefMDIChildProc (hwnd, message, wParam, lParam) ;
   }
