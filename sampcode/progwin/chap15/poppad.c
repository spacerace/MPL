/*---------------------------------------
   POPPAD.C -- Popup Editor
               (c) Charles Petzold, 1990
  ---------------------------------------*/

#include <windows.h>
#include "poppad.h"
#define  EDITID 1

long FAR PASCAL WndProc (HWND, WORD, WORD, LONG);

BOOL ReadFile  (HANDLE, HWND, HWND, POFSTRUCT, char *, BOOL) ;
BOOL WriteFile (HANDLE, HWND, HWND, POFSTRUCT, char *, BOOL) ;
BOOL PrintFile (HANDLE, HWND, HWND, char *) ;

LPSTR lstrrchr (LPSTR, char) ;

char szAppName  [] = "PopPad" ;
char szFileSpec [] = "*.TXT"  ;
char szUntitled [] = "(untitled)" ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     MSG      msg;
     HWND     hwnd ;
     HANDLE   hAccel ;
     WNDCLASS wndclass ;

     if (!hPrevInstance) 
          {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
          wndclass.lpfnWndProc   = WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = LoadIcon (hInstance, szAppName) ;
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = szAppName ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }

     hwnd = CreateWindow (szAppName, NULL,
                          WS_OVERLAPPEDWINDOW,
                          GetSystemMetrics (SM_CXSCREEN) / 4,
                          GetSystemMetrics (SM_CYSCREEN) / 4,
                          GetSystemMetrics (SM_CXSCREEN) / 2,
                          GetSystemMetrics (SM_CYSCREEN) / 2,
                          NULL, NULL, hInstance, lpszCmdLine) ;

     ShowWindow (hwnd, nCmdShow) ;

     UpdateWindow (hwnd); 

     hAccel = LoadAccelerators (hInstance, szAppName) ;

     while (GetMessage (&msg, NULL, 0, 0))
          {
          if (!TranslateAccelerator (hwnd, hAccel, &msg))
               {
               TranslateMessage (&msg) ;
               DispatchMessage (&msg) ;
               }
          }
     return msg.wParam ;
     }

BOOL FAR PASCAL AboutDlgProc (HWND hDlg, WORD message, WORD wParam, LONG lParam)
     {
     switch (message)
          {
          case WM_INITDIALOG:
               return TRUE ;

          case WM_COMMAND:
               switch (wParam)
                    {
                    case IDOK:
                         EndDialog (hDlg, 0) ;
                         return TRUE ;
                    }
               break ;
          }
     return FALSE ;
     }

void DoCaption (HWND hwnd, char *szFileName)
     {
     char szCaption [40] ;

     wsprintf (szCaption, "%s - %s", (LPSTR) szAppName,
               (LPSTR) (szFileName [0] ? szFileName : szUntitled)) ;

     SetWindowText (hwnd, szCaption) ;
     }

short AskAboutSave (HWND hwnd, char *szFileName)
     {
     char  szBuffer [40] ;
     short nReturn ;

     wsprintf (szBuffer, "Save current changes: %s",
               (LPSTR) (szFileName [0] ? szFileName : szUntitled)) ;

     if (IDYES == (nReturn = MessageBox (hwnd, szBuffer, szAppName,
                                    MB_YESNOCANCEL | MB_ICONQUESTION)))

          if (!SendMessage (hwnd, WM_COMMAND, IDM_SAVE, 0L))
               return IDCANCEL ;

     return nReturn ;
     }

long FAR PASCAL WndProc (HWND hwnd, WORD message, WORD wParam, LONG lParam)
     {
     static BOOL    bNeedSave = FALSE ;
     static char    szRealFileName [16] ;
     static FARPROC lpfnAboutDlgProc ;
     static HANDLE  hInst ;
     static HWND    hwndEdit ;
     char           szFileName [16] ;
     LONG           lSelect ;
     OFSTRUCT       of ;
     WORD           wEnable ;

     switch (message)
          {
          case WM_CREATE:
               hInst = ((LPCREATESTRUCT) lParam)->hInstance ;
               lpfnAboutDlgProc = MakeProcInstance (AboutDlgProc, hInst) ;

               hwndEdit = CreateWindow ("edit", NULL,
                         WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
                              WS_BORDER | ES_LEFT | ES_MULTILINE |
                              ES_AUTOHSCROLL | ES_AUTOVSCROLL,
                         0, 0, 0, 0,
                         hwnd, EDITID, hInst, NULL) ;

               SendMessage (hwndEdit, EM_LIMITTEXT, 32000, 0L) ;

               if (lstrlen (((LPCREATESTRUCT) lParam)->lpCreateParams))
                    {
                    OpenFile (((LPCREATESTRUCT) lParam)->lpCreateParams,
                                        &of, OF_PARSE) ;
                    lstrcpy (szFileName,
                              AnsiNext (lstrrchr (of.szPathName, '\\'))) ;

                    if (ReadFile (hInst, hwnd, hwndEdit, &of,
                              szFileName, FALSE))
                         lstrcpy (szRealFileName, szFileName) ;
                    }
               DoCaption (hwnd, szRealFileName) ;
               return 0 ;

          case WM_SETFOCUS:
               SetFocus (hwndEdit) ;
               return 0 ;

          case WM_SIZE: 
               MoveWindow (hwndEdit, 0, 0, LOWORD (lParam),
                                           HIWORD (lParam), TRUE) ;
               return 0 ;

          case WM_INITMENUPOPUP:
               if (lParam == 1)
                    {
                    EnableMenuItem (wParam, IDM_UNDO,
                         SendMessage (hwndEdit, EM_CANUNDO, 0, 0L) ?
                              MF_ENABLED : MF_GRAYED) ;

                    EnableMenuItem (wParam, IDM_PASTE,
                         IsClipboardFormatAvailable (CF_TEXT) ?
                              MF_ENABLED : MF_GRAYED) ;

                    lSelect = SendMessage (hwndEdit, EM_GETSEL, 0, 0L) ;

                    if (HIWORD (lSelect) == LOWORD (lSelect))
                         wEnable = MF_GRAYED ;
                    else
                         wEnable = MF_ENABLED ;

                    EnableMenuItem (wParam, IDM_CUT,   wEnable) ;
                    EnableMenuItem (wParam, IDM_COPY,  wEnable) ;
                    EnableMenuItem (wParam, IDM_CLEAR, wEnable) ;
                    }
               return 0 ;

          case WM_COMMAND :
               if (LOWORD (lParam) && wParam == EDITID)
                    {
                    switch (HIWORD (lParam))
                         {
                         case EN_UPDATE:
                              bNeedSave = TRUE ;
                              return 0 ;

                         case EN_ERRSPACE:
                              MessageBox (hwnd, "Edit control out of space.",
                                        szAppName, MB_OK | MB_ICONSTOP) ;
                              return 0 ;
                         }
                    break ;
                    }

               switch (wParam)
                    {
                    case IDM_NEW:
                         if (bNeedSave && IDCANCEL ==
                                   AskAboutSave (hwnd, szRealFileName))
                              return 0 ;

                         SetWindowText (hwndEdit, "\0") ;
                         szRealFileName [0] = '\0' ;
                         DoCaption (hwnd, szRealFileName) ;
                         bNeedSave = FALSE ;
                         return 0 ;

                    case IDM_OPEN:
                         if (bNeedSave && IDCANCEL ==
                                   AskAboutSave (hwnd, szRealFileName))
                              return 0 ;

                         if (ReadFile (hInst, hwnd, hwndEdit, &of,
                                        szFileName, TRUE))
                              {
                              lstrcpy (szRealFileName, szFileName) ;
                              DoCaption (hwnd, szRealFileName) ;
                              bNeedSave = FALSE ;
                              }

                         return 0 ;

                    case IDM_SAVE:
                         if (szRealFileName [0])
                              {
                              if (WriteFile (hInst, hwnd, hwndEdit, &of,
                                             szRealFileName, FALSE))
                                   {
                                   bNeedSave = FALSE ;
                                   return 1 ;
                                   }
                              return 0 ;
                              }
                                                  // fall through
                    case IDM_SAVEAS:
                         if (WriteFile (hInst, hwnd, hwndEdit, &of,
                                        szFileName, TRUE))
                              {
                              lstrcpy (szRealFileName, szFileName) ;
                              DoCaption (hwnd, szFileName) ;
                              bNeedSave = FALSE ;
                              return 1 ;
                              }
                         return 0 ;

                    case IDM_PRINT:
                         PrintFile (hInst, hwnd, hwndEdit,
                              szRealFileName [0] ? szRealFileName :
                                                   szUntitled) ;
                         return 0 ;

                    case IDM_EXIT:
                         SendMessage (hwnd, WM_CLOSE, 0, 0L) ;
                         return 0 ;

                    case IDM_ABOUT:
                         DialogBox (hInst, "AboutBox", hwnd,
                                   lpfnAboutDlgProc) ;
                         return 0 ;

                    case IDM_UNDO:
                         SendMessage (hwndEdit, WM_UNDO, 0, 0L) ;
                         return 0 ;

                    case IDM_CUT:
                         SendMessage (hwndEdit, WM_CUT, 0, 0L) ;
                         return 0 ;

                    case IDM_COPY:
                         SendMessage (hwndEdit, WM_COPY, 0, 0L) ;
                         return 0 ;

                    case IDM_PASTE:
                         SendMessage (hwndEdit, WM_PASTE, 0, 0L) ;
                         return 0 ;

                    case IDM_CLEAR:
                         SendMessage (hwndEdit, WM_CLEAR, 0, 0L) ;
                         return 0 ;

                    case IDM_SELALL:
                         SendMessage (hwndEdit, EM_SETSEL, 0,
                                        MAKELONG (0, 32767)) ;
                         return 0 ;
                    }
               break ;

          case WM_CLOSE:
               if (!bNeedSave || IDCANCEL !=
                         AskAboutSave (hwnd, szRealFileName))
                    DestroyWindow (hwnd) ;

               return 0 ;

          case WM_QUERYENDSESSION:
               if (!bNeedSave || IDCANCEL !=
                         AskAboutSave (hwnd, szRealFileName))
                    return 1L ;

               return 0 ;

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
