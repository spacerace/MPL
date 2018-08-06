/*
 *  GetKeyboardState
 *  
 *  This program demonstrates the use of the function GetKeyboardState.
 *  This function copies the status of the 256 virtual-keyboard keys to the
 *  buffer specified by the parameter.
 *  
 */

#include <windows.h>

int PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
  static char szFuncName[] = "GetKeyboardState";
  char rgcKeyState[256];
  char szOutputBuffer1 [500];
  int ncount, I;
    
  MessageBox(GetFocus(), (LPSTR)"Getting the state of the keyboard",
             (LPSTR)szFuncName, MB_OK);

  (void)GetKeyboardState((LPSTR)rgcKeyState);

  if (rgcKeyState [VK_F1] & 0x80)
     MessageBox(GetFocus(), (LPSTR)"The F1 key is Down",
                (LPSTR)szFuncName, MB_OK);
  else
     MessageBox(GetFocus (), (LPSTR)"The F1 Key is Up",
                (LPSTR)szFuncName, MB_OK);

  rgcKeyState [VK_F1] = 0x80;

  MessageBox (GetFocus (),(LPSTR)"The F1 key has been set to the down state.", 
              (LPSTR)"About to use SetKeyboardState.", MB_OK);

  SetKeyboardState (rgcKeyState);
  GetKeyboardState (rgcKeyState);

  if (rgcKeyState [VK_F1] & 0x80)
     MessageBox(GetFocus(), (LPSTR)"The F1 key has been set successfully", 
                (LPSTR)"The F1 key is Down", MB_OK);
  else
     MessageBox(GetFocus(), (LPSTR)"The F1 was not set successfully",
                (LPSTR)"The F1 Key is Up", MB_OK);
  return 0;
}

