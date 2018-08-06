/*
 *
 * Functions: hDC CreateDC(lpString, lpString, lpString, lpString)
 *	      hDC CreateCompatibleDC(hDC)
 *	  HBITMAP LoadBitmap(hInstance, lpString)
 *	  hObject SelectObject(hDC, hObject)
 *	     BOOL StretchBlt(hDC,X,Y,nWidth,nHeight,hDC,
 *			     XSrc,YSrc,XSrcWidth,YSrcHeight,dwROP)
 *	     BOOL BitBlt(hDC,X,Y,nWidth,nHeight,hDC,XSrc,YSrc,dwROP)
 *	     BOOL DeleteDC(hDC)
 *	     BOOL DeleteObject(hObject)
 */
#include "windows.h"
#include "stdio.h"

int	PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int	cmdShow;
{
  HDC     oneDC, screenDC;
  HBITMAP hWorld;
  int	Index;
  char	LoadBitmapString [40];

  screenDC = CreateDC("DISPLAY", NULL, NULL, NULL);
  oneDC = CreateCompatibleDC(screenDC);
  Index = 1;
  sprintf (LoadBitmapString, "WORLD%i", Index);
  hWorld = LoadBitmap(hInstance, LoadBitmapString);
  SelectObject(oneDC, hWorld);
  StretchBlt(screenDC, 250, 140, 99, 99, oneDC, 0, 0, 99, 99, SRCCOPY);
/*
    StretchBlt(screenDC, 150, 75, 51, 100, oneDC, 0, 0, 51, 31, SRCCOPY);
    BitBlt(screenDC, 75, 75, 52, 32, oneDC, 0, 0, SRCCOPY);
*/
  DeleteDC(oneDC);
  DeleteDC(screenDC);
  DeleteObject(hWorld);
  return 0;
}


