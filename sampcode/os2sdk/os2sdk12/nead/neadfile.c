/*************************************************************

 This module contains subroutines for nead.c that specifically
 deal with the Open file interface.  Most of the routines were
 taken from Charles Petzold's book "Programming the OS/2
 Presentation Manager" and were slightly modified.

 Procedures in this file:
   OpenFile()          Asks user for new file name and opens it
   OpenFileProc()      Dialog proc that prompts user for file name
   FillDirListBox()    Fills the directory list box
   FillFileListBox()   Fills the file list box

**************************************************************/
#include "nead.h"

/************ External GLOBALS *******************************/

extern CHAR szFileName[CCHMAXPATH];
extern CHAR szEAName[MAXEANAME+1];
extern USHORT usRetEAType;
extern BOOL FILE_ISOPEN;
extern BOOL FILE_CHANGED;
extern BOOL COMMAND_LINE_FILE;
extern HHEAP hhp;
extern CHAR *pAlloc,*szEditBuf,*szAscii,*szScratch;
extern HOLDFEA *pHoldFEA;
extern DELETELIST *pDelList;
extern EADATA ConvTable[EATABLESIZE];

/*************************************************************/


/*
 * Function name: OpenFile()
 *
 * Parameters:  hwnd which is the current window handle.
 *              usMode which will attempt to load the file from the command
 *              line iff usMode == ARGFILE which is set in main().  Otherwise,
 *              the selector box is brought up for the user to select from.
 *
 * Returns: TRUE iff a file is open upon exit.
 *
 * Purpose: This routine handles opening a new file.  It will also query the
 *          user for the disposition of the current file if it has been
 *          modified before loading the new file.
 *
 * Usage/Warnings:  Routine should be bullet proof as it does its own
 *                  error checking.  It assumes that hwnd points to the
 *                  correct window with the name listbox in it.
 *
 * Calls: WriteEAs(), Free_FEAList()
 */

BOOL OpenFile(HWND hwnd,USHORT usMode)
{
   CHAR szOldFile[CCHMAXPATH];
   USHORT usRet;

   strcpy(szOldFile,szFileName);  /* Save name of the currently open file */

   if(usMode != ARGFILE)          /* It isn't the command line file */
   {
      if(!WinDlgBox(HWND_DESKTOP, /* Get the file name from the user */
                    hwnd,
                    OpenFileProc,
                    (HMODULE) NULL,
                    IDD_OPENBOX,
                    NULL))
      {
         strcpy(szFileName,szOldFile); /* They canceled, restore old file */
         return(FILE_ISOPEN);
      }
   }

   if(FILE_CHANGED) /* Give them a chance to save modifications */
   {
      usRet=WinMessageBox(HWND_DESKTOP,hwnd,
                          "The current file has been changed.  Do you \
wish to save the changes before proceeding?",
                          "Warning",0,MB_YESNOCANCEL | MB_ICONQUESTION);
      switch(usRet)
      {
         case MBID_YES:
            WriteEAs(hwnd);
            break;
         case MBID_CANCEL:
            return FILE_ISOPEN;
      }
   }

   if(FILE_ISOPEN) /* Free up everything associated with the current file */
   {
      Free_FEAList(pHoldFEA,pDelList);
      FILE_ISOPEN = FALSE;
   }

   if(QueryEAs(hwnd,szFileName)) /* We were successful */
   {
      HOLDFEA *pFEA=pHoldFEA;

      FILE_ISOPEN = TRUE;
      FILE_CHANGED = FALSE;

      WinSendDlgItemMsg(hwnd, IDD_LBOX, LM_DELETEALL,0L,0L); /* Fill L-box */

      while(pFEA)
      {
         WinSendDlgItemMsg(hwnd, IDD_LBOX, LM_INSERTITEM,
                           MPFROM2SHORT(LIT_END,0),
                           MPFROMP(pFEA->szName));
         pFEA = pFEA->next;
      }
   }
   else /* We couldn't query the EAs */
   {
      *szFileName = '\000';
      WinSetDlgItemText(hwnd,IDD_FNAME,szFileName);
      return(FILE_ISOPEN = FALSE);
   }
   WinSetDlgItemText(hwnd,IDD_FNAME,szFileName);
   pDelList = NULL;
   return(TRUE);
}


/*
 * Function name: OpenFileProc()
 *
 * Parameters:  hwnd, msg, mp1, mp2.  Standard PM Dialog Proc params.
 *              Expects no user pointer.
 *
 * Returns: TRUE if user selects OK, FALSE if Cancel is selected.
 *
 * Purpose: This proc handles the user interface to select a file name.
 *          Some elementary checks are done to make sure the filename is
 *          valid.
 *
 * Usage/Warnings:  The interface is NOT foolproof as it is possible to
 *                  continue with a non-existant file name.  Also, users
 *                  are not currently allowed to view/edit the EAs attached
 *                  to a directory.
 *
 * Calls: FillDirListBox(), FillFileListBox(), ParseFileName()
 */

MRESULT EXPENTRY OpenFileProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   static CHAR szCurrentPath[CCHMAXPATH],szBuffer[CCHMAXPATH];
   CHAR        szParsedPath[CCHMAXPATH];
   SHORT       sSelect;

   switch(msg)
   {
      case WM_INITDLG:
         FillDirListBox(hwnd,szCurrentPath);
         FillFileListBox(hwnd);
         WinSendDlgItemMsg(hwnd, IDD_FILEEDIT,EM_SETTEXTLIMIT,
                                 MPFROM2SHORT(260,0),NULL);
         return 0L;

      case WM_CONTROL:
         if(SHORT1FROMMP(mp1) == IDD_DIRLIST ||  /* An lbox item is selected */
            SHORT1FROMMP(mp1) == IDD_FILELIST)
         {
            sSelect = SHORT1FROMMR( WinSendDlgItemMsg(hwnd, /* Get item->szBuffer */
                               SHORT1FROMMP(mp1),
                               LM_QUERYSELECTION, 0L, 0L));

            WinSendDlgItemMsg(hwnd, SHORT1FROMMP(mp1),
                              LM_QUERYITEMTEXT,
                              MPFROM2SHORT(sSelect, sizeof szBuffer),
                              MPFROMP(szBuffer));
         }

         switch(SHORT1FROMMP(mp1))
         {
            case IDD_DIRLIST:              /* Item was in the directory lbox */
               switch(SHORT2FROMMP(mp1))
               {
                  case LN_ENTER:           /* Go to the select drive/dir */
                     if(*szBuffer == ' ')
                        DosSelectDisk(*(szBuffer+1) - '@');
                     else
                        DosChDir(szBuffer, 0L);

                     FillDirListBox(hwnd, szCurrentPath);
                     FillFileListBox(hwnd);

                     WinSetDlgItemText(hwnd, IDD_FILEEDIT, "");
                     return 0L;
               }
               break;

            case IDD_FILELIST:             /* Item was in the file lbox */
               switch(SHORT2FROMMP(mp1))
               {
                  case LN_SELECT:          /* Copy name to entry field  */
                     WinSetDlgItemText(hwnd, IDD_FILEEDIT, szBuffer);
                     return 0L;

                  case LN_ENTER:           /* Try to query the file */
                     if(ParseFileName(szFileName, szBuffer) != FILE_VALID)
                        return 0; /* Some error, don't finish */
                     WinDismissDlg(hwnd, TRUE);
                     return 0L;
               }
               break;
         }
         break;

      case WM_COMMAND:
         switch(COMMANDMSG(&msg)->cmd)
         {
            case DID_OK:            /* Try to query file in the entry field */
               WinQueryDlgItemText(hwnd, IDD_FILEEDIT,
                                   sizeof szBuffer, szBuffer);

               switch(ParseFileName(szParsedPath, szBuffer))
               {
                  case FILE_INVALID:    /* Can't open the file */
                     WinAlarm(HWND_DESKTOP, WA_ERROR);
                     FillDirListBox(hwnd, szCurrentPath);
                     FillFileListBox(hwnd);
                     return 0L;

                  case FILE_PATH:    /* It was an incomplete path name */
                     strcpy(szCurrentPath,szBuffer);
                     FillDirListBox(hwnd, szCurrentPath);
                     FillFileListBox(hwnd);
                     WinSetDlgItemText(hwnd, IDD_FILEEDIT, "");
                     return 0L;

                  case FILE_VALID:    /* It was valid */
                     strcpy(szFileName, szParsedPath);
                     WinDismissDlg(hwnd, TRUE);
                     return 0L;
               }
               break;

            case DID_CANCEL:
               WinDismissDlg(hwnd, FALSE);
               return 0L;
         }
         break;
   }
   return WinDefDlgProc(hwnd, msg, mp1, mp2);
}


/*
 * Function name: FillDirListBox()
 *
 * Parameters:  hwnd points to the current window handle
 *              pcCurrentPath points to a buffer which will be filled in
 *              with the current path.
 *
 * Returns: VOID
 *
 * Purpose: This routine is called by OpenFileProc to fill in the directory
 *          list box
 *
 * Usage/Warnings:  Adequete error checking is NOT done on the return
 *                  values of the system calls.  Also, it is remotely
 *                  possible that the calls to add to the list box could fail.
 *
 * Calls:
 */

VOID FillDirListBox(HWND hwnd, CHAR *pcCurrentPath)
{
   static CHAR szDrive [] = "  :";
   FILEFINDBUF findbuf;
   HDIR        hDir = 1;
   SHORT       sDrive;
   USHORT      usDriveNum, usCurPathLen, usSearchCount = 1;
   ULONG       ulDriveMap;

   DosQCurDisk(&usDriveNum, &ulDriveMap);
   *pcCurrentPath     = (CHAR)((CHAR) usDriveNum + '@');
   *(pcCurrentPath+1) = ':';
   *(pcCurrentPath+2) = '\\';
   usCurPathLen = CCHMAXPATH;
   DosQCurDir(0, pcCurrentPath + 3, &usCurPathLen);

   WinSetDlgItemText(hwnd, IDD_PATH, pcCurrentPath);
   WinSendDlgItemMsg(hwnd, IDD_DIRLIST, LM_DELETEALL, NULL, NULL);

   for(sDrive = ('A'-'A'); sDrive <= ('Z'-'A'); sDrive++)
   {
      if(ulDriveMap & (1L << sDrive))
      {
         *(szDrive+1) = (CHAR)((CHAR) sDrive + 'A');

         WinSendDlgItemMsg(hwnd, IDD_DIRLIST, LM_INSERTITEM,
                           MPFROM2SHORT(LIT_END, 0),
                           MPFROMP(szDrive));
      }
   }
   DosFindFirst("*", &hDir, FILE_DIRECTORY | FILE_ALL, &findbuf,
                sizeof findbuf, &usSearchCount, 0L);
   while(usSearchCount)
   {
      if((findbuf.attrFile & FILE_DIRECTORY) &&
         (findbuf.achName[0] != '.' || findbuf.achName[1]))

         WinSendDlgItemMsg(hwnd, IDD_DIRLIST, LM_INSERTITEM,
                           MPFROM2SHORT(LIT_SORTASCENDING, 0),
                           MPFROMP(findbuf.achName));

      if(DosFindNext(hDir, &findbuf, sizeof findbuf, &usSearchCount))
         break;
   }
}


/* This routine is called by OpenFileProc to fill the file list box */

/*
 * Function name: FillFileListBox()
 *
 * Parameters:  hwnd points to the current window handle
 *
 * Returns: VOID
 *
 * Purpose: This routine is called by OpenFileProc to fill in the file
 *          list box with files in the current directory.
 *
 * Usage/Warnings:  Adequete error checking is NOT done on the return
 *                  values of the system calls.  Also, it is remotely
 *                  possible that the calls to add to the list box could fail.
 *
 * Calls:
 */

VOID FillFileListBox(HWND hwnd)
{
   FILEFINDBUF findbuf;
   HDIR        hDir = 1;
   USHORT      usSearchCount = 1; /* Read 1 entry at a time */

   WinSendDlgItemMsg(hwnd, IDD_FILELIST, LM_DELETEALL, NULL, NULL);

   DosFindFirst("*", &hDir, FILE_ALL, &findbuf, sizeof findbuf,
                       &usSearchCount, 0L);
   while(usSearchCount)
   {
      WinSendDlgItemMsg(hwnd, IDD_FILELIST, LM_INSERTITEM,
                        MPFROM2SHORT(LIT_SORTASCENDING, 0),
                        MPFROMP(findbuf.achName));

      if(DosFindNext(hDir, &findbuf, sizeof findbuf, &usSearchCount))
         break;
   }
}


/*
 * Function name: ParseFileName()
 *
 * Parameters:  pcOut points to a buffer for the return file specification.
 *              pcIn  points to the buffer containing the raw input spec.
 *
 * Returns: FILE_INVALID if pcIn had invalid drive or no directory
 *          FILE_PATH    if pcIn was empty or had just a path/no file name.
 *          FILE_VALID   if pcIn point to good file.
 *
 * Purpose: This routine changes drive and directory as per pcIn string.
 *
 * Usage/Warnings:  Note that pcOut is only valid if FILE_VALID is returned.
 *                  in place of strupr(), a codepage should be fetched and
 *                  DosCaseMap() should be used to allow for extended chars.
 *                  This routine could use some cleanup work.
 *
 * Calls:
 */

SHORT ParseFileName(CHAR *pcOut, CHAR *pcIn)
{
   CHAR   *pcLastSlash, *pcFileOnly ;
   ULONG  ulDriveMap ;
   USHORT usDriveNum, usDirLen = CCHMAXPATH;

   strupr(pcIn);  /* Does NOT handle extended chars, should use DosCaseMap */

   if(*pcIn == '\000')  /* If string is empty, return FILE_PATH */

      return FILE_PATH;

   /* Get drive from input string or use current drive */

   if(*(pcIn+1) == ':') /* Yup, they specified a drive */
   {
      if(DosSelectDisk(*pcIn - '@')) /* Change to selected drive */
         return FILE_INVALID;
      pcIn += 2;
   }
   DosQCurDisk(&usDriveNum, &ulDriveMap); /* Get current drive */

   *pcOut++ = (CHAR)((CHAR) usDriveNum + '@'); /* Build drive letter */
   *pcOut++ = ':';
   *pcOut++ = '\\';

   if(*pcIn == '\000') /* If rest of the string is empty, return FILE_PATH */
      return FILE_PATH;

   /* Search for the last backslash.  If none, it could be a directory. */

   if(!(pcLastSlash = strrchr(pcIn, '\\'))) /* No slashes? */
   {
      if(!DosChDir(pcIn, 0L))
         return FILE_PATH;            /* It was a directory */

      DosQCurDir(0, pcOut, &usDirLen); /* Get current dir & attach input fn */

      if(*(pcOut+strlen(pcOut)-1) != '\\')
         strcat(pcOut++, "\\");

      strcat(pcOut, pcIn);
      return FILE_VALID;
   }

   /* If the only backslash is at the beginning, change to root */

   if(pcIn == pcLastSlash)
   {
      DosChDir("\\", 0L);

      if(*(pcIn+1) == '\000')
         return FILE_PATH;

      strcpy(pcOut, pcIn+1);
      return FILE_VALID;
   }

   /* Attempt to change directory -- Get current dir if OK */

   *pcLastSlash = 0;

   if(DosChDir(pcIn, 0L))
      return FILE_INVALID;

   DosQCurDir(0, pcOut, &usDirLen);

   /* Append input filename if any */

   pcFileOnly = pcLastSlash+1;

   if(*pcFileOnly == '\000')
      return FILE_PATH;

   if(*(pcOut+strlen(pcOut)-1) != '\\')
      strcat(pcOut++, "\\");

   strcat(pcOut, pcFileOnly);
   return FILE_VALID;
}


