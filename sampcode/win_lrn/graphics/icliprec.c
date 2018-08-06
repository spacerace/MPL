/*
 *
 *  IntersectClipRect
 *
 *  This program demonstrates the use of the IntersectClipRect function.
 *  IntersectClipRect creates a new clipping region for the specified DC
 *  by taking the intersection of the current clipping region and the
 *  rectangle represented by the four values sent it. IntersectClipRect
 *  is called from FunctionDemonstrated in this sample application.
 */

#include <windows.h>
HWND hWnd;

BOOL FAR PASCAL InitMaster (HANDLE, HANDLE, int);
long FAR PASCAL MasterWindowProc (HANDLE, unsigned, WORD, LONG);
void FAR PASCAL FunctionDemonstrated ();

/***********************   main procedure   *******************************/

int     PASCAL WinMain  (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int	cmdShow;
  {
  MSG  msg;

  InitMaster (hInstance, hPrevInstance, cmdShow);

  FunctionDemonstrated ();

  while (GetMessage ( (LPMSG) & msg, NULL, 0, 0))
    {
    TranslateMessage ( (LPMSG) & msg);
    DispatchMessage ( (LPMSG) & msg);
    }
  exit (msg.wParam);
  }


/*******************************   initialization   ***********************/

BOOL FAR PASCAL InitMaster (hInstance, hPrevInstance, cmdShow)
HANDLE hInstance;
HANDLE hPrevInstance;
int	cmdShow;
  {
  WNDCLASS  wcMasterClass;
  HMENU     hMenu;

  wcMasterClass.lpszClassName = (LPSTR) "Master";
  wcMasterClass.hInstance     = hInstance;
  wcMasterClass.lpfnWndProc   = MasterWindowProc;
  wcMasterClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
  wcMasterClass.hIcon         = NULL;
  wcMasterClass.lpszMenuName  = (LPSTR) NULL;
  wcMasterClass.hbrBackground = GetStockObject (WHITE_BRUSH);
  wcMasterClass.style         = CS_HREDRAW | CS_VREDRAW;
  wcMasterClass.cbClsExtra    = 0;
  wcMasterClass.cbWndExtra    = 0;

  RegisterClass ( (LPWNDCLASS) & wcMasterClass);

  hMenu = CreateMenu ();
  ChangeMenu (hMenu, NULL, "Execute Function", 100, MF_APPEND);

  hWnd = CreateWindow ( (LPSTR) "Master", (LPSTR) "IntersectClipRect",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, 0,
                      CW_USEDEFAULT, 0,
                      NULL,hMenu , hInstance, NULL);

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);

  return TRUE;
  }

/*********************   window procedure - process messages   *************/
long    FAR PASCAL MasterWindowProc (hWnd, message, wParam, lParam)
HWND        hWnd;
unsigned    message;
WORD	    wParam;
LONG        lParam;
  {
  switch (message)
    {
    case WM_COMMAND:
      if (wParam == 100)
        FunctionDemonstrated ();
      else
        return (DefWindowProc (hWnd, message, wParam, lParam));
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

/****************************   function demonstrated   ********************/

void FAR PASCAL FunctionDemonstrated ()
  {
  HDC hDC = GetDC (hWnd);
  HRGN hRgn;		 /* handle to first clipping region    */
  RECT ClientRect;
  POINT RectPoint[2];
  short	nNewClipRect;	 /* return value from IntersectClipRect */

  GetClientRect (hWnd, (LPRECT) & ClientRect); /* load device coordinates */

  SetMapMode (hDC, MM_ANISOTROPIC);
  SetWindowOrg (hDC, 0, 0);
  SetWindowExt (hDC, 120, 120);
  SetViewportOrg (hDC, 0, 0);
  SetViewportExt (hDC, (short) ClientRect.right, (short) ClientRect.bottom);

  RectPoint[0].x = 0;
  RectPoint[0].y = 0;
  RectPoint[1].x = 100;
  RectPoint[1].y = 100;

  LPtoDP (hDC, (LPPOINT) & RectPoint[0].x, 2);

  hRgn = CreateRectRgn ( (short) RectPoint[0].x,
      (short) RectPoint[0].y,
      (short) RectPoint[1].x,
      (short) RectPoint[1].y);

  SelectClipRgn (hDC, hRgn);

  MessageBox (GetFocus (), (LPSTR)"into the first clipping region",
      (LPSTR)"I am about to do a TextOut and InvertRect...",
      MB_OK);

  TextOut (hDC, 0, 5,
      "THIS LINE HERE IS VERY LONG . WILL IT ALL BE SHOWN IN THE CLIPPING REGION?",
      (short)75);
  InvertRect (hDC, (LPRECT) & ClientRect);

/*** call IntersectClipRect with first clipping region and new coordinates ***/

  RectPoint[0].x = 5;
  RectPoint[0].y = 5;
  RectPoint[1].x = 20;
  RectPoint[1].y = 20;

  LPtoDP (hDC, (LPPOINT) & RectPoint[0].x, 2);

  nNewClipRect = IntersectClipRect (hDC,
      (short)RectPoint[0].x,
      (short)RectPoint[0].y,
      (short)RectPoint[1].x,
      (short)RectPoint[1].y);

  if (nNewClipRect == ERROR)
    MessageBox (GetFocus (), (LPSTR)"The DC sent IntersectClipRect is not valid",
        (LPSTR)NULL,
        MB_OK);

  MessageBox (GetFocus (), (LPSTR)"into the intersected clipping region",
      (LPSTR)"I am about to do a TextOut and InvertRect...",
      MB_OK);

  TextOut (hDC, 0, 15,
      "THIS LINE HERE IS VERY LONG . WILL IT ALL BE SHOWN IN THE CLIPPING REGION?",
      (short)75);
  InvertRect (hDC, (LPRECT) & ClientRect);

  ReleaseDC (hWnd, hDC);
  DeleteObject (hRgn);

  return;
  }
