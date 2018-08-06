/*--------------------------------------------------------------
   BITLIB.C -- Code entry point for BITLIB dynamic link library
               (c) Charles Petzold,  1990
  --------------------------------------------------------------*/
#include <windows.h>

int FAR PASCAL LibMain (HANDLE hInstance, WORD wDataSeg, WORD wHeapSize,
                        LPSTR lpszCmdLine)
     {
     if (wHeapSize > 0)
          UnlockData (0) ;

     return 1 ;
     }
