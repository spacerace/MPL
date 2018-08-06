/*
 *
 * GetTempFileName
 *
 * This program demonstrates the use of the GetTempFileName function
 *  by creating a filename called "temp####" in the directory pointed
 *  to by the TEMP environment varible.  The #### stands for a four
 *  digit number based upon the current system time (Windows generates
 *  this number).  The file is created and then closes the file.
 *
 */

#include "windows.h"

/* WINMAIN */
int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
   char szTmpFle[80];                  /* Buffer for Temp file name.      */
   char szOutBufr[80];                 /* Output Buffer.                  */
   int iFNumb;                         /* Unique number Windows provides. */

   MessageBox(NULL,                    /* Prep the user with a message.   */
              (LPSTR)"About to create Temporary File.",
              (LPSTR)"GetTempFileName Test",
              MB_OK);

    /* Get the new Temporary File Name, create, then close the file. */

   iFNumb = GetTempFileName((BYTE)'D',
                            (LPSTR)"TMP",
                            (WORD)0,
                            (LPSTR)szTmpFle);

    /* Place the unique number (last four characters of filename)    */
    /* into an output buffer for display in a message box.           */

   sprintf(szOutBufr,
           "%s%4i\0",
           "The file number is: ",
           iFNumb);

    /* Output the unique file number in a message box for clarity.   */

   MessageBox(NULL,
              (LPSTR)szOutBufr,
              (LPSTR)"Allocated File Number.",
              MB_OK);

    /* Put the complete path and file name into an output buffer.    */

   sprintf(szOutBufr, "%s%s%s\0",
           "The temporary filename is ",
           szTmpFle, ".");

    /* Output the complete file name in a message box for clarity.   */

   MessageBox(NULL, (LPSTR)szOutBufr,
              (LPSTR)"GetTempFileName",
              MB_OK);

} /* WinMain */
