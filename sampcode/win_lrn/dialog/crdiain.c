/*
 *
 *  CreateDialogIndirect
 *
 *  This program demonstrates the use of the function CreateDialogIndirect.
 *  It allows a user to create a modeless dialogbox on the fly. Memory for the
 *  structure is dynamcally allocated to conform to variable string lengths.
 */

#include <windows.h>
#include "crdiain.h"

BOOL FAR PASCAL InitDiabox (HANDLE, HANDLE, int);
long	FAR PASCAL DiaboxWindowProc (HANDLE, unsigned, WORD, LONG);
BOOL FAR PASCAL DialogBoxWindowProc (HANDLE, unsigned, WORD, LONG);

HANDLE hInst;
FARPROC lpDlgTest;
HWND hDlgTest;

typedef struct dtHdrType {
  LONG  dtStyle;
  BYTE  dtItemCount;
  int	dtX;
  int	dtY;
  int	dtCX;
  int	dtCY;
  char	dtResourceName[1];
  char	dtClassName[1];
  char	dtCaptionText[1];
} DTHDR;

typedef struct dtItmType {
  int	dtilX;
  int	dtilY;
  int	dtilCX;
  int	dtilCY;
  int	dtilID;
  LONG  dtilWindowStyle;
  BYTE  dtilControlClass;
  char	dtilText[1];
} DTITM;


/**************************************************************************/

int	PASCAL WinMain  (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE  hInstance, hPrevInstance;
LPSTR   lpszCmdLine;
int	cmdShow;
{
  MSG  msg;

  InitDiabox (hInstance, hPrevInstance, cmdShow);
  while (GetMessage ( (LPMSG) & msg, NULL, 0, 0))
  {
    if ( (hDlgTest == NULL) || !IsDialogMessage (hDlgTest, (LPMSG) & msg))
    {
      TranslateMessage ( (LPMSG) & msg);
      DispatchMessage ( (LPMSG) & msg);
    }
  }
  exit (msg.wParam);
}


/*******************************   initialization   ***********************/

BOOL FAR PASCAL InitDiabox (hInstance, hPrevInstance, cmdShow)
HANDLE  hInstance;
HANDLE  hPrevInstance;
int	cmdShow;
{
  WNDCLASS  wcDiaboxClass;
  HWND  hWnd;

  wcDiaboxClass.lpszClassName = (LPSTR) "Diabox";
  wcDiaboxClass.hInstance     = hInstance;
  wcDiaboxClass.lpfnWndProc   = DiaboxWindowProc;
  wcDiaboxClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
  wcDiaboxClass.hIcon         = NULL;
  wcDiaboxClass.lpszMenuName  = (LPSTR) "diabox";
  wcDiaboxClass.hbrBackground = GetStockObject (WHITE_BRUSH);
  wcDiaboxClass.style         = CS_HREDRAW | CS_VREDRAW;
  wcDiaboxClass.cbClsExtra    = 0;
  wcDiaboxClass.cbWndExtra    = 0;

  RegisterClass ( (LPWNDCLASS) & wcDiaboxClass);
  hWnd = CreateWindow ( (LPSTR) "Diabox",
      (LPSTR) "CreateDialogIndirect",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      (HWND)NULL,
      (HMENU)NULL,
      (HANDLE)hInstance,
      (LPSTR)NULL);

  hInst = hInstance;                    /*  instance saved for dialog box  */

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  return TRUE;
}


/*********************   window procedure - process messages   *************/

long	FAR PASCAL DiaboxWindowProc (hWnd, message, wParam, lParam)
HWND        hWnd;
unsigned	message;
WORD        wParam;
LONG        lParam;
{
  switch (message)
  {
  case WM_COMMAND:
    switch (wParam)
    {
    case IDDBOX:
      DisplayDialogBox (hWnd);
      break;
    default:
      return DefWindowProc (hWnd, message, wParam, lParam);
      break;
    }

  case WM_DESTROY:
    PostQuitMessage (0);
    break;

  default:
    return (DefWindowProc (hWnd, message, wParam, lParam));
    break;
  }
  return (0L);
}


/***************************************************************************/

DisplayDialogBox (hWnd)
HWND   hWnd;
{
  DTHDR    dtHdr;
  DTITM    dtItm;
  HANDLE   hDTemplate;
  LPSTR    lpDTemplate;
  WORD     wByteCount;
  BYTE     work[256];

  if (hDlgTest != NULL)
    return (FALSE);

  if (!(hDTemplate = GlobalAlloc (GMEM_MOVEABLE, (DWORD) sizeof (DTHDR))))
    return (FALSE);
  wByteCount = 0;

  dtHdr.dtStyle     = WS_POPUP | WS_VISIBLE | WS_BORDER | WS_CAPTION;
  dtHdr.dtItemCount = 2;
  dtHdr.dtX         = 10;
  dtHdr.dtY         = 10;
  dtHdr.dtCX        = 200;
  dtHdr.dtCY        = 100;
  dtHdr.dtResourceName[0] = 0;
  dtHdr.dtClassName[0]    = 0;
  if (!writeGMem (hDTemplate, wByteCount, (BYTE * ) & dtHdr, sizeof (DTHDR) - 1))
    return (FALSE);
  wByteCount += sizeof (DTHDR) - 1;

  strcpy (work, "Howdy !!!");
  if (!writeGMem (hDTemplate, wByteCount, work, strlen (work) + 1))
    return (FALSE);
  wByteCount += strlen (work) + 1;

/* add BUTTON item */

  dtItm.dtilX     = 30;
  dtItm.dtilY     = 50;
  dtItm.dtilCX    = 32;
  dtItm.dtilCY    = 12;
  dtItm.dtilID    = 0x0200;
  dtItm.dtilWindowStyle = BS_DEFPUSHBUTTON | WS_TABSTOP | WS_CHILD | WS_VISIBLE;
  dtItm.dtilControlClass = 0x80;
  if (!writeGMem (hDTemplate, wByteCount, (BYTE * ) & dtItm, sizeof (DTITM) - 1))
    return (FALSE);
  wByteCount += sizeof (DTITM) - 1;

  strcpy (work, "OK");
  if (!writeGMem (hDTemplate, wByteCount, work, strlen (work) + 1))
    return (FALSE);
  wByteCount += strlen (work) + 1;

  work[0] = 0;
  if (!writeGMem (hDTemplate, wByteCount, work, 1))
    return (FALSE);
  wByteCount += 1;

/* add MESSAGE item */

  dtItm.dtilX     = 30;
  dtItm.dtilY     = 20;
  dtItm.dtilCX    = 100;
  dtItm.dtilCY    = 8;
  dtItm.dtilID    = 0x0100;
  dtItm.dtilWindowStyle = SS_LEFT | WS_CHILD | WS_VISIBLE;
  dtItm.dtilControlClass = 0x82;
  if (!writeGMem (hDTemplate, wByteCount, (BYTE * ) & dtItm, sizeof (DTITM) - 1))
    return (FALSE);
  wByteCount += sizeof (DTITM) - 1;

  strcpy (work, "Modeless DialogBox");
  if (!writeGMem (hDTemplate, wByteCount, work, strlen (work) + 1))
    return (FALSE);
  wByteCount += strlen (work) + 1;

  work[0] = 0;
  if (!writeGMem (hDTemplate, wByteCount, work, 1))
    return (FALSE);

  lpDlgTest = MakeProcInstance ( (FARPROC) DialogBoxWindowProc, hInst);

  lpDTemplate = GlobalLock (hDTemplate);
/*  function demonstrated  */
  hDlgTest = CreateDialogIndirect (hInst, lpDTemplate, hWnd, lpDlgTest);

  if (hDlgTest == -1)
    MessageBox (NULL, (LPSTR)"CreateDialogIndirect failed",
        (LPSTR)"error", MB_ICONHAND);
  GlobalUnlock (hDTemplate);

  return (TRUE);
}


/**************************************************************************/

BOOL FAR PASCAL DialogBoxWindowProc (hDlg, message, wParam, lParam)
HWND      hDlg;
unsigned	message;
WORD      wParam;
LONG      lParam;
{
  switch (message)
  {
  case WM_COMMAND:
    switch (wParam)
    {
    case 0x0100:
    case 0x0200:
      break;
    default:
      return (TRUE);
    }
    break;
  default:
    return (FALSE);
  }

  DestroyWindow (hDlgTest);
  hDlgTest = NULL;
  return (TRUE);
}


/****************************************************************************/

BOOL NEAR writeGMem (hData, offset, data, length)
HANDLE  hData;
WORD    offset;
BYTE    *data;
WORD    length;
{
  HANDLE  hNewData;
  LPSTR   lpDstData;
  int	n;

  if (offset + length > GlobalSize (hData))
  {
    if (!(hNewData = GlobalReAlloc (hData, (DWORD) offset + length, GMEM_MOVEABLE)))
      GlobalFree (hData);

    hData = hNewData;
  }

  if (lpDstData = GlobalLock (hData))
  {
    lpDstData = lpDstData + offset;
    for (n = 0; n < length; n++)
      *lpDstData++ = *data++;
    GlobalUnlock (hData);
    return (TRUE);
  }

  return (FALSE);
}


