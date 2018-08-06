/*
 *  GetVersion
 *  version.c
 *
 *  This function returns the current version of windows in an unsigned
 *   short integer.  The low byte corresponding to the major version
 *   number and the high byte corresponding to the minor version number.
 *  This function calls the GetVersion function and places the version
 *   number in a message box with the appropriate bytes to convey the
 *   actual current version number.
 *
 */

#include <windows.h>

int PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;

{
   unsigned short version;	   /* The Current Version Number.    */
   unsigned short majvers;	   /* The Major Version Number.      */
   unsigned short minvers;	   /* The Minor Version Number.      */
   char szbuf[14];		      /* The Version Number Buffer.     */

   version = GetVersion();	   /* Get the Current Version.	    */
   majvers = LOBYTE(version);	/* Cast Major Version.	          */
   minvers = HIBYTE(version);	/* Cast the Minor Version.	       */
   sprintf(szbuf,             /* Place the version number in    */
           "%9s%1d%1s%02d",   /*  the output buffer along with  */
           " Version ",       /*  some explatitory text.        */
           majvers,
           ".",
           minvers);

				   /* Create Version Number Message. */

   MessageBox(NULL, (LPSTR)szbuf, (LPSTR)"MS-Windows", MB_OK);

				   /* Print Out Version Message.     */

   return 0;			   /* Exit With a Successful Return. */

}
