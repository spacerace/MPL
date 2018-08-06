/*-------------------------------------------------------------------
   HEXCALC2.C -- Hexadecimal Calculator with Clipboard Cut and Paste
  -------------------------------------------------------------------*/

#define INCL_WIN
#include <os2.h>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "hexcalc.h"

#define IDM_COPY    256
#define IDM_PASTE   257

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

HAB  hab ;

int main (void)
     {
     HMQ  hmq ;
     HWND hwndFrame ;
     QMSG qmsg ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, CLIENTCLASS, ClientWndProc, 0L, 0) ;

     hwndFrame = WinLoadDlg (HWND_DESKTOP, HWND_DESKTOP,
                             NULL, NULL, ID_HEXCALC, NULL) ;

     WinSendMsg (hwndFrame, WM_SETICON,
                 WinLoadPointer (HWND_DESKTOP, NULL, ID_ICON), NULL) ;

     WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwndFrame, FID_CLIENT)) ;

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

HACCEL AddItemsToSysMenu (HWND hwndFrame)
     {
     static CHAR     *szMenuText [3] = { NULL, "~Copy\tCtrl+Ins",
                                               "~Paste\tShift+Ins" } ;
     static MENUITEM mi [3] = {
                              MIT_END, MIS_SEPARATOR, 0, 0,         NULL, NULL,
                              MIT_END, MIS_TEXT,      0, IDM_COPY,  NULL, NULL,
                              MIT_END, MIS_TEXT,      0, IDM_PASTE, NULL, NULL
                              } ;
     ACCELTABLE      *pacct ;
     HACCEL          haccel ;
     HWND            hwndSysMenu, hwndSysSubMenu ;
     MENUITEM        miSysMenu ;
     SHORT           idSysMenu, sItem ;

                              // Add items to system menu

     hwndSysMenu = WinWindowFromID (hwndFrame, FID_SYSMENU) ;
     idSysMenu = SHORT1FROMMR (WinSendMsg (hwndSysMenu,
                                           MM_ITEMIDFROMPOSITION,
                                           NULL, NULL)) ;

     WinSendMsg (hwndSysMenu, MM_QUERYITEM,
                 MPFROM2SHORT (idSysMenu, FALSE),
                 MPFROMP (&miSysMenu)) ;

     hwndSysSubMenu = miSysMenu.hwndSubMenu ;

     for (sItem = 0 ; sItem < 3 ; sItem++)
          WinSendMsg (hwndSysSubMenu, MM_INSERTITEM,
                      MPFROMP (mi + sItem),
                      MPFROMP (szMenuText [sItem])) ;

                              // Create and set accelerator table

     pacct = malloc (sizeof (ACCELTABLE) + sizeof (ACCEL)) ;

     pacct->cAccel        = 2 ;    // Number of accelerators
     pacct->codepage      = 0 ;    // Not used

     pacct->aaccel[0].fs  = AF_VIRTUALKEY | AF_CONTROL ;
     pacct->aaccel[0].key = VK_INSERT ;
     pacct->aaccel[0].cmd = IDM_COPY ;

     pacct->aaccel[1].fs  = AF_VIRTUALKEY | AF_SHIFT ;
     pacct->aaccel[1].key = VK_INSERT ;
     pacct->aaccel[1].cmd = IDM_PASTE ;

     haccel = WinCreateAccelTable (hab, pacct) ;
     WinSetAccelTable (hab, haccel, hwndFrame) ;

     free (pacct) ;

     return haccel ;
     }

VOID EnableSysMenuItem (HWND hwnd, USHORT idItem, BOOL fEnable)
     {
     HWND hwndSysMenu ;

     hwndSysMenu = WinWindowFromID (WinQueryWindow (hwnd, QW_PARENT, FALSE),
                                    FID_SYSMENU) ;

     WinSendMsg (hwndSysMenu, MM_SETITEMATTR,
                 MPFROM2SHORT (idItem, TRUE),
                 MPFROM2SHORT (MIA_DISABLED, fEnable ? 0 : MIA_DISABLED)) ;
     }

void ShowNumber (HWND hwnd, ULONG ulNumber)
     {
     CHAR szBuffer [20] ;

     WinSetWindowText (WinWindowFromID (hwnd, ESCAPE),
                       strupr (ltoa (ulNumber, szBuffer, 16))) ;
     }

ULONG CalcIt (ULONG ulFirstNum, SHORT sOperation, ULONG ulNum)
     {
     switch (sOperation)
          {
          case '=' : return ulNum ;
          case '+' : return ulFirstNum +  ulNum ;
          case '-' : return ulFirstNum -  ulNum ;
          case '*' : return ulFirstNum *  ulNum ;
          case '&' : return ulFirstNum &  ulNum ;
          case '|' : return ulFirstNum |  ulNum ;
          case '^' : return ulFirstNum ^  ulNum ;
          case '<' : return ulFirstNum << ulNum ;
          case '>' : return ulFirstNum >> ulNum ;
          case '/' : return ulNum ? ulFirstNum / ulNum : ULONG_MAX ;
          case '%' : return ulNum ? ulFirstNum % ulNum : ULONG_MAX ;
          default  : return 0L ;
          }
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static BOOL   fNewNumber = TRUE ;
     static HACCEL haccel ;
     static ULONG  ulNumber, ulFirstNum ;
     static SHORT  sOperation = '=' ;
     HWND          hwndButton ;
     PCHAR         pchClipText ;
     QMSG          qmsg ;
     SEL           selClipText ;
     SHORT         s, sLen, idButton ;

     switch (msg)
          {
          case WM_CREATE:
               haccel = AddItemsToSysMenu (
                              WinQueryWindow (hwnd, QW_PARENT, FALSE)) ;
               return 0 ;

          case WM_CHAR:
               if (CHARMSG(&msg)->fs & KC_KEYUP)
                    return 0 ;

               if (CHARMSG(&msg)->fs & KC_VIRTUALKEY)
                    switch (CHARMSG(&msg)->vkey)
                         {
                         case VK_LEFT:
                              if (!(CHARMSG(&msg)->fs & KC_CHAR))
                                   {
                                   CHARMSG(&msg)->chr = '\b' ;
                                   CHARMSG(&msg)->fs |= KC_CHAR ;
                                   }
                              break ;

                         case VK_ESC:
                              CHARMSG(&msg)->chr = ESCAPE ;
                              CHARMSG(&msg)->fs |= KC_CHAR ;
                              break ;

                         case VK_NEWLINE:
                         case VK_ENTER:
                              CHARMSG(&msg)->chr = '=' ;
                              CHARMSG(&msg)->fs |= KC_CHAR ;
                              break ;
                         }

               if (CHARMSG(&msg)->fs & KC_CHAR)
                    {
                    CHARMSG(&msg)->chr = toupper (CHARMSG(&msg)->chr) ;

                    if (hwndButton = WinWindowFromID (hwnd,CHARMSG(&msg)->chr))
                         WinSendMsg (hwndButton, BM_CLICK, NULL, NULL) ;
                    else
                         WinAlarm (HWND_DESKTOP, WA_ERROR) ;
                    }
               return 1 ;

          case WM_COMMAND:
               idButton = COMMANDMSG(&msg)->cmd ;

               if (idButton == IDM_COPY)                    // "Copy"
                    {
                    hwndButton = WinWindowFromID (hwnd, ESCAPE) ;
                    sLen = WinQueryWindowTextLength (hwndButton) + 1 ;

                    DosAllocSeg (sLen, &selClipText, SEG_GIVEABLE) ;
                    pchClipText = MAKEP (selClipText, 0) ;
                    WinQueryWindowText (hwndButton, sLen, pchClipText) ;

                    WinOpenClipbrd (hab) ;
                    WinEmptyClipbrd (hab) ;
                    WinSetClipbrdData (hab, (ULONG) selClipText, CF_TEXT,
                                       CFI_SELECTOR) ;
                    WinCloseClipbrd (hab) ;
                    }

               else if (idButton == IDM_PASTE)              // "Paste"
                    {
                    EnableSysMenuItem (hwnd, IDM_COPY,  FALSE) ;
                    EnableSysMenuItem (hwnd, IDM_PASTE, FALSE) ;

                    WinOpenClipbrd (hab) ;

                    selClipText = (SEL) WinQueryClipbrdData (hab, CF_TEXT) ;

                    if (selClipText != 0)
                         {
                         pchClipText = MAKEP (selClipText, 0) ;

                         for (s = 0 ; pchClipText[s] ; s++)
                              {
                              if (pchClipText[s] == '\r')
                                   WinSendMsg (hwnd, WM_CHAR,
                                               MPFROM2SHORT (KC_CHAR, 1),
                                               MPFROM2SHORT ('=', 0)) ;

                              else if (pchClipText[s] != '\n' &&
                                       pchClipText[s] != ' ')
                                   WinSendMsg (hwnd, WM_CHAR,
                                               MPFROM2SHORT (KC_CHAR, 1),
                                               MPFROM2SHORT (pchClipText[s],
                                                             0)) ;

                              while (WinPeekMsg (hab, &qmsg, NULL, 0, 0,
                                                 PM_NOREMOVE))
                                   {
                                   if (qmsg.msg == WM_QUIT)
                                        {
                                        WinCloseClipbrd (hab) ;
                                        return 0 ;
                                        }
                                   else
                                        {
                                        WinGetMsg (hab, &qmsg, NULL, 0, 0) ;
                                        WinDispatchMsg (hab, &qmsg) ;
                                        }
                                   }
                              }
                         }
                    WinCloseClipbrd (hab) ;

                    EnableSysMenuItem (hwnd, IDM_COPY,  TRUE) ;
                    EnableSysMenuItem (hwnd, IDM_PASTE, TRUE) ;
                    }

               else if (idButton == '\b')                   // backspace
                    ShowNumber (hwnd, ulNumber /= 16) ;

               else if (idButton == ESCAPE)                 // escape
                    ShowNumber (hwnd, ulNumber = 0L) ;

               else if (isxdigit (idButton))                // hex digit
                    {
                    if (fNewNumber)
                         {
                         ulFirstNum = ulNumber ;
                         ulNumber = 0L ;
                         }
                    fNewNumber = FALSE ;

                    if (ulNumber <= ULONG_MAX >> 4)
                         ShowNumber (hwnd,
                              ulNumber = 16 * ulNumber + idButton -
                                   (isdigit (idButton) ? '0' : 'A' - 10)) ;
                    else
                         WinAlarm (HWND_DESKTOP, WA_ERROR) ;
                    }
               else                                         // operation
                    {
                    if (!fNewNumber)
                         ShowNumber (hwnd, ulNumber =
                              CalcIt (ulFirstNum, sOperation, ulNumber)) ;
                    fNewNumber = TRUE ;
                    sOperation = idButton ;
                    }
               return 0 ;

          case WM_BUTTON1DOWN:
               WinAlarm (HWND_DESKTOP, WA_ERROR) ;
               break ;

          case WM_ERASEBACKGROUND:
               return 1 ;

          case WM_DESTROY:
               WinDestroyAccelTable (haccel) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
