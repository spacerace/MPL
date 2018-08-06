/*
 *  This program demonstrates the use of the function GetMetaFileBits.
 *  This function returns a handle to a global memory block containing the
 *  specified metafile as a collection of bits.
 */

#include <windows.h>

static HANDLE hWnd;
HANDLE hMem;

int PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
  static char szFuncName[] = "GetMetaFileBits()";
  char szOutBuf[40];
  HANDLE hMF;
  HANDLE hMetaDC; 
  
  MessageBox(GetFocus(), (LPSTR)"Creating Memory Metafile",
             (LPSTR)"CretaeMetaFile()", MB_OK);
  hMetaDC = CreateMetaFile(NULL);
  if (hMetaDC == NULL)
     MessageBox(GetFocus(), (LPSTR)"Error in Creating Memory Metafile",
                (LPSTR)"CreateMetaFile() Error!", MB_OK | MB_ICONEXCLAMATION);
  else {
     Rectangle(hMetaDC, 10, 10, 200, 200);
     Rectangle(hMetaDC, 201, 10, 401, 200);
     LineTo(hMetaDC, 100, 250);
     Ellipse(hMetaDC, 0, 0, 100, 100);
     hMF = CloseMetaFile(hMetaDC);
     MessageBox (GetFocus(),
                 (LPSTR)"Getting Handle to Metafile Bits",
                 (LPSTR)szFuncName, MB_OK);
     hMem = GetMetaFileBits(hMF);
     if (hMem == NULL)
        MessageBox (GetFocus(), (LPSTR)"Error in Obtaining Metafile Bit Handle",
                    (LPSTR)szFuncName, MB_OK);
     else {
        MessageBox (GetFocus(), (LPSTR)"Handle Obtained Successfully",
                    (LPSTR)szFuncName, MB_OK);
        if (GlobalSize(hMem) != 0) {
           sprintf(szOutBuf, "%s%lu%s", "The Size of the Metafile is: ",
                   GlobalSize(hMem), " bytes.");
           MessageBox(GetFocus(), (LPSTR)szOutBuf, (LPSTR)"GlobalSize()", MB_OK);
           }
        else
           MessageBox(GetFocus(), (LPSTR)"Error in Obtaining the Size of the Metafile",
                      (LPSTR)"GlobalSize", MB_OK);
        }
     }
  return 0;
}
