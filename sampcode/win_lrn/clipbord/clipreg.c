/*
 *
 *  RegisterClipboardFormat
 *  
 *  This program demonstrates the use of RegisterClipboardFormat function.
 *  This function registers a new clipboard format whose name is pointed to
 *  by a long pionter to a character string naming the new format. The
 *  registered format can be used in subsequent clipboard functions as valid
 *  format in which to render data, and it will appear in th clipboard's list
 *  of formats.
 */

/*************************************************************************/
/*                           INCLUDE FILES                               */
/*************************************************************************/

#include <windows.h>

/*************************************************************************/
/*                        STRUCTURE DEFINTIONS                           */
/*************************************************************************/

typedef struct
{
   int nDummy;
}
SETUPDATA;

/*************************************************************************/
/*                         GLOBAL VARIABLES                              */
/*************************************************************************/

static SETUPDATA strSetUpData;
static HANDLE hInst;
static char szFileName[] = "clipreg";
static char szFuncName[] = "RegisterClipboardFormat";

/*************************************************************************/
/*                       FORWARD REFERENCES                              */
/*************************************************************************/

long FAR PASCAL WindowProc ( HANDLE , unsigned , WORD , LONG );

/*************************************************************************/
/*                         MAIN PROCEDURE                                */
/*************************************************************************/

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
  MSG  msg;

  WindowInit (hInstance, hPrevInstance, cmdShow );

  while ( GetMessage((LPMSG)&msg, NULL, 0 , 0 ))
    {
    TranslateMessage((LPMSG)&msg);
    DispatchMessage((LPMSG)&msg);
    }
  exit(msg.wParam);
}

/*************************************************************************/
/*                      INITIALIZATION                                   */
/*************************************************************************/

BOOL WindowInit (hInstance , hPrevInstance , cmdShow)
HANDLE hInstance, hPrevInstance;
int cmdShow;
{
  HWND  hWnd;

  if ( !hPrevInstance )
     {
     WNDCLASS rClass;

     rClass.style         = CS_HREDRAW | CS_VREDRAW;
     rClass.lpfnWndProc   = WindowProc;
     rClass.cbClsExtra    = 0;
     rClass.cbWndExtra    = 0;
     rClass.hInstance     = hInstance;
     rClass.hCursor       = LoadCursor ( NULL , IDC_ARROW );
     rClass.hIcon         = LoadIcon ( hInstance, IDI_APPLICATION );
     rClass.hbrBackground = GetStockObject ( WHITE_BRUSH );
     rClass.lpszMenuName  = (LPSTR) NULL;
     rClass.lpszClassName = (LPSTR) szFileName;
   
     RegisterClass ( ( LPWNDCLASS ) &rClass );
     }
  else
     GetInstanceData ( hPrevInstance, (PSTR) &strSetUpData,
        sizeof ( SETUPDATA ) );

  hInst = hInstance;

  hWnd = CreateWindow ( (LPSTR) szFileName, (LPSTR) szFuncName,
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      CW_USEDEFAULT, CW_USEDEFAULT,
                      (HWND) NULL, (HMENU) NULL,
                      (HANDLE) hInstance, (LPSTR) NULL );

  ShowWindow ( hWnd , cmdShow );
  UpdateWindow (hWnd);

  return TRUE;
}

/*************************************************************************/
/*                 WINDOW PROCEDURE - PROCESS MESSAGES                   */
/*************************************************************************/

long FAR PASCAL WindowProc (hWnd , message , wParam , lParam)
HWND        hWnd;
unsigned    message;
WORD        wParam;
LONG        lParam;
{
    PAINTSTRUCT ps;

    switch (message)
    {

    case WM_PAINT:
        BeginPaint ( hWnd, (LPPAINTSTRUCT)&ps );
        FunctionDemonstrated ( hWnd );
        EndPaint ( hWnd, (LPPAINTSTRUCT)&ps );
        break;

    case WM_DESTROY:
        PostQuitMessage ( 0 );
        break;

    default:
        return ( DefWindowProc ( hWnd , message , wParam , lParam ) );
        break;
    }
  return ( 0L );
}

/*************************************************************************/
/*              FUNCTION DEMONSTRATED HERE - LOOK HERE                   */
/*************************************************************************/

FunctionDemonstrated ( hWnd )
HWND hWnd;
{
  WORD wFormat;

  MessageBox (NULL, (LPSTR)"Register clipboard format", (LPSTR)szFuncName,
     MB_OK);

  wFormat = RegisterClipboardFormat ( (LPSTR)"format_name");

  if ( wFormat != 0 )
      MessageBox (NULL, (LPSTR)"Format registered", (LPSTR)szFuncName, MB_OK);
  else
      MessageBox (NULL, (LPSTR)"Format can not be registered",
         (LPSTR)szFuncName, MB_OK);

  return TRUE;
}
