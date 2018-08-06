/******************************* Module Header ******************************\
* Module Name: DlgProc.c
*
* Created by Microsoft Corporation, 1989
*
*
* System Test Application
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
#define INCL_WINLISTBOXES
#define INCL_WINENTRYFIELDS
#define INCL_WINDIALOGS
#define INCL_GPIBITMAPS
#define INCL_GPIREGIONS
#define INCL_GPILCIDS
#define INCL_GPIPRIMITIVES
#define INCL_DEV

#include <string.h>
#include <stdio.h>

#include <os2.h>

#include "IniEdit.h"


/******************************* Constants *********************************/

#define BUF_SIZE 132


/******************************** Globals **********************************/

static CHAR   szSearch[BUF_SIZE] = { 0 };           // Current search string
static USHORT usLastIndex = 0;                      // Last Searched Item

/******************************* Externals *********************************/

extern USHORT        cAppNames;                     // see iniedit.c
extern HWND          hwndList;
extern PGROUPSTRUCT  pGroups;
extern HAB           habIniEdit;
extern HWND          FocusWindow;


/****************************** Function Header ****************************\
*
* SearchWndProc
*
*
* Handles the Search Dialog Box messages
*
\***************************************************************************/

MRESULT _loadds EXPENTRY SearchWndProc(HWND hwndDialog, USHORT msg,
        MPARAM mp1, MPARAM mp2)
{
    HWND   hwndText;                            // Current Text Window


    switch (msg)
        {

        case WM_INITDLG:
            hwndText = WinWindowFromID( hwndDialog, IDDI_SEARCH_TEXT );
            WinSetWindowText(hwndText, szSearch);
            WinSendMsg( hwndText, EM_SETSEL,
                    MPFROM2SHORT(0, strlen(szSearch)), (MPARAM)0 );

            break;

        case WM_COMMAND:
            switch( LOUSHORT( mp1 ) )
                {

                case IDDI_SEARCH_OK:
                    hwndText = WinWindowFromID( hwndDialog, IDDI_SEARCH_TEXT );
                    WinQueryWindowText( hwndText, BUF_SIZE, szSearch );
                    WinDismissDlg( hwndDialog, 0 );

                    if( (usLastIndex = SHORT1FROMMR(WinSendMsg( hwndList, LM_SEARCHSTRING,
                            MPFROM2SHORT( LSS_SUBSTRING, LIT_FIRST),
                            MPFROMP( szSearch )) ) != LIT_NONE ))
                        {
                        WinSendMsg( hwndList, LM_SELECTITEM,
                                MPFROM2SHORT( (usLastIndex), NULL),
                                MPFROM2SHORT( TRUE, NULL ) );
                        }
                    else  /* not found */
                        {
                        usLastIndex = LIT_FIRST;
                        WinAlarm( HWND_DESKTOP, 0);
                        }
                    break;

                case IDDI_SEARCH_NEXT:
                    FindNext();
                    break;

                default:
                    return WinDefDlgProc(hwndDialog, msg, mp1, mp2);
                    break;
                }

        default:
            return WinDefDlgProc(hwndDialog, msg, mp1, mp2);
            break;
        }

    return 0L;

}  /* SearchWndProc */


/****************************** Function Header ****************************\
*
* FindNext
*
*
* Finds the next instance of the current search string starting from the
* Last searched position
*
\***************************************************************************/

VOID FindNext()
{
   if( (usLastIndex = SHORT1FROMMR(WinSendMsg( hwndList, LM_SEARCHSTRING,
           MPFROM2SHORT( LSS_SUBSTRING, usLastIndex),
           MPFROMP( szSearch )) ) != LIT_NONE ))
       {
       WinSendMsg( hwndList, LM_SELECTITEM,
               MPFROM2SHORT( (usLastIndex), NULL),
               MPFROM2SHORT( TRUE, NULL ) );
       }
   else   /* alarm if not found */
       WinAlarm( HWND_DESKTOP, 0);

}  /* FindNext */


/****************************** Function Header ****************************\
*
* AddKeyWndProc
*
*
* Handles the AddKey Dialog Box messages
* Will facilitate adding new keys for a given App Name
*
\***************************************************************************/

MRESULT _loadds EXPENTRY AddKeyWndProc(HWND hwndDialog, USHORT msg,
        MPARAM mp1, MPARAM mp2)
{
    HWND   hwndTextApp;                         // Handle for App Text Window
    HWND   hwndTextKey;
    HWND   hwndTextValue;
    CHAR   szApp[BUF_SIZE];                     // String Contents
    CHAR   szKey[BUF_SIZE];
    CHAR   szValue[BUF_SIZE];


    switch (msg)
        {
        case WM_INITDLG:
            WinSendDlgItemMsg(hwndDialog, IDDI_ADD_KEY_TEXT_APP, EM_SETTEXTLIMIT,
                    MPFROMSHORT(MAX_STRING_LEN), 0L);
            WinSendDlgItemMsg(hwndDialog, IDDI_ADD_KEY_TEXT_KEY, EM_SETTEXTLIMIT,
                    MPFROMSHORT(MAX_STRING_LEN), 0L);
            WinSendDlgItemMsg(hwndDialog, IDDI_ADD_KEY_TEXT_VAL, EM_SETTEXTLIMIT,
                    MPFROMSHORT(MAX_STRING_LEN), 0L);
            break;
        case WM_COMMAND:
            switch( LOUSHORT( mp1 ) )
                {

                case IDDI_ADD_KEY_OK:
                    hwndTextApp = WinWindowFromID( hwndDialog, IDDI_ADD_KEY_TEXT_APP );
                    WinQueryWindowText( hwndTextApp, BUF_SIZE, szApp );

                    hwndTextKey = WinWindowFromID( hwndDialog, IDDI_ADD_KEY_TEXT_KEY );
                    WinQueryWindowText( hwndTextKey, BUF_SIZE, szKey );

                    hwndTextValue = WinWindowFromID( hwndDialog, IDDI_ADD_KEY_TEXT_VAL );
                    WinQueryWindowText( hwndTextValue, BUF_SIZE, szValue );

                    WinDismissDlg( hwndDialog, 0 );

                    /* if the App is NULL forget it */
                    if( *szApp == (CHAR)0 )
                        {
                        break;
                        }

                    /* if the Key is NULL forget it */
                    if( *szKey == (CHAR)0 )
                        {
                        break;
                        }

                    /* if the Value is NULL forget it */
                    if( *szValue == (CHAR)0 )
                        {
                        break;
                        }

                    if( !WinWriteProfileString( habIniEdit, szApp, szKey, szValue ) )
                        ;
                    break;

                default:
                    return WinDefDlgProc(hwndDialog, msg, mp1, mp2);
                    break;
                }

        default:
            return WinDefDlgProc(hwndDialog, msg, mp1, mp2);
            break;
        }

    return 0L;

}  /* AddKeyWndProc */


/****************************** Function Header ****************************\
*
* ChangeKeyWndProc
*
*
* Handles the ChangeKey Dialog Box messages
* Will facilitate changing a key's value given an app, key and new value
*
\***************************************************************************/

MRESULT _loadds EXPENTRY ChangeKeyWndProc(HWND hwndDialog, USHORT msg,
        MPARAM mp1, MPARAM mp2)
{
    HWND     hwndTextApp;                       // Handle for App Text Window
    HWND     hwndTextKey;
    HWND     hwndTextVal;
    CHAR     szApp[BUF_SIZE];                   // String Contents
    CHAR     szKey[BUF_SIZE];
    CHAR     szVal[BUF_SIZE];


    switch (msg)
        {
        case WM_INITDLG:
            if( FocusWindow )
                {

                FocusWindow = WinWindowFromID( hwndDialog, IDDI_CHANGE_KEY_TEXT_VAL );
                WinSetFocus( HWND_DESKTOP, FocusWindow);
                WinQueryWindowText( FocusWindow, BUF_SIZE, szVal );

                FocusWindow = (HWND)NULL;

                return((MRESULT) TRUE );
                } else {
                WinSendDlgItemMsg(hwndDialog, IDDI_CHANGE_KEY_TEXT_APP, EM_SETTEXTLIMIT,
                        MPFROMSHORT(MAX_STRING_LEN), 0L);
                WinSendDlgItemMsg(hwndDialog, IDDI_CHANGE_KEY_TEXT_KEY, EM_SETTEXTLIMIT,
                        MPFROMSHORT(MAX_STRING_LEN), 0L);
                WinSendDlgItemMsg(hwndDialog, IDDI_CHANGE_KEY_TEXT_VAL, EM_SETTEXTLIMIT,
                        MPFROMSHORT(MAX_STRING_LEN), 0L);
                }
            break;

        case WM_COMMAND:
            switch( LOUSHORT( mp1 ) )
                {

                case IDDI_CHANGE_KEY_OK:
                    hwndTextApp = WinWindowFromID( hwndDialog, IDDI_CHANGE_KEY_TEXT_APP );
                    WinQueryWindowText( hwndTextApp, BUF_SIZE, szApp );

                    hwndTextKey = WinWindowFromID( hwndDialog, IDDI_CHANGE_KEY_TEXT_KEY );
                    WinQueryWindowText( hwndTextKey, BUF_SIZE, szKey );

                    hwndTextVal = WinWindowFromID( hwndDialog, IDDI_CHANGE_KEY_TEXT_VAL );
                    WinQueryWindowText( hwndTextVal, BUF_SIZE, szVal );


                    WinDismissDlg( hwndDialog, IDDI_CHANGE_KEY_OK );

                    /* if the App is NULL forget it */
                    if( *szApp == (CHAR)0 )
                        {
                        break;
                        }

                    /* if the Key is NULL forget it */
                    if( *szKey == (CHAR)0 )
                        {
                        break;
                        }

                    /* if the Value is NULL forget it */
                    if( *szVal == (CHAR)0 )
                        {
                        break;
                        }


                    if( !WinWriteProfileString( habIniEdit, szApp, szKey, szVal ) )

                    break;

                default:
                    return WinDefDlgProc(hwndDialog, msg, mp1, mp2);
                    break;
                }

        default:
            return WinDefDlgProc(hwndDialog, msg, mp1, mp2);
            break;
        }

    return 0L;

}  /* ChangeKeyWndProc */


/****************************** Function Header ****************************\
*
* DelKeyWndProc
*
*
* Handles the DelKey Dialog Box messages
* Will facilitate deleting a key value given an app and the key
*
\***************************************************************************/

MRESULT _loadds EXPENTRY DelKeyWndProc(HWND hwndDialog, USHORT msg,
        MPARAM mp1, MPARAM mp2)
{
    HWND   hwndTextApp;                         // Handle for App Text Window
    HWND   hwndTextKey;
    CHAR   szApp[BUF_SIZE];                     // String Contents
    CHAR   szKey[BUF_SIZE];


    switch (msg)
        {
        case WM_INITDLG:
            WinSendDlgItemMsg(hwndDialog, IDDI_DEL_KEY_TEXT_APP, EM_SETTEXTLIMIT,
                    MPFROMSHORT(MAX_STRING_LEN), 0L);
            WinSendDlgItemMsg(hwndDialog, IDDI_DEL_KEY_TEXT_KEY, EM_SETTEXTLIMIT,
                    MPFROMSHORT(MAX_STRING_LEN), 0L);
            break;
        case WM_COMMAND:
            switch( LOUSHORT( mp1 ) )
                {

                case IDDI_DEL_KEY_OK:
                    hwndTextApp = WinWindowFromID( hwndDialog, IDDI_DEL_KEY_TEXT_APP );
                    WinQueryWindowText( hwndTextApp, BUF_SIZE, szApp );

                    hwndTextKey = WinWindowFromID( hwndDialog, IDDI_DEL_KEY_TEXT_KEY );
                    WinQueryWindowText( hwndTextKey, BUF_SIZE, szKey );


                    WinDismissDlg( hwndDialog, 0 );

                    /* if the App is NULL forget it */
                    if( *szApp == (CHAR)0 )
                        {
                        break;
                        }

                    /* if the Key is NULL forget it */
                    if( *szKey == (CHAR)0 )
                        {
                        break;
                        }


                    if( !WinWriteProfileString( habIniEdit, szApp, szKey, (PCHAR)NULL ) )
                        ;
                    break;

                default:
                    return WinDefDlgProc(hwndDialog, msg, mp1, mp2);
                    break;
                }

        default:
            return WinDefDlgProc(hwndDialog, msg, mp1, mp2);
            break;
        }

    return 0L;

}  /* DelKeyProc */


/****************************** Function Header ****************************\
*
* DelAppWndProc
*
*
* Handles the DelApp Dialog Box messages
* Will facilitate deleting all keys from a given app name
*
\***************************************************************************/

MRESULT _loadds EXPENTRY DelAppWndProc(HWND hwndDialog, USHORT msg,
        MPARAM mp1, MPARAM mp2)
{
    HWND   hwndTextApp;                         // App Name Window
    CHAR   szApp[BUF_SIZE];                     // String Contents of Window


    switch (msg)
        {
        case WM_INITDLG:
            WinSendDlgItemMsg(hwndDialog, IDDI_DEL_APP_TEXT_APP, EM_SETTEXTLIMIT,
                    MPFROMSHORT(MAX_STRING_LEN), 0L);
            break;

        case WM_COMMAND:
            switch( LOUSHORT( mp1 ) )
                {

                case IDDI_DEL_APP_OK:
                    hwndTextApp = WinWindowFromID( hwndDialog, IDDI_DEL_APP_TEXT_APP );
                    WinQueryWindowText( hwndTextApp, BUF_SIZE, szApp );

                    WinDismissDlg( hwndDialog, 0 );

                    /* if the App is NULL forget it */
                    if( *szApp == (CHAR)0 )
                        {
                        break;
                        }

                    if( !WinWriteProfileString( habIniEdit, szApp, (PCHAR)NULL, (PCHAR)NULL ) )
                        ;

                    break;

                default:
                    return WinDefDlgProc(hwndDialog, msg, mp1, mp2);
                    break;
                }

        default:
            return WinDefDlgProc(hwndDialog, msg, mp1, mp2);
            break;
        }

    return 0L;

}  /* DelAppWndProc */

