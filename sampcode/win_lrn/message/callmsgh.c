#include <windows.h>
#include "callmsgh.h"

HANDLE   hTASK;
FARPROC  lpfnMessageFilterHook;
FARPROC  lpfnOldMessageFilterHook;
HWND     hWnd;

/****************************************************************************/
void FAR PASCAL InitHook (hTask)
HANDLE hTask;
  {
  hTASK = hTask;
  lpfnMessageFilterHook = MakeProcInstance ( (FARPROC)CallMsgMessageFilter,
      LIBINST);
  lpfnOldMessageFilterHook = SetWindowsHook (WH_MSGFILTER,
      lpfnMessageFilterHook);
  }


/****************************************************************************/

BOOL FAR PASCAL KillHook ()
  {
  BOOL  fSuccess;

  fSuccess = UnhookWindowsHook (WH_MSGFILTER, lpfnMessageFilterHook);
  if (fSuccess)
    {
    FreeProcInstance (lpfnMessageFilterHook);
    MessageBox (GetFocus (), "Message filter Unhooked.",
        "Hook", MB_OK);
    }
  return fSuccess;
  }


/****************************************************************************/

DWORD FAR PASCAL CallMsgMessageFilter (nCode, wParam, lParam)
int	nCode;
WORD  wParam;
LONG  lParam;
  {
  if (nCode == HC_ACTION)
    {
    hWnd = FindWindow (NULL, "CallMsgFilter");
    PostMessage (hWnd, WM_COMMAND, IDM_FROMMSGFILTER, 0L);
    return TRUE;
    }
  else
    return DefHookProc (nCode, wParam, lParam, &lpfnOldMessageFilterHook);
  }
