/*----------------------------------------------------------
   WELC.C -- A Program that Creates a Standard Frame Window
  ----------------------------------------------------------*/

#include <os2.h>

int main (void)
     {
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU |
                                 FCF_SIZEBORDER    | FCF_MINMAX  |
                                 FCF_SHELLPOSITION | FCF_TASKLIST ;
     HAB          hab ;
     HMQ          hmq ;
     HWND         hwndFrame ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     hwndFrame = WinCreateStdWindow (
                    HWND_DESKTOP,       // Parent window handle
                    WS_VISIBLE,         // Style of frame window
                    &flFrameFlags,      // Pointer to control data
                    NULL,               // Client window class name
                    NULL,               // Title bar text
                    0L,                 // Style of client window
                    NULL,               // Module handle for resources
                    0,                  // ID of resources
                    NULL) ;             // Pointer to client window handle

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }
