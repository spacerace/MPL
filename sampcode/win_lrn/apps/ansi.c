/*
 *
 *   Function (s) demonstrated in this program:
 *        AnsiLower (LPSTR)
 *        AnsiUpper (LPSTR)
 *        AnsiToOem (LPSTR, LPSTR)
 *        AnsiNext (LPSTR)
 *        AnsiPrev (LPSTR, LPSTR)
 *        OemToAnsi (LPSTR, LPSTR)
 */

#include <windows.h>
#include <stdio.h>

#define MAXSTRING 80

long    FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE   hInstance, hPrevInstance;
LPSTR    lpszCmdLine;
int      cmdShow;
  {
  HWND      hWnd;
  WNDCLASS  wndclass;
  MSG       msg;

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
    wndclass.lpszMenuName  = NULL;
    wndclass.lpszClassName = "ANSI";

    if (!RegisterClass (&wndclass))
      return FALSE;
    }

  hWnd = CreateWindow ("ANSI",
                      "Ansi Functions",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT,
                      0,
                      CW_USEDEFAULT,
                      0,
                      NULL,
                      NULL,
                      hInstance,
                      NULL);

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  while (GetMessage (&msg, NULL, 0, 0))
    {
    TranslateMessage (&msg);
    DispatchMessage (&msg);
    }
  return (msg.wParam);
  }


long    FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd;
unsigned iMessage;
WORD     wParam;
LONG     lParam;
  {
  HDC         hDC;
  HMENU       hMenu;
  TEXTMETRIC  tm;
  char	szAnsiBuff[MAXSTRING];
  char	szOemBuff[MAXSTRING];

  switch (iMessage)
    {
    case WM_CREATE:
      hMenu = CreateMenu ();
      ChangeMenu (hMenu, NULL, (LPSTR)"Display", 100, MF_APPEND);
      SetMenu (hWnd, hMenu);
      break;

    case WM_INITMENU:
      InvalidateRect (hWnd, NULL, TRUE);
      break;

    case WM_COMMAND:
      if (wParam == 100)
        {
        hDC = GetDC (hWnd);
        GetTextMetrics (hDC, (LPTEXTMETRIC) & tm);
        sprintf (szAnsiBuff, "A Sample String: aBcDwXyZ ©«®²³º");
        TextOut (hDC, 1, tm.tmHeight * 1, (LPSTR)"ANSI Character Set", 18);
        TextOut (hDC, 1, tm.tmHeight * 2, (LPSTR)szAnsiBuff, 31);

        AnsiToOem ( (LPSTR)szAnsiBuff, (LPSTR)szOemBuff);
        TextOut (hDC, 1, tm.tmHeight * 4,
            (LPSTR)"ANSI Characters Converted to Nearest OEM Characters by AnsiToOem ()",
            67);
        TextOut (hDC, 1, tm.tmHeight * 5, (LPSTR)szOemBuff, 31);

        OemToAnsi ( (LPSTR)szOemBuff, (LPSTR)szAnsiBuff);
        TextOut (hDC, 1, tm.tmHeight * 7,
            (LPSTR)"OEM Characters Converted to Nearest ANSI Characters by OemToAnsi ()",
            67);
        TextOut (hDC, 1, tm.tmHeight * 8, (LPSTR)szAnsiBuff, 31);

  /* Convert original ANSI string to all lower case. */
        TextOut (hDC, 1, tm.tmHeight * 10,
            (LPSTR)"Original ANSI String Processed by AnsiLower ()", 46);
        TextOut (hDC, 1, tm.tmHeight * 11, AnsiLower ( (LPSTR)szAnsiBuff), 31);

  /* Convert lower case ANSI string to all upper case. */
        TextOut (hDC, 1, tm.tmHeight * 13,
            (LPSTR)"Lower Case ANSI String Processed by AnsiUpper ()", 48);
        TextOut (hDC, 1, tm.tmHeight * 14, AnsiUpper ( (LPSTR)szAnsiBuff), 31);

  /* Use AnsiNext () and AnsiPrev (). */
        TextOut (hDC, 1, tm.tmHeight * 16,
            (LPSTR)"Display String Starting at Next Char After Fifth Char Using AnsiNext ()",
            71);
        TextOut (hDC, 1, tm.tmHeight * 17, AnsiNext ( (LPSTR) & szAnsiBuff[4]),
            26);

        TextOut (hDC, 1, tm.tmHeight * 19,
            (LPSTR)"Display String Starting at Char Previous to Fifth Char Using AnsiPrev ()",
            72);
        TextOut (hDC, 1, tm.tmHeight * 20, AnsiPrev ( (LPSTR)szAnsiBuff,
            (LPSTR) & szAnsiBuff[4]), 27);

        ReleaseDC (hWnd, hDC);
        }
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return DefWindowProc (hWnd, iMessage, wParam, lParam);
    }
  return (0L);
  }
