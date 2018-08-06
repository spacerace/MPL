/*-------------------------------------------------------------
   WELCOME4.C -- Creates a Top-Level Window and Three Children
  -------------------------------------------------------------*/

#define INCL_WIN
#include <os2.h>

#define ID_BUTTON 1
#define ID_SCROLL 2
#define ID_ENTRY  3

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

int main (void)
     {
     static CHAR  szClientClass [] = "Welcome4" ;
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU   |
                                 FCF_BORDER        | FCF_MINBUTTON |
                                 FCF_SHELLPOSITION | FCF_TASKLIST ;
     HAB          hab ;
     HMQ          hmq ;
     HWND         hwndFrame,  hwndClient ;
     QMSG         qmsg ;
     RECTL        rcl ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (
                    hab,                // Anchor block handle
                    szClientClass,      // Name of class being registered
                    ClientWndProc,      // Window procedure for class
                    CS_SIZEREDRAW,      // Class style
                    0) ;                // Extra bytes to reserve

     hwndFrame = WinCreateStdWindow (
                    HWND_DESKTOP,       // Parent window handle
                    WS_VISIBLE,         // Style of frame window
                    &flFrameFlags,      // Pointer to control data
                    szClientClass,      // Client window class name
                    NULL,               // Title bar text
                    0L,                 // Style of client window
                    NULL,               // Module handle for resources
                    0,                  // ID of resources
                    &hwndClient) ;      // Pointer to client window handle

     WinSendMsg (hwndFrame, WM_SETICON,
                      WinQuerySysPointer (HWND_DESKTOP, SPTR_APPICON, FALSE),
                      NULL) ;

          /*--------------------------------------------------------
             Find dimensions of client window for sizes of children
            --------------------------------------------------------*/

     WinQueryWindowRect (hwndClient, &rcl) ;
     rcl.xRight /= 3 ;                            // divide width in thirds

          /*---------------------------
             Create push button window
            ---------------------------*/

     WinCreateWindow (
                    hwndClient,                   // Parent window handle
                    WC_BUTTON,                    // Window class
                    "Big Button",                 // Window text
                    WS_VISIBLE                    // Window style
                         | BS_PUSHBUTTON,
                    10,                           // Window position
                    10,
                    (SHORT) rcl.xRight - 20,      // Window size
                    (SHORT) rcl.yTop - 20,
                    hwndClient,                   // Owner window handle
                    HWND_BOTTOM,                  // Placement window handle
                    ID_BUTTON,                    // Child window ID
                    NULL,                         // Control data
                    NULL) ;                       // Presentation parameters

          /*--------------------------
             Create scroll bar window
            --------------------------*/

     WinCreateWindow (
                    hwndClient,                   // Parent window handle
                    WC_SCROLLBAR,                 // Window class
                    NULL,                         // Window text
                    WS_VISIBLE                    // Window style
                         | SBS_VERT,
                    (SHORT) rcl.xRight + 10,      // Window position
                    10,
                    (SHORT) rcl.xRight - 20,      // Window size
                    (SHORT) rcl.yTop - 20,
                    hwndClient,                   // Owner window handle
                    HWND_BOTTOM,                  // Placement window handle
                    ID_SCROLL,                    // Child window ID
                    NULL,                         // Control data
                    NULL) ;                       // Presentation parameters

          /*---------------------------
             Create entry field window
            ---------------------------*/

     WinCreateWindow (
                    hwndClient,                   // Parent window handle
                    WC_ENTRYFIELD,                // Window class
                    NULL,                         // Window text
                    WS_VISIBLE                    // Window style
                         | ES_MARGIN
                         | ES_AUTOSCROLL,
                    2 * (SHORT) rcl.xRight + 10,  // Window position
                    10,
                    (SHORT) rcl.xRight - 20,      // Window size
                    (SHORT) rcl.yTop - 20,
                    hwndClient,                   // Owner window handle
                    HWND_BOTTOM,                  // Placement window handle
                    ID_ENTRY,                     // Child window ID
                    NULL,                         // Control data
                    NULL) ;                       // Presentation parameters

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     switch (msg)
          {
          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case ID_BUTTON:
                         WinAlarm (HWND_DESKTOP, WA_NOTE) ;
                         return 0 ;
                    }
               break ;

          case WM_ERASEBACKGROUND:
               return 1 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
