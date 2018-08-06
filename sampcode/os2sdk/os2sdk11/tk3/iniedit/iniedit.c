/******************************* Module Header ******************************\
* Module Name: IniEdit.c
*
*
* PM OS2.ini Editor
*
* Allows adding, deleting and modifying of os2.ini entries through PM
* interface
*
*
\***************************************************************************/


#define LINT_ARGS                           // Include needed parts of PM
#define INCL_WININPUT                       //    definitions
#define INCL_WINSYS
#define INCL_WINMESSAGEMGR
#define INCL_WINBUTTONS
#define INCL_WINPOINTERS
#define INCL_WINHEAP
#define INCL_WINSHELLDATA
#define INCL_WINMENUS
#define INCL_WINFRAMEMGR
#define INCL_WINDIALOGS
#define INCL_WINLISTBOXES
#define INCL_WINENTRYFIELDS
#define INCL_DOSMEMMGR
#define INCL_WINSWITCHLIST
#define INCL_DOSPROCESS
#define INCL_GPIBITMAPS
#define INCL_GPIREGIONS
#define INCL_GPILCIDS
#define INCL_GPIPRIMITIVES
#define INCL_DEV

#include <string.h>
#include <stdio.h>

#include <os2.h>

#include "IniEdit.h"


/******************************* Constants **********************************/

#define STACK_SIZE            0x2000        // Stack size for second thread
#define UPPER_SEGMENT_LIMIT   0xFD00        // Amount of Segment used

/******************************** Globals **********************************/

char szIniEdit[] = "IniEdit";               // App String Name

HAB       habIniEdit;                       // Handle Anchor Block
HMQ       hmqIniEdit;                       // Handle Message Queue
HWND      hwndIniEdit;                      // Main Client Window
HWND      hwndIniEditFrame;                 // Frame Window
HDC       hdcScreen;                        // DC for Client Window
HPS       hpsScreen;                        // PS for Client Window


USHORT    cAppNames = 0;                    // Count of App names in os2.ini
USHORT    usShift = 0;                      // DosHugeAlloc segment offsets
HWND      FocusWindow = (HWND)NULL;         // Focus of Dialog Box

USHORT    usFormat = APP_FORM;              // Current Display format
USHORT    usPrintFormat = APP_FORM;         // Format for Printing
USHORT    usLineHeight = 12;                // Current font Height
HWND      hwndList = (HWND)NULL;            // Handle of Main ListBox
HWND      hwndMenu = (HWND)NULL;            // Handle of Main Menu

PGROUPSTRUCT  pGroups;                      // Pointer to String Groups
PPAIRSTRUCT   pPairsBase;                   // Pointer to Key-Value Pairs
PPAIRSTRUCT   pPairsAlloc;                  // Pointer to next Avail Memory
PBYTE         pPrintStack;                  // Pointer to Print Thread Stack

#define	HOLD_LEN 4096
CHAR	      achNames[HOLD_LEN];	    // Array of Character from Query
CHAR          szBuf[MAX_STRING_LEN];	    // Character buffer for Pairs


/***************************** Function Decls ******************************/

VOID    ProcessMenuItem( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 );
VOID    cdecl main( VOID );
VOID    IniEditPaint( VOID );
VOID    ReadIni( VOID );
VOID    OldProfilePrint( VOID );
VOID    UpdateListBox( BOOL fRead, USHORT usForm );

MRESULT CALLBACK IniEditWndProc(HWND, USHORT, MPARAM, MPARAM);


/***************************** Function Header *****************************\
*
* main
*
*
* Do initialization then do a message loop
*
\***************************************************************************/

VOID cdecl main()
{

    QMSG         qmsg;                      // Current Queue Message
    ULONG        fcf;                       // Frame Control Flags
    SIZEL        sizel;                     // Size of PS
    RECTL        rclWindow;                 // Size Rect for ListBox Window
    SEL          sel;                       // Selector of allocated segments
    SWCNTRL      swcntrl;                   // Switch Control Block
    FONTMETRICS  fmetrics;                  // FontMetrics of current font


    /*** Set up and Initialization ***/

    /* Initialize the anchor block handle */
    habIniEdit = WinInitialize(NULL);

    /* Create the message queue */
    hmqIniEdit = WinCreateMsgQueue(habIniEdit, 0);

    /* Register the window class for the IniEdit window */
    WinRegisterClass(habIniEdit, (PCH)szIniEdit, IniEditWndProc,
            CS_SIZEREDRAW, 0);

    /* Create the window for IniEdit */
    fcf = FCF_TITLEBAR | FCF_MINMAX | FCF_SYSMENU | FCF_SIZEBORDER | FCF_MENU
        | FCF_SHELLPOSITION | FCF_ACCELTABLE | FCF_ICON;

    hwndIniEditFrame = WinCreateStdWindow( HWND_DESKTOP, WS_VISIBLE,
        (PVOID)&fcf, (PSZ)szIniEdit, (PSZ)szIniEdit, WS_VISIBLE,
        (HMODULE)NULL, IDI_INIEDIT, (PHWND)&hwndIniEdit);

    /* Create a DC for the IniEdit window */
    hdcScreen = WinOpenWindowDC(hwndIniEdit);

    /* also create a screen PS */

    sizel.cx= 0L;   // To use the default screen page size.
    sizel.cy= 0L;

    if( (hpsScreen = GpiCreatePS( habIniEdit, hdcScreen, &sizel,
            (PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC ))) == (HPS)NULL )
        {
		;
        }


    /* Initially set the keyboard focus to us */
    WinSetFocus(HWND_DESKTOP, hwndIniEdit);

    /* get the font size */
    GpiQueryFontMetrics( hpsScreen, (LONG)sizeof( FONTMETRICS ), &fmetrics );
    usLineHeight = (USHORT)(fmetrics.lMaxDescender + fmetrics.lMaxBaselineExt);

    /* this menu handle is often used */
    hwndMenu = WinWindowFromID( hwndIniEditFrame, FID_MENU );

    /* add program to switch list */
    swcntrl.hwnd             = hwndIniEditFrame;
    swcntrl.hwndIcon         = NULL;
    swcntrl.hprog            = NULL;
    swcntrl.idProcess        = NULL;
    swcntrl.idSession        = NULL;
    swcntrl.uchVisibility    = NULL;
    swcntrl.fbJump           = NULL;
    strcpy( swcntrl.szSwtitle, szIniEdit);
    swcntrl.fReserved        = NULL;

    WinAddSwitchEntry( &swcntrl );

    /* Create main list box in main window */
    WinQueryWindowRect( hwndIniEdit, &rclWindow);
    rclWindow.yTop -= usLineHeight;
    hwndList = WinCreateWindow( hwndIniEdit,            // parent
                                WC_LISTBOX,             // class
                                (PSZ)"Scroll",          // name
                                LS_NOADJUSTPOS,         // style
                                0, 0,                   // position
                                (USHORT)rclWindow.xRight,
                                (USHORT)rclWindow.yTop,
                                hwndIniEditFrame,       // Owner
                                HWND_TOP,               // InsertBehind
                                IDI_LIST,               // ID
                                (PVOID)NULL,            // pCtlData,
                                (PVOID)NULL);


    /*** Memory Allocation ***/

    /* Alloc the needed space for the groups */
    if( DosAllocSeg( 32000, &sel, 0) )
        ErrMessage( "main: DosAlloc for pGroup failed" );
    pGroups = MAKEP( sel, 0);

    if( DosAllocHuge( 4, 0, &sel, 0, 0) )
        ErrMessage( "main: DosAlloc for pPairs failed" );
    pPairsAlloc = pPairsBase = MAKEP( sel, 0);

    /* create a stack for second thread */
    if( DosAllocSeg( STACK_SIZE, &sel, 0) )
        ErrMessage( "main: DosAlloc for Stack failed" );
    pPrintStack = MAKEP( sel, 0);

    DosGetHugeShift( &usShift );

    /* read in os2.ini and fill in list box */
    UpdateListBox( TRUE, APP_FORM );

    WinShowWindow( hwndList, TRUE );

    /* Process messages for the window */
    while ( WinGetMsg(habIniEdit, (PQMSG)&qmsg, (HWND)NULL, 0, 0 ) )
        {

        /* Dispatch the message */
        WinDispatchMsg(habIniEdit, (PQMSG)&qmsg);
        }


    /*** CleanUp ***/

    /* Destroy the IniEdit window and message queue */
    GpiDestroyPS( hpsScreen );
    WinDestroyWindow(hwndIniEditFrame);
    WinDestroyMsgQueue(hmqIniEdit);

    /* Exit PM */
    WinTerminate( habIniEdit );
    DosExit( EXIT_PROCESS, 0 );

}  /* main */


/****************************** Function Header ****************************\
*
* ReadIni
*
*
* Reads in OS2.ini
*
\***************************************************************************/

VOID ReadIni()
{
//    CHAR      achNames[HOLD_LEN];           // Array of Character from Query
    USHORT    cchNames;                     // Count of Character from Query
    USHORT    Index[MAX_APP_NAMES];         // Index of Names into achNames
    USHORT    cPairs;                       // Count of pairs in current AppName
    INT       i,j;                          // Loop Counters


    /* Reset Count of App Names */
    cAppNames = 0;

    /* Reset memory available pointer to Base */
    pPairsAlloc = pPairsBase;

    /* Determine number of characters in app Names Strings */
    WinQueryProfileSize( habIniEdit, NULL, NULL, &cchNames );

    /* Read in the App Name strings */
    WinQueryProfileString( habIniEdit, NULL, NULL, " ", achNames, cchNames );

    /*** Find the starting index of each App ***/

    /* step through each string in set of app characters
     * adding length of current string to find begining of next string
     * also store each App Name into szAppName element of Group
     */
    for( i=0; i<cchNames; i += (strlen(pGroups[cAppNames-1].szAppName)+1) )
        {
        if( achNames[i] != (char)0 )
            {
            strcpy( pGroups[cAppNames++].szAppName, &achNames[i]);
            }  /* if */
        else
            if( achNames[i+1] == (char)0 )
                break;
        }  /* for */


    /*** Read elements of each App Name ***/
    for( i=0; i<cAppNames; i++ )
        {
        /* Get number of Character Associated with App Name */
        WinQueryProfileSize( habIniEdit, pGroups[i].szAppName, NULL, &cchNames );

        /* Enumerate all KeyNames for this app name */
        WinQueryProfileString( habIniEdit, pGroups[i].szAppName, NULL, " ", achNames, HOLD_LEN );

        /* Count the number of key Names */
        cPairs = 0;
        for( j=0; j<cchNames; j++)
            if( achNames[j] != (CHAR)0 )
                {
                Index[cPairs++] = j;
                j += strlen( &achNames[j] );
                }

        pGroups[i].cKeys = cPairs;

        /* Allocate the number of pair structures for the current group */
        pGroups[i].pPairs = pPairsAlloc;

        pPairsAlloc += sizeof(PAIRSTRUCT)*cPairs;

        /* Step to next segment if near end of current segment */
        if( LOUSHORT(pPairsAlloc) > UPPER_SEGMENT_LIMIT)
            {
            pPairsAlloc = MAKEP( (HIUSHORT(pPairsAlloc)+(1<<usShift)), 0);
            }

        /* Store the KeyName into the pair structure */
        for( j=0; j<cPairs; j++ )
            {
            strcpy( pGroups[i].pPairs[j].szKey, &achNames[Index[j]] );

            /* store the key value */
            WinQueryProfileString( habIniEdit, pGroups[i].szAppName,
		     pGroups[i].pPairs[j].szKey, " ",
		     pGroups[i].pPairs[j].szValue, MAX_STRING_LEN );

            }
        }  /* each App Name */

}  /* ReadIni */


/****************************** Function Header ****************************\
*
* ProcessMenuItem
*
*
* Act on the corresponding Menu Item Choosen
*
\***************************************************************************/

VOID ProcessMenuItem( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 )
{
    TID  Tid;                               // ID of new thread; Not used


    /* Switch on the Menu Item choosen */
    switch( LOUSHORT( mp1 ) )
        {
        case IDMI_SHOW_ALL:
        case IDMI_SHOW_APPNAMES:
            usFormat = (LOUSHORT(mp1) == IDMI_SHOW_ALL);
            UpdateListBox( FALSE, usFormat ? ALL_FORM : APP_FORM );
            break;

        case IDM_SEARCH:
            WinDlgBox(HWND_DESKTOP, hwndIniEditFrame, (PFNWP)SearchWndProc,
                            (HMODULE)NULL, IDD_SEARCH, (PVOID)NULL);
            break;

        case IDMI_EDIT_DELETE_KEY:
            WinDlgBox(HWND_DESKTOP, hwndIniEditFrame, (PFNWP)DelKeyWndProc,
                            (HMODULE)NULL, IDD_DEL_KEY, (PVOID)NULL);
            UpdateListBox( TRUE, usFormat ? ALL_FORM : APP_FORM );
            break;

        case IDMI_EDIT_DELETE_APP:
            WinDlgBox(HWND_DESKTOP, hwndIniEditFrame, (PFNWP)DelAppWndProc,
                            (HMODULE)NULL, IDD_DEL_APP, (PVOID)NULL);
            UpdateListBox( TRUE, usFormat ? ALL_FORM : APP_FORM );
            break;

        case IDMI_EDIT_ADD_KEY:
            WinDlgBox(HWND_DESKTOP, hwndIniEditFrame, (PFNWP)AddKeyWndProc,
                            (HMODULE)NULL, IDD_ADD_KEY, (PVOID)NULL);
            UpdateListBox( TRUE, usFormat ? ALL_FORM : APP_FORM );
            break;

        case IDMI_EDIT_CHANGE:
            WinDlgBox(HWND_DESKTOP, hwndIniEditFrame, (PFNWP)ChangeKeyWndProc,
                            (HMODULE)NULL, IDD_CHANGE_KEY, (PVOID)NULL);
            UpdateListBox( TRUE, usFormat ? ALL_FORM : APP_FORM );
            break;

        case IDMI_PRINT_ALL:
        case IDMI_PRINT_APP:
            usPrintFormat = LOUSHORT(mp1) == IDMI_PRINT_ALL ? ALL_FORM : APP_FORM;
            if( DosCreateThread( PrintThread, &Tid, ((PBYTE)(pPrintStack)+STACK_SIZE) ) )
                ErrMessage("StartThread2: DosCreateThread Failed");
            break;

        case IDMI_REFRESH:
            UpdateListBox( TRUE, usFormat );
            break;

        case IDMI_ABOUT:
            WinDlgBox(HWND_DESKTOP, hwndIniEditFrame, (PFNWP)DelAppWndProc,
                            (HMODULE)NULL, IDD_ABOUT, (PVOID)NULL);
            break;

        default:
            WinDefWindowProc(hwnd, msg, mp1, mp2);

            break;

        }  /* switch */

}  /* ProcessMenuItem */


/****************************** Function Header ****************************\
*
* UpdateListBox
*
*
* Update Main List Box to correct state
*    May Also:
*    - Check correct menu item
*    - Repaint title of List Box
*    - ReRead os2.ini file
*
\***************************************************************************/

VOID UpdateListBox( BOOL fReadIni, USHORT usNewFormat )
{
    INT       i,j;                          // Loop Counters
    USHORT    Index;                        // Index into ListBox
    static    USHORT    usLastFormat = -1;  // Last displayed format


    /* Check the correct item if format changed */
    if( usLastFormat != usNewFormat )
        {
        WinSendMsg( hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(IDMI_SHOW_ALL, TRUE),
                MPFROM2SHORT(MIA_CHECKED, usFormat ? MIA_CHECKED:FALSE));

        WinSendMsg( hwndMenu, MM_SETITEMATTR, MPFROM2SHORT(IDMI_SHOW_APPNAMES, TRUE),
                MPFROM2SHORT(MIA_CHECKED, (!usFormat) ? MIA_CHECKED:FALSE));
        usLastFormat = usNewFormat;

        WinSendMsg( hwndIniEdit, WM_PAINT, (MPARAM)NULL, (MPARAM)NULL );
        }


    /* Turn off list box updates */
    WinEnableWindowUpdate( hwndList, FALSE );

    /* Remove all items from list box */
    WinSendMsg( hwndList, LM_DELETEALL, (MPARAM)0, (MPARAM)0 );

    /* ReRead os2.ini if needed */
    if( fReadIni )
        ReadIni();

    /* Add elements to listbox */
    if( usNewFormat == ALL_FORM )
        {

        /* Insert all app Names */
        for( i=0; i<cAppNames; i++ )
            {
            Index = (USHORT)WinSendMsg( hwndList, LM_INSERTITEM,
                    MPFROM2SHORT(LIT_END, NULL),
                    MPFROMP(pGroups[i].szAppName) );

            WinSendMsg( hwndList, LM_SETITEMHANDLE,
                    MPFROMSHORT(Index),
                    MPFROMSHORT(i) );

            /* Insert Key Value Pairs for App Name */
            for( j=0; j<pGroups[i].cKeys; j++ )
                {
                sprintf( szBuf, "     %s: %s", pGroups[i].pPairs[j].szKey,
                        pGroups[i].pPairs[j].szValue );
                Index = (USHORT)WinSendMsg( hwndList, LM_INSERTITEM,
                        MPFROM2SHORT(LIT_END, NULL),
                        MPFROMP(szBuf) );

                WinSendMsg( hwndList, LM_SETITEMHANDLE,
                        MPFROMSHORT(Index),
                        MPFROM2SHORT(i,j) );

                }
            }
        }  /* if */
    else
        {
        /* Insert all app Names */
        for( i=0; i<cAppNames; i++ )
            {
            WinSendMsg( hwndList, LM_INSERTITEM,
                    MPFROM2SHORT(LIT_SORTASCENDING, NULL),
                    MPFROMP(pGroups[i].szAppName) );
            }
        }  /* else */

    /* Do All repainting of ListBox */
    WinEnableWindowUpdate( hwndList, TRUE );

}  /* UpdateListBox */


/****************************** Function Header ****************************\
*
* IniEditPaint
*
*
* Window Paint Routine
*
\***************************************************************************/

VOID IniEditPaint()
{
    RECTL     rclWindow;                    // Current size of Main Window
    RECTL     rclBlit;                      // Size of Area to Blank for Title
    CHAR      szShowMode[MAX_STRING_LEN];   // String Description of mode


    /* Get the size of the whole window */
    WinQueryWindowRect( hwndIniEdit, &rclWindow );

    /* Paint the window Title Area */
    rclBlit = rclWindow;
    rclBlit.yBottom = rclBlit.yTop - usLineHeight;

    GpiBitBlt( hpsScreen, (HPS)NULL, 2L, (PPOINTL)&rclBlit, ROP_ONE, (LONG)NULL);

    /* Write the Title */
    strcpy( szShowMode, usFormat == APP_FORM ? SZAPP : SZALL );
    WinDrawText( hpsScreen, strlen(szShowMode), szShowMode, &rclWindow,
            CLR_BLUE, CLR_WHITE, DT_CENTER|DT_TOP);

}  /* IniEditPaint */


/****************************** Function Header ****************************\
*
* IniEditWndProc
*
*
* Window Proc for IniEdit
*
\***************************************************************************/

MRESULT CALLBACK IniEditWndProc(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{

    CHAR      szBuf[MAX_STRING_LEN];        // Input character Buffer
    CHAR      szBuf2[MAX_STRING_LEN];       // Second Input Character Buffer
    USHORT    Index;                        // Index of Current ListBox Item
    USHORT    TopIndex;                     // Current Top Item in ListBox
    ULONG     Handle;                       // ListBox Item Handle Info
    HWND      hwndDialog;                   // Window handle of Dailog Box
    HWND      hwndText;                     // Handle of current text window
    HPS       hpsPaint;                     // PS to Paint
    RECTL     rclPaint;                     // Rect in hpsPaint to Paint
    BOOL      fScroll = FALSE;              // Scroll List Box Flag


    /* Switch on message being processed */
    switch( msg )
        {
        case WM_PAINT:
            /* Paint the IniEdit window portion not covered by List Box */
            hpsPaint = WinBeginPaint(hwnd, (HPS)NULL, &rclPaint);
            IniEditPaint();
            WinEndPaint(hpsPaint);
            break;

        case WM_COMMAND:
            /* If menu item call Processing Routine */
            if( LOUSHORT( mp2 ) == CMDSRC_MENU )
                ProcessMenuItem( hwnd, msg, mp1, mp2 );

            /* If accelorator call appropriate routine */
            if( LOUSHORT( mp2 ) == CMDSRC_ACCELERATOR )
                {
                switch( LOUSHORT( mp1 ) )
                    {
                    case IDDI_SEARCH_NEXT:
                        FindNext();
                        break;
                    }
                }
            break;

        case WM_SIZE:
            /* Put the list box in the correct location of the window */
            if( hwndList != (HWND)NULL )
                WinSetWindowPos( hwndList, HWND_TOP, 0, 0,
                        SHORT1FROMMP(mp2), SHORT2FROMMP(mp2)-usLineHeight, SWP_SIZE );
            break;

        case WM_CONTROL:
            /* Switch on Control activated */
            switch( SHORT1FROMMP(mp1) )
                {

                /*** Process List Box Activity ***/
                case IDI_LIST:
                    /* was it a double click? */
                    if( SHORT2FROMMP(mp1) == LN_ENTER )
                        {
                        /* get the item clicked on */
                        Index = (USHORT)WinSendMsg( hwndList, LM_QUERYSELECTION,
                                (MPARAM)0, (MPARAM)0 );

                        /* grab its text */
                        WinSendMsg( hwndList, LM_QUERYITEMTEXT,
                                MPFROM2SHORT(Index, MAX_STRING_LEN), MPFROMP(szBuf) );

                        /* if in APP form toggle to ALL form */
                        if( usFormat == APP_FORM )
                            {
                            usFormat = ALL_FORM;
                            fScroll = TRUE;
                            }
                        else
                            {
                            /* if an App name was choosen then go to APP form */
                            if( szBuf[0] != ' ')
                                {
                                usFormat = APP_FORM;
                                fScroll = TRUE;
                                }
                            else
                                /* A Key Value Pair was double clicked
                                 * allow editing of key Value
                                 */
                                {

                                FocusWindow = (HWND)1;

                                hwndDialog = WinLoadDlg( HWND_DESKTOP,
                                    hwndIniEditFrame, ChangeKeyWndProc,
                                    (HMODULE)NULL, IDD_CHANGE_KEY, NULL);

                                Handle = (ULONG)WinSendMsg( hwndList, LM_QUERYITEMHANDLE,
                                    MPFROMSHORT(Index), (MPARAM)NULL );

                                hwndText = WinWindowFromID( hwndDialog, IDDI_CHANGE_KEY_TEXT_APP );
                                WinSetWindowText( hwndText, pGroups[LOUSHORT(Handle)].szAppName);

                                /* note bug in PMWin GPs if full segment */
                                hwndText = WinWindowFromID( hwndDialog, IDDI_CHANGE_KEY_TEXT_KEY );
                                strcpy( szBuf2, pGroups[LOUSHORT(Handle)].pPairs[HIUSHORT(Handle)].szKey );
                                WinSetWindowText( hwndText, szBuf2 );

                                hwndText = WinWindowFromID( hwndDialog, IDDI_CHANGE_KEY_TEXT_VAL );
                                strcpy( szBuf2, pGroups[LOUSHORT(Handle)].pPairs[HIUSHORT(Handle)].szValue );
                                WinSetWindowText( hwndText, szBuf2 );

                                WinPostMsg( hwndText, EM_SETSEL,
                                        MPFROM2SHORT(0, strlen(szBuf2)), (MPARAM)0 );

                                if( WinProcessDlg( hwndDialog ) == IDDI_CHANGE_KEY_OK )
                                    {
                                    TopIndex = (USHORT)WinSendMsg( hwndList, LM_QUERYTOPINDEX,
                                         (MPARAM)NULL, (MPARAM)NULL );

                                    UpdateListBox( TRUE, usFormat );

                                    /* scroll to top */
                                    WinSendMsg( hwndList, LM_SETTOPINDEX,
                                         MPFROMSHORT(TopIndex), (MPARAM)NULL );

                                    /* make the item selected */
                                    WinSendMsg( hwndList, LM_SELECTITEM,
                                            MPFROMSHORT(Index), MPFROMSHORT(TRUE) );

                                    /* make selected */
                                    }

                                WinDestroyWindow( hwndDialog );
                                }
                            }

                        /* Make the double clicked item selected in new form */
                        if( fScroll )
                            {
                            /* put in correct form */
                            UpdateListBox( FALSE, usFormat );

                            /* get the index of the item clicked on */
                            Index = (USHORT)WinSendMsg( hwndList, LM_SEARCHSTRING,
                                    MPFROM2SHORT(LSS_SUBSTRING, LIT_FIRST),
                                    MPFROMP(szBuf) );

                            /* scroll that item to the top */
                            WinSendMsg( hwndList, LM_SETTOPINDEX,
                                    MPFROMSHORT(Index), (MPARAM)NULL );

                            /* make the item selected */
                            WinSendMsg( hwndList, LM_SELECTITEM,
                                    MPFROMSHORT(Index), MPFROMSHORT(TRUE) );
                            }
                        }  /* if ENTER */
                }
            break;

        default:
            return WinDefWindowProc(hwnd, msg, mp1, mp2);
            break;
        }

    return 0L;

}  /* IniEditWndProc */
