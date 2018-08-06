/*
 *
 *  FreeProcInstance
 *  
 *  This program demonstrates the use of the function FreeProcInstance.  
 *  This function frees the function specified bye the parameter from the
 *  data segment bound to it by the MakeProcInstance function.
 *  
 *  Other references: DlgOpen.c, DlgSave.c, Print.c
 */

#include <windows.h>
#include "procinst.h"

typedef struct  {
  int	nDummy;
} SETUPDATA;

static SETUPDATA strSetUpData;
static HANDLE hInst;
static char	szFileName[] = "procinst";
static char	szFuncName[] = "FreeProcInstance";

long	FAR PASCAL WndProc (HANDLE, unsigned, WORD, LONG);
BOOL FAR PASCAL TestFarFunc ();

int	PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int	cmdShow;
{
  HWND hWnd;
  MSG  msg;
  if (!hPrevInstance)
  {
    WNDCLASS rClass;

    rClass.style         = CS_HREDRAW | CS_VREDRAW;
    rClass.lpfnWndProc   = WndProc;
    rClass.cbClsExtra    = 0;
    rClass.cbWndExtra    = 0;
    rClass.hInstance     = hInstance;
    rClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    rClass.hIcon         = LoadIcon (hInstance, IDI_APPLICATION);
    rClass.hbrBackground = GetStockObject (WHITE_BRUSH);
    rClass.lpszMenuName  = (LPSTR) NULL;
    rClass.lpszClassName = (LPSTR) szFileName;

    RegisterClass ((LPWNDCLASS) & rClass);
  }
  else
    GetInstanceData (hPrevInstance, (PSTR) & strSetUpData,
        sizeof (SETUPDATA));

  hInst = hInstance;

  hWnd = CreateWindow ((LPSTR) szFileName, (LPSTR) szFuncName,
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0,
      CW_USEDEFAULT, 0,
      NULL, NULL, hInstance, NULL);

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  while (GetMessage((LPMSG) & msg, NULL, 0, 0))
  {
    TranslateMessage((LPMSG) & msg);
    DispatchMessage((LPMSG) & msg);
  }
  exit(msg.wParam);
}


BOOL FAR PASCAL TestFarFunc ()
{
  return TRUE;
}

long FAR PASCAL WndProc (hWnd, iMessage, wParam, lParam)
HWND     hWnd ;
unsigned iMessage ;
WORD     wParam ;
LONG     lParam ;
{
 static HANDLE hInstance;
 FARPROC  lpProc;
 HMENU hMenu;
 switch(iMessage)
 {
  case WM_CREATE:
  {
   hInstance = GetWindowWord(hWnd, GWW_HINSTANCE);
   hMenu = LoadMenu(hInstance, "ResMenu");
   SetMenu(hWnd, hMenu);
   DrawMenuBar(hWnd);
   break;
  }
  case WM_COMMAND:
  {
   switch(wParam)
   {
    case IDM_EXECUTE:
    {
    lpProc = MakeProcInstance ((FARPROC) TestFarFunc, hInst);
    MessageBox (NULL, (LPSTR)"Freeing procedure instance", (LPSTR)szFuncName, MB_OK);
    FreeProcInstance (lpProc);
    MessageBox (NULL, (LPSTR)"Instance procedure freed", (LPSTR)szFuncName, MB_OK);
    }
   }
   break;
  }
  case WM_DESTROY:
  {
   PostQuitMessage(0);
   break;
  }
  default:
  {
   return DefWindowProc (hWnd, iMessage, wParam, lParam) ;
  }
 }
 return (0L); 
}
