/*
 *
 *  Function demonstrated in this program: FlushComm
 *  
 *  This program demonstrates the use of the function FlushComm.
 *  This function flushes all characters from the transmit or recieve
 *  queue of the communication device specified by the first parameter.
 *  The second parameter speifies which queue (transmit or receive) to
 *  be flushed.
 *  
 */

#include <windows.h>

static char	szAppName[] = "flushcom";
static char	szFuncName[] = "FlushComm";

long    FAR PASCAL WndProc (HANDLE, unsigned, WORD, LONG);

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int	cmdShow;
  {
  HWND     hWnd;
  WNDCLASS rClass;
  MSG      msg;

  if (!hPrevInstance)
    {
    rClass.style         = CS_HREDRAW | CS_VREDRAW;
    rClass.lpfnWndProc   = WndProc;
    rClass.cbClsExtra    = 0;
    rClass.cbWndExtra    = 0;
    rClass.hInstance     = hInstance;
    rClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    rClass.hIcon         = LoadIcon (hInstance, IDI_APPLICATION);
    rClass.hbrBackground = GetStockObject (WHITE_BRUSH);
    rClass.lpszMenuName  = NULL;
    rClass.lpszClassName = szAppName;

    if (!RegisterClass (&rClass))
      return FALSE;
    }

  hWnd = CreateWindow (szAppName,
                      szFuncName,
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

  while (GetMessage ( (LPMSG) & msg, NULL, 0, 0))
    {
    TranslateMessage ( (LPMSG) & msg);
    DispatchMessage ( (LPMSG) & msg);
    }
  return (msg.wParam);
  }

long    FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd;
unsigned iMessage;
WORD     wParam;
LONG     lParam;
  {
  HMENU hMenu;
  BOOL  bError;
  int	nCid;
  int	nResult;

  switch (iMessage)
    {
    case WM_CREATE:
      hMenu = CreateMenu ();
      ChangeMenu (hMenu, NULL, (LPSTR)"Flush Port", 100, MF_APPEND);
      SetMenu (hWnd, hMenu);
      break;

    case WM_COMMAND:
      if (wParam == 100)
        {
        bError = FALSE;
        nCid = OpenComm ( (LPSTR) "LPT1", 255, 256);

        if ( (nCid & IE_BADID) == IE_BADID)
          {
          MessageBox (hWnd, (LPSTR)"Invalid or unsupported ID.",
              (LPSTR)szFuncName, MB_OK);
          bError = TRUE;
          }
        if ( (nCid & IE_BAUDRATE) == IE_BAUDRATE)
          {
          MessageBox (hWnd, (LPSTR)"Unsupported baud rate.",
              (LPSTR)szFuncName, MB_OK);
          bError = TRUE;
          }
        if ( (nCid & IE_BYTESIZE) == IE_BYTESIZE)
          {
          MessageBox (hWnd, (LPSTR)"Invalid byte size.", (LPSTR)szFuncName,
              MB_OK);
          bError = TRUE;
          }
        if ( (nCid & IE_DEFAULT) == IE_DEFAULT)
          {
          MessageBox (hWnd, (LPSTR)"Error in default parameters.",
              (LPSTR)szFuncName, MB_OK);
          bError = TRUE;
          }
        if ( (nCid & IE_HARDWARE) == IE_HARDWARE)
          {
          MessageBox (hWnd, (LPSTR)"Hardware not present.", (LPSTR)szFuncName,
              MB_OK);
          bError = TRUE;
          }
        if ( (nCid & IE_MEMORY) == IE_MEMORY)
          {
          MessageBox (hWnd, (LPSTR)"Device not open.", (LPSTR)szFuncName,
              MB_OK);
          bError = TRUE;
          }
        if ( (nCid & IE_NOPEN) == IE_NOPEN)
          {
          MessageBox (hWnd, (LPSTR)"Device not opened.", (LPSTR)szFuncName,
              MB_OK);
          bError = TRUE;
          }
        if ( (nCid & IE_OPEN) == IE_OPEN)
          {
          MessageBox (hWnd, (LPSTR)"Device already open.", (LPSTR)szFuncName,
              MB_OK);
          bError = TRUE;
          }
        if (!bError)
          {
          MessageBox (hWnd, (LPSTR)"Flushing LPT1", (LPSTR)szFuncName, MB_OK);
          nResult = FlushComm (nCid, 0);
          if (nResult == 0)
            MessageBox (hWnd, (LPSTR)"LPT1 flushed", (LPSTR)szFuncName,
                MB_OK);
          else
            MessageBox (hWnd, (LPSTR)"LPT1 not flushed", (LPSTR)szFuncName,
                MB_OK);
          }
        CloseComm (nCid);
        }
      break;

    case WM_DESTROY:
      PostQuitMessage (0);
      break;

    default:
      return (DefWindowProc (hWnd, iMessage, wParam, lParam));
    }
  return (0L);
  }
