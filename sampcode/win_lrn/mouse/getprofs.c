/*
 * This program demonstrates the GetProfileString() function. It attempts
 * to obtain the string corresponding to the following:
 *
 *   [windows]
 *   spooler=<string>
 *
 */

#include <windows.h>
#include <stdio.h>

#define BUFSIZE 10

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    nCmdShow;
{
   int iCopied;             /* Number of Characters Copied from WIN.INI */
   char szOutBuf[BUFSIZE];  /* Output Buffer to Receive WIN.INI info.   */

   iCopied = GetProfileString((LPSTR)"windows",     /* Application Name */
                              (LPSTR)"spooler",     /* Key Name         */
                              (LPSTR)"Default!",    /* Default String   */
                              (LPSTR)szOutBuf,      /* Output Buffer    */
                              (int)BUFSIZE);        /* Number of Chars. */
   if (iCopied == 0) /* If No Chars. Copied, Issue Error. */
      MessageBox(GetFocus(), (LPSTR)"Error Getting Profile String!",
                 (LPSTR)"GetProfileString() Error!",
                 MB_OK | MB_ICONEXCLAMATION);
   else /* Otherwise, print out the results. */
      MessageBox(GetFocus(), (LPSTR)szOutBuf,
                 (LPSTR)"GetProfileString() - spooler =",
                 MB_OK);
   return FALSE;
}
