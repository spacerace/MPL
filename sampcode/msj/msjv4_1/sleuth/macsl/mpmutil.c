/*-----------------------------------------------------------------*/
/* MpmUtil.c                                                       */
/* Miscellaneous utility functions                                 */
/*-----------------------------------------------------------------*/

#include "MacPM.h"

CHAR      _szNull[1];

LONG      _alSysVal[SV_CSYSVALUES];

SHORT     _sSetFocusDepth;

HWND      _hwndObject;
HWND      _hwndDesktop;
HWND      _hwndActive;
HWND      _hwndFocus;
HWND      _hwndMenu;

PS        _ps1;
PPS       _pps1;
HPS       _hps1;

/*-----------------------------------------------------------------*/
/* Fill *ppid with process and task ID's.  These are fake numbers! */
/*-----------------------------------------------------------------*/

USHORT APIENTRY DosGetPid( ppid )
    PPIDINFO    ppid;
{
    ppid->pid = MY_PID;
    ppid->tid = MY_PID;
    ppid->pidParent = 0;
}

/*-----------------------------------------------------------------*/
/* Find out if a rectangle is empty.                               */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinIsRectEmpty( hab, prcl )
    HAB         hab;
    PRECTL      prcl;
{
    return prcl->xRight <= prcl->xLeft  ||
           prcl->yTop   <= prcl->yBottom;
}

/*-----------------------------------------------------------------*/
/* Load a string from the resource file.                           */
/*-----------------------------------------------------------------*/

SHORT APIENTRY WinLoadString( hab, hmod, id, sMax, pszBuf )
    HAB         hab;
    HMODULE     hmod;
    USHORT      id;
    SHORT       sMax;
    PSZ         pszBuf;
{
    CHAR        szBuf[256];
    SHORT       sLen;

    GetIndString( szBuf, 256, id );

    sLen = szBuf[0];  /* Pascal string length */
    if( sLen > sMax-1 )
      sLen = sMax - 1;

    memcpy( pszBuf, szBuf+1, sLen );
    pszBuf[sLen] = '\0';

    return sLen;
}

/*-----------------------------------------------------------------*/
/* Set one of the SV_ system values.  We should check whether the  */
/* the value is changeable, but who cares...                       */
/*-----------------------------------------------------------------*/

BOOL APIENTRY WinSetSysValue( hwndDesktop, iSysValue, lValue )
    HWND        hwndDesktop;
    SHORT       iSysValue;
    LONG        lValue;
{
    if( iSysValue < 0 || iSysValue >= SV_CSYSVALUES )
      return FALSE;

    _alSysVal[iSysValue] = lValue;

    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Debugging function to display an error message.  Works with     */
/* any Mac debugger.                                               */
/*-----------------------------------------------------------------*/

LOCAL VOID MpmErrorBox( pszMsg )
    PSZ         pszMsg;
{
    CHAR        szMsg[80];

    strcpy( szMsg, pszMsg );
    DebugStr( CtoPstr(szMsg) );
    ExitToShell();
}

/*-----------------------------------------------------------------*/
/* Allocate a Mac memory handle (movable) and zero out the block.  */
/*-----------------------------------------------------------------*/

LOCAL PVOID MpmNewHandleZ( usLen )
    USHORT      usLen;
{
    asm
    {
      clr.L       D0
      move.W      usLen, D0
      NewHandle   CLEAR
      move.L      A0, D0
    }
}

/*-----------------------------------------------------------------*/
/* Allocate a Mac fixed memory block and zero it out.              */
/*-----------------------------------------------------------------*/

LOCAL PVOID MpmNewPtrZ( usLen )
    USHORT      usLen;
{
    asm
    {
      clr.L       D0
      move.W      usLen, D0
      NewPtr      CLEAR
      move.L      A0, D0
    }
}

/*-----------------------------------------------------------------*/
/* Make sure hwnd bears some resemblance to a valid window handle, */
/* by checking its signature word.                                 */
/*-----------------------------------------------------------------*/

LOCAL BOOL MpmValidateWindow( hwnd )
    HWND        hwnd;
{
    if( MYWNDOF(hwnd).signature != WND_SIGNATURE )
    {
      ERROR( "Invalid window handle!" );
      return FALSE;
    }

    return TRUE;
}

/*-----------------------------------------------------------------*/
/* Same as MpmValidateWindow, but allows NULL without complaining. */
/*-----------------------------------------------------------------*/

LOCAL BOOL MpmValidateWindowNull( hwnd )
    HWND        hwnd;
{
    if( hwnd  &&  MYWNDOF(hwnd).signature != WND_SIGNATURE )
    {
      ERROR( "Invalid window handle!" );
      return FALSE;
    }

    return TRUE;
}

/*-----------------------------------------------------------------*/
