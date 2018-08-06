/*------------------------------------------------------------
   WELCOME2.C -- A Program that Creates Two Top-Level Windows
  ------------------------------------------------------------*/

#define INCL_WIN
#include <os2.h>

MRESULT EXPENTRY Client1WndProc (HWND, USHORT, MPARAM, MPARAM) ;
MRESULT EXPENTRY Client2WndProc (HWND, USHORT, MPARAM, MPARAM) ;

int main (void)
     {
     static CHAR  szClientClass1 [] = "Welcome2.1",
                  szClientClass2 [] = "Welcome2.2" ;
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU |
                                 FCF_SIZEBORDER    | FCF_MINMAX  |
                                 FCF_SHELLPOSITION | FCF_TASKLIST ;
     HAB          hab ;
     HMQ          hmq ;
     HWND         hwndFrame1, hwndFrame2, hwndClient1, hwndClient2 ;
     QMSG         qmsg ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (
                    hab,                // Anchor block handle
                    szClientClass1,     // Name of class being registered
                    Client1WndProc,     // Window procedure for class
                    CS_SIZEREDRAW,      // Class style
                    0) ;                // Extra bytes to reserve

     WinRegisterClass (
                    hab,                // Anchor block handle
                    szClientClass2,     // Name of class being registered
                    Client2WndProc,     // Window procedure for class
                    CS_SIZEREDRAW,      // Class style
                    0) ;                // Extra bytes to reserve

     hwndFrame1 = WinCreateStdWindow (
                    HWND_DESKTOP,       // Parent window handle
                    WS_VISIBLE,         // Style of frame window
                    &flFrameFlags,      // Pointer to control data
                    szClientClass1,     // Client window class name
                    NULL,               // Title bar text
                    0L,                 // Style of client window
                    NULL,               // Module handle for resources
                    0,                  // ID of resources
                    &hwndClient1) ;     // Pointer to client window handle

     hwndFrame2 = WinCreateStdWindow (
                    HWND_DESKTOP,       // Parent window handle
                    WS_VISIBLE,         // Style of frame window
                    &flFrameFlags,      // Pointer to control data
                    szClientClass2,     // Client window class name
                    " - Window No. 2",  // Title bar text
                    0L,                 // Style of client window
                    NULL,               // Module handle for resources
                    0,                  // ID of resources
                    &hwndClient2) ;     // Pointer to client window handle

     WinSendMsg (hwndFrame1, WM_SETICON,
                 WinQuerySysPointer (HWND_DESKTOP, SPTR_APPICON, FALSE),
                 NULL) ;

     WinSendMsg (hwndFrame2, WM_SETICON,
                 WinQuerySysPointer (HWND_DESKTOP, SPTR_APPICON, FALSE),
                 NULL) ;

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndFrame1) ;
     WinDestroyWindow (hwndFrame2) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

MRESULT EXPENTRY Client1WndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static CHAR szText [] = "Welcome to Window No. 1" ;
     HPS         hps ;
     RECTL       rcl ;

     switch (msg)
          {
          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;

               WinQueryWindowRect (hwnd, &rcl) ;

               WinDrawText (hps, -1, szText, &rcl, CLR_NEUTRAL, CLR_BACKGROUND,
                            DT_CENTER | DT_VCENTER | DT_ERASERECT) ;

               WinEndPaint (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }

MRESULT EXPENTRY Client2WndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static CHAR szText [] = "Welcome to Window No. 2" ;
     HPS         hps ;
     RECTL       rcl ;

     switch (msg)
          {
          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;

               WinQueryWindowRect (hwnd, &rcl) ;

               WinDrawText (hps, -1, szText, &rcl, CLR_NEUTRAL, CLR_BACKGROUND,
                            DT_CENTER | DT_VCENTER | DT_ERASERECT) ;

               WinEndPaint (hps) ;
               return 0 ;

          case WM_CLOSE:
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
