/*
 *
 *  GetObject
 *  getobj.c
 *  
 *  This program demonstrates the use of the function GetObject.
 *  This funtion fills a buffer with the logical data the defines the
 *  logical object specified by the first parameter.
 *   
 *  Other references: cfclip.c, cfnew.c
 *  
 */

#include "windows.h"

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
  long	  dwCopied;
  long	  dwCount;
  HBITMAP hBitmap;
  BITMAP  bitmap;
  char	  szJunk[50];

  hBitmap = LoadBitmap(hInstance, "SCOTTIE");

  MessageBox (NULL, "Getting object ", "GetObject", MB_OK);
  dwCount = GetObject(hBitmap, (long)sizeof(BITMAP), (LPSTR) &bitmap);

  if (dwCount)
     {
     sprintf(szJunk, "Bitmap Width=%d, Height=%d",
	     bitmap.bmWidth, bitmap.bmHeight);
     MessageBox(NULL, szJunk, "GetObject Successful", MB_OK);
     }
  else
     MessageBox(NULL, "Object not gotten", "GetObject", MB_OK);

  return 0;
}
