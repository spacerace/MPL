/*
 *
 *  DialogBoxIndirect
 *
 *  This program demonstrates the use of the function DialogBoxIndirect.
 *  It allows the user to create a modal dialogbox on the fly. Memory is
 *  dynamically allocated so that the structure will accomadate variable
 *  string sizes.
 */



#include <windows.h>
#include "diaboxin.h"

BOOL FAR PASCAL InitDiabox ( HANDLE , HANDLE , int );
long FAR PASCAL DiaboxWindowProc ( HANDLE , unsigned , WORD , LONG );
BOOL FAR PASCAL DialogBoxWindowProc (HANDLE, unsigned, WORD, LONG); /* added */

HANDLE hInst;
FARPROC lpDlgTest;
HWND hDlgTest;

#define DTHDR        struct dtHdrType

struct dtHdrType
{
   LONG  dtStyle;
   BYTE  dtItemCount;
   int   dtX;
   int   dtY;
   int   dtCX;
   int   dtCY;
   char  dtResourceName[7];
   char  dtClassName[1];
   char  dtCaptionText[1];
};

#define DTITM        struct dtItmType

struct dtItmType
{
   int   dtilX;
   int   dtilY;
   int   dtilCX;
   int   dtilCY;
   int   dtilID;
   LONG  dtilWindowStyle;
   BYTE  dtilControlClass;
   char  dtilText[1];
};


/**************************************************************************/

int PASCAL WinMain  (hInstance , hPrevInstance , lpszCmdLine , cmdShow )
HANDLE hInstance , hPrevInstance;
LPSTR  lpszCmdLine;
int cmdShow;
  {
  MSG  msg;

  InitDiabox (hInstance, hPrevInstance, cmdShow );
  while ( GetMessage((LPMSG)&msg, NULL, 0 , 0 ))
    {
        TranslateMessage((LPMSG)&msg);
        DispatchMessage((LPMSG)&msg);
    }
  exit(msg.wParam);
  }

/*******************************   initialization   ***********************/

BOOL FAR PASCAL InitDiabox (hInstance , hPrevInstance , cmdShow)
HANDLE hInstance;
HANDLE hPrevInstance;
int cmdShow;
  {
  WNDCLASS  wcDiaboxClass;
  HWND  hWnd;

  wcDiaboxClass.lpszClassName = (LPSTR) "Diabox";
  wcDiaboxClass.hInstance     = hInstance;
  wcDiaboxClass.lpfnWndProc   = DiaboxWindowProc;
  wcDiaboxClass.hCursor       = LoadCursor ( NULL , IDC_ARROW );
  wcDiaboxClass.hIcon         = NULL;
  wcDiaboxClass.lpszMenuName  = (LPSTR) "diabox";             /* menu added  */
  wcDiaboxClass.hbrBackground = GetStockObject (WHITE_BRUSH);
  wcDiaboxClass.style         = CS_HREDRAW | CS_VREDRAW;
  wcDiaboxClass.cbClsExtra    = 0;
  wcDiaboxClass.cbWndExtra    = 0;

  RegisterClass ((LPWNDCLASS) &wcDiaboxClass);
  hWnd = CreateWindow((LPSTR) "Diabox",
                      (LPSTR) "DialogBoxIndirect",
                      WS_OVERLAPPEDWINDOW,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      CW_USEDEFAULT,
                      (HWND)NULL,
                      (HMENU)NULL,
                      (HANDLE)hInstance,
                      (LPSTR)NULL
                     );

  hInst = hInstance;                    /*  instance saved for dialog box  */

  ShowWindow (hWnd , cmdShow);
  UpdateWindow (hWnd);

  return TRUE;
  }

/*********************   window procedure - process messages   *************/

long FAR PASCAL DiaboxWindowProc (hWnd , message , wParam , lParam)
HWND        hWnd;
unsigned    message;
WORD        wParam;
LONG        lParam;
  {
    PAINTSTRUCT ps;

    switch (message)
    {

    case WM_COMMAND:
        switch (wParam)
        {
        case IDDBOX:
            DisplayDialogBox(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
            break;
        }

    case WM_PAINT:
        BeginPaint (hWnd, (LPPAINTSTRUCT)&ps);
        EndPaint(hWnd, (LPPAINTSTRUCT)&ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return( DefWindowProc( hWnd , message , wParam , lParam ) );
        break;
    }
  return( 0L );
  }

/***************************************************************************/

DisplayDialogBox(hWnd)
HWND   hWnd;
{

   DTHDR    dtHdr;
   DTITM    dtItm;
   HANDLE   hDTemplate;
   LPSTR    lpDTemplate;
   WORD     wByteCount;
   BYTE     work[256];
   static   char     name[7] = "diabox";

   if(hDlgTest != NULL)
      return(FALSE);

   if(!(hDTemplate = GlobalAlloc(GMEM_MOVEABLE, (DWORD) sizeof(DTHDR))))
      return(FALSE);
   wByteCount = 0;

   dtHdr.dtStyle     = WS_POPUP | WS_VISIBLE | WS_BORDER | WS_CAPTION |
                       WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
   dtHdr.dtItemCount = 2;
   dtHdr.dtX         = 10;
   dtHdr.dtY         = 10;
   dtHdr.dtCX        = 200;
   dtHdr.dtCY        = 100;
   strcpy(dtHdr.dtResourceName, name);
   dtHdr.dtClassName[0]    = 0;
   if(!writeGMem(hDTemplate, wByteCount, (BYTE *) &dtHdr, sizeof(DTHDR)-1))
      return(FALSE);
   wByteCount += sizeof(DTHDR)-1;

   strcpy(work, "Howdy !!!");
   if(!writeGMem(hDTemplate, wByteCount, work, strlen(work)+1))
      return(FALSE);
   wByteCount += strlen(work)+1;

/* add BUTTON item */

   dtItm.dtilX     = 30;
   dtItm.dtilY     = 50;
   dtItm.dtilCX    = 32;
   dtItm.dtilCY    = 12;
   dtItm.dtilID    = 0x0200;
   dtItm.dtilWindowStyle = BS_DEFPUSHBUTTON | WS_TABSTOP | WS_CHILD | WS_VISIBLE;
   dtItm.dtilControlClass = 0x80;
   if(!writeGMem(hDTemplate, wByteCount, (BYTE *) &dtItm, sizeof(DTITM)-1))
      return(FALSE);
   wByteCount += sizeof(DTITM)-1;

   strcpy(work, "OK");
   if(!writeGMem(hDTemplate, wByteCount, work, strlen(work)+1))
      return(FALSE);
   wByteCount += strlen(work)+1;

   work[0] = 0;
   if(!writeGMem(hDTemplate, wByteCount, work, 1))
      return(FALSE);
   wByteCount += 1;

/* add MESSAGE item */

   dtItm.dtilX     = 30;
   dtItm.dtilY     = 20;
   dtItm.dtilCX    = 100;
   dtItm.dtilCY    = 8;
   dtItm.dtilID    = 0x0100;
   dtItm.dtilWindowStyle = SS_LEFT | WS_CHILD | WS_VISIBLE;
   dtItm.dtilControlClass = 0x82;
   if(!writeGMem(hDTemplate, wByteCount, (BYTE *) &dtItm, sizeof(DTITM)-1))
      return(FALSE);
   wByteCount += sizeof(DTITM)-1;

   strcpy(work, "Modal DialogBox");
   if(!writeGMem(hDTemplate, wByteCount, work, strlen(work)+1))
      return(FALSE);
   wByteCount += strlen(work)+1;

   work[0] = 0;
   if(!writeGMem(hDTemplate, wByteCount, work, 1))
      return(FALSE);

   lpDlgTest = MakeProcInstance((FARPROC) DialogBoxWindowProc, hInst);

   hDlgTest = DialogBoxIndirect (hInst, hDTemplate, hWnd, lpDlgTest);
                                             /*  function demonstrated  */
   if (hDlgTest == -1)
     MessageBox (NULL,(LPSTR)"DialogBoxIndirect failed",
                (LPSTR) "error",MB_ICONHAND);
   FreeProcInstance ((FARPROC) lpDlgTest);
   hDlgTest = NULL;
  return(TRUE);

}
/**************************************************************************/

BOOL FAR PASCAL DialogBoxWindowProc (hDlg, message, wParam, lParam)
HWND      hDlg;
unsigned  message;
WORD      wParam;
LONG      lParam;
{
  switch(message)
  {
   case WM_COMMAND:
      switch(wParam)
      {
      case 0x0100:
      case 0x0200:
         break;
      default:
         return(TRUE);
      }
      break;
   default:
      return(FALSE);
   }

   EndDialog (hDlg, TRUE);
   return(TRUE);
}


/****************************************************************************/

BOOL NEAR writeGMem(hData, offset, data, length)
HANDLE  hData;
WORD    offset;
BYTE    *data;
WORD    length;
{
   HANDLE  hNewData;
   LPSTR   lpDstData;
   int     n;

   if(offset+length > GlobalSize(hData))
   {
      if(!(hNewData = GlobalReAlloc(hData, (DWORD) offset+length, GMEM_MOVEABLE)))
         GlobalFree(hData);

      hData = hNewData;
   }

   if(lpDstData = GlobalLock(hData))
   {
      lpDstData = lpDstData + offset;
      for (n = 0 ; n < length ; n++)
         *lpDstData++ = *data++;
      GlobalUnlock(hData);
      return(TRUE);
   }

   return(FALSE);
}




