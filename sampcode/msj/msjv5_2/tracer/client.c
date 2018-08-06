//-------------------------
//-CLIENT.C----------------
//-------------------------

#define INCL_PM
#define INCL_WIN
#define INCL_DOS

#include <os2.h>
#include <stdio.h>
#include <string.h>

#include "client.h"

// TRACER step 1 - include tracer.h
#include "tracer.h"

// TRACER step 2 - declare TRACER VARIABLES
TRACERVARIABLES

//----------------------------------------------
//-main()---------------------------------------
//----------------------------------------------

int cdecl main( )
{
    QMSG      qmsg;
    ULONG     ctldata;

    hAB = WinInitialize( NULL );

    hmqClient = WinCreateMsgQueue( hAB, 0 );

    if ( !WinRegisterClass( hAB,
                           (PCH)szPClass,
                           (PFNWP)ClientWndProc,
                           CS_SIZEREDRAW,
                           0) )
        return( 0 );

    ctldata = FCF_TITLEBAR      | FCF_SYSMENU       |
              FCF_SIZEBORDER    | FCF_MINMAX        |
              FCF_ICON          | FCF_SHELLPOSITION |
              FCF_TASKLIST ;

    hPanelFrm = WinCreateStdWindow( HWND_DESKTOP,
                                     WS_DISABLED,
                                     &ctldata,
                                     (PCH)szPClass,
                                     NULL,
                                     0L,
                                     (HMODULE)NULL,
                                     CLIENTICON,
                                     (HWND FAR *)&hPanelWnd );

    WinEnableWindow( hPanelFrm, TRUE );
    WinShowWindow( hPanelFrm, TRUE );

    while( WinGetMsg( hAB, (PQMSG)&qmsg, (HWND)NULL, 0, 0 ) )
        WinDispatchMsg( hAB, (PQMSG)&qmsg );

    WinDestroyWindow( hPanelFrm );
    WinDestroyMsgQueue( hmqClient );
    WinTerminate( hAB );
}

//----------------------------------------------
//-ClientWndProc()------------------------------
//----------------------------------------------

MRESULT EXPENTRY ClientWndProc( HWND hWnd, USHORT msg,
                                MPARAM mp1, MPARAM mp2 )

{
    switch (msg)
    {
    case WM_CREATE:

         // TRACER step 3 - attach to TRACER
         TRACERHELLO

         return( WinDefWindowProc( hWnd, msg, mp1, mp2 ) );
         break;

    // TRACER step 4 - obtain system resources - Don't forget break;
    TRACERATTACH

    break;

    case WM_CLOSE:

         // TRACER step 5
         TRACER("Posting QUIT");
         WinPostMsg( hWnd, WM_QUIT, 0L, 0L );

         break;

    case WM_SETFOCUS:

         // TRACER step 5
         TRACER("Setting Focus");
         return( WinDefWindowProc( hWnd, msg, mp1, mp2 ) );

         break;

    case WM_ENABLE:

         return( WinDefWindowProc( hWnd, msg, mp1, mp2 ) );

         break;

    case WM_BUTTON1DOWN:

         sprintf( szTraceBuf, "WM_BUTTON1DOWN hit at %ld",
                        WinGetCurrentTime( hAB ) );

         TRACER( szTraceBuf );

         break;

    case WM_PAINT:

         ClientPaint( hWnd, msg, mp1, mp2 );

         break;

    case WM_ERASEBACKGROUND:

         return( TRUE );

         break;

    default:
         return( WinDefWindowProc( hWnd, msg, mp1, mp2 ) );
         break;

    }
    return(0L);
}

//----------------------------------------------
//-ClientPaint()--------------------------------
//----------------------------------------------

void FAR PASCAL ClientPaint( HWND hWnd, USHORT msg,
                             MPARAM mp1, MPARAM mp2 )
{

int         aiCharWidth;
HPS         hPS;
FONTMETRICS fm;
RECTL       rRect;
CHARBUNDLE  cb;
POINTL      pt;

    hPS = WinBeginPaint( hWnd, (HPS)NULL, (PWRECT)NULL );

    // TRACER step 5
    TRACER("ClientPaint()");

    GpiErase( hPS );

    GpiQueryFontMetrics( hPS, (LONG)sizeof fm , &fm);
    aiCharWidth = (SHORT)fm.lAveCharWidth;

    WinQueryWindowRect( hWnd, &rRect );
    pt.x = (rRect.xRight / 2) -
            ((strlen(szMessage) / 2) * aiCharWidth);
    pt.y = (rRect.yTop / 2);

    cb.lColor = CLR_BLACK;
    GpiSetAttrs( hPS, PRIM_CHAR, CBB_COLOR, 0L, (PBUNDLE)&cb );

    GpiCharStringAt( hPS, &pt, (LONG)strlen( szMessage ), szMessage );
    WinEndPaint( hPS );
}
