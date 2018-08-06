/*
 *  SetDoubleClickTime
 *  setdblck.c
 *
 *  This program demonstrates the use of the function SetDoubleClickTime.
 *  This function changes the range of time Windows will wait to
 *  acknowledge a double-click from the mouse.
 *
 */

#include "windows.h"

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE          hInstance;
HANDLE          hPrevInstance;
LPSTR           lpszCmdLine;
int             cmdShow;
{
                static WORD     wTime;
                static WORD     NewDblCkTime = 1000;
                static char     szbuff[80];
  
        wTime = GetDoubleClickTime ();
        sprintf ( szbuff, "%s%ld", "Current Double click time in milliseconds is ", (LONG) wTime );
        MessageBox (NULL, (LPSTR)szbuff, (LPSTR)"SetDoubleClickTime", MB_OK);

        SetDoubleClickTime (NewDblCkTime);

        wTime = GetDoubleClickTime ();
        sprintf ( szbuff, "%s%ld", "New Double click time in milliseconds is ",(LONG) wTime );
        MessageBox (NULL, (LPSTR)szbuff, (LPSTR)"Program Finished", MB_OK);

        exit (0);
}
