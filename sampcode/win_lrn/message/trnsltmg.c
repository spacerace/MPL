/*
 *   TranslateMessage
 *   trnsltmg.c
 *
 *   This program demonstrates the function TranslateMessage.  A virtual-
 *   keystroke message is translated into an ascii character message and
 *   posted to the application queue, to be read the next time the
 *   application calls the GetMessage function.  The character is finally
 *   displayed on the screen.  This sequence is repeated until a
 *   WM_DESTROY message is received (the window is closed by the user).
 * 
 *   After testing this program, try removing the TranslateMessage function.
 *   You will find that the keyboard will appear dead.  None of the
 *   virtual-keystroke messages are translated into character messages.
 *   Mouse input will still be available even Without TranslateMessage.
 *
 */

#include <windows.h>
#include <string.h>
#include <stdio.h>

#define FIRST_ASCII_CHAR  0
#define LAST_ASCII_CHAR   127

/* Forward references */

int	PASCAL      WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL            TranslateMsgInit(HANDLE);
void PASCAL     PaintCharacter(HWND);
long	FAR PASCAL TranslateMsgWndProc(HWND, unsigned, WORD, LONG);

BYTE chAsciiChar = 255;             /* Current keyboard character */

int	PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, CmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int	CmdShow;
{
  MSG    msg;
  HANDLE hInst;
  HWND   hWnd;
  BOOL   bInit;

  if (!hPrevInstance)
    bInit = TranslateMsgInit(hInstance);

  hInst = hInstance;

  hWnd = CreateWindow((LPSTR)"TranslateMessage",
      (LPSTR)"TranslateMessage",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      (HWND)NULL,        /* no parent */
  (HMENU)NULL,       /* use class menu */
  (HANDLE)hInstance, /* handle to window instance */
  (LPSTR)NULL        /* no params to pass on */
  );

/* Make window visible */
  ShowWindow(hWnd, CmdShow);
  UpdateWindow(hWnd);

/* Polling messages from event queue */
  while (GetMessage((LPMSG) & msg, NULL, 0, 0))
  {
    TranslateMessage((LPMSG) & msg);
    DispatchMessage((LPMSG) & msg);
  }

  return (int)msg.wParam;
}


/* Procedure called when the application is loaded for the first time */
BOOL TranslateMsgInit(hInstance)
HANDLE hInstance;
{
  WNDCLASS   rTranslateMsgClass;

  rTranslateMsgClass.hCursor        = LoadCursor(NULL, IDC_ARROW );
  rTranslateMsgClass.hIcon          = LoadIcon(hInstance, NULL);
  rTranslateMsgClass.lpszMenuName   = (LPSTR)NULL;
  rTranslateMsgClass.lpszClassName  = (LPSTR)"TranslateMessage";
  rTranslateMsgClass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
  rTranslateMsgClass.hInstance      = hInstance;
  rTranslateMsgClass.style          = CS_HREDRAW | CS_VREDRAW;
  rTranslateMsgClass.lpfnWndProc    = TranslateMsgWndProc;

  if (!RegisterClass((LPWNDCLASS) & rTranslateMsgClass))
/* Initialization failed.
     * Windows will automatically deallocate all allocated memory.
     */
    return FALSE;

  return TRUE;        /* Initialization succeeded */
}


/* Every message for this window will be delevered right here.         */
long	FAR PASCAL TranslateMsgWndProc( hWnd, message, wParam, lParam )
HWND hWnd;
unsigned	message;
WORD wParam;
LONG lParam;
{
  switch (message)
  {
  case WM_PAINT:
    PaintCharacter(hWnd);
    break;
  case WM_CHAR:
/* The value of the key is stored in the low order byte of wParam */
    chAsciiChar = LOBYTE(wParam);
    InvalidateRect(hWnd, (LPRECT)NULL, TRUE);
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  }
  return(0L);
}


void PASCAL PaintCharacter(hWnd)
HWND hWnd;
{
  PAINTSTRUCT ps;
  HDC         hDC;
  char	szOutBuff [80];

  hDC = BeginPaint(hWnd, (LPPAINTSTRUCT) & ps);

  if ((chAsciiChar >= FIRST_ASCII_CHAR) && (chAsciiChar <= LAST_ASCII_CHAR))
  {
    sprintf(szOutBuff, "The '%c' key was pressed.",  chAsciiChar);
    TextOut(hDC, 30, 30, (LPSTR)szOutBuff, strlen(szOutBuff));
  }
  else
    TextOut(hDC, 30, 30, (LPSTR)"Please press a key", 18);
  EndPaint(hWnd, (LPPAINTSTRUCT) & ps);
}


