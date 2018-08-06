/**************************************************************************
 *	LOCINIT	version 1.00
 *
 * This program tests the LocalInit function, which initializes the local
 * heap at the specified segment location with the specified amount of
 * memory for the heap.
 *
 **************************************************************************/

#include <windows.h>
#include <stdio.h>

#define	HEAPSIZE	1024
#define	HEAPSEG	0

char	szPurpose[80] = "This is a program to initialize the local heap";
HANDLE  hLocMem;

/**************************************************************************/

int     PASCAL  WinMain (HANDLE, HANDLE, LPSTR, int);
BOOL              LocInit (HANDLE);
long    FAR PASCAL      LocInitWndProc (HWND, unsigned, WORD, LONG);

/***************************************************************************/

int     PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE  hInstance;
HANDLE  hPrevInstance;
LPSTR   lpszCmdLine;
int	cmdShow;
  {
  MSG	msg;
  BOOL	Init;
  HANDLE	hInst;
  HWND	hWnd;
  char	MesBuf[80];
  WORD	wMemsize;

  MessageBox (GetFocus (), (LPSTR)szPurpose, (LPSTR)"LOCINIT", MB_OK);

  if (!hPrevInstance)
/* Ensure that windows knows where to find parts of this
		 * task on disk by Registering a window class. Registering
                 * a class binds an executable name to an internal name,
		 * known to Windows. */
    if (LocInit (hInstance) == FALSE)
      return FALSE;

  hInst = hInstance;

  hWnd = CreateWindow ( (LPSTR) "locinit", /* Window class name */
  (LPSTR) "locinit", /* Window title */
  WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      (HWND)NULL,               /* No parent */
  (HMENU)NULL,       /* Use the class menu */
  (HANDLE)hInstance,  /* .EXE file for Class */
  (LPSTR)NULL           /* No Parameters */
 );

  ShowWindow (hWnd, cmdShow);
  UpdateWindow (hWnd);             /* Paint the client area */

  wMemsize = LocalSize (hLocMem);

  sprintf (MesBuf, "New size of local heap: %d\nLocated at Segment: %d",
      HEAPSIZE, HEAPSEG);

  MessageBox (hWnd, (LPSTR)MesBuf, (LPSTR)"LocalInit", MB_OK);

  MessageBox (hWnd, (LPSTR)"Initialized and Accessed Local Heap Succesfully",
      (LPSTR)"LocalInit", MB_OK);

  while (GetMessage ( (LPMSG) & msg, NULL, 0, 0))
    {
    TranslateMessage (&msg);
    DispatchMessage (&msg);
    }
  return (msg.wParam);
  }


/* WINMAIN */
/***************************************************************************/

BOOL    LocInit (hInstance)
HANDLE	hInstance;
  {
  NPWNDCLASS	npLocInitClass;
  BOOL			bLocInit;

  bLocInit = LocalInit ( (WORD)HEAPSEG, (WORD)0, (char *)HEAPSIZE);

  if (!bLocInit)
    MessageBox (GetFocus (), (LPSTR)"Local Initialization of Heap failed",
        (LPSTR)"LOCINIT", MB_ICONHAND | MB_OK);
  return FALSE;

  hLocMem = LocalAlloc (LMEM_DISCARDABLE, sizeof (WNDCLASS));
  npLocInitClass = (NPWNDCLASS)LocalLock (hLocMem);

  npLocInitClass->hCursor       = LoadCursor (NULL, IDC_ARROW);
  npLocInitClass->hIcon         = NULL;
  npLocInitClass->lpszMenuName  = (LPSTR)NULL;
  npLocInitClass->lpszClassName = (LPSTR)"LocInit";
  npLocInitClass->hbrBackground = (HBRUSH)GetStockObject (WHITE_BRUSH);
  npLocInitClass->hInstance     = hInstance;
  npLocInitClass->style         = CS_HREDRAW | CS_VREDRAW;
  npLocInitClass->lpfnWndProc   = LocInitWndProc;
  npLocInitClass->cbClsExtra    = 0;
  npLocInitClass->cbWndExtra    = 0;

  if (!RegisterClass ( (NPWNDCLASS)npLocInitClass))
/* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
    return (FALSE);

  return (TRUE);

  }


/* END locinitINIT */

/***************************** WINDOWPROC *****************************/
/*	Every message dispatched to any window of type "locinit" will
 *	result in a call to this procedure.
 */
long    FAR PASCAL LocInitWndProc (hWnd, identifier, wParam, lParam)
HWND		hWnd;		/* Intended window */
unsigned	identifier;	/* Message Number */
WORD		wParam;		/* 16 bit param */
LONG		lParam;		/* 32 bit param */
  {
  switch (identifier)
    {
    default:
      return DefWindowProc (hWnd, identifier, wParam, lParam);
      break;
    }

  return (0L);
  } /* windowproc */
