#include "os2.h"
#include "st.h"
#include "client.h"

/*
 *  Routine ClientInit registers the window classes for
 *  the Graphics Exchange client application and creates the
 *  frame, graphics, and DDE anchor windows.
 */


HWND ClientInit()
{
RECTL rect;            /* window rectangle work space for positioning */
ULONG CtlData;         /* frame window control specifications         */
HWND hFrame, hClient;  /* frame and client window handles             */

    if (!WinRegisterClass(hab1,(PSZ)"Master",(PFNWP)ClientWndProc,(ULONG)NULL,3*4))
        return(FALSE);
    if (!WinRegisterClass(hab1,(PSZ)"DDE_Win_P",(PFNWP)ClientDDEParWndProc,(ULONG)NULL,4*4))
        return(FALSE);
    if (!WinRegisterClass(hab1,(PSZ)"DDE_Win",(PFNWP)ClientDDEWndProc,(ULONG)NULL,4*4))
        return(FALSE);
    WinRegisterClass((HAB)NULL,(PSZ)"GraphicsCtl",(PFNWP)GraphicsCtlWndProc,CS_SIZEREDRAW,4);

    CtlData = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER | FCF_MINMAX;
    hFrame = WinCreateStdWindow(HWND_DESKTOP, (ULONG)NULL, &CtlData, (PSZ)"Master",
                (PSZ)"Client", 0L, (HMODULE)NULL, ID_MASTER, (HWND FAR *)&hClient);

    WinQueryWindowRect(HWND_DESKTOP, &rect);
    WinSetWindowPos(hFrame, HWND_TOP, 50,LOUSHORT(rect.yTop) -200,450,180,SWP_SIZE|SWP_MOVE);
    WinShowWindow(hFrame,TRUE);
    WinSetFocus(HWND_DESKTOP,hFrame);

    WinQueryWindowRect(hClient, &rect);
    GdCreateGraphicsCtl(hClient,WS_VISIBLE|ICS_AUTOPOS,0,0,LOUSHORT(rect.xRight-rect.xLeft),
                    LOUSHORT(rect.yTop-rect.yBottom),120,10,10,hClient,ID_GRAPHICS1,(PVOID)NULL,(PVOID)NULL);

    return (hFrame);
}
