/*
 *  OpenSound
 *
 *  This program demonstrates the use of the function OpenSound.
 *  This function opens access to the play device ande prevents subsequent
 *  opening of the device by other applications.  This function has no
 *  parameters.
 *
 */

#include "windows.h"

int	sprintf (PSTR, PSTR, int);

int	PASCAL WinMain (hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int	cmdShow;
{
  short	nVoices;
  char	szBuffer[30];

  nVoices = OpenSound ();   /* Opens access to the play device and prevents
                             * subsequent opening of the device by other
                             * applications.  Return value specifies the
                                   * number of voices available. */

  CloseSound ();  /* CloseSound must be invoked to allow other applications
                        * to access the play device */

/* return code for OpenSound routine */
  if (nVoices == S_SERDVNA)
  {
    MessageBox (NULL, (LPSTR)"Play device in use",
        (LPSTR)"Done", MB_OK);
  }
  else if (nVoices == S_SEROFM)
  {
    MessageBox (NULL, (LPSTR)"Insufficient memory available",
        (LPSTR)"Done", MB_OK);
  }
  else
  {
    sprintf (szBuffer, "%d voice (s) are available", nVoices);
    MessageBox (GetFocus (), (LPSTR)szBuffer,
        (LPSTR)"Done", MB_OK);
  }
  return 0;
}


