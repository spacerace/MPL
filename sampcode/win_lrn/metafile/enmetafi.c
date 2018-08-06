/*
 *  Function Name:   EnumMetaFile
 *  Program Name:    enmetafi.c
 *  SDK Version:         2.03
 *  Runtime Version:     2.03
 *  Microsoft C Version: 5.1
 *
 *  Description:
 *   The program below will create a metafile, close it, then
 *   enumerate it.  The enumeration will play each record in
 *   the metafile.
 */

#include "windows.h"

long FAR PASCAL WndProc(HWND, unsigned, WORD, LONG);

HANDLE hInst;

/*************************************************************************/

BOOL FAR PASCAL EnumProc(hDC, lpHTable, lpMFR, nObj, lpClientData)
HDC      hDC;
LPHANDLETABLE lpHTable;
LPMETARECORD  lpMFR;
int           nObj;
BYTE FAR *    lpClientData;
{
  PlayMetaFileRecord(hDC, lpHTable, lpMFR, nObj);
  return(1);
}

/***********************************************************************/

void CALL_EnumMetaFile(hDC)
HDC hDC;
{
  FARPROC lpprocEnumMF;
  char szstring[81];
  LOCALHANDLE hMF;
  HDC     hDCMeta;


  hDCMeta = CreateMetaFile((LPSTR) "enmetafi.met");
  Rectangle (hDCMeta, 10, 10, 400, 200);       /*  metafile records    */
  Rectangle (hDCMeta, 20, 20, 300, 180);       /*  for enumeration count  */
  TextOut (hDCMeta, 40, 40, (LPSTR)"Metafile Enumerated", 19);

  hMF = CloseMetaFile (hDCMeta);

    lpprocEnumMF = MakeProcInstance ((FARPROC) EnumProc, hInst);
    EnumMetaFile(hDC, hMF, lpprocEnumMF, (BYTE FAR *) NULL);
    FreeProcInstance ((FARPROC) lpprocEnumMF);

  return;
}

/**************************************************************************/

/* Procedure called when the application is loaded for the first time */
BOOL WinInit( hInstance )
HANDLE hInstance;
{
    WNDCLASS   wcClass;

    wcClass.style          = CS_HREDRAW | CS_VREDRAW;
    wcClass.lpfnWndProc    = WndProc;
    wcClass.cbClsExtra     =0;
    wcClass.cbWndExtra     =0;
    wcClass.hInstance      = hInstance;
    wcClass.hIcon          = LoadIcon( hInstance,NULL );
    wcClass.hCursor        = LoadCursor( NULL, IDC_ARROW );
    wcClass.hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    wcClass.lpszMenuName   = (LPSTR)NULL;
    wcClass.lpszClassName  = (LPSTR)"EnumMetaFile";

    if (!RegisterClass( (LPWNDCLASS)&wcClass ) )
        /* Initialization failed.
         * Windows will automatically deallocate all allocated memory.
         */
        return FALSE;

    return TRUE;        /* Initialization succeeded */
}


int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    MSG   msg;
    HWND  hWnd;

    if (!hPrevInstance)
        {
        /* Call initialization procedure if this is the first instance */
        if (!WinInit( hInstance ))
            return FALSE;
        }

    hWnd = CreateWindow((LPSTR)"EnumMetaFile",
                        (LPSTR)"EnumMetaFile()",
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
    hInst = hInstance;

    /* Make window visible according to the way the app is activated */
    ShowWindow( hWnd, cmdShow );
    UpdateWindow( hWnd );

    /* Polling messages from event queue */
    while (GetMessage((LPMSG)&msg, NULL, 0, 0))
        {
        TranslateMessage((LPMSG)&msg);
        DispatchMessage((LPMSG)&msg);
        }

    return (int)msg.wParam;
}

/* Procedures which make up the window class. */
long FAR PASCAL WndProc( hWnd, message, wParam, lParam )
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    PAINTSTRUCT ps;

    switch (message)
    {

    case WM_PAINT:
        BeginPaint( hWnd, (LPPAINTSTRUCT)&ps );
        CALL_EnumMetaFile(ps.hdc);
        EndPaint( hWnd, (LPPAINTSTRUCT)&ps );
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
        break;
    }
    return(0L);
}
