/*
 *
 *  GetCodeHandle
 *  
 *  This program demonstrates the use of the function GetCodeHandle.
 *  This function determines which code segment contains the funtion
 *  pointed to by the parameter.
 *  
 *  Windows Version 2.0 function demonstration application
 *
 */

#include <windows.h>
char szBuffer [70];

void FAR PASCAL GetMe ()
{
   int nTest = 0;

   nTest++;
}

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
  HANDLE hCode;
  
  MessageBox (NULL, (LPSTR)"Finding code segment of GetMe function",
     (LPSTR)"GetCodeHandle", MB_OK);

  hCode = GetCodeHandle ( (FARPROC) GetMe );
  sprintf (szBuffer, "The handle to GetMe's code segment is %x", hCode);
  MessageBox (NULL, (LPSTR)szBuffer, (LPSTR)"GetCodeHandle",
              MB_OK);
  return 0;

}

