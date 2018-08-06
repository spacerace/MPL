/*
 *
 * GetTempDrive
 *
 *  This program demonstrates the use of the GetTempDrive function
 *  by returning the drive letter of the drive which can provide the
 *  best access time during disk operations involving a temporary file.
 *
 */

#include "windows.h"

/* WINMAIN */
int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
   char chBestDrive;                /* Drive Letter of the best           */
                                    /*  performance drive in system.      */
   char szbuf[80];                  /* Output buffer.                     */

   MessageBox(NULL,
	      (LPSTR)"Hit OK to return the highest \
performance drive in the system \
for temp files.",
              (LPSTR)"Drive Test",
              MB_OK);

   chBestDrive = GetTempDrive('A'); /* Get the temp drive.                */

   sprintf(szbuf, "%s%c%s\0",       /* Set up report in output buffer.    */
           "Drive ", chBestDrive,
	   ": is the best performance drive in the system \
for temporary files.");

   MessageBox(NULL,                 /* Show the user which drive is best. */
              szbuf,
              (LPSTR)"GetTempDrive",
              MB_OK);

} /* WinMain */
