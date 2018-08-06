/*
 * This program demonstrates the use of WriteProfileString(). In this
 * example, the string "fubar=yes" keyword and value is written to an
 * application name called "foo".  If this construct currently exists
 * in the win.ini file, it is reported to the user. Note! This program
 * makes permanent changes to the WIN.INI file, but should not affect
 * any existing applications.
 */

#include <windows.h>

#define CHARLEN  4

int PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    nCmdShow;
{
   char  szFooStr[5];
   int   iCopied;
   int   iResult;
   BOOL  bResult;

   iCopied = GetProfileString((LPSTR)"foo", (LPSTR)"fubar", (LPSTR)"ERR",
                              (LPSTR)szFooStr, CHARLEN);

   if (iCopied == NULL)
      MessageBox(GetFocus(), (LPSTR)"Error in Checking WIN.INI",
                 (LPSTR)"GetProfileString()", MB_OK);
   else if ((szFooStr[0] == 'y') &&
            (szFooStr[1] == 'e') &&
            (szFooStr[2] == 's'))
      MessageBox(GetFocus(), (LPSTR)"Profile String Currently Exists!",
                 (LPSTR)"GetProfileString()", MB_OK);

   else {
      MessageBox(GetFocus(),
                 (LPSTR)"Inserting 'fubar=yes' into [foo] section of WIN.INI.",
                 (LPSTR)"WriteProfileString()", MB_OK);
      iResult = MessageBox(GetFocus(),
                           (LPSTR)"Continuing Will Change WIN.INI Contents!",
                           (LPSTR)"WriteProfileString()",
                           MB_OKCANCEL | MB_ICONEXCLAMATION);
      if (iResult != IDCANCEL) {
         bResult = WriteProfileString((LPSTR)"foo", (LPSTR)"fubar",
                                      (LPSTR)"yes");
         if (bResult == NULL)
            MessageBox(GetFocus(), (LPSTR)"Error In Writing To WIN.INI!",
                       (LPSTR)"WriteProfileString() Error!",
                       MB_OK | MB_ICONEXCLAMATION);
         }
      }
   
   return(0);
}
