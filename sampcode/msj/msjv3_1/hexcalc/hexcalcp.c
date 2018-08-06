/*

Figure 6pm
=========

*/

/*------------------------------------------------------------------
   HEXCALCP.C -- Hexadecimal Calculator (OS/2 Presentation Manager)
  ------------------------------------------------------------------*/

#include <os2.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hexcalcp.h"

ULONG EXPENTRY ClientWndProc (HWND, USHORT, ULONG, ULONG) ;

main ()
     {
     static CHAR szClassName [] = "HexCalcP" ;
     HAB         hAB ;
     HMQ         hMQ ;
     HWND        hWnd ;
     QMSG        qmsg ;

     hAB = WinInitialize () ;
     hMQ = WinCreateMsgQueue (hAB, 0) ;

     WinRegisterClass (hAB, szClassName, ClientWndProc, 0L, 0, NULL) ;

     hWnd = WinLoadDlg (HWND_DESKTOP, HWND_DESKTOP,
                                      NULL, NULL, ID_HEXCALC, NULL) ;

     WinSetFocus (HWND_DESKTOP, WinWindowFromID (hWnd, FID_CLIENT)) ;

     while (WinGetMsg (hAB, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hAB, &qmsg) ;

     WinDestroyWindow (hWnd) ;
     WinDestroyMsgQueue (hMQ) ;
     WinTerminate (hAB) ;

     return 0 ;
     }

void ShowNumber (hWnd, lNumber)
     HWND  hWnd ;
     ULONG lNumber ;
     {
     CHAR  szBuffer [20] ;
     HWND  hWndResult ;

     hWndResult = WinWindowFromID (hWnd, VK_ESCAPE) ;

     WinSetWindowText (hWndResult,
                         strupr (ltoa (lNumber, szBuffer, 16))) ;
     }

ULONG CalcIt (lFirstNum, iOperation, lNum)
     ULONG lFirstNum, lNum ;
     SHORT iOperation ;
     {
     switch (iOperation)
          {
          case '=' : return lNum ;
          case '+' : return lFirstNum +  lNum ;
          case '-' : return lFirstNum -  lNum ;
          case '*' : return lFirstNum *  lNum ;
          case '&' : return lFirstNum &  lNum ;
          case '|' : return lFirstNum |  lNum ;
          case '^' : return lFirstNum ^  lNum ;
          case '<' : return lFirstNum << lNum ;
          case '>' : return lFirstNum >> lNum ;
          case '/' : return lNum ? lFirstNum / lNum : ULONG_MAX ;
          case '%' : return lNum ? lFirstNum % lNum : ULONG_MAX ;
          default  : return 0L ;
          }
     }

ULONG EXPENTRY ClientWndProc (hWnd, nMessage, lParam1, lParam2)
     HWND         hWnd ;
     USHORT       nMessage ;
     ULONG        lParam1 ;
     ULONG        lParam2 ;
     {
     static BOOL  bNewNumber = TRUE ;
     static ULONG lNumber, lFirstNum ;
     static SHORT iOperation = '=' ;
     HWND         hWndButton ;
     SHORT        idButton ;

     switch (nMessage)
          {
          case WM_CHAR:
               if (lParam1 & KC_KEYUP)
                    break ;

               if (HIUSHORT (lParam2) == VK_LEFT)   /* left arrow to */
                    LOUSHORT (lParam2) = VK_BACK ;  /*   backspace   */

               if (HIUSHORT (lParam2) == VK_RETURN) /* return to     */
                    LOUSHORT (lParam2) = '=' ;      /*   equals      */

               if (LOUSHORT (lParam2) == 0)
                    break ;

               LOUSHORT (lParam2) = toupper (LOUSHORT (lParam2)) ;

               if (hWndButton =
                         WinWindowFromID (hWnd, LOUSHORT (lParam2)))
                    WinSendMsg (hWndButton, BM_CLICK, 0L, 0L) ;
               else
                    WinAlarm (HWND_DESKTOP, WA_ERROR) ;

               return 1L ;

          case WM_COMMAND:
               idButton = LOUSHORT (lParam1) ;

               if (idButton == VK_BACK)                /* backspace */
                    ShowNumber (hWnd, lNumber /= 16) ;

               else if (idButton == VK_ESCAPE)         /* escape    */
                    ShowNumber (hWnd, lNumber = 0L) ;

               else if (isxdigit (idButton))           /* hex digit */
                    {
                    if (bNewNumber)
                         {
                         lFirstNum = lNumber ;
                         lNumber = 0L ;
                         }
                    bNewNumber = FALSE ;

                    if (lNumber <= ULONG_MAX >> 4)
                         ShowNumber (hWnd, lNumber = 
                              16 * lNumber + idButton -
                              (isdigit (idButton) ? '0' : 'A' - 10)) ;
                    else
                         WinAlarm (HWND_DESKTOP, WA_ERROR) ;
                    }
               else                                    /* operation */
                    {
                    if (!bNewNumber)
                         ShowNumber (hWnd, lNumber =
                              CalcIt (lFirstNum, iOperation, lNumber)) ;
                    bNewNumber = TRUE ;
                    iOperation = idButton ;
                    }
               break ;

          default :
               return WinDefWindowProc (hWnd, nMessage, lParam1,
                                                        lParam2) ;
          }
     return 0L ;
     }

