/*-----------------------------------
   POPPADF -- Popup Notepad File I/O
  -----------------------------------*/

#include <windows.h>
                                        // in FILEDLG.C

int DoFileOpenDlg (HANDLE, WORD, char *, char *, WORD,   char *, POFSTRUCT) ;
int DoFileSaveDlg (HANDLE, WORD, char *, char *, WORD *, char *, POFSTRUCT) ;

extern char szAppName  [] ;             // in POPPAD.C
extern char szFileSpec [] ;

long FileLength (HANDLE hFile)
     {
     long   lCurrentPos = _llseek (hFile, 0L, 1) ;
     long   lFileLength = _llseek (hFile, 0L, 2) ;
     
     _llseek (hFile, lCurrentPos, 0) ;

     return lFileLength ;
     }

void OkMessageBox (HWND hwnd, char *szString, char *szFileName)
     {
     char szBuffer [40] ;

     wsprintf (szBuffer, szString, (LPSTR) szFileName) ;

     MessageBox (hwnd, szBuffer, szAppName, MB_OK | MB_ICONEXCLAMATION) ;
     }

BOOL ReadFile (HANDLE hInstance, HWND hwnd, HWND hwndEdit, POFSTRUCT pof,
	       char *szFileName, BOOL bAskName)
     {
     DWORD  dwLength ;
     HANDLE hFile, hTextBuffer ;
     LPSTR  lpTextBuffer ;

     if (bAskName)
          {
          if (!DoFileOpenDlg (hInstance, hwnd, szFileSpec, szFileSpec + 1,
                                        0x4010, szFileName, pof))
               return FALSE ;
          }

     if (-1 == (hFile = OpenFile (szFileName, pof, OF_READ | OF_REOPEN)))
          {
          OkMessageBox (hwnd, "Cannot open file %s", szFileName) ;
          return FALSE ;
          }

     if ((dwLength = FileLength (hFile)) >= 32000)
          {
          _lclose (hFile) ;
          OkMessageBox (hwnd, "File %s too large", szFileName) ;
          return FALSE ;
          }

     if (NULL == (hTextBuffer = GlobalAlloc (GHND, (DWORD) dwLength + 1)))
          {
          _lclose (hFile) ;
          OkMessageBox (hwnd, "Cannot allocate memory for %s", szFileName) ;
          return FALSE ;
          }

     lpTextBuffer = GlobalLock (hTextBuffer) ;
     _lread (hFile, lpTextBuffer, (WORD) dwLength) ;
     _lclose (hFile) ;
     lpTextBuffer [(WORD) dwLength] = '\0' ;

     SetWindowText (hwndEdit, lpTextBuffer) ;
     GlobalUnlock (hTextBuffer) ;
     GlobalFree (hTextBuffer) ;

     return TRUE ;
     }

BOOL WriteFile (HANDLE hInstance, HWND hwnd, HWND hwndEdit, POFSTRUCT pof,
		char *szFileName, BOOL bAskName)
     {
     char      szBuffer [40] ;
     HANDLE    hFile, hTextBuffer ;
     NPSTR     npTextBuffer ;
     WORD      wStatus, wLength ;

     if (bAskName)
          {
          if (!DoFileSaveDlg (hInstance, hwnd, szFileSpec, szFileSpec + 1,
                                   &wStatus, szFileName, pof))
               return FALSE ;

          if (wStatus == 1)
               {
               wsprintf (szBuffer, "Replace existing %s", (LPSTR) szFileName) ;
               if (IDNO == MessageBox (hwnd, szBuffer, szAppName, 
                                             MB_YESNO | MB_ICONQUESTION))
                    return FALSE ;
               }
          }
     else
          OpenFile (szFileName, pof, OF_PARSE) ;

     if (-1 == (hFile = OpenFile (szFileName, pof, OF_CREATE | OF_REOPEN)))
          {
          OkMessageBox (hwnd, "Cannot create file %s", szFileName) ;
          return FALSE ;
          }

     wLength = GetWindowTextLength (hwndEdit) ;
     hTextBuffer = (HANDLE) SendMessage (hwndEdit, EM_GETHANDLE, 0, 0L) ;
     npTextBuffer = LocalLock (hTextBuffer) ;

     if (wLength != _lwrite (hFile, npTextBuffer, wLength))
          {
          _lclose (hFile) ;
          OkMessageBox (hwnd, "Cannot write file %s to disk", szFileName) ;
          return FALSE ;
          }

     _lclose (hFile) ;
     LocalUnlock (hTextBuffer) ;

     return TRUE ;
     }
