//-----------------------------------------------
//-TRACERP.C Source code for TRACER paint routine
//-----------------------------------------------

#define INCL_PM
#define INCL_WIN
#define INCL_DOS

#include <os2.h>
#include <string.h>
#include "tracer.h"

//----------------------------------------------
//-Module declarations--------------------------
//----------------------------------------------
void FAR PASCAL ClearLB( HWND hLB );
static int iFirstTimeOnly;

//----------------------------------------------
//-External references--------------------------
//----------------------------------------------
extern   HWND       hLB;
extern   HWND       hParentWnd;
extern   int        iNumberItems;

//----------------------------------------------
//-TRACERPaint()--------------------------------
//----------------------------------------------

void FAR PASCAL TRACERPaint( HWND hWnd, USHORT msg,
                               MPARAM mp1, MPARAM mp2 )
{

HPS         hPS;
RECTL       rRect;

    // create listbox on first paint only
    if ( ! iFirstTimeOnly )
    {
        hLB = WinCreateWindow( hParentWnd,
                               WC_LISTBOX,
                               "",
                               WS_VISIBLE | WS_SYNCPAINT,
                               4, 4, 5, 5,
                               hParentWnd,
                               HWND_TOP,
                               ID_TRACERLB,
                               NULL,
                               0 );

        WinQueryWindowRect( hWnd, &rRect );
        WinSetWindowPos ( hLB, HWND_TOP, 6,
                          4,
                          (SHORT)(rRect.xRight - rRect.xLeft - 14),
                          (SHORT)(rRect.yTop - rRect.yBottom - 4),
                          SWP_SIZE | SWP_MOVE | SWP_SHOW );
        ClearLB( hLB );
        iFirstTimeOnly = 1;
    }

    // quick and dirty
    hPS = WinBeginPaint( hWnd, (HPS)NULL, (PWRECT)NULL );
    GpiErase( hPS );
    WinEndPaint( hPS );
}

//----------------------------------------------
//-ClearLB()------------------------------------
//----------------------------------------------

void FAR PASCAL ClearLB( HWND hLB )
{
        iNumberItems = 0;
        WinSendMsg( hLB, LM_DELETEALL,
                       (MPARAM)-1L, (MPARAM)0L );
        WinSendMsg( hLB, LM_INSERTITEM,
                       (MPARAM)-1L, (MPARAM)(PCH)"Begin..." );
        WinSendMsg( hLB, LM_SELECTITEM,  (MPARAM)iNumberItems++,
                       (MPARAM)TRUE );
}
