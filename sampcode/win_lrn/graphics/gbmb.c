/*
 *
 *  GetBitmapBits
 *  gbmb.c
 *  
 *  This program demonstrates the use of the function GetBitmapBits.
 *  This funtion copies the bits of the specified bitmap into the buffer
 *  that is pointed to by the last parameter.  The middle parameter
 *  specifies the number of bytes to be copied to the buffer.
 *   
 *  Other references: cffile.c
 *  
 */

#include "windows.h"

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
  long dwCount;
  HBITMAP hBitmap;
  PBITMAP pBitmap;
  int nBraemar[500];

  hBitmap = LoadBitmap(hInstance, "SCOTTIE");
  pBitmap = (PBITMAP)LocalAlloc(LMEM_MOVEABLE, sizeof(BITMAP));
  dwCount = GetObject(hBitmap, (long)sizeof(BITMAP), (LPSTR) pBitmap);

  MessageBox(NULL, "Get bitmap", "GetBitmapBits", MB_OK);

  if (GetBitmapBits(hBitmap, dwCount, (LPSTR) nBraemar))
     MessageBox(NULL, "Bitmap copied", "GetBitmapBits", MB_OK);
  else
     MessageBox(NULL, "Bitmap did not copy", "GetBitmapBits", MB_OK);


  return 0;
}
