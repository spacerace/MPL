/*
 *
 *  GetCommEventMask
 *  
 *  This program demonstrates the use of the function GetCommEventMask.
 *  This function retrieves the value of the current event mask, and then
 *  clears the mask.
 *
 */

#include <windows.h>

int PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
  int nCid;         /* Comm Port ID.     */
  WORD wEvent;      /* Event mask value. */

  /* Open COM1. */
  nCid = OpenComm((LPSTR)"COM1", 50, 50);
  if (nCid < 0)
    {
    MessageBox(GetFocus(), (LPSTR)"OpenComm Failed", (LPSTR)"OpenComm()",
               MB_OK | MB_ICONEXCLAMATION);
    return 1;
    }

  /* Set the event mask to allow for detection of input break events. */
  SetCommEventMask(nCid, EV_BREAK);
  
  /* Get the event mask and check to see if a break was detected. This
     would normally be put in a WM_TIMER message for use over time in
     a communications setting. */
  MessageBox(GetFocus(), (LPSTR)"Requesting Communications Event Mask", 
             (LPSTR)"GetCommEventMask()", MB_OK);
  wEvent = GetCommEventMask(nCid, EV_BREAK);
  if ((wEvent & EV_BREAK) != 0)
     MessageBox(GetFocus(),
                (LPSTR)"A Break Event was Detected.",
                (LPSTR)"GetCommEventMask()",
                MB_OK);
  else
     MessageBox(GetFocus(),
                (LPSTR)"A Break Event has Not Arrived.",
                (LPSTR)"GetCommEventMask()",
                MB_OK);

  /* Cleanup. */
  CloseComm(nCid);
  return 0;
}
