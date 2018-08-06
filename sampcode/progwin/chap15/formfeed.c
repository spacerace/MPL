/*---------------------------------------------
   FORMFEED.C -- Advances printer to next page
                 (c) Charles Petzold, 1990
  ---------------------------------------------*/

#include <windows.h>
#include <string.h>

HDC  GetPrinterDC (void) ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szMsg [] = "FormFeed" ; 
     HDC         hdcPrint ;

     if (hdcPrint = GetPrinterDC ())
          {
          if (Escape (hdcPrint, STARTDOC, sizeof szMsg - 1, szMsg, NULL) > 0)
               if (Escape (hdcPrint, NEWFRAME, 0, NULL, NULL) > 0)
                    Escape (hdcPrint, ENDDOC, 0, NULL, NULL) ;

          DeleteDC (hdcPrint) ;
          }
     return FALSE ;
     }

HDC GetPrinterDC (void)
     {
     static char szPrinter [80] ;
     char        *szDevice, *szDriver, *szOutput ;

     GetProfileString ("windows", "device", ",,,", szPrinter, 80) ;

     if ((szDevice = strtok (szPrinter, "," )) &&
         (szDriver = strtok (NULL,      ", ")) && 
         (szOutput = strtok (NULL,      ", ")))
          
               return CreateDC (szDriver, szDevice, szOutput, NULL) ;

     return 0 ;
     }
