#include <windows.h>
#include <stdio.h>

#define BUFSIZE 80 /* Output Buffer Size */

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    nCmdShow;
{
   char szOutBuf[BUFSIZE];              /* Output Buffer                  */
   int iReturn;                         /* WIN.INI DoubleClickSpeed Value */

   iReturn = GetProfileInt((LPSTR)"windows",          /* Application Name */
                           (LPSTR)"DoubleClickSpeed", /* Key Name         */
                           (int)-999);                /* Default Value    */
   if (iReturn == -999) {  /* If the default was returned, say so */
      sprintf(szOutBuf, "%s%d",
              "The Default Value was Returned: ",
              iReturn);
      MessageBox(GetFocus(), (LPSTR)szOutBuf,
                 (LPSTR)"GetProfileInt() - Default!",
                 MB_OK | MB_ICONEXCLAMATION);
      }
   else {  /* Otherwise, display the speed in a message box */
      sprintf(szOutBuf, "%s%d",
              "The DoubleClickSpeed Returned is: ",
              iReturn);
      MessageBox(GetFocus(), (LPSTR)szOutBuf,
                 (LPSTR)"GetProfileInt()", MB_OK);
      }
   return FALSE;
}
