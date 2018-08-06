#include <windows.h>

void FAR PASCAL locate();


int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR  lpszCmdLine;
int    cmdShow;
{
    locate();
    return 0;
}

void FAR PASCAL locate()
{
  char   buf[60];

  GetModuleFileName( GetModuleHandle((LPSTR)"winfind"), buf, 60 );
  MessageBox( GetFocus(), (LPSTR)buf, (LPSTR)"module loaded from", MB_OK);

}

