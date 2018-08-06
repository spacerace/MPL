/*
Microsoft Systems Journal
Volume 2; Issue 2; May, 1987

Code Listings For:

	FINDDEMO
	pp. 51-59

Author(s): Charles Petzold
Title:     A Compleat Guide to Writing Your First OS/2 Program



==============================================================================
==============================================================================
Figure 2: FINDDEMO.C

==============================================================================
*/

/* finddemo.c -- program to demonstrate IPC and multitasking */

#include <doscall.h>

#define WORD  unsigned int
#define DWORD unsigned long

#define NUMPROC     9                   /* number of process */
#define CHILDPROG   "FINDER.EXE"
#define QUEUENAME   "\\QUEUES\\FINDDEMO"
#define THREADSTACK 1024

void far dispthread (void) ;
void far waitthread (void) ;

WORD   queuehandle, count [NUMPROC] ;
DWORD  runsem [NUMPROC], waitsem [NUMPROC] ;
struct ResultCodes rc [NUMPROC] ;
struct {
     WORD count ;
     struct {
          WORD  reserved ;
          DWORD far *sem ;
          } index [NUMPROC] ;
     } semtab ;
          
main ()
     {
     static char prompt[] = "Line number or Esc to end -->  \b" ;
     struct KeyData keydata ;
     WORD   index, len, i, dispID, waitID, sigaction ;
     DWORD  sigaddr ;
     char   dirmask [15], dispstack [THREADSTACK],
            waitstack [THREADSTACK] ;

     /* 
         Initialize: Set up "semtab" structure for DOSMUXSEMWAIT.
         ----------  Disable Ctrl-Break and Ctrl-C exits.
                     Create queue for IPC with FINDER.EXE.
                     Create threads for messages from FINDER
                         and waiting for FINDER terminations.
                     Display text. 
     */

     semtab.count = NUMPROC ;
     for (index = 0 ; index < NUMPROC ; index++) {
          DOSSEMSET ((DWORD) &waitsem[index]) ;          
          semtab.index[index].sem = &waitsem[index] ;
          }
     DOSSETSIGHANDLER (0L, &sigaddr, &sigaction, 1, 1) ;
     DOSSETSIGHANDLER (0L, &sigaddr, &sigaction, 1, 4) ;

     if (DOSCREATEQUEUE (&queuehandle, 0, QUEUENAME)) {
          puts ("FINDDEMO: Cannot create new queue") ;
          DOSEXIT (1, 1) ;
          }
     if (DOSCREATETHREAD (dispthread, &dispID, dispstack 
                          + THREADSTACK) ||
         DOSCREATETHREAD (waitthread, &waitID, waitstack 
                          + THREADSTACK)) {
          puts ("FINDDEMO: Cannot create threads") ;
          DOSEXIT (1, 1) ;
          }
     displayheadings () ;

               /*
                    Main Loop: Display prompt and read keyboard.
                    ---------  Execute FINDER.EXE.
               */
     do   {
          VIOSETCURPOS (18, 0, 0) ;
          VIOWRTTTY (prompt, sizeof prompt - 1, 0) ;
          KBDCHARIN (&keydata, 0, 0) ;

          index = keydata.char_code - '1' ;

          if (index <= NUMPROC && rc[index].TermCode_PID == 0) {
               VIOWRTTTY (&keydata.char_code, 1, 0) ;
               VIOWRTNCHAR (" ", 77, 7 + index, 3, 0) ;
               do   {
                    VIOSETCURPOS (7 + index, 3, 0) ;
                    len = 13 ;
                    KBDSTRINGIN (dirmask, &len, 0, 0) ;
                    }
               while (len == 0) ;

               dirmask [len] = '\0' ;
               executeprogram (index, dirmask) ;
               }
          }
     while (keydata.char_code != 27) ;

               /*
                    Clean-up: Kill all existing FINDER.EXE processes.
                    --------  Wait for processes to terminate.
                              Close the queue and exit.
               */

     for (index = 0 ; index < NUMPROC ; index++)
          if (rc[index].TermCode_PID)
               DOSKILLPROCESS (0, rc[index].TermCode_PID) ;

     for (index = 0 ; index < NUMPROC ; index++)
          DOSSEMWAIT ((DWORD) &runsem [index], -1L) ;

     DOSCLOSEQUEUE (queuehandle) ;
     DOSEXIT (1, 0) ;
     }

displayheadings ()
     {
     static char heading  [] = "286DOS File Finder Demo Program",
                 colheads [] = "Dir Mask     Status  Files",
                 colunder [] = "--------     ------  -----" ;
     char        buffer  [5] ;
     WORD        row, col, i, len ;

     VIOGETCURPOS (&row, &col, 0) ;              /* get current attr */
     VIOWRTTTY (" ", 1, 0) ;
     len = 2 ;
     VIOREADCELLSTR (buffer, &len, row, col, 0) ;
     VIOSCROLLUP (0, 0, -1, -1, -1, buffer, 0) ;     /* clear screen */

     len = sizeof heading - 1 ;
     col = (80 - len) / 2 ;
     VIOWRTCHARSTR (heading, len, 1, col, 0) ;            /* heading */
     VIOWRTNCHAR   ("\xC6",   1, 2, col - 1,   0) ;     /* underline */
     VIOWRTNCHAR   ("\xCD", len, 2, col,       0) ;
     VIOWRTNCHAR   ("\xB5",   1, 2, col + len, 0) ;
     VIOWRTCHARSTR (colheads, sizeof colheads - 1, 5, 3, 0) ; 
     VIOWRTCHARSTR (colunder, sizeof colunder - 1, 6, 3, 0) ;

     for (i = 0 ; i < NUMPROC ; i++) {                    /* numbers */
          sprintf (buffer, "%d.", i + 1) ;
          VIOWRTCHARSTR (buffer, 2, 7 + i, 0, 0) ;
          }
     }

executeprogram (index, dirmask)
     WORD index ;
     char *dirmask ;
     {
     char objbuf [32] ;
     char args [128] ;

     strcat (strcpy (args, CHILDPROG), " ") ;      /* construct args */
     strcat (strcat (args, dirmask), " ") ;
     strcat (strcat (args, QUEUENAME), " ") ;
     itoa (index, args + strlen (args), 10) ;

     count [index] = 0 ;                         /* initialize count */

     if (DOSEXECPGM (objbuf, 32, 2, args, 0, &rc[index], CHILDPROG))
          {
          puts ("FINDDEMO: Can't run FINDER.EXE") ;
          DOSEXIT (1, 1) ;
          }
     VIOWRTCHARSTR ("Running", 7, index + 7, 16, 0) ;/* now executing */
     DOSSEMSET   ((DWORD) &runsem [index]) ;
     DOSSEMCLEAR ((DWORD) &waitsem[index]) ;
     }

void far dispthread ()        /* thread to read messages from FINDER */
     {                        /*   and display filenames.            */
     DWORD request ;
     WORD  len, index, i ;
     char  far *farptr ;
     char  priority, pathname [80], buffer [64] ;

     while (1) {
          DOSREADQUEUE (queuehandle, &request, &len, 
                         &(DWORD)farptr, 0, 0, &priority, 0L) ;
          i = 0 ;
          while (pathname [i++] = *farptr++) ;
          index = (WORD) (request >> 16) ;
          count [index] += len > 0 ;
          sprintf (buffer, "%5d   %-48.48s", count [index], pathname) ;
          VIOWRTCHARSTR (buffer, 56, 7 + index, 24, 0) ;
          DOSFREESEG ((WORD) ((DWORD) farptr >> 16)) ;
          }
     }

void far waitthread ()     /* thread to wait for FINDER terminations */
     {
     WORD   index, PID ;
     struct ResultCodes rescode ;
     
     while (1) {
          DOSMUXSEMWAIT (&index, (WORD far *) &semtab, -1L) ;
          DOSCWAIT (0, 0, &rescode, &PID, 0) ;

          for (index = 0 ; index < NUMPROC ; index++)  /* find index */
               if (PID == rc[index].TermCode_PID) 
                    break ;

          VIOWRTCHARSTR (rescode.TermCode_PID ? "Halted " : "Done   ",
                              7, index + 7, 16, 0) ;
          rc[index].TermCode_PID = 0 ;
          DOSSEMCLEAR ((DWORD) &runsem [index]) ;
          DOSSEMSET   ((DWORD) &waitsem[index]) ;
          }
     }
