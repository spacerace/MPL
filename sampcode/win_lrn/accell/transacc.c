/*
 *  TranslateAccelerator
 *  transacc.c
 *
 * This program demonstrates the use of the function TranslateAccelerator.
 * When the function keys F9 or F10 are pressed then a message box appears
 * which notifies the user that the TranslateAccelerator function has
 * successfully converted and dispatched an accelerator message.
 *
 * All messages to this application are first passed to the
 * TranslateAccelerator function.  TranslateAccelerator will compare
 * the message to a predefined accelerator to see if they match.  If
 * the message doesn't match then TranslateAccelerator returns a 0,
 * and the message will be processed normally using TranslateMessage
 * and DispatchMessage.  If they do match, the message is an accelerator,
 * and the routine translates the message into a WM_COMMAND or
 * WM_SYSCOMMAND message.
 */

#include <windows.h>
#include "transacc.h"

static HANDLE hInst;

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE    hInstance, hPrevInstance;
LPSTR     lpszCmdLine;
int       cmdShow;
  {
  MSG       msg;
  HWND      hWnd;
  BOOL      bDecrementDisplayCount = 0;
  BOOL      bIncrementDisplayCount = 1;
  short     nResult;
  WNDCLASS  Class;

  Class.lpfnWndProc    = TransAccelWindowProc;
  Class.hCursor        = LoadCursor (NULL, IDC_ARROW);
  Class.lpszMenuName   = MAKEINTRESOURCE (TRANSACCELMENU1);
  Class.lpszClassName  = (LPSTR)"Window";
  Class.hbrBackground  = (HBRUSH)GetStockObject (WHITE_BRUSH);
  Class.hInstance      = hInstance;
  Class.style          = CS_HREDRAW | CS_VREDRAW;

  if (!RegisterClass ( (LPWNDCLASS)&Class))
    return FALSE;

  hInst = hInstance;

  hWnd = CreateWindow ( (LPSTR)"Window",
                     (LPSTR)"ShowCaret",
                     WS_OVERLAPPEDWINDOW,
                     CW_USEDEFAULT,          /* x         */
                     CW_USEDEFAULT,          /* y         */
                     CW_USEDEFAULT,          /* width     */
                     CW_USEDEFAULT,          /* height    */
                     (HWND)NULL,             /* no parent */
                     (HMENU)NULL,            /* use class menu */
                     (HANDLE)hInstance,      /* handle to window instance */
                     (LPSTR)NULL);           /* no params to pass on */

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  while (GetMessage ( (LPMSG) & msg, NULL, 0, 0))
    if (TranslateAccelerator (msg.hwnd, LoadAccelerators (hInstance,
        MAKEINTRESOURCE (TRANSACCELMENU1)), (LPMSG) & msg) == 1)
      MessageBox (hWnd, (LPSTR)"TranslateAccelerator processed a message",
          (LPSTR)"Done", MB_OK);
    else
      {
      TranslateMessage ( (LPMSG) & msg);
      DispatchMessage ( (LPMSG) & msg);
      }

  return TRUE;
  }

long    FAR PASCAL TransAccelWindowProc (hWnd, message, wParam, lParam)
HWND     hWnd;
unsigned message;
WORD     wParam;
LONG     lParam;
  {
  switch (message)
    {
    case WM_COMMAND:
      TransAccelMenuProc (hWnd, wParam);
      break;

    default:
      return (DefWindowProc (hWnd, message, wParam, lParam));
      break;
    }
  return (0L);
  }

void TransAccelMenuProc (hWnd, wId)
HWND hWnd;
WORD wId;
  {
  HMENU hMenu;

  switch (wId)
    {
    case ID_NEXT:
      hMenu = GetMenu (hWnd);  /*  Get the old menu  */
      DestroyMenu (hMenu);     /*  Get rid of it  */
      hMenu = LoadMenu (hInst, MAKEINTRESOURCE (TRANSACCELMENU2));
      SetMenu (hWnd, hMenu);
      DrawMenuBar (hWnd);
      break;

    case ID_PREV:
      hMenu = GetMenu (hWnd);  /*  Get the old menu  */
      DestroyMenu (hMenu);     /*  Get rid of it  */
      hMenu = LoadMenu (hInst, MAKEINTRESOURCE (TRANSACCELMENU1));
      SetMenu (hWnd, hMenu);
      DrawMenuBar (hWnd);
      break;
    }
  }
