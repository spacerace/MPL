/*
 * WINDOWS FILE FINDER UTILITY - SOURCE
 *
 * LANGUAGE : Microsoft C 5.1
 * TOOLKIT  : Windows 2.10 SDK
 * MODEL    : Small
 * STATUS   : Operational
 *
 * Eikon Systems, Inc.
 * 989 East Hillsdale Blvd, Suite 260
 * Foster City, California 94404
 *
 * 07/20/88 - Kevin P. Welch - initial creation.
 *
 */
 
#include <windows.h>
#include <process.h>
#include <string.h>
#include <dos.h>

#include "finder.h"

char     szApp[64];        /* name of selected application */
char     szPath[128];      /* path & name of selected file */

/**/

/*
 * WinMain( hInst, hPrevInst, lpszCmdLine, wCmdShow ) : int
 *
 *    hInst          handle of current instance
 *    hPrevInst      handle to previous instance (if any)
 *    lpszCmdLine    pointer to command line arguments
 *    wCmdShow       initial ShowWindow command
 *
 * This function is the system entry point for the application
 * and is responsible for defining the appropriate window 
 * classes and for processing all the messages.  Note how
 * the dialog box manager is responsible for the operation of
 * the file finder window.
 *
 */
 
int PASCAL WinMain(
   HANDLE         hInst,
   HANDLE         hPrevInst,
   LPSTR          lpszCmdLine,
   WORD           wCmdShow )
{
   /* local variables */
   FARPROC        lpProc;          

   lpProc = MakeProcInstance( (FARPROC)FinderDlgFn, hInst );
   DialogBox( hInst, "Finder", NULL, lpProc );
   FreeProcInstance( lpProc );

   /* end program */
   return( FALSE );

}

/**/

/*
 * FinderDlgFn( hWnd, wMsg, wParam, lParam ) : BOOL 
 *
 *    hWnd              handle to palette window
 *    wMsg              message number
 *    wParam            single word parameter
 *    lParam            double word parameter
 *
 * This function is responsible for processing all the messages
 * which relate to the file finder dialog box.  This mainly
 * involves the definition and retrieval of the various
 * events generated by the user.
 *
 */

BOOL FAR PASCAL FinderDlgFn(
   HWND        hDlg,
   WORD        wMsg,
   WORD        wParam,
   LONG        lParam )
{
   /* local variables */
   BOOL        bResult; 
 
   /* initialization */
   bResult = TRUE;
   
   /* process messages */
   switch( wMsg )
      {
   case WM_INITDIALOG : /* initialize dialog box */
      {
         WORD     wItem;               
         WORD     wNumber;             
         HWND     hSysMenu;            
         HWND     hListBox;            
         char     szDrive[12];         
   
         /* center dialog box */
         CenterPopup( hDlg, NULL );
            
         /* define search pattern & initial drive list */
         SetDlgItemText( hDlg, ID_PATTERN, "*.*" );
         DlgDirList( hDlg, "*.*", ID_DRIVES, NULL, 0xC000 );
      
         /* select all fixed drives */
         hListBox = GetDlgItem( hDlg, ID_DRIVES );
         wNumber = (WORD)SendMessage( hListBox, LB_GETCOUNT, 0, 0L );
         for ( wItem=0; wItem<wNumber; wItem++ ) {
            SendMessage( hListBox, LB_GETTEXT, wItem, (LONG)(LPSTR)szDrive );
            if ( strcmp(szDrive,"[-C-]") >= 0 )
               SendMessage( hListBox, LB_SETSEL, TRUE, (LONG)wItem );
         }
      
         /* define icon for dialog box */
         SetClassWord( 
            hDlg, 
            GCW_HICON, 
            LoadIcon( INSTANCE(hDlg), (LPSTR)"Icon" )
         );
               
         /* add about box to system menu */
         hSysMenu = GetSystemMenu( hDlg, FALSE );
         ChangeMenu( hSysMenu, NULL, NULL, NULL, MF_SEPARATOR|MF_APPEND );
         ChangeMenu( hSysMenu, NULL, "&About...", SC_ABOUT, MF_APPEND );

      }
      break;
   case WM_SYSCOMMAND : /* system command */
      
      /* process sub-messages */
      switch( wParam )
         {
      case SC_CLOSE : /* destroy dialog box */
         EndDialog( hDlg, TRUE );
         break;
      case SC_ABOUT : /* display about box */
         Dialog( hDlg, "About", AboutDlgFn );
         break;
      default :
         bResult = FALSE;
         break;
      }
      
      break;
   case WM_COMMAND : /* window command */
      
      /* process sub-message */
      switch( wParam )
         {
      case ID_PATTERN : /* pattern specified */
         
         /* enable-disable search button */
         if ( HIWORD(lParam) == EN_CHANGE ) 
            EnableWindow(
               GetDlgItem(hDlg,ID_SEARCH),
               (WORD)SendMessage(LOWORD(lParam),WM_GETTEXTLENGTH,0,0L)
            );
         
         break;
      case ID_FILES : /* file selected */
         {
            int      nItem;
            PSTR     pszExt;
            BOOL     bEnable;
            char     szExt[16];
            char     szDir[64];
            char     szFile[64];
            char     szProg[64];

            /* process listbox notification code */
            switch( HIWORD(lParam) )
               {
            case LBN_SELCHANGE : /* selection is changing */
         
               /* initialization */
               bEnable = FALSE;
            
               /* retrieve index to selected item */
               nItem = SendMessage( LOWORD(lParam), LB_GETCURSEL, 0, 0L );
               if ( nItem >= 0 ) {
   
                  /* retrieve text of selected item */
                  SendMessage(
                     LOWORD(lParam), 
                     LB_GETTEXT, 
                     nItem, 
                     (LONG)(LPSTR)szFile
                  );

                  /* continue if file name selected */
                  if ( szFile[0] == ' ' ) {

                     /* retrieve current directory */
                     do {
                        SendMessage(
                           GetDlgItem(hDlg,ID_FILES), 
                           LB_GETTEXT, 
                           --nItem, 
                           (LONG)(LPSTR)szDir
                        );
                     } while( szDir[0] == ' ' );

                     /* define full path name */
                     sprintf( 
                        szPath, 
                        "%s\\%s", 
                        szDir, 
                        strtok(&szFile[2]," ")
                     );
                     
                     /* check if extension defined */
                     if ( strrchr(szPath,'.') ) {

                        /* retrieve extension */
                        strcpy( szExt, strrchr(szPath,'.')+1 );

                        /* retrieve list of program extensions */
                        GetProfileString(
                           "windows",
                           "programs",
                           "",
                           szProg,
                           sizeof(szProg)
                        );

                        /* continue verification if not a program */
                        if ( !Present(szProg,szExt) ) {

                           /* retrieve extension profile string */
                           GetProfileString(
                              "extensions",
                              szExt,
                              "",
                              szApp,
                              sizeof(szApp)
                           );

                           /* check if extension listed */
                           if ( szApp[0] ) {
                              bEnable = TRUE;
                              *strchr(szApp,' ') = 0;
                           }
                           
                        } else {
                           bEnable = TRUE;
                           strcpy( szApp, szPath );
                           szPath[0] = 0;
                        }
                     
                     } 

                  }

               }
         
               /* enable run button */
               EnableWindow( GetDlgItem(hDlg,ID_RUN), bEnable );

               break;
            case LBN_DBLCLK : /* double-click */
               
               /* perform run if button enabled */
               if ( IsWindowEnabled(GetDlgItem(hDlg,ID_RUN)) ) 
                  if ( spawnl(P_NOWAIT,szApp,szApp,szPath,NULL) >= 0 )
                     ShowWindow( hDlg, SW_MINIMIZE );
            
               break;
            default : /* something else */
               break;
            }

         }
         break;
      case ID_SEARCH : /* search using path */
      case IDOK :
         {
            WORD     wItem;            
            WORD     wNumber;          
            HWND     hListBox;         
            BOOL     bContinue;        
            HCURSOR  hOldCursor;       
            char     szDrive[8];       
            char     szPattern[64];    
            char     szFileSpec[64];   
         
            /* disable run button & change cursor to hourglass */
            EnableWindow( GetDlgItem(hDlg,ID_RUN), FALSE );
            hOldCursor = SetCursor( LoadCursor(NULL,IDC_WAIT) );
            
            /* retrieve search pattern & erase current file list */
            GetDlgItemText( hDlg, ID_PATTERN, szPattern, sizeof(szPattern) );
            
            SendMessage( GetDlgItem(hDlg,ID_FILES), WM_SETREDRAW, FALSE, 0L );
            SendMessage( GetDlgItem(hDlg,ID_FILES), LB_RESETCONTENT, 0, 0L );

            /* perform search on each selected drive */
            bContinue = TRUE;
            hListBox = GetDlgItem( hDlg, ID_DRIVES );
            wNumber = (WORD)SendMessage( hListBox, LB_GETCOUNT, 0, 0L );
            
            for ( wItem=0; (bContinue)&&(wItem<wNumber); wItem++ ) 
               if ( SendMessage(hListBox,LB_GETSEL,wItem,0L) ) {
                  
                  /* define file specification */
                  SendMessage( hListBox, LB_GETTEXT, wItem, (LONG)(LPSTR)szDrive );
                  sprintf( szFileSpec, "%c:\\%s", szDrive[2], szPattern );
                  
                  /* create directory listing using pattern */
                  bContinue = Directory( szFileSpec, _A_NORMAL, GetDlgItem(hDlg,ID_FILES) );
                  
               }
            
            /* activate redrawing */
            SendMessage( GetDlgItem(hDlg,ID_FILES), WM_SETREDRAW, TRUE, 0L );

            /* check for null search operation */
            if ( SendMessage( GetDlgItem(hDlg,ID_FILES), LB_GETCOUNT, 0, 0L ) == 0L )
               MessageBox( hDlg, "No files found matching criteria!", "File Finder", MB_OK|MB_ICONHAND );
               
            /* restore cursor & repaint display */
            SetCursor( hOldCursor );
            InvalidateRect( GetDlgItem(hDlg,ID_FILES), NULL, TRUE );

         }
         break;
      case ID_RUN : /* run selection */

         /* start application & make finder iconic */
         if ( spawnl(P_NOWAIT,szApp,szApp,szPath,NULL) >= 0 )
            ShowWindow( hDlg, SW_MINIMIZE );

         break;
      case ID_QUIT : /* close dialog */
      case IDCANCEL :
         EndDialog( hDlg, TRUE );
         break;
      default : /* something else */
         break;
      }
      
      break;
   default :
      bResult = FALSE;
      break;
   }  
   
   /* return result */
   return( bResult );
 
}

/**/

/*
 * AboutDlgFn( hDlg, wMsg, wParam, lParam ) : BOOL ;
 *
 *    hDlg           handle to dialog box
 *    wMsg           message or event
 *    wParam         word portion of message
 *    lParam         long portion of message
 *
 * This function is responsible for processing all the messages
 * that relate to the About dialog box.  About the only useful actions
 * this function performs is to center the dialog box and to wait for
 * the OK button to be pressed.
 *
 */

BOOL FAR PASCAL AboutDlgFn(
   HWND        hDlg,
   WORD        wMsg,
   WORD        wParam,
   LONG        lParam )
{
   BOOL        bResult;

   /* process message */
   switch( wMsg )
      {
   case WM_INITDIALOG :
      bResult = TRUE;
      CenterPopup( hDlg, GetParent(hDlg) );
      break;
   case WM_COMMAND :

      /* process sub-message */
      if ( wParam == IDOK ) {
         bResult = TRUE;
         EndDialog( hDlg, TRUE );
      } else
         bResult = FALSE;

      break;
   default :
      bResult = FALSE;
      break;
   }

   /* return final result */
   return( bResult );

}

/**/

/*
 * Dialog( hParentWnd, lpszTemplate, lpfnDlgProc ) : BOOL
 *
 *    hParentWnd        handle to parent window
 *    lpszTemplate      dialog box template
 *    lpfnDlgProc       dialog window function
 *
 * This utility function displays the specified dialog box, using the
 * template provided.  It automatically makes a new instance of the
 * dialog box function.  Note that this function will NOT work
 * correctly if an invalid or NULL parent window handle is provided.
 *
 */

BOOL FAR PASCAL Dialog( hParentWnd, lpszTemplate, lpfnDlgProc )
   HWND        hParentWnd;
   LPSTR       lpszTemplate;
   FARPROC     lpfnDlgProc;
{
   /* local variables */
   BOOL           bResult;
   FARPROC        lpProc;                 
      
   /* display palette dialog box */
   lpProc = MakeProcInstance( lpfnDlgProc, INSTANCE(hParentWnd) );
   bResult = DialogBox( INSTANCE(hParentWnd), lpszTemplate, hParentWnd, lpProc );
   FreeProcInstance( lpProc );

   /* return result */
   return( bResult );

}

/**/

/*
 * Directory( szFileSpec, wAttributes, hListBox ) : BOOL
 *
 *    szFileSpec        file search specification
 *    wAttributes       file attributes to search for
 *    hListBox          handle to listbox for directory
 *
 * This function searches the disk for files of the specified type
 * and appends the results found to the list box whose handle is
 * provided.  An error is generated and the process aborted if
 * insufficient memory is available for the search.
 *
 * A value of TRUE is returned if the directory operation was
 * successful.
 *
 */
 
BOOL FAR PASCAL Directory(
   PSTR     szFileSpec,
   WORD     wAttributes,
   HWND     hListBox )
{
   BOOL     bContinue;           
   BOOL     bDirOutput;          
   
   WORD     wEntry;              
   WORD     wDirEntries;         
   
   struct find_t     DirEntry;   
   
   char     szPath[64];          
   char     szSpec[64];          
   char     szEntry[64];         

   /* initialization */
   bContinue = TRUE;
   bDirOutput = FALSE;

   wDirEntries = 0;

   /* separate file spec into path and wildcards */
   for (wEntry=strlen(szFileSpec)-1; szFileSpec[wEntry]!='\\'; wEntry-- );
   
   strcpy( szPath, szFileSpec );
   szPath[wEntry] = 0;
   
   strcpy( szSpec, &szFileSpec[wEntry+1] );
      
   /* perform search for normal files */
   if ( _dos_findfirst(szFileSpec,wAttributes,&DirEntry) == 0 ) {
   
      /* repeat until all entries exhausted */
      do {
      
         /* output current directory if not done */
         if ( bDirOutput == FALSE ) {
            bDirOutput = TRUE;
            if ( SendMessage(hListBox,LB_INSERTSTRING,-1,(LONG)(LPSTR)szPath) < LB_OKAY ) {
               MessageBox( GetParent(hListBox), "Insufficient Memory!", "File Finder", MB_OK|MB_ICONHAND );
               return( FALSE );
            }
         }     

         /* output current file name */
         sprintf(
            szEntry, 
            "  %-12s %7ld  %2u/%02u/%02u  %2u:%02u",
            DirEntry.name,
            DirEntry.size,
            (DirEntry.wr_date&0x01E0) >> 5,
            DirEntry.wr_date&0x001F,
            ((DirEntry.wr_date&0xFE00) >> 9)+80,
            (DirEntry.wr_time&0xF800) >> 11,
            (DirEntry.wr_time&0x07E0) >> 5
         );
               
         /* add entry to file list box */
         if ( SendMessage(hListBox,LB_INSERTSTRING,-1,(LONG)(LPSTR)szEntry) < LB_OKAY ) {
            MessageBox( GetParent(hListBox), "Insufficient Memory!", "File Finder", MB_OK|MB_ICONHAND );
            return( FALSE );
         }
         
      } while ( _dos_findnext(&DirEntry) == 0 );
      
   } 
   
   /* perform search for sub-directories */
   sprintf( szEntry, "%s\\*.*", szPath );
   if ( _dos_findfirst(szEntry,_A_SUBDIR,&DirEntry) == 0 ) {
   
      /* repeat until all entries exhausted */
      do {
      
         /* eliminate special directory entries */
         if ( (DirEntry.attrib==_A_SUBDIR) && (DirEntry.name[0]!='.') ) {
            sprintf( szEntry, "%s\\%s\\%s", szPath, DirEntry.name, szSpec );
            bContinue = Directory( szEntry, wAttributes, hListBox );
         }
         
      } while ( (bContinue)&&(_dos_findnext(&DirEntry) == 0) );
      
   }        
   
   /* return final result */
   return( bContinue );

}

/**/

/*
 * CenterPopup( hWnd, hParentWnd ) : BOOL
 *
 *    hWnd              window handle
 *    hParentWnd        parent window handle
 *
 * This routine centers the popup window in the screen or display
 * using the window handles provided.  The window is centered over
 * the parent if the parent window is valid.  Special provision
 * is made for the case when the popup would be centered outside
 * the screen - in this case it is positioned at the appropriate
 * border.
 *
 */

BOOL FAR PASCAL CenterPopup( 
      HWND     hWnd,
      HWND     hParentWnd
   )
{
   /* local variables */
   int      xPopup;   
   int      yPopup;   
   int      cxPopup;  
   int      cyPopup;  
   int      cxScreen; 
   int      cyScreen; 
   int      cxParent; 
   int      cyParent; 
   RECT     rcWindow; 

   /* retrieve main display dimensions */
   cxScreen = GetSystemMetrics( SM_CXSCREEN );
   cyScreen = GetSystemMetrics( SM_CYSCREEN );

   /* retrieve popup rectangle  */
   GetWindowRect( hWnd, (LPRECT)&rcWindow );

   /* calculate popup extents */
   cxPopup = rcWindow.right - rcWindow.left;
   cyPopup = rcWindow.bottom - rcWindow.top;

   /* calculate bounding rectangle */
   if ( hParentWnd ) {

      /* retrieve parent rectangle */
      GetWindowRect( hParentWnd, (LPRECT)&rcWindow );

      /* calculate parent extents */
      cxParent = rcWindow.right - rcWindow.left;
      cyParent = rcWindow.bottom - rcWindow.top;

      /* center within parent window */
      xPopup = rcWindow.left + ((cxParent - cxPopup)/2);
      yPopup = rcWindow.top + ((cyParent - cyPopup)/2);

      /* adjust popup x-location for screen size */
      if ( xPopup+cxPopup > cxScreen )
         xPopup = cxScreen - cxPopup;

      /* adjust popup y-location for screen size */
      if ( yPopup+cyPopup > cyScreen )
         yPopup = cyScreen - cyPopup;

   } else {

      /* center within entire screen */
      xPopup = (cxScreen - cxPopup) / 2;
      yPopup = (cyScreen - cyPopup) / 2;

   }

   /* move window to new location & display */
   MoveWindow(
      hWnd,
      ( xPopup > 0 ) ? xPopup : 0,
      ( yPopup > 0 ) ? yPopup : 0,
      cxPopup,
      cyPopup,
      TRUE
   );

   /* normal return */
   return( TRUE );

}

/**/

/*
 * Present( pszList, pszToken ) : BOOL
 *
 *    pszList        list of tokens
 *    pszToken       search token
 *
 * This function searches for a particular token from a list tokens.  It
 * takes into account spaces and other delimiting characters.  A value
 * of TRUE is returned if the specified token was found in the list
 * provided.
 *
 * Note that this function produces a side-effect in that it breaks up
 * the token list into individual components.
 *
 */

BOOL FAR PASCAL Present(
   PSTR     pszList,
   PSTR     pszToken )
{
   BOOL     bFound;
   PSTR     pszEntry;

   /* initialization */
   bFound = FALSE;
   pszEntry = strtok( pszList, " " );

   /* search for token in list */
   while ( (!bFound)&&(pszEntry) ) 
      if ( stricmp(pszEntry,pszToken) )
         pszEntry = strtok( NULL, " " );
      else
         bFound = TRUE;

   return( bFound );

}



 