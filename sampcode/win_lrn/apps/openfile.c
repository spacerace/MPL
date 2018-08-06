/*   This program demonstrates the use of the function OpenFile
 *   This function creates, opens, reopens or deletes a file.  In
 *   this program, "OpenFile.txt" will be opened, and the contents
 *   displayed in a MessageBox.
 */

#include "string.h"
#include <windows.h>

#define Buf_Size 12                /* Buffer size to read with */
#define Name_Size 13               /* Max length for name of external file */

int     read (int, char *, int);
int     close (int);

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int       cmdShow;
  {
  HANDLE hMem;

  OFSTRUCT ReOpenBuff;
  int   errchk;
  int   hFile;
  char  *szBuf;

/***************************************************************************/

  MessageBox (NULL, (LPSTR)"About to open and read OPENFILE.TXT.",
      (LPSTR)"Ready", MB_OK);

  hFile = OpenFile ( (LPSTR)"openfile.txt",   /* Open file.          */
  (LPOFSTRUCT) & ReOpenBuff, OF_READ);
  if (hFile == -1)
    {                                      /* If not successful, say so.  */
    MessageBox (NULL,
        (LPSTR) "Problem opening file - OpenFile.txt",
        (LPSTR) "Error",
        MB_OK);
    }
  else
    {
    hMem = LocalAlloc (LMEM_FIXED | LMEM_ZEROINIT, Buf_Size);
    szBuf = LocalLock (hMem);
    errchk = read (hFile, szBuf, Buf_Size - 1);
    szBuf[Buf_Size - 1] = '\0';

    if (errchk == -1)
      {                     /* If problem while reading,   */
                            /* say so.                     */
      MessageBox (NULL, (LPSTR) "Problem reading file.",
          (LPSTR) "ERROR", MB_OK);
      }
    errchk = close (hFile);
    if (errchk != -1)
      {                             /* If no error, print message box */
                                    /* with file contents.            */
      MessageBox (NULL, (LPSTR) szBuf, (LPSTR) "Success", MB_OK);
      }
    else
      {                             /* Error closing file.           */
      MessageBox (NULL, (LPSTR) "File not closed", (LPSTR) "ERROR", MB_OK);
      }
    }
  LocalUnlock (hMem);
  LocalFree (hMem);
  return TRUE;
  }
