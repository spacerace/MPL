/***************************************************************************
 *                                                                         *
 *  PROGRAM     : NovaNew.c                                                *
 *                                                                         *
 *  PURPOSE     : Extends Word for Windows "New" function by providing     *
 *                two dialogs to facilitate the selection of a template    *
 *                                                                         *
 *  FUNCTIONS   : LibMain()           - Generalized library initialization *
 *                                      function.  Used to retrieve        *
 *                                      handle.                            *
 *                                                                         *
 *                WEP ()              - Generalized library termination    *
 *                                      function.  Returns true.           *
 *                                                                         *
 *                NovaNew()           - Entry point for calls from WinWord *
 *                                                                         *
 *                fnNew()             - Dialog function                    *
 *                                                                         *
 *                ExtractTemplate()   - Extracts the last word and         *
 *                                      validates that it is the name      *
 *                                      of a template file                 *
 *                                                                         *
 *                LastWord()          - Extract the last word from a       *
 *                                      listbox entry.                     *
 *                                                                         *
 *                FillListBox()       - Reads the contents of a file into  *
 *                                      a listbox.                         *
 *                                                                         *
 ***************************************************************************/

#include "windows.h"
#include "novanew.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>


int FAR PASCAL LibMain(HANDLE hModule,WORD wDataSeg,WORD cbHeapSize,LPSTR lpszCmdLine);
int FAR PASCAL WEP (BOOL bSystemExit);
int NovaNew(LPSTR lpTemplate);
int far PASCAL fnNew(HWND hwnd,unsigned msg,WORD wParam,LONG lParam);
BOOL ExtractTemplate(void);
int LastWord(PSTR szString);
BOOL FillListBox(HWND hwnd, int nControl,PSTR szFileName);

#define CCHLBMAX 3000        /* Maximum size of listbox data */
#define CCHBUFFERMAX 200   /* Maximum size of text in the template listbox */
char     rgchBuffer[CCHBUFFERMAX+1]; /* General purpose buffer */
char     rgchTemplate[CCHBUFFERMAX+1];
HANDLE hInstance;            /* Library instance handle */

/****************************************************************************
   FUNCTION: LibMain(HANDLE, WORD, WORD, LPSTR)

   PURPOSE:  Is called by LibEntry.  LibEntry is called by Windows when
             the DLL is loaded.  The LibEntry routine is provided in
             the LIBENTRY.OBJ in the SDK Link Libraries disk.  (The
             source LIBENTRY.ASM is also provided.)

             LibEntry initializes the DLL's heap, if a HEAPSIZE value is
             specified in the DLL's DEF file.  Then LibEntry calls
             LibMain.  The LibMain function below satisfies that call.

             The LibMain function should perform additional initialization
             tasks required by the DLL.  In this example, no initialization
             tasks are required.  LibMain should return a value of 1 if
             the initialization is successful.

*******************************************************************************/
int FAR PASCAL LibMain(HANDLE hModule,WORD wDataSeg,WORD cbHeapSize,LPSTR lpszCmdLine)
{
    hInstance = hModule;
    return 1;
}


/****************************************************************************
    FUNCTION:  WEP(int)

    PURPOSE:  Performs cleanup tasks when the DLL is unloaded.  WEP() is
              called automatically by Windows when the DLL is unloaded (no
              remaining tasks still have the DLL loaded).  It is strongly
              recommended that a DLL have a WEP() function, even if it does
              nothing but returns success (1), as in this example.

*******************************************************************************/
int FAR PASCAL WEP (BOOL bSystemExit)
{
    return(TRUE);
}

int NovaNew(LPSTR lpTemplate)
{
    int ret;

    rgchTemplate[0] = 0;
    lpTemplate[0] = 0;
    ret = DialogBox(hInstance, "GET_TEMPLATE", GetActiveWindow(),
                    (FARPROC)fnNew);
    lstrcpy (lpTemplate, rgchTemplate);
    return ret;
}

/*--------------------------------------------------------------------*/
/* ** Dialog function for NovaNew */
int far PASCAL fnNew(HWND hwnd,unsigned msg,WORD wParam,LONG lParam)
{
   LONG lSel;
   int nLen;

   switch (msg)
   {
      case WM_INITDIALOG:
         FillListBox(hwnd, GET_TEMPLATE_GRP_LB, "TEMPLATE.LB");
         rgchTemplate[0] = 0;
         break;

      case WM_COMMAND:
         switch (wParam)
         {
            case IDOK:
               if (ExtractTemplate())
                  EndDialog(hwnd, wParam);
               return TRUE;

            case IDCANCEL:
               EndDialog(hwnd, wParam);
               return TRUE;

            case GET_TEMPLATE_LB:
               if (HIWORD(lParam) == LBN_DBLCLK ||
                   HIWORD(lParam) == LBN_SELCHANGE)
               {
                  lSel = SendDlgItemMessage(hwnd, wParam,
                                   LB_GETCURSEL,0,0L);
                  nLen = (int)SendDlgItemMessage(hwnd, wParam,
                                   LB_GETTEXTLEN, LOWORD(lSel), 0L);
                  rgchTemplate[0] = 0;
                  if (nLen < sizeof(rgchTemplate))
                      SendDlgItemMessage(hwnd, wParam, LB_GETTEXT,
                                   LOWORD(lSel),
                                   (LONG)(LPSTR)rgchTemplate);
                  if ((HIWORD(lParam) == LBN_DBLCLK) &&
                      (ExtractTemplate()))
                     EndDialog(hwnd, IDOK);
               }
               return TRUE;

            case GET_TEMPLATE_GRP_LB:
               if (HIWORD(lParam) == LBN_SELCHANGE)
               {
                  lSel = SendDlgItemMessage(hwnd, wParam,
                                   LB_GETCURSEL,0,0L);
                  nLen = (int)SendDlgItemMessage(hwnd, wParam,
                                   LB_GETTEXTLEN, LOWORD(lSel), 0L);
                  rgchTemplate[0] = 0;
                  if (nLen < sizeof(rgchTemplate))
                      SendDlgItemMessage(hwnd, wParam, LB_GETTEXT,
                                   LOWORD(lSel),
                                   (LONG)(LPSTR)rgchBuffer);
                  LastWord(rgchBuffer);
                  strcat(rgchBuffer, ".LB");
                  /* if a new model name */
                  if (strcmp(rgchTemplate, rgchBuffer) != 0)
                  {
                     strcpy(rgchTemplate, rgchBuffer);
                     FillListBox(hwnd, GET_TEMPLATE_LB, rgchTemplate);
                  }
                  return TRUE;
               }
               else
                  return FALSE;
               break;

            default:
               return FALSE;
         }
         break;

      default:
         return FALSE;
   }
   return TRUE;
}

BOOL ExtractTemplate(void)
{
   OFSTRUCT rgbOpen;
   int      fp;

   LastWord(rgchTemplate);
   strcat(rgchTemplate, ".DOT");

   fp = OpenFile(rgchTemplate, (LPOFSTRUCT)&rgbOpen, OF_EXIST);
   if (fp == -1)
      return FALSE;
   return TRUE;
}

/* Extract the word following at least two blanks */
int LastWord(PSTR szString)
{
   PSTR p, p2;

   for (p = szString; *p && !(*p == ' ' && *(p+1) == ' '); p++)
      ;
   while (*p == ' ')
      p++;
   for (p2 = p; isalnum(*p2); p2++)
      ;
   *p2 = 0;
   memcpy(szString, p, strlen(p) + 1);
   return strlen(szString);
}

BOOL FillListBox(HWND hwnd, int nControl,PSTR szFileName)
{
   int      fp;
   long     lcch;
   LPSTR    lpch;
   LPSTR    lpchItem;
   HANDLE   hLBData;
   WORD     cch;
   HWND     hCtl;
   char     ch;
   OFSTRUCT rgbOpen;

   hCtl=GetDlgItem(hwnd,nControl);
   SendMessage(hCtl, LB_RESETCONTENT, 0, 0L);
   InvalidateRect(hCtl, NULL, TRUE);
   fp=_lopen(szFileName, READ);
   if (fp == -1)
   {
      fp = OpenFile(szFileName, (LPOFSTRUCT)&rgbOpen ,OF_READ);
      if (fp == -1)
         return FALSE;
   }
   /* Get size of file, then reset file pointer */
   lcch = _llseek(fp, 0L, 2);
   cch = LOWORD(lcch);
   _llseek(fp, 0L, 0);

   hLBData = GlobalAlloc(GMEM_MOVEABLE, (LONG)cch+1);
   /* If not enough memory, quit */
   if (!hLBData)
   {
      _lclose(fp);
      return FALSE;
   }
   /* Read file into buffer */
   if (!(lpch = GlobalLock(hLBData)))
   {
      _lclose(fp);
      GlobalFree(hLBData);
      return FALSE;
   }
   _lread(fp, lpch, cch);
   _lclose(fp);
      if (cch && *(lpch+cch-1)==0x1a)
         cch--; /* get rid of eof */
   *(lpch + cch) = 0;      /* zero terminate the thing */
   SendMessage(hCtl, WM_SETREDRAW, (WORD)FALSE, 0L);
   for (lpchItem = lpch; *lpchItem; lpch++)
   {
      if (*lpch==0x0a || *lpch==0x0d || *lpch==0)
      {
         ch=*lpch;
         if ((lpch-lpchItem) < CCHBUFFERMAX)
            *lpch=0;
         else
            *(lpchItem+CCHBUFFERMAX)=0; /* Max length */
         SendMessage(hCtl, LB_ADDSTRING, (WORD)0, (LONG)lpchItem);
         *lpch=ch;
         if (*lpch)
            while (*lpch==0x0a || *lpch==0x0d)
               lpch++;
         lpchItem=lpch;
      }
   }
   SendMessage(hCtl, WM_SETREDRAW, (WORD)TRUE, 0L);
   InvalidateRect(hCtl,(LPRECT)NULL,TRUE);
   GlobalUnlock(hLBData);
   GlobalFree(hLBData);
   return TRUE;
}

