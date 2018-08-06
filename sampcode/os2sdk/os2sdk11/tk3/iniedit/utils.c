/******************************* Module Header ******************************\
* Module Name: Utils.c
*
*
*
*
* PM OS2.ini Editor
*
* Miscellaneous Utility Functions
*
*
\***************************************************************************/

#define LINT_ARGS
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define INCL_WINERRORS
#define INCL_WINMESSAGEMGR

#include <os2.h>

#include "IniEdit.h"

/******************************** Globals **********************************/


/******************************* Externals *********************************/

extern HAB habIniEdit;

void ErrMessage( char *szMsg )
{

    char      szMsgBuf[132];
    ERRORID   ErrorNo;

    ErrorNo = WinGetLastError( habIniEdit );

    sprintf( szMsgBuf, "%s; Error: %p", szMsg, ErrorNo );

    WinMessageBox( HWND_DESKTOP, HWND_DESKTOP, szMsgBuf, "System Print", 0,
         MB_ICONEXCLAMATION );
}
