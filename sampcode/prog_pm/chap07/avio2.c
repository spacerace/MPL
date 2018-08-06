/*-----------------------------------------
   AVIO2.C -- Advanced VIO Display of Text
  -----------------------------------------*/

#define INCL_WIN
#define INCL_VIO
#define INCL_AVIO
#include <os2.h>

#define VIDEOWIDTH 40

typedef struct
     {
     CHAR ch ;
     CHAR attr ;
     }
     VIDEO [][VIDEOWIDTH] ;

typedef VIDEO FAR *PVIDEO ;

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;

HAB  hab ;

int main (void)
     {
     static CHAR    szClientClass [] = "Avio2" ;
     static ULONG   flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU |
                                   FCF_SIZEBORDER    | FCF_MINMAX  |
                                   FCF_SHELLPOSITION | FCF_TASKLIST ;
     HMQ            hmq ;
     HWND           hwndFrame, hwndClient ;
     QMSG           qmsg ;

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
                                 "reading, but it had no pictures or",
                                 "conversations in it, \"and what is the",
                                 "use of a book,\" thought Alice, \"without",
                                 "pictures or conversations?\""
                                 } ;
     static HPS    hps ;
     static HVPS   hvps ;
     static PVIDEO pvideo ;
     static SHORT  sNumLines = sizeof aszAlice / sizeof aszAlice[0] ;
     static USHORT usVideoLength ;
     HDC           hdc ;
     RECTL         rcl ;
     SHORT         sRow, sCol ;
     SIZEL         sizl ;
     ULONG         ulVideoBuffer ;

     switch (msg)
          {
          case WM_CREATE:
               hdc = WinOpenWindowDC (hwnd) ;

               sizl.cx = sizl.cy = 0 ;
               hps = GpiCreatePS (hab, hdc, &sizl, PU_PELS    | GPIF_DEFAULT |
                                                   GPIT_MICRO | GPIA_ASSOC) ;

               VioCreatePS (&hvps, sNumLines, VIDEOWIDTH, 0, 1, NULL) ;
               VioAssociate (hdc, hvps) ;

               VioGetBuf (&ulVideoBuffer, &usVideoLength, hvps) ;
               pvideo = (PVIDEO) ulVideoBuffer ;

               for (sRow = 0 ; sRow < sNumLines ; sRow++)
                    for (sCol = 0 ; sCol < VIDEOWIDTH ; sCol++)
                         (*pvideo) [sRow][sCol].attr = '\x1E' ;

               for (sRow = 0 ; sRow < sNumLines ; sRow++)
                    for (sCol = 0 ; aszAlice [sRow][sCol] ; sCol++)
                         (*pvideo) [sRow][sCol].ch = aszAlice [sRow][sCol] ;

               return 0 ;

          case WM_SIZE:
               WinDefAVioWindowProc (hwnd, msg, mp1, mp2) ;
               return 0 ;

          case WM_PAINT:
               WinBeginPaint (hwnd, hps, NULL) ;

               WinQueryWindowRect (hwnd, &rcl) ;
               WinFillRect (hps, &rcl, CLR_DARKBLUE) ;

               VioShowBuf (0, usVideoLength, hvps) ;

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
