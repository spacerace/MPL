/*
 *  CopyMetaFile
 *  This program demonstrates the use of the function CopyMetaFile.
 *  This function copied the source metafile to the file named by second
 *  parameter and returns a handle to the new metafile.
 */

#include <windows.h>

static HANDLE hWnd;

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE  hInstance, hPrevInstance;
LPSTR   lpszCmdLine;
int	cmdShow;
  {
  static char	szFuncName[] = "CopyMetaFile";
  HANDLE hMF;
  HANDLE hMF2;

  hMF = GetMetaFile ( (LPSTR) "CPMETAFI.MET");

  MessageBox (NULL, (LPSTR)"Coping metafile", (LPSTR) szFuncName, MB_OK);

  hMF2 = CopyMetaFile (hMF, (LPSTR) "METAFILE");

  if (hMF2 != NULL)
    MessageBox (NULL, (LPSTR)"Copied", (LPSTR) szFuncName, MB_OK);
  else
    MessageBox (NULL, (LPSTR)"Not copied", (LPSTR) szFuncName, MB_OK);

  return 0;
  }
