#include <windows.h>

int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{

  int     i, j, q;

  MessageBox( GetFocus(), (LPSTR)"orginal", (LPSTR)"Sound routine", MB_OK);

   OpenSound();
   q=SetVoiceQueueSize(1,192);
   for (i=0; i < 60 ; i++) {  /* change frequency */
      for (j=0; j < 6 ; j++) {  /* change duration */
         SetVoiceSound(q,(LONG)440+(i*2),j);
         StartSound();
         WaitSoundState(QUEUEEMPTY);
      }
   }
   CloseSound();

  MessageBox( GetFocus(), (LPSTR)"Excel macro", (LPSTR)"Sound from", MB_OK);
 /* excel music macro */
  OpenSound();
  SetVoiceAccent(1,(LONG)100,255,0,0);
  q=SetVoiceQueueSize(1,400);
  SetVoiceNote(q,45,16,0); /* G# */
  StartSound();
  WaitSoundState(QUEUEEMPTY);
  CloseSound();

  StopSound();
  return 0;
}

