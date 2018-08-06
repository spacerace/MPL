
#include <windows.h>
#include <string.h>
#include "diabox.h"

BOOL FAR PASCAL InitDiabox (HANDLE, HANDLE, int);
LONG FAR PASCAL DiaboxWindowProc   (HANDLE, unsigned, WORD, LONG);
BOOL FAR PASCAL DialogBoxWindowProc (HANDLE, unsigned, WORD, LONG); /* added */
int     NEAR writeGMem (HANDLE, WORD, BYTE *, WORD);

HANDLE  hDTemplate;
HANDLE  hInst;
FARPROC lpDlgTest;
HWND    hDlgTest;
BOOL    nModeless;
BOOL    bResult;

struct dtHdrType   {
  LONG  dtStyle;
  BYTE  dtItemCount;
  int	dtX;
  int	dtY;
  int	dtCX;
  int	dtCY;
  char	dtResourceName[1];
  char	dtClassName[1];
  char	dtCaptionText[1];
  };

#define DTHDR    struct dtHdrType

struct dtItmType   {
  int	dtilX;
  int	dtilY;
  int	dtilCX;
  int	dtilCY;
  int	dtilID;
  LONG  dtilWindowStyle;
  BYTE  dtilControlClass;
  char	dtilText[1];
  };

#define DTITM    struct dtItmType

DTHDR    dtHdr;
DTITM    dtItm;
WORD     wByteCount;
BYTE     work[256];

/*  Forward references and type checking */

BOOL DisplayDialogBox (HWND);

/**************************************************************************/

int     PASCAL WinMain  (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int	cmdShow;
  {
  MSG  msg;

  if (hPrevInstance)   /* do not allow more than one instance */
    return FALSE;

  InitDiabox (hInstance, hPrevInstance, cmdShow);

  while (GetMessage ( (LPMSG) & msg, NULL, 0, 0))
    {
    if (nModeless == TRUE)
      {
      TranslateMessage ( (LPMSG) & msg);
      DispatchMessage ( (LPMSG) & msg);
      }
    else 
      {
      if ( (hDlgTest == NULL) || !IsDialogMessage (hDlgTest, (LPMSG) & msg))
        {
        TranslateMessage ( (LPMSG) & msg);
        DispatchMessage ( (LPMSG) & msg);
        }
      }
    }
  return (msg.wParam);
  }

/*******************************   initialization   ***********************/
BOOL FAR PASCAL InitDiabox (hInstance, hPrevInstance, cmdShow)
HANDLE   hInstance;
HANDLE   hPrevInstance;
int      cmdShow;
  {
  WNDCLASS  wcDiaboxClass;
  HWND      hWnd;

  wcDiaboxClass.lpszClassName = (LPSTR) "Diabox";
  wcDiaboxClass.hInstance     = hInstance;
  wcDiaboxClass.lpfnWndProc   = DiaboxWindowProc;
  wcDiaboxClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
  wcDiaboxClass.hIcon         = NULL;
  wcDiaboxClass.lpszMenuName  = (LPSTR) "diabox";             /* menu added  */
  wcDiaboxClass.hbrBackground = GetStockObject (WHITE_BRUSH);
  wcDiaboxClass.style         = CS_HREDRAW | CS_VREDRAW;
  wcDiaboxClass.cbClsExtra    = 0;
  wcDiaboxClass.cbWndExtra    = 0;

  RegisterClass ( (LPWNDCLASS) & wcDiaboxClass);
  hWnd = CreateWindow ( (LPSTR) "Diabox",
      (LPSTR) "DialogBox",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      (HWND)NULL,
      (HMENU)NULL,
      (HANDLE)hInstance,
      (LPSTR)NULL
     );

  hInst = hInstance;       /*  instance saved for dialog box  */

  ShowWindow   (hWnd, cmdShow);
  UpdateWindow (hWnd);

  return TRUE;
  }


/*********************   window procedure - process messages   *************/

LONG FAR PASCAL DiaboxWindowProc (hWnd, message, wParam, lParam)
HWND        hWnd;
unsigned    message;
WORD        wParam;
LONG        lParam;
  {
  switch (message)
    {
    case WM_COMMAND:
      switch (wParam)
        {
        case IDDBOX:
          nModeless = FALSE;
          bResult = DisplayDialogBox (hWnd);
          break;

        case IDDBOX2:
          nModeless = TRUE;
          bResult = DisplayDialogBox (hWnd);
          break;

        default:
          return DefWindowProc (hWnd, message, wParam, lParam);
          break;
        }
      break;

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
BOOL DisplayDialogBox (hWnd)
HWND   hWnd;
  {
  LPSTR    lpDTemplate;

  if (hDlgTest != NULL)
    return (FALSE);

  if (! (hDTemplate = GlobalAlloc (GMEM_MOVEABLE, (DWORD) sizeof (DTHDR))))
    return (FALSE);

  wByteCount = 0;
  dtHdr.dtStyle           = WS_POPUP | WS_VISIBLE | WS_BORDER | WS_CAPTION;
  dtHdr.dtItemCount       = 4;
  dtHdr.dtX               = 10;
  dtHdr.dtY               = 10;
  dtHdr.dtCX              = 200;
  dtHdr.dtCY              = 100;
  dtHdr.dtResourceName[0] = 0;
  dtHdr.dtClassName[0]    = 0;

  if (!writeGMem (hDTemplate, wByteCount, (BYTE *) & dtHdr, sizeof (DTHDR) - 1))
    return (FALSE);
  wByteCount += sizeof (DTHDR) - 1;

  strcpy (work, "Template Test");
  if (!writeGMem (hDTemplate, wByteCount, work, strlen (work) + 1))
    return (FALSE);
  wByteCount += strlen (work) + 1;

/* add OK BUTTON item */

  dtItm.dtilX            = 30;
  dtItm.dtilY            = 60;
  dtItm.dtilCX           = 32;
  dtItm.dtilCY           = 12;
  dtItm.dtilID           = 0x0300;
  dtItm.dtilWindowStyle  = BS_DEFPUSHBUTTON | WS_TABSTOP | WS_CHILD | WS_VISIBLE;
  dtItm.dtilControlClass = 0x80; /* BUTTON */
  if (!writeGMem (hDTemplate, wByteCount, (BYTE *) & dtItm, sizeof (DTITM) - 1))
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

/* add CANCEL BUTTON item */

  dtItm.dtilX            = 70;
  dtItm.dtilY            = 60;
  dtItm.dtilCX           = 32;
  dtItm.dtilCY           = 12;
  dtItm.dtilID           = 0x0400;
  dtItm.dtilWindowStyle  = BS_PUSHBUTTON | WS_TABSTOP | WS_CHILD | WS_VISIBLE;
  dtItm.dtilControlClass = 0x80; /* BUTTON */
  if (!writeGMem (hDTemplate, wByteCount, (BYTE *) & dtItm, sizeof (DTITM) - 1))
    return (FALSE);
  wByteCount += sizeof (DTITM) - 1;

  strcpy (work, "Cancel");
  if (!writeGMem (hDTemplate, wByteCount, work, strlen (work) + 1))
    return (FALSE);
  wByteCount += strlen (work) + 1;

  work[0] = 0;
  if (!writeGMem (hDTemplate, wByteCount, work, 1))
    return (FALSE);
  wByteCount += 1;

/* add EDIT item */

  dtItm.dtilX            = 30;
  dtItm.dtilY            = 40;
  dtItm.dtilCX           = 32;
  dtItm.dtilCY           = 12;
  dtItm.dtilID           = 0x0200;
  dtItm.dtilWindowStyle  = ES_LEFT | WS_TABSTOP | WS_CHILD | WS_BORDER | WS_VISIBLE;
  dtItm.dtilControlClass = 0x81; /* EDITCODE */
  if (!writeGMem (hDTemplate, wByteCount, (BYTE *) & dtItm, sizeof (DTITM) - 1))
    return (FALSE);
  wByteCount += sizeof (DTITM) - 1;

  strcpy (work, "Edit");
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
  dtItm.dtilControlClass = 0x82; /* STATICCODE */
  if (!writeGMem (hDTemplate, wByteCount, (BYTE *) & dtItm, sizeof (DTITM) - 1))
    return (FALSE);
  wByteCount += sizeof (DTITM) - 1;

  if (nModeless == FALSE)
    strcpy (work, "Modal DialogBox");
  else
    strcpy (work, "Modeless DialogBox");

  if (!writeGMem (hDTemplate, wByteCount, work, strlen (work) + 1))
    return (FALSE);
  wByteCount += strlen (work) + 1;

  work[0] = 0;
  if (!writeGMem (hDTemplate, wByteCount, work, 1))
    return (FALSE);

  lpDlgTest = MakeProcInstance ( (FARPROC) DialogBoxWindowProc, hInst);

  if (nModeless == FALSE)
    {
    hDlgTest = DialogBoxIndirect (hInst, hDTemplate, hWnd, lpDlgTest);
    FreeProcInstance ( (FARPROC) lpDlgTest);
    hDlgTest = NULL;
    }
  else
    {
    lpDTemplate = GlobalLock (hDTemplate);
    hDlgTest = CreateDialogIndirect (hInst, lpDTemplate, hWnd, lpDlgTest);
    GlobalUnlock (hDTemplate);
    }
  return (TRUE);
  }

/**************************************************************************/
BOOL FAR PASCAL DialogBoxWindowProc (hDlg, message, wParam, lParam)
HWND      hDlg;
unsigned  message;
WORD      wParam;
LONG      lParam;
  {
  char	TempEdit[24];

  switch (message)
    {
    case WM_INITDIALOG:
  /* SetFocus to first control that is not a static or group box */
      SetFocus (GetDlgItem (hDlg, 0x0200));
      break;

    case WM_COMMAND:
      switch (wParam)
        {
        case 0x0100: /* static */
        case 0x0200: /* edit   */
          break;
        case 0x0300: /* ok     */
          GetDlgItemText (hDlg, 0x0200, TempEdit, 24);
          MessageBox (GetFocus (), (LPSTR)TempEdit,
                     (LPSTR)"Edit Control Contains", MB_OK);

          /*  No Break Here... We want to end the Dialog Box  */
        case 0x0400: /* cancel */
          if (nModeless == FALSE)
            EndDialog (hDlg, TRUE);
          else
            {
            DestroyWindow (hDlg);
            hDlgTest = NULL;
            }
          return (TRUE);
        default:
          return (TRUE);
        }           /* end wParam switch */
      break;      /* end command */

    default:
      return (FALSE);
    }                 /* end message switch */
  return (FALSE);
  }

/****************************************************************************/
int     NEAR writeGMem (hData, offset, data, length)
HANDLE  hData;
WORD    offset;
BYTE    *data;
WORD    length;
  {
  HANDLE  hNewData;
  LPSTR   lpDstData;
  int     n;

  if (offset + length > (WORD)GlobalSize (hData))
    {
    if (! (hNewData = GlobalReAlloc (hData, (DWORD) offset + length, GMEM_MOVEABLE)))
      GlobalFree (hData);
    hDTemplate = hNewData;
    }

  if (lpDstData = GlobalLock (hData))
    {
    lpDstData = lpDstData + offset;
    for (n = 0; n < length; n++)
      *lpDstData++ = *data++;
    GlobalUnlock (hData);
    return (TRUE);
/* return (lpDstData); */  /* return new offset position */
    }
  return (FALSE);
  }
