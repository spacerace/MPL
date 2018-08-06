/*
 *  SetBitmapDimension
 *  sbitdim.c,
 *  
 *  This program demonstrates the use of the function SetBitmapDimension.
 *  This funtion assigns a width and height to a bitmap in 0.1 millimeters
 *  units.  These values are not used internally by GDI.
 *   
 */

#include "windows.h"

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
  HBITMAP hBitmap;
  DWORD ptOldDimensions;
  DWORD ptDimensions;

  hBitmap = LoadBitmap ( hInstance, (LPSTR)"SMILE" );

  MessageBox (NULL, (LPSTR)"Setting new dimensions ",
             (LPSTR)"SetBitmapDimension", MB_OK);

  ptOldDimensions = SetBitmapDimension ( hBitmap, 15, 15 );

  ptDimensions = GetBitmapDimension ( hBitmap );

  if ( (HIWORD(ptDimensions) != 0) || (LOWORD(ptDimensions) != 0) )
     MessageBox (NULL, (LPSTR)"New dimensions set.",
                (LPSTR)"SetBitmapDimension", MB_OK);
  else
     MessageBox (NULL, (LPSTR)"New dimensions not set",
                (LPSTR)"SetBitmapDimension", MB_OK);

  return 0;
}
