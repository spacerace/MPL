/*
 *
 *  GetBitmapDimension
 *  gbitdim.c
 *  
 *  This program demonstrates the use of the function GetBitmapDimension.
 *  This funtion returns the width and height of the bitmap specified by 
 *  the parameter.
 *   
 */

#include "windows.h"
#include <stdio.h>

int PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
  {
  HBITMAP hBitmap;          /* Handle to the loaded bitmap.   */
  DWORD   dwOldDimensions;  /* Old dimensions of the bitmap.  */
  DWORD   dwDimensions;     /* New dimensions of the bitmap.  */
  char    szOutBuf[50];     /* Output buffer for Message Box. */

       /* Load in the bitmap to examine. */
  hBitmap = LoadBitmap (hInstance, "SCOTTIE");
  if (hBitmap == NULL)
    {
    MessageBox (GetFocus (), (LPSTR)"Cannot Find Bitmap SCOTTIE",
               (LPSTR)"LoadBitmap () Error!", MB_OK | MB_ICONEXCLAMATION);
    return 1;
    }

       /* Set the new dimensions of the bitmap and show the old ones. */
  dwOldDimensions = SetBitmapDimension (hBitmap, (short)15, (short)15);
  sprintf (szOutBuf, "The old dimensions were %hu * %hu",
          LOWORD (dwOldDimensions), HIWORD (dwOldDimensions));
  MessageBox (GetFocus (), (LPSTR)szOutBuf,
             (LPSTR)"SetBitmapDimension ()", MB_OK);

       /* Get the new dimensions. */
  MessageBox (GetFocus (), (LPSTR)"Getting new dimensions.",
             "GetBitmapDimension ()", MB_OK);
  dwDimensions = GetBitmapDimension (hBitmap);

       /* And display them (or an error). */
  sprintf (szOutBuf, "New dimensions are %hu * %hu",
          LOWORD (dwDimensions), HIWORD (dwDimensions));
  if (dwDimensions)
    MessageBox (GetFocus (), (LPSTR)szOutBuf,
               (LPSTR)"GetBitmapDimension ()", MB_OK);
  else
    MessageBox (GetFocus (), (LPSTR)"Function failed",
               (LPSTR)"GetBitmapDimension () Error!",
               MB_OK | MB_ICONEXCLAMATION);

  if (!DeleteObject (hBitmap))
    {
    MessageBox (GetFocus (), (LPSTR)"Bitmap Resources Not Deleted!",
               (LPSTR)"DeleteObject () Error!",
               MB_OK | MB_ICONEXCLAMATION);
    return 1;
    }

  return 0;
  }
