/*----------------------------------------------------------
   THREADS.C -- Multi-Thread OS/2 Program for CodeView Demo
                Programmed by Charles Petzold, 6/88
  ----------------------------------------------------------*/

#define INCL_DOS
#define INCL_VIO
#define INCL_KBD

#define INCL_SUB

#include <os2.h>
#include <mt\process.h>
#include <mt\stdlib.h>
#include <mt\stddef.h>

     /*------------------------------------------------
        Structure definition and function declarations
       ------------------------------------------------*/

typedef struct
     {
     SHORT xLeft ;
     SHORT yTop ;
     SHORT xRight ;
     SHORT yBottom ;
     }
     THREADPARAM ;

typedef THREADPARAM FAR *PTHREADPARAM ;

BOOL            CheckKey       (CHAR ch) ;
VOID FAR ThreadFunction (PTHREADPARAM ptp) ;
VOID            WriteChar      (CHAR ch, USHORT usRow, USHORT usCol) ;
VOID FAR ExitRoutine    (USHORT usTermCode) ;
VOID            ClearScreen    (BOOL fCursorOff) ;

     /*-----------------
        main() function
       -----------------*/

INT _CDECL main (VOID)
     {
     static INT  iStack [4][2048] ;
     static CHAR szErrorMsg[] = "THREADS: Cannot start thread" ;
     KBDKEYINFO  kbci ;
     SHORT       i ;
     THREADPARAM tp[4] ;
     VIOMODEINFO viomi ;

          /*---------------------------------------------
             Clear screen and get video mode information
            ---------------------------------------------*/

     DosExitList (EXLST_ADD, ExitRoutine) ;
     ClearScreen (TRUE) ;

     viomi.cb = sizeof viomi ;
     VioGetMode (&viomi, 0) ;

          /*-----------------------
             Start up four threads
            -----------------------*/

     for (i = 0 ; i < 4 ; i++)
          {
          tp[i].xLeft = i % 2 ? viomi.col / 2 : 0 ;
          tp[i].yTop  = i > 1 ? viomi.row / 2 : 0 ;

          tp[i].xRight  = tp[i].xLeft + viomi.col / 2 ;
          tp[i].yBottom = tp[i].yTop  + viomi.row / 2 ;

          if (-1 == _beginthread (ThreadFunction, iStack[i],
                                  sizeof iStack[i], tp + i))
               {
               VioWrtTTy (szErrorMsg, sizeof szErrorMsg - 1, 0) ;
               return 1 ;
               }
          }

          /*--------------------------
             Wait for Esc key to exit
            --------------------------*/
     do
          KbdCharIn (&kbci, IO_WAIT, 0) ;
     while (!CheckKey (kbci.chChar)) ;

     return 0 ;
     }

     /*-----------------------------------------------------
        CheckKey() function -- Returns TRUE if ch is Escape
       -----------------------------------------------------*/

BOOL CheckKey (CHAR ch)
     {
     return ch == '\33' ;
     }

     /*------------------
        ThreadFunction()
       ------------------*/

VOID FAR ThreadFunction (PTHREADPARAM ptp)
     {
     CHAR   ch ;
     SEL    selGlobal, selLocal ;
     TID    tid ;
     USHORT usRow, usCol ;

          /*----------------------------------------
             Get thread ID and convert to character
            ----------------------------------------*/

     DosGetInfoSeg (&selGlobal, &selLocal) ;
     tid = ((PLINFOSEG) MAKEP (selLocal, 0)) -> tidCurrent ;
     ch  = (CHAR) tid + '0' ;
     srand (tid) ;

          /*---------------------------------------
             Display thread ID in random locations
            ---------------------------------------*/

     while (TRUE)
          {
          usCol = ptp->xLeft + rand () % (ptp->xRight - ptp->xLeft) ;
          usRow = ptp->yTop  + rand () % (ptp->yBottom - ptp->yTop) ;
          WriteChar (ch, usRow, usCol) ;
          }
     }

     /*----------------------------------------------------------
        WriteChar() function -- Displays TID, pauses, then blank
       ----------------------------------------------------------*/

VOID WriteChar (CHAR ch, USHORT usRow, USHORT usCol)
     {
     VioWrtCharStr (&ch, 1, usRow, usCol, 0) ;
     DosSleep (100L) ;
     VioWrtCharStr (" ", 1, usRow, usCol, 0) ;
     }

     /*------------------------------------------------
        ExitRoutine() function -- Exit list processing
       ------------------------------------------------*/

VOID FAR ExitRoutine (USHORT usTermCode)
     {
     usTermCode ;   /* to prevent compiler warning */

     ClearScreen (FALSE) ;
     DosExitList (EXLST_EXIT, NULL) ;
     }

     /*--------------------------------------------------------
        ClearScreen() function -- Also turns cursor off and on
       --------------------------------------------------------*/

VOID ClearScreen (BOOL fCursorOff)
     {
     USHORT        usAnsi ;
     VIOCURSORINFO vioci ;

     VioGetAnsi (&usAnsi, 0) ;
     VioSetAnsi (ANSI_ON, 0) ;
     VioWrtTTy ("\33[2J", 4, 0) ;
     VioSetAnsi (usAnsi, 0) ;

     VioGetCurType (&vioci, 0) ;
     vioci.attr = fCursorOff ? -1 : 0 ;
     VioSetCurType (&vioci, 0) ;
     }
