/*
 *  Function Name:   CloseSound
 *  Program Name:    CloseSnd.c
 *
 *  Description:
 *   CloseSound closes access to the play device and frees the device for
 *   opening by other applications.  the CloseSound function flushes all
 *   voice queues and frees any buffers allocated for these queues.
 */

#include "windows.h"
#include "stdio.h"

long	FAR PASCAL WndProc (HWND, unsigned, WORD, LONG);

/***********************************************************************/

void CALL_CloseSound (hWnd)
HWND hWnd;

{
  char	szBuffer[30];
  short	nVoices; /* Specifies the number of voices available. */

  nVoices = OpenSound (); /* Opens access to play device, locks it from  */
/* further users, returns the number of voices */
/* available.                                  */

  if (nVoices == S_SERDVNA)
    MessageBox (NULL, (LPSTR) "Play device in use.", (LPSTR) "Error", MB_OK);
  else if (nVoices == S_SEROFM)
    MessageBox (NULL, (LPSTR) "Insufficient memory available.",
        (LPSTR) "Error\0", MB_OK);
  else
  {
    sprintf (szBuffer, "%d voice (s) are available.", nVoices);
    MessageBox (NULL, (LPSTR) szBuffer,
        (LPSTR)"Mission Accomplished\0", MB_OK);

    CloseSound ();    /* Closesnd closes the play device, allowing other */
/* applications to use the device.                 */
  }
  if ( (nVoices != S_SEROFM) && (nVoices != S_SERDVNA))
  {
    MessageBox (NULL, (LPSTR)"CloseSound complete, device available.",
        (LPSTR)"Mission Accomplished\0", MB_OK);
  }
  else
  {
    MessageBox (NULL, (LPSTR)"Device unavailable, try later",
        (LPSTR)"Mission Failed", MB_OK);

  }

  return;
}


/**************************************************************************/

int	PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int	cmdShow;
{
  MSG   msg;
  HWND  hWnd;
  HMENU hMenu;

  CALL_CloseSound (hWnd);
  return 0;
}


