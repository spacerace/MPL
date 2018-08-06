/*-----------------------------------------
   AVIO1.C -- Advanced VIO Display of Text
  -----------------------------------------*/

#define INCL_WIN
#define INCL_VIO
#define INCL_AVIO
#include <os2.h>
#include <string.h>

#define VIDEOWIDTH 40 

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

HAB  hab ;

int main (void)
     {
     static CHAR  szClientClass [] = "Avio1" ;
     static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU  |
                                 FCF_SIZEBORDER    | FCF_MINMAX   |
                                 FCF_SHELLPOSITION | FCF_TASKLIST ;
     HMQ          hmq ;
     HWND         hwndFrame, hwndClient ;
     QMSG         qmsg ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClientClass, ClientWndProc, CS_SIZEREDRAW, 0) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP, WS_VISIBLE,
                                     &flFrameFlags, szClientClass, NULL,
                                     0L, NULL, 0, &hwndClient) ;

     WinSendMsg (hwndFrame, WM_SETICON,
                 WinQuerySysPointer (HWND_DESKTOP, SPTR_APPICON, FALSE),
                 NULL) ;

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;
     return 0 ;
     }

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static CHAR  *aszAlice [] = {
                                 "Alice was beginning to get very tired of",
                                 "sitting by her sister on the bank and of",
                                 "having nothing to do: once or twice she",
                                 "had peeped into the book her sister was",
                                 "reading, but it had no pictures or ",
                                 "conversations in it, \"and what is the",
                                 "use of a book,\" thought Alice, \"without",
                                 "pictures or conversations?\""
                                 } ;
     static HPS   hps ;
     static HVPS  hvps ;
     static SHORT sNumLines = sizeof aszAlice / sizeof aszAlice[0] ;
     HDC          hdc ;
     SHORT        sRow ;
     SIZEL        sizl ;

     switch (msg)
          {
          case WM_CREATE:
               hdc = WinOpenWindowDC (hwnd) ;

               sizl.cx = sizl.cy = 0 ;
               hps = GpiCreatePS (hab, hdc, &sizl, PU_PELS    | GPIF_DEFAULT |
                                                   GPIT_MICRO | GPIA_ASSOC) ;

               VioCreatePS (&hvps, sNumLines, VIDEOWIDTH, 0, 1, NULL) ;
               VioAssociate (hdc, hvps) ;

               for (sRow = 0 ; sRow < sNumLines ; sRow++)
                    VioWrtCharStr (aszAlice[sRow], 
                                   strlen (aszAlice[sRow]),
                                   sRow, 0, hvps) ;
               return 0 ;

          case WM_SIZE:
               WinDefAVioWindowProc (hwnd, msg, mp1, mp2) ;
               return 0 ;

          case WM_PAINT:
               WinBeginPaint (hwnd, hps, NULL) ;
               GpiErase (hps) ;

               VioShowBuf (0, 2 * sNumLines * VIDEOWIDTH, hvps) ;

               WinEndPaint (hps) ;
               return 0 ;

          case WM_DESTROY:
               VioAssociate (NULL, hvps) ;
               VioDestroyPS (hvps) ;
               GpiDestroyPS (hps) ;
               return 0 ;
          }
     return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
     }
