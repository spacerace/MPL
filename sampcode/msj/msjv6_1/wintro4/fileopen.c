/*===========================================================================*/
/*                                                                           */
/* File    : FILEOPEN.C                                                      */
/*                                                                           */
/* Purpose : Standard file-open dialog box processing. Taken from the        */
/*           sample code in the Windows 3.0 SDK.                             */
/*                                                                           */
/* History :                                                                 */
/*                                                                           */
/*===========================================================================*/

#include <string.h>
#include <windows.h>
#include "stock.h"

char FileName[128];
char PathName[128];
char OpenName[128];
char DefPath[128];
char DefSpec[13] = "*.sto";
char DefExt[] = ".sto";
char str[255];


HANDLE FAR PASCAL OpenDlg(hDlg, message, wParam, lParam)
  HWND hDlg;
  unsigned message;
  WORD wParam;
  LONG lParam;
{
  WORD index;
  PSTR pTptr;
  HANDLE hFile;

  switch (message)
  {
    case WM_COMMAND:
      switch (wParam)
      {
        case IDC_LISTBOX:
           switch (HIWORD(lParam))
           {
             case LBN_SELCHANGE:
               if (!DlgDirSelect(hDlg, str, IDC_LISTBOX))
               {
                 SetDlgItemText(hDlg, IDC_EDIT, str);
                 SendDlgItemMessage(hDlg,
                                    IDC_EDIT,
                                    EM_SETSEL,
                                    NULL,
                                    MAKELONG(0, 0x7fff));
               }
               else
               {
                 strcat(str, DefSpec);
                 DlgDirList(hDlg, str, IDC_LISTBOX, IDC_PATH, 0x4010);
               }
               break;

             case LBN_DBLCLK:
               goto openfile;
           }
           return TRUE;

         case IDOK:
openfile:
           GetDlgItemText(hDlg, IDC_EDIT, OpenName, 128);
           if (strchr(OpenName, '*') || strchr(OpenName, '?'))
           {
             SeparateFile(hDlg, (LPSTR) str, (LPSTR) DefSpec, (LPSTR) OpenName);
             if (str[0])
               strcpy(DefPath, str);
             ChangeDefExt(DefExt, DefSpec);
             UpdateListBox(hDlg);
             return TRUE;
           }
           if (!OpenName[0])
           {
             MessageBox(hDlg, "No filename specified.", NULL, MB_OK | MB_ICONHAND);
             return TRUE;
           }

           AddExt(OpenName, DefExt);

           /* The routine to open the file would go here, and the */
           /* handle would be returned instead of NULL.           */
           StockFileRead((LPSTR) OpenName);

           EndDialog(hDlg, hFile);
           return (TRUE);

          case IDCANCEL:
            EndDialog(hDlg, NULL);
            return (TRUE);
          }
          break;

        case WM_INITDIALOG:                        /* message: initialize    */
            UpdateListBox(hDlg);
            SetDlgItemText(hDlg, IDC_EDIT, DefSpec);
            SendDlgItemMessage(hDlg,               /* dialog handle      */
                IDC_EDIT,                          /* where to send message  */
                EM_SETSEL,                         /* select characters      */
                NULL,                              /* additional information */
                MAKELONG(0, 0x7fff));              /* entire contents      */
            SetFocus(GetDlgItem(hDlg, IDC_EDIT));
            return (FALSE); /* Indicates the focus is set to a control */
    }
    return FALSE;
}


void UpdateListBox(hDlg)
  HWND hDlg;
{
  strcpy(str, DefPath);
  strcat(str, DefSpec);
  DlgDirList(hDlg, str, IDC_LISTBOX, IDC_PATH, 0x4010);
  SetDlgItemText(hDlg, IDC_EDIT, DefSpec);
}


void ChangeDefExt(Ext, Name)
  PSTR Ext, Name;
{
  PSTR pTptr;

  pTptr = Name;
  while (*pTptr && *pTptr != '.')
    pTptr++;
  if (*pTptr)
    if (!strchr(pTptr, '*') && !strchr(pTptr, '?'))
      strcpy(Ext, pTptr);
}


void SeparateFile(hDlg, lpDestPath, lpDestFileName, lpSrcFileName)
  HWND hDlg;
  LPSTR lpDestPath, lpDestFileName, lpSrcFileName;
{
  LPSTR lpTmp;
  char  cTmp;

  lpTmp = lpSrcFileName + (long) lstrlen(lpSrcFileName);
  while (*lpTmp != ':' && *lpTmp != '\\' && lpTmp > lpSrcFileName)
    lpTmp = AnsiPrev(lpSrcFileName, lpTmp);
  if (*lpTmp != ':' && *lpTmp != '\\')
  {
    lstrcpy(lpDestFileName, lpSrcFileName);
    lpDestPath[0] = 0;
    return;
  }
  lstrcpy(lpDestFileName, lpTmp + 1);
  cTmp = *(lpTmp + 1);
  lstrcpy(lpDestPath, lpSrcFileName);
  *(lpTmp + 1) = cTmp;
  lpDestPath[(lpTmp - lpSrcFileName) + 1] = 0;
}


void AddExt(Name, Ext)
  PSTR Name, Ext;
{
  PSTR pTptr;

  pTptr = Name;
  while (*pTptr && *pTptr != '.')
    pTptr++;
  if (*pTptr != '.')
    strcat(Name, Ext);
}

