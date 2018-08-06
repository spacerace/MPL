/*
 *  This program will demonstrate the use of the CreateCaret function.
 *  It will define a new shape for the system caret and give ownership
 *  of the system caret to the current window.
 *
 */

#include <windows.h>
#include "crcaret.h"

HANDLE hInst;

int     PASCAL WinMain (hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE  hInstance;
HANDLE  hPrevInstance;
LPSTR   lpCmdLine;
int     nCmdShow;
  {
  HWND hWnd;    /* window handle */
  MSG msg;      /* message */
  HMENU hMenu;	/* handle to the menu */

  if (!hPrevInstance)   /* app already initialized? */
    if (!CrCaretInit (hInstance))        /* nope, init it */
      return (NULL);             /* couldn't init */

  hInst = hInstance;		/* store the current instance */

  hWnd = CreateWindow ("CrCaret",  /* window class */
                      "CreateCaret Sample Application", /* window name */
                      WS_OVERLAPPEDWINDOW,              /* window style */
                      CW_USEDEFAULT,                    /* x position */
                      CW_USEDEFAULT,                    /* y position */
                      CW_USEDEFAULT,                    /* width */
                      CW_USEDEFAULT,                    /* height */
                      NULL,                             /* parent handle */
                      NULL,                             /* menu or child */
                      hInstance,                        /* instance */
                      NULL);                            /* additional info */

  if (!hWnd)
    return (NULL);

  ShowWindow (hWnd, nCmdShow);
  UpdateWindow (hWnd);

  while (GetMessage (&msg, NULL, NULL, NULL))
    {
    TranslateMessage (&msg);
    DispatchMessage (&msg);
    }
  return (msg.wParam);
  }

/* register the window */
BOOL CrCaretInit (hInstance)
HANDLE hInstance; /* current instance */
  {
  HANDLE hMemory;
  PWNDCLASS pWndClass;
  BOOL bSuccess;

  hMemory = LocalAlloc (LPTR, sizeof (WNDCLASS));
  pWndClass = (PWNDCLASS) LocalLock (hMemory);

  pWndClass->lpszClassName = (LPSTR)  "CrCaret";
  pWndClass->hInstance = hInstance;
  pWndClass->lpfnWndProc = CrCaretWndProc;
  pWndClass->style = NULL;
  pWndClass->hbrBackground = GetStockObject (WHITE_BRUSH);
  pWndClass->hCursor = LoadCursor (NULL, IDC_ARROW);
  pWndClass->hIcon = LoadIcon (NULL, IDI_APPLICATION);
  pWndClass->lpszMenuName = (LPSTR) NULL;

  bSuccess = RegisterClass (pWndClass);

  LocalUnlock (hMemory);
  LocalFree (hMemory);

  return (bSuccess);
  }


/* process messages to the window */

long    FAR PASCAL CrCaretWndProc (hWnd, message, wParam, lParam)
HWND hWnd;				/* window handle */
unsigned	message;			/* type of message */
WORD wParam;				/* additional information */
LONG lParam;				/* additional information */
  {
  FARPROC lpProcAbout;		/* pointer to "About" procedure */
  HMENU hMenu;			/* menu handle */

  switch (message)
    {
    case WM_CREATE :
          /* add command to system menu */
      hMenu = GetSystemMenu (hWnd, FALSE);
      ChangeMenu (hMenu, NULL, NULL, NULL, MF_APPEND | MF_SEPARATOR);
      ChangeMenu (hMenu, NULL, "A&bout CreateCaret...",
                  ID_ABOUT, MF_APPEND | MF_STRING);
      break;

    case WM_SYSCOMMAND :
      switch (wParam)
        {
        case ID_ABOUT:
          lpProcAbout = MakeProcInstance (About, hInst);
          DialogBox (hInst, "AboutBox", hWnd, lpProcAbout);
          FreeProcInstance (lpProcAbout);
          break;

        default:
          return (DefWindowProc (hWnd, message, wParam, lParam));
        }
      break;

    case WM_SETFOCUS:             /* create and show caret */
      CreateCaret (hWnd, NULL, 0, 5);
      SetCaretPos (20, 20);
      ShowCaret (hWnd);
      break;

    case WM_KILLFOCUS:            /* destroy the caret */
      DestroyCaret ();
      break;

    case WM_DESTROY:              /* quit application */
      PostQuitMessage (NULL);      /* notify windows */
      break;

    default:                      /* pass it on if unprocessed */
      return (DefWindowProc (hWnd, message, wParam, lParam));
    }
  return (NULL);
  }


/* this function handles the "About" box */

BOOL FAR PASCAL About (hDlg, message, wParam, lParam)
HWND hDlg;
unsigned	message;			/* type of message */
WORD wParam;				/* additional information */
LONG lParam;				/* additional information */
  {
  switch (message)
    {
    case WM_INITDIALOG:           /* init dialog box */
      return (TRUE);               /* don't need to do anything */
    case WM_COMMAND:              /* received a command */
      if (wParam == IDOK)         /* OK box selected? */
        {
        EndDialog (hDlg, NULL);           /* then exit */
        return (TRUE);
        }
      break;
    }
  return (FALSE);                        /* didn't process a message */
  }
