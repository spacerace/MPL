/*-----------------------------------------------
   FILEDLG.C -- Open and Close File Dialog Boxes
  -----------------------------------------------*/

#include <windows.h>
#include "filedlg.h"

BOOL FAR PASCAL FileOpenDlgProc (HWND, WORD, WORD, LONG) ;
BOOL FAR PASCAL FileSaveDlgProc (HWND, WORD, WORD, LONG) ;

LPSTR lstrchr  (LPSTR str, char ch) ;
LPSTR lstrrchr (LPSTR str, char ch) ;

static char      szDefExt   [5]  ;
static char      szFileName [96] ;
static char      szFileSpec [16] ;
static POFSTRUCT pof ;
static WORD      wFileAttr, wStatus ;

int DoFileOpenDlg (HANDLE hInst, HWND hwnd, char *szFileSpecIn,
                   char *szDefExtIn, WORD wFileAttrIn,
                   char *szFileNameOut, POFSTRUCT pofIn)
     {
     FARPROC lpfnFileOpenDlgProc ;
     int     iReturn ;

     lstrcpy (szFileSpec, szFileSpecIn) ;
     lstrcpy (szDefExt,   szDefExtIn) ;
     wFileAttr = wFileAttrIn ;
     pof = pofIn ;

     lpfnFileOpenDlgProc = MakeProcInstance (FileOpenDlgProc, hInst) ;

     iReturn = DialogBox (hInst, "FileOpen", hwnd, lpfnFileOpenDlgProc) ;

     FreeProcInstance (lpfnFileOpenDlgProc) ;

     lstrcpy (szFileNameOut, szFileName) ;
     return iReturn ;
     }

int DoFileSaveDlg (HANDLE hInst, HWND hwnd, char *szFileSpecIn,
                   char *szDefExtIn, WORD *pwStatusOut,
                   char *szFileNameOut, POFSTRUCT pofIn)
     {
     FARPROC lpfnFileSaveDlgProc ;
     int     iReturn ;

     lstrcpy (szFileSpec, szFileSpecIn) ;
     lstrcpy (szDefExt,   szDefExtIn) ;
     pof = pofIn ;

     lpfnFileSaveDlgProc = MakeProcInstance (FileSaveDlgProc, hInst) ;

     iReturn = DialogBox (hInst, "FileSave", hwnd, lpfnFileSaveDlgProc) ;

     FreeProcInstance (lpfnFileSaveDlgProc) ;

     lstrcpy (szFileNameOut, szFileName) ;
     *pwStatusOut = wStatus ;
     return iReturn ;
     }

BOOL FAR PASCAL FileOpenDlgProc (HWND hDlg, WORD message,
                                 WORD wParam, LONG lParam)
     {
     char  cLastChar ;
     short nEditLen ;

     switch (message)
        {
        case WM_INITDIALOG:
           SendDlgItemMessage (hDlg, IDD_FNAME, EM_LIMITTEXT, 80, 0L) ;
           DlgDirList (hDlg, szFileSpec, IDD_FLIST, IDD_FPATH, wFileAttr) ;
           SetDlgItemText (hDlg, IDD_FNAME, szFileSpec) ;
           return TRUE ;

        case WM_COMMAND:
           switch (wParam)
              {
              case IDD_FLIST:
                 switch (HIWORD (lParam))
                    {
                    case LBN_SELCHANGE:
                       if (DlgDirSelect (hDlg, szFileName, IDD_FLIST))
                          lstrcat (szFileName, szFileSpec) ;
                       SetDlgItemText (hDlg, IDD_FNAME, szFileName) ;
                       return TRUE ;

                    case LBN_DBLCLK:
                       if (DlgDirSelect (hDlg, szFileName, IDD_FLIST))
                          {
                          lstrcat (szFileName, szFileSpec) ;
                          DlgDirList (hDlg, szFileName, IDD_FLIST, IDD_FPATH,
                                                              wFileAttr) ;
                          SetDlgItemText (hDlg, IDD_FNAME, szFileSpec) ;
                          }
                       else
                          {
                          SetDlgItemText (hDlg, IDD_FNAME, szFileName) ;
                          SendMessage (hDlg, WM_COMMAND, IDOK, 0L) ;
                          }
                       return TRUE ;
                    }
                 break ;

              case IDD_FNAME:
                 if (HIWORD (lParam) == EN_CHANGE)
                    EnableWindow (GetDlgItem (hDlg, IDOK),
                       (BOOL) SendMessage (LOWORD (lParam),
                                             WM_GETTEXTLENGTH, 0, 0L)) ;
                 return TRUE ;

              case IDOK:
                 GetDlgItemText (hDlg, IDD_FNAME, szFileName, 80) ;

                 nEditLen  = lstrlen (szFileName) ;
                 cLastChar = *AnsiPrev (szFileName, szFileName + nEditLen) ;
               
                 if (cLastChar == '\\' || cLastChar == ':')
                    lstrcat (szFileName, szFileSpec) ;

                 if (lstrchr (szFileName, '*') || lstrchr (szFileName, '?'))
                    {
                    if (DlgDirList (hDlg, szFileName, IDD_FLIST,
                                          IDD_FPATH, wFileAttr))
                       {
                       lstrcpy (szFileSpec, szFileName) ;
                       SetDlgItemText (hDlg, IDD_FNAME, szFileSpec) ;
                       }
                    else
                       MessageBeep (0) ;

                    return TRUE ;
                    }

                 lstrcat (lstrcat (szFileName, "\\"), szFileSpec) ;

                 if (DlgDirList (hDlg, szFileName, IDD_FLIST,
                                                   IDD_FPATH, wFileAttr))
                    {
                    lstrcpy (szFileSpec, szFileName) ;
                    SetDlgItemText (hDlg, IDD_FNAME, szFileSpec) ;
                    return TRUE ;
                    }

                 szFileName [nEditLen] = '\0' ;

                 if (-1 == OpenFile (szFileName, pof, OF_READ | OF_EXIST))
                    {
                    lstrcat (szFileName, szDefExt) ;
                    if (-1 == OpenFile (szFileName, pof, OF_READ | OF_EXIST))
                       {
                       MessageBeep (0) ;
                       return TRUE ;
                       }
                    }
                    lstrcpy (szFileName,
                             AnsiNext (lstrrchr (pof->szPathName, '\\'))) ;
                 
                 OemToAnsi (szFileName, szFileName) ;
                 EndDialog (hDlg, TRUE) ;
                 return TRUE ;

              case IDCANCEL:
                 EndDialog (hDlg, FALSE) ;
                 return TRUE ;
              }
        }
     return FALSE ;
     }

BOOL FAR PASCAL FileSaveDlgProc (HWND hDlg, WORD message,
                                 WORD wParam, LONG lParam)
     {
     switch (message)
        {
        case WM_INITDIALOG:
           SendDlgItemMessage (hDlg, IDD_FNAME, EM_LIMITTEXT, 80, 0L) ;
           DlgDirList (hDlg, szFileSpec, 0, IDD_FPATH, 0) ;
           SetDlgItemText (hDlg, IDD_FNAME, szFileSpec) ;
           return TRUE ;

        case WM_COMMAND:
           switch (wParam)
              {
              case IDD_FNAME:
                 if (HIWORD (lParam) == EN_CHANGE)
                    EnableWindow (GetDlgItem (hDlg, IDOK),
                       (BOOL) SendMessage (LOWORD (lParam),
                                             WM_GETTEXTLENGTH, 0, 0L)) ;
                 return TRUE ;

              case IDOK:
                 GetDlgItemText (hDlg, IDD_FNAME, szFileName, 80) ;

                 if (-1 == OpenFile (szFileName, pof, OF_PARSE))
                    {
                    MessageBeep (0) ;
                    return TRUE ;
                    }

                 if (!lstrchr (AnsiNext (lstrrchr (pof->szPathName, '\\')),
                               '.'))
                    lstrcat (szFileName, szDefExt) ;

                 if (-1 != OpenFile (szFileName, pof, OF_WRITE | OF_EXIST))
                    wStatus = 1 ;

                 else if (-1 != OpenFile (szFileName, pof,
                                                  OF_CREATE | OF_EXIST))
                    wStatus = 0 ;

                 else
                    {
                    MessageBeep (0) ;
                    return TRUE ;
                    }

                 lstrcpy (szFileName,
                          AnsiNext (lstrrchr (pof->szPathName, '\\'))) ;

                 OemToAnsi (szFileName, szFileName) ;
                 EndDialog (hDlg, TRUE) ;
                 return TRUE ;

              case IDCANCEL:
                 EndDialog (hDlg, FALSE) ;
                 return TRUE ;
              }
        }
     return FALSE ;
     }

LPSTR lstrchr (LPSTR str, char ch)
     {
     while (*str)
          {
          if (ch == *str)
               return str ;

          str = AnsiNext (str) ;
          }
     return NULL ;
     }

LPSTR lstrrchr (LPSTR str, char ch)
     {
     LPSTR strl = str + lstrlen (str) ;

     do
          {
          if (ch == *strl)
               return strl ;

          strl = AnsiPrev (str, strl) ;
          }
     while (strl > str) ;

     return NULL ;
     }
