/*
 * This program demonstrates the use of SetSysColor(). In this example,
 * the background desktop color of the current windows session is changed
 * to black. This change is not permanent. If the background is already
 * set to black, no change will be visible.
 */

#include <windows.h>

#define MAXCOLORS         13

DWORD lpColorValues[MAXCOLORS];
int lpSysColor[MAXCOLORS] = { COLOR_SCROLLBAR,
                              COLOR_BACKGROUND,
                              COLOR_ACTIVECAPTION,
                              COLOR_INACTIVECAPTION,
                              COLOR_MENU,
                              COLOR_WINDOW,
                              COLOR_WINDOWFRAME,
                              COLOR_MENUTEXT,
                              COLOR_WINDOWTEXT,
                              COLOR_CAPTIONTEXT,
                              COLOR_ACTIVEBORDER,
                              COLOR_INACTIVEBORDER,
                              COLOR_APPWORKSPACE };

int PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    nCmdShow;
{
   int         i;

   MessageBox(GetFocus(), (LPSTR)"Setting Background Color to Black.",
              (LPSTR)"SetSysColors()", MB_OK);
   
   for (i = 0; i < MAXCOLORS; i++)
      lpColorValues[i] = GetSysColor(lpSysColor[i]);
   
   lpColorValues[COLOR_BACKGROUND] = RGB(0, 0, 0);
   SetSysColors(MAXCOLORS, (int far *)lpSysColor, (long far *)lpColorValues);

   MessageBox(GetFocus(), (LPSTR)"Background Set For Current Session Only.",
              (LPSTR)"SetSysColors()", MB_OK);

   return(0);

}
