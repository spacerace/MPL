/*
 *
 * Function (s) demonstrated in this program: CreateFont, CreateFontIndirect,
 *      CreateBitmapIndirect, CreateCompatibleBitmap, SetBitmapBits,
 *      GetBitmapBits, SetBitmapBits.
 *
 * Description:
 * This application demonstrates the GetBitmapBits, SetBitmapBits,
 * CreateBitmapIndirect, CreateFont, CreateCompatibleBitmap,
 * CreateFontIndirect, and SetBitmapBits Windows functions.
 *
 */

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include "CreatFon.h"

long	FAR PASCAL WndProc  (HWND, unsigned, WORD, LONG);
HBITMAP StretchBitmap (HBITMAP, int);
HBITMAP GetBitmapFont (int);

long	dwCount;
HBITMAP  hBitmapScottie;
PBITMAP  pBitmap;
char	nBits[500] = "ABC";
HBITMAP  hBitmapPopFont;
HBITMAP  hBitmapAbout;
HANDLE   hInstMain;
char	szAppName [] = "CreatFon";
char	szOutputBuffer1 [70];
char	szOutputBuffer2 [500];

struct {
  char	*szMessage;
} Messages [] =   {
  "About\0",
  "  This application demonstrates the GetBitmapBits, \n\
SetBitmapBits, CreateBitmapIndirect, CreateFont, \n\
CreateCompatibleBitmap, CreateFontIndirect, and\n\
SetBitmapBits Windows functions.",

      "Help Message",
  "     This program demonstrates the use of many\n\
Windows functions.  Use the menu to test these\n\
functions.",

  "CreateFont",
  "     The menu has a new item the font for which\n\
was created using the CreateFont Windows function.",

  "CreateFontIndirect",
  "     The menu has a new item the font for which\n\
was created using the CreateFontIndirect Windows\n\
function.",

  "CreateBitmapIndirect",
  "     The menu has a new item the bitmap for which\n\
was created using the CreateBitmapIndirect Windows\n\
function.",

      "CreateCompatibleBitmap",
  "     The menu has a new item the bitmap for which\n\
was created using the CreateCompatibleBitmap Windows\n\
function.",

      "GetBitmapBits",
  "    Bitmap bits are stored in a buffer.",

  "SetBitmapBits",
  "     The menu has a new item the bitmap for which\n\
was created using the SetBitmapBits Windows\n\
function.",

  "SetBitmapBits",
  "     You must Get the Bits first!",

};


/****************************************************************************/

void ProcessMessage (HWND, int);

void ProcessMessage (hWnd, MessageNumber)  /*  For outputting messages  */
HWND     hWnd;
int	MessageNumber;
{
  sprintf (szOutputBuffer1, "%s", Messages [MessageNumber]);
  sprintf (szOutputBuffer2, "%s", Messages [MessageNumber + 1]);
  MessageBox (hWnd, szOutputBuffer2, szOutputBuffer1, MB_OK);
}


/****************************************************************************/

int	PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HANDLE   hInstance, hPrevInstance;
LPSTR    lpszCmdLine;
int	nCmdShow;
{
  HMENU    hMenu, hMenuPopup;
  HWND     hWnd;
  MSG      msg;
  int	i;
  WNDCLASS wndclass;

  if (!hPrevInstance)
  {
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = WndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    wndclass.hbrBackground = GetStockObject (WHITE_BRUSH);
    wndclass.lpszMenuName  = szAppName;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass (&wndclass))
      return FALSE;
  }

  hInstMain = hInstance;

  hWnd = CreateWindow (szAppName, "Bitmap Menu Demonstration",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0,
      CW_USEDEFAULT, 0,
      NULL, NULL, hInstance, NULL);

  hMenu = GetSystemMenu (hWnd, FALSE);
  hBitmapAbout = StretchBitmap (LoadBitmap (hInstance, "BitmapAbout"), 1);
  ChangeMenu (hMenu, NULL, NULL, 0, MF_APPEND | MF_SEPARATOR);
  ChangeMenu (hMenu, NULL, (PSTR) hBitmapAbout, IDM_ABOUT,
      MF_APPEND | MF_BITMAP);

  ShowWindow (hWnd, nCmdShow);
  UpdateWindow (hWnd);

  while (GetMessage (&msg, NULL, 0, 0))
  {
    TranslateMessage (&msg);
    DispatchMessage (&msg);
  }

  DeleteObject (hBitmapPopFont);  /*  Free up memory taken by bitmaps  */
  DeleteObject (hBitmapAbout);
  DeleteObject (hBitmapScottie);

  return msg.wParam;
}


/****************************************************************************/

HBITMAP StretchBitmap (hBitmap1, I)
HBITMAP    hBitmap1;
int	I;
{
  BITMAP     bm1, bm2;
  HBITMAP    hBitmap2;
  HDC        hDC, hMemDC1, hMemDC2;
  TEXTMETRIC tm;

  hDC = CreateIC ("DISPLAY", NULL, NULL, NULL);
  GetTextMetrics (hDC, &tm);
  hMemDC1 = CreateCompatibleDC (hDC);
  hMemDC2 = CreateCompatibleDC (hDC);
  DeleteDC (hDC);

  GetObject (hBitmap1, sizeof (BITMAP), (LPSTR) & bm1);

  bm2 = bm1;
  bm2.bmWidth      = (tm.tmAveCharWidth * bm2.bmWidth)  / 4;
  bm2.bmHeight     = (tm.tmHeight       * bm2.bmHeight) / 8;
  bm2.bmWidthBytes = ( (bm2.bmWidth + 15) / 16) * 2;

  if (I == 1)
  {
    hBitmap2 = CreateBitmapIndirect (&bm2);
    SelectObject (hMemDC1, hBitmap1);
    SelectObject (hMemDC2, hBitmap2);
  }
  else    
  {
    SelectObject (hMemDC1, hBitmap1);
    hBitmap2 = CreateCompatibleBitmap (hMemDC1, bm2.bmWidth, bm2.bmHeight);
    SelectObject (hMemDC2, hBitmap2);
  }

  StretchBlt (hMemDC2, 0, 0, bm2.bmWidth, bm2.bmHeight,
      hMemDC1, 0, 0, bm1.bmWidth, bm1.bmHeight, SRCCOPY);

  DeleteDC (hMemDC1);
  DeleteDC (hMemDC2);
  DeleteObject (hBitmap1);

  return hBitmap2;
}


/****************************************************************************/

HBITMAP GetBitmapFont (i)
int	i;
{
  static struct  
  {
    BYTE lfPitchAndFamily;
    BYTE lfFaceName [LF_FACESIZE];
    char	*szMenuText;
  } lfSet [2] = 
  {
    VARIABLE_PITCH | FF_ROMAN,  "Tms Rmn",   "Times Roman",
    VARIABLE_PITCH | FF_SWISS,  "Helvetica", "Helvetica"
  };
  DWORD   dwSize;
  HBITMAP hBitmap;
  HDC     hDC, hMemDC;
  HFONT   hFont, hNewFont;
  LOGFONT lf;

  hFont = GetStockObject (SYSTEM_FONT);
  GetObject (hFont, sizeof (LOGFONT), (LPSTR) & lf);

  lf.lfHeight *= 2;
  lf.lfWidth  *= 2;
  lf.lfPitchAndFamily = lfSet[i].lfPitchAndFamily;
  strcpy (lf.lfFaceName, lfSet[i].lfFaceName);

  hDC = CreateIC ("DISPLAY", NULL, NULL, NULL);
  hMemDC = CreateCompatibleDC (hDC);

  if (i == 0)
  {
    hNewFont = CreateFont (lf.lfHeight, lf.lfWidth, lf.lfEscapement,
        lf.lfOrientation, lf.lfWeight, lf.lfItalic,
        lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
        lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
        lf.lfPitchAndFamily, lf.lfFaceName);
    SelectObject (hMemDC, hNewFont);
  }
  else
    SelectObject (hMemDC, CreateFontIndirect (&lf));

  dwSize = GetTextExtent (hMemDC, lfSet[i].szMenuText,
      strlen (lfSet[i].szMenuText));

  hBitmap = CreateBitmap (LOWORD (dwSize), HIWORD (dwSize), 1, 1, NULL);
  SelectObject (hMemDC, hBitmap);
  TextOut (hMemDC, 0, 0, lfSet[i].szMenuText, strlen (lfSet[i].szMenuText));

  DeleteObject (SelectObject (hMemDC, hFont));
  DeleteDC (hMemDC);
  DeleteDC (hDC);

  return hBitmap;
}


/****************************************************************************/

long	FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd;
unsigned	iMessage;
WORD     wParam;
LONG     lParam;
{
  HMENU    hMenu;
  static short	nCurrentFont = IDM_TMSRMN;

  switch (iMessage)
  {
  case WM_SYSCOMMAND:
    switch (wParam)
    {
    case IDM_ABOUT:
      ProcessMessage (hWnd, 0);
      break;
    default:
      return DefWindowProc (hWnd, iMessage, wParam, lParam);
    }
    break;

  case WM_COMMAND:
    switch (wParam)
    {
    case IDM_CREATEFONT:
      hMenu = GetSubMenu (GetMenu (hWnd), 0);
      hBitmapPopFont = GetBitmapFont (0);
      ChangeMenu (hMenu, NULL, NULL, 0,
          MF_APPEND | MF_SEPARATOR);
      ChangeMenu (hMenu, NULL, (PSTR) hBitmapPopFont,
          IDM_TMSRMN, MF_APPEND | MF_BITMAP);
      ProcessMessage (hWnd, 4);
      break;

    case IDM_CREATEFONTINDIRECT:
      hMenu = GetSubMenu (GetMenu (hWnd), 0);
      hBitmapPopFont = GetBitmapFont (1);
      ChangeMenu (hMenu, NULL, NULL, 0,
          MF_APPEND | MF_SEPARATOR);
      ChangeMenu (hMenu, NULL, (PSTR) hBitmapPopFont,
          IDM_TMSRMN, MF_APPEND | MF_BITMAP);
      ProcessMessage (hWnd, 6);
      break;

    case IDM_CREATEBMPINDIRECT:
      hMenu = GetSubMenu (GetMenu (hWnd), 0);
      hBitmapAbout = StretchBitmap (LoadBitmap (hInstMain,
          "BitmapAbout"), 1);
      ChangeMenu (hMenu, NULL, NULL, 0,
          MF_APPEND | MF_SEPARATOR);
      ChangeMenu (hMenu, NULL, (PSTR) hBitmapAbout,
          IDM_ABOUT, MF_APPEND | MF_BITMAP);
      ProcessMessage (hWnd, 8);
      break;

    case IDM_CREATECOMPATIBLEBMP:
      hMenu = GetSubMenu (GetMenu (hWnd), 0);
      hBitmapAbout = StretchBitmap (LoadBitmap (hInstMain,
          "BitmapAbout"), 2);
      ChangeMenu (hMenu, NULL, NULL, 0,
          MF_APPEND | MF_SEPARATOR);
      ChangeMenu (hMenu, NULL, (PSTR) hBitmapAbout,
          IDM_ABOUT, MF_APPEND | MF_BITMAP);
      ProcessMessage (hWnd, 10);
      break;

    case IDM_GETBITMAPBITS:
      hBitmapScottie = LoadBitmap (hInstMain,
          "BitmapScottie");
      pBitmap = (PBITMAP)LocalAlloc (LMEM_MOVEABLE,
          sizeof (BITMAP));
      dwCount = GetObject (hBitmapScottie,
          (long)sizeof (BITMAP), (LPSTR) pBitmap);
      if (GetBitmapBits (hBitmapScottie, dwCount,
          (LPSTR)nBits))
	ProcessMessage (hWnd, 12);
      else
	MessageBox (hWnd, "Bitmap did not copy!",
	    "GetBitmapBits", MB_OK);
      break;

    case IDM_SETBITMAPBITS:
      if (nBits [0] == 'A')
	ProcessMessage (hWnd, 16);
      else
      {
	SetBitmapBits (hBitmapScottie, dwCount, nBits);
	hMenu = GetSubMenu (GetMenu (hWnd), 0);
	hBitmapScottie = StretchBitmap (hBitmapScottie, 1);
	ChangeMenu (hMenu, NULL, NULL, 0,
	    MF_APPEND | MF_SEPARATOR);
	ChangeMenu (hMenu, NULL, (PSTR) hBitmapScottie,
	    IDM_SCOTTIE, MF_APPEND | MF_BITMAP);
	ProcessMessage (hWnd, 14);
      }
      break;

    case IDM_HELP:
      ProcessMessage (hWnd, 2);
      break;
    }
    break;

  case WM_DESTROY:
    PostQuitMessage (0);
    break;

  default:
    return DefWindowProc (hWnd, iMessage, wParam, lParam);
  }
  return 0L;
}


