/*
==============================================================================
==============================================================================
Figure 3: FINDER.C

==============================================================================
*/

/* finder.c -- child process of finddemo */

#include <doscall.h>

unsigned index ;
unsigned queuehandle ;
unsigned parentPID ;

main (argc, argv)
     int  argc ;
     char *argv [] ;
     {
     if (argc < 4) {
          puts ("Run FINDDEMO rather than FINDER") ;
          DOSEXIT (1, 1) ;
          }
     if (DOSOPENQUEUE (&parentPID, &queuehandle, argv [2])) {
          puts ("FINDER: Cannot open queue") ;
          DOSEXIT (1, 1) ;
          }
     index = atoi (argv [3]) ;

     writequeue ("") ;
     chdir ("\\") ;
     find (argv [1]) ;
     writequeue ("") ;

     DOSCLOSEQUEUE (queuehandle) ;
     DOSEXIT (1, 0) ;
     }

find (searchstr)
     char     *searchstr ;
     {
     struct   FileFindBuf ffb ;
     char     cwd [81], pathname [100] ;
     unsigned handle = 0xFFFF, num = 1 ;

     if (cwd [strlen (getcwd (cwd, 80)) - 1] != '\\')
          strcat (cwd, "\\") ;

     DOSFINDFIRST (searchstr, &handle, 7, &ffb, sizeof ffb, &num, 0L) ;
     while (num) {
          writequeue (strcat (strcpy (pathname, cwd), ffb.file_name)) ;
          DOSFINDNEXT (handle, &ffb, sizeof ffb, &num) ;
          }
     DOSFINDCLOSE (handle) ;

     handle = 0xFFFF ;
     num  = 1 ;
     DOSFINDFIRST ("*.*", &handle, 0x17, &ffb, sizeof ffb, &num, 0L) ;
     while (num) {
          if (ffb.attributes & 0x10 && ffb.file_name [0] != '.') {
               chdir (ffb.file_name) ;
               find (searchstr) ;
               chdir ("..") ;
               }
          DOSFINDNEXT (handle, &ffb, sizeof ffb, &num) ;
          }
     DOSFINDCLOSE (handle) ;
     }

writequeue (str)
     char *str ;
     {
     unsigned selector, parentselector ;
     char far *farptr ;
     int      len = strlen (str) ;

     DOSALLOCSEG (len + 1, &selector, 1) ;
     farptr = (char far *) (((unsigned long) selector) << 16) ;

     while (*farptr++ = *str++) ;

     DOSGIVESEG (selector, parentPID, &parentselector) ;
     DOSFREESEG (selector) ;
     farptr = (char far *) (((unsigned long) parentselector) << 16) ;

     DOSWRITEQUEUE (queuehandle, index, len, farptr, 0) ;
     }
