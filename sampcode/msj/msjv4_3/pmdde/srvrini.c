/*
 *  Routine PhoneInit registers the window classes for
 *  the Graphics Exchange server application and creates the
 *  frame and listbox windows.  Note that the DDE anchor
 *  window uses the system default winproc.
 */

#include "os2.h"
#include "st.h"
#include "server.h"

HWND   hStdLB;     /* listbox handle */

HWND PhoneInit()
{

     HWND        hGrFrame, hGrClient;  /* frame and client window handles */
     RECTL       rect;                 /* rectangle for positioning       */
     ULONG       CtlData;              /* frame control bit field         */

     if(!WinRegisterClass(hab,(PSZ)"Graphics",(PFNWP)GraphicWndProc,CS_SIZEREDRAW, 4))
         return(FALSE);
     if(!WinRegisterClass(hab,(PSZ)"DDEConversation",(PFNWP)ConversationWndProc,(ULONG)NULL, 4))
         return(FALSE);
     if(!WinRegisterClass(hab,(PSZ)"DDEManager",(PFNWP)WinDefWindowProc,(ULONG)NULL, 0))
         return(FALSE);

     CtlData = FCF_TITLEBAR | FCF_SYSMENU | FCF_MENU | FCF_SIZEBORDER | FCF_MINMAX;

     hGrFrame = WinCreateStdWindow(HWND_DESKTOP, (ULONG)NULL, &CtlData, (PSZ)"Graphics",
                                   (PSZ)"Server", 0L, (HMODULE)NULL, ID_GRAPHICS,
                                   (HWND FAR *)&hGrClient);
     hStdLB = WinCreateWindow(hGrClient, WC_LISTBOX, NULL, WS_VISIBLE,0,0,0,0,
                              hGrClient, HWND_TOP, ID_STD_LB, (PVOID)NULL, (PVOID)NULL);

     WinSetWindowPos(hGrFrame, HWND_TOP,100,5,500,340,SWP_SIZE|SWP_MOVE);
     WinQueryWindowRect(hGrClient, &rect);
     WinSetWindowPos(hStdLB, HWND_TOP, LOUSHORT(rect.xLeft), LOUSHORT(rect.yBottom),
                     LOUSHORT(rect.xRight-rect.xLeft), LOUSHORT(rect.yTop-rect.yBottom),
                     SWP_SIZE | SWP_MOVE);
     WinShowWindow(hGrFrame,TRUE);
     WinSetFocus(HWND_DESKTOP,hGrFrame);
     return(hGrFrame);
}
