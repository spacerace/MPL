/*------------------------------
   HEAD.C -- Displays File Head
  ------------------------------*/

#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include "easyfont.h"
#include "head.h"

#define LCID_FIXEDFONT   1L
#define LCID_BOLDFONT    2L

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;
MRESULT EXPENTRY AboutDlgProc  (HWND, USHORT, MPARAM, MPARAM) ;
MRESULT EXPENTRY OpenDlgProc   (HWND, USHORT, MPARAM, MPARAM) ;
SHORT            ParseFileName (CHAR *, CHAR *) ;

CHAR szClientClass [] = "Head" ;
CHAR szFileName [80] ;
HAB  hab ;

int main (int argc, char *argv[])
     {
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU  |
                                 FCF_SIZEBORDER    | FCF_MINMAX   |
                                 FCF_SHELLPOSITION | FCF_TASKLIST |
                                 FCF_MENU ;
     HMQ          hmq ;
     HWND         hwndFrame, hwndClient ;
     QMSG         qmsg ;

               // Check for filename parameter and copy to szFileName

     if (argc > 1)
          ParseFileName (szFileName, argv [1]) ;

               // Continue normally
     
     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClientClass, ClientWndProc, CS_SIZEREDRAW, 0) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP, WS_VISIBLE,
                                     &flFrameFlags, szClientClass, NULL,
                                     0L, NULL, ID_RESOURCE, &hwndClient) ;

     if (hwndFrame != NULL)
          {
          WinSendMsg (hwndFrame, WM_SETICON,
                      WinQuerySysPointer (HWND_DESKTOP, SPTR_APPICON, FALSE),
                      NULL) ;

          while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
               WinDispatchMsg (hab, &qmsg) ;

          WinDestroyWindow (hwndFrame) ;
          }
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static CHAR  szErrorMsg [] = "File not found or could not be opened" ;
     static SHORT cxClient, cyClient, cxChar, cyChar, cyDesc ;
     CHAR         *pcReadBuffer ;
     FILE         *fileInput ;
     FONTMETRICS  fm ;
     HPS          hps ;
     POINTL       ptl ;
     SHORT        sLength ;

     switch (msg)
          {
          case WM_CREATE:
               hps = WinGetPS (hwnd) ;
               EzfQueryFonts (hps) ;

               if (!EzfCreateLogFont (hps, LCID_FIXEDFONT, FONTFACE_COUR,
                                                           FONTSIZE_10, 0))
                    {
                    WinReleasePS (hps) ;

                    WinMessageBox (HWND_DESKTOP, HWND_DESKTOP,
                         "Cannot find a fixed-pitch font.  Load the Courier "
                         "fonts from the Control Panel and try again.",
                         szClientClass, 0, MB_OK | MB_ICONEXCLAMATION) ;

                    return 1 ;
                    }
               GpiQueryFontMetrics (hps, (LONG) sizeof fm, &fm) ;
               cxChar = (SHORT) fm.lAveCharWidth ;
               cyChar = (SHORT) fm.lMaxBaselineExt ;
               cyDesc = (SHORT) fm.lMaxDescender ;

               GpiSetCharSet (hps, LCID_DEFAULT) ;
               GpiDeleteSetId (hps, LCID_FIXEDFONT) ;
               WinReleasePS (hps) ;
               return 0 ;
          
          case WM_SIZE:
               cxClient = SHORT1FROMMP (mp2) ;
               cyClient = SHORT2FROMMP (mp2) ;
               return 0 ;

          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case IDM_OPEN:
                         if (WinDlgBox (HWND_DESKTOP, hwnd, OpenDlgProc,
                                        NULL, IDD_OPEN, NULL))
                              WinInvalidateRect (hwnd, NULL, FALSE) ;
                         return 0 ;

                    case IDM_ABOUT:
                         WinDlgBox (HWND_DESKTOP, hwnd, AboutDlgProc,
                                    NULL, IDD_ABOUT, NULL) ;
                         return 0 ;
                    }
               break ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;
               GpiErase (hps) ;

               if (szFileName [0] != '\0')
                    {
                    EzfCreateLogFont (hps, LCID_FIXEDFONT, FONTFACE_COUR,
                                           FONTSIZE_10,    0) ;
                    EzfCreateLogFont (hps, LCID_BOLDFONT,  FONTFACE_COUR,
                                           FONTSIZE_10,    FATTR_SEL_BOLD) ;

                    GpiSetCharSet (hps, LCID_BOLDFONT) ;
                    ptl.x = cxChar ;
                    ptl.y = cyClient - cyChar + cyDesc ;
                    GpiCharStringAt (hps, &ptl, (LONG) strlen (szFileName),
                                                szFileName) ;
                    ptl.y -= cyChar ;
                                
                    if ((fileInput = fopen (szFileName, "r")) != NULL)
                         {
                         GpiSetCharSet (hps, LCID_FIXEDFONT) ;
                         pcReadBuffer = malloc (cxClient / cxChar) ;

                         while ((ptl.y -= cyChar) > 0 &&
                                fgets (pcReadBuffer, cxClient / cxChar - 2,
                                       fileInput) != NULL)
                              {
                              sLength = strlen (pcReadBuffer) ;

                              if (pcReadBuffer [sLength - 1] == '\n')
                                   sLength-- ;

                              if (sLength > 0)
                                   GpiCharStringAt (hps, &ptl, (LONG) sLength,
                                                         pcReadBuffer) ;
                              }
                         free (pcReadBuffer) ;
                         fclose (fileInput) ;
                         }
                    else           // file cannot be opened
                         {
                         ptl.y -= cyChar ;
                         GpiCharStringAt (hps, &ptl,
                                          (LONG) strlen (szErrorMsg),
                                          szErrorMsg) ;
                         }
                    GpiSetCharSet (hps, LCID_DEFAULT) ;
                    GpiDeleteSetId (hps, LCID_FIXEDFONT) ;
                    GpiDeleteSetId (hps, LCID_BOLDFONT) ;
                    }
               WinEndPaint (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }

MRESULT EXPENTRY AboutDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     switch (msg)
          {
          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case DID_OK:
                    case DID_CANCEL:
                         WinDismissDlg (hwnd, TRUE) ;
                         return 0 ;
                    }
               break ;
          }
     return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
     }

VOID FillDirListBox (HWND hwnd, CHAR *pcCurrentPath)
     {
     static CHAR szDrive [] = "  :" ;
     FILEFINDBUF findbuf ;
     HDIR        hDir = 1 ;
     SHORT       sDrive ;
     USHORT      usDriveNum, usCurPathLen, usSearchCount = 1 ;
     ULONG       ulDriveMap ;

     DosQCurDisk (&usDriveNum, &ulDriveMap) ;
     pcCurrentPath [0] = (CHAR) usDriveNum + '@' ;
     pcCurrentPath [1] = ':' ;
     pcCurrentPath [2] = '\\' ;
     usCurPathLen = 64 ;
     DosQCurDir (0, pcCurrentPath + 3, &usCurPathLen) ;

     WinSetDlgItemText (hwnd, IDD_PATH, pcCurrentPath) ;
     WinSendDlgItemMsg (hwnd, IDD_DIRLIST, LM_DELETEALL, NULL, NULL) ;

     for (sDrive = 0 ; sDrive < 26 ; sDrive++)
          if (ulDriveMap & 1L << sDrive)
               {
               szDrive [1] = (CHAR) sDrive + 'A' ;

               WinSendDlgItemMsg (hwnd, IDD_DIRLIST, LM_INSERTITEM,
                                  MPFROM2SHORT (LIT_END, 0),
                                  MPFROMP (szDrive)) ;
               }

     DosFindFirst ("*.*", &hDir, 0x0017, &findbuf, sizeof findbuf,
                              &usSearchCount, 0L) ;
     while (usSearchCount)
          {
          if (findbuf.attrFile & 0x0010 &&
                    (findbuf.achName [0] != '.' || findbuf.achName [1]))
               
               WinSendDlgItemMsg (hwnd, IDD_DIRLIST, LM_INSERTITEM,
                                  MPFROM2SHORT (LIT_SORTASCENDING, 0),
                                  MPFROMP (findbuf.achName)) ;

          DosFindNext (hDir, &findbuf, sizeof findbuf, &usSearchCount) ;
          }
     }

VOID FillFileListBox (HWND hwnd)
     {
     FILEFINDBUF findbuf ;
     HDIR        hDir = 1 ;
     USHORT      usSearchCount = 1 ;

     WinSendDlgItemMsg (hwnd, IDD_FILELIST, LM_DELETEALL, NULL, NULL) ;

     DosFindFirst ("*.*", &hDir, 0x0007, &findbuf, sizeof findbuf,
                              &usSearchCount, 0L) ;
     while (usSearchCount)
          {
          WinSendDlgItemMsg (hwnd, IDD_FILELIST, LM_INSERTITEM,
                             MPFROM2SHORT (LIT_SORTASCENDING, 0),
                             MPFROMP (findbuf.achName)) ;

          DosFindNext (hDir, &findbuf, sizeof findbuf, &usSearchCount) ;
          }
     }

MRESULT EXPENTRY OpenDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static CHAR szCurrentPath [80], szBuffer [80] ;
     SHORT       sSelect ;

     switch (msg)
          {
          case WM_INITDLG:
               FillDirListBox (hwnd, szCurrentPath) ;
               FillFileListBox (hwnd) ;

               WinSendDlgItemMsg (hwnd, IDD_FILEEDIT, EM_SETTEXTLIMIT,
                                        MPFROM2SHORT (80, 0), NULL) ;
               return 0 ;

          case WM_CONTROL:
               if (SHORT1FROMMP (mp1) == IDD_DIRLIST ||
                   SHORT1FROMMP (mp1) == IDD_FILELIST)
                    {
                    sSelect = (USHORT) WinSendDlgItemMsg (hwnd,
                                                  SHORT1FROMMP (mp1),
                                                  LM_QUERYSELECTION, 0L, 0L) ;

                    WinSendDlgItemMsg (hwnd, SHORT1FROMMP (mp1),
                                       LM_QUERYITEMTEXT,
                                       MPFROM2SHORT (sSelect, sizeof szBuffer),
                                       MPFROMP (szBuffer)) ;
                    }

               switch (SHORT1FROMMP (mp1))             // Control ID
                    {
                    case IDD_DIRLIST:
                         switch (SHORT2FROMMP (mp1))   // notification code
                              {
                              case LN_ENTER:
                                   if (szBuffer [0] == ' ')
                                        DosSelectDisk (szBuffer [1] - '@') ;
                                   else
					DosChDir (szBuffer, 0L) ;

                                   FillDirListBox (hwnd, szCurrentPath) ;
                                   FillFileListBox (hwnd) ;

                                   WinSetDlgItemText (hwnd, IDD_FILEEDIT, "") ;
                                   return 0 ;
                              }
                         break ;

                    case IDD_FILELIST:
                         switch (SHORT2FROMMP (mp1))   // notification code
                              {
                              case LN_SELECT:
                                   WinSetDlgItemText (hwnd, IDD_FILEEDIT,
                                                      szBuffer) ;
                                   return 0 ;

                              case LN_ENTER:
                                   ParseFileName (szFileName, szBuffer) ;
                                   WinDismissDlg (hwnd, TRUE) ;
                                   return 0 ;
                              }
                         break ;
                    }
               break ;

          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case DID_OK:
                         WinQueryDlgItemText (hwnd, IDD_FILEEDIT,
                                              sizeof szBuffer, szBuffer) ;

                         switch (ParseFileName (szCurrentPath, szBuffer))
                              {
                              case 0:
                                   WinAlarm (HWND_DESKTOP, WA_ERROR) ;
                                   FillDirListBox (hwnd, szCurrentPath) ;
                                   FillFileListBox (hwnd) ;
                                   return 0 ;

                              case 1:
                                   FillDirListBox (hwnd, szCurrentPath) ;
                                   FillFileListBox (hwnd) ;
                                   WinSetDlgItemText (hwnd, IDD_FILEEDIT, "") ;
                                   return 0 ;

                              case 2:
                                   strcpy (szFileName, szCurrentPath) ;
                                   WinDismissDlg (hwnd, TRUE) ;
                                   return 0 ;
                              }
                         break ;

                    case DID_CANCEL:
                         WinDismissDlg (hwnd, FALSE) ;
                         return 0 ;
                    }
               break ;
          }
     return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
     }

SHORT ParseFileName (CHAR *pcOut, CHAR *pcIn)
     {
          /*----------------------------------------------------------------
             Input:    pcOut -- Pointer to parsed file specification.
                       pcIn  -- Pointer to raw file specification.
                       
             Returns:  0 -- pcIn had invalid drive or directory.
                       1 -- pcIn was empty or had no filename.
                       2 -- pcOut points to drive, full dir, and file name.

             Changes current drive and directory per pcIn string.
            ----------------------------------------------------------------*/

     CHAR   *pcLastSlash, *pcFileOnly ;
     ULONG  ulDriveMap ;
     USHORT usDriveNum, usDirLen = 64 ;

     strupr (pcIn) ;

               // If input string is empty, return 1

     if (pcIn [0] == '\0')
          return 1 ;

               // Get drive from input string or current drive

     if (pcIn [1] == ':')
          {
          if (DosSelectDisk (pcIn [0] - '@'))
               return 0 ;

          pcIn += 2 ;
          }
     DosQCurDisk (&usDriveNum, &ulDriveMap) ;

     *pcOut++ = (CHAR) usDriveNum + '@' ;
     *pcOut++ = ':' ;
     *pcOut++ = '\\' ;

               // If rest of string is empty, return 1

     if (pcIn [0] == '\0')
          return 1 ;

               // Search for last backslash.  If none, could be directory.

     if (NULL == (pcLastSlash = strrchr (pcIn, '\\')))
          {
	  if (!DosChDir (pcIn, 0L))
               return 1 ;

                    // Otherwise, get current dir & attach input filename

          DosQCurDir (0, pcOut, &usDirLen) ;

          if (strlen (pcIn) > 12)
               return 0 ;

          if (*(pcOut + strlen (pcOut) - 1) != '\\')
               strcat (pcOut++, "\\") ;

          strcat (pcOut, pcIn) ;
          return 2 ;
          }
               // If the only backslash is at beginning, change to root

     if (pcIn == pcLastSlash)
          {
	  DosChDir ("\\", 0L) ;

          if (pcIn [1] == '\0')
               return 1 ;

          strcpy (pcOut, pcIn + 1) ;
          return 2 ;
          }
               // Attempt to change directory -- Get current dir if OK

     *pcLastSlash = '\0' ;

     if (DosChDir (pcIn, 0L))
          return 0 ;

     DosQCurDir (0, pcOut, &usDirLen) ;

               // Append input filename, if any

     pcFileOnly = pcLastSlash + 1 ;

     if (*pcFileOnly == '\0')
          return 1 ;

     if (strlen (pcFileOnly) > 12)
          return 0 ;

     if (*(pcOut + strlen (pcOut) - 1) != '\\')
          strcat (pcOut++, "\\") ;

     strcat (pcOut, pcFileOnly) ;
     return 2 ;
     }
