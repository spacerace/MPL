/*
**                INPATH UTILITY
** This is a program similar to many "filefind" programs, except that
** it works like the UNIX "whereis" command.  The command line argument
** (only one, please) is taken to be the name of an executable
** program, and is searched under extensions .BAT, .COM, and .EXE
** for the first occurance of the program.  The search starts
** in the current directory, and if not found, procedes using the
** PATH environment variable (if there is one).  The program reports the
** first occurance of the specified file, just as it would be found
** by DOS if executed.
**
** The input filename need not contain an extension, since the
** extension is discarded anyway.
**
** Version 1.03  for Datalight C
** 9-18-86 A
**
** ======== COPYRIGHT 1986 BY STEVEN E. MARGISON ==============
**
**
**   As distributed, this program requires (for compilation):
**     "Steve's Datalight Library" version 1.10 or later
**   which may be obtained without registration from many Bulletin
**   Board Systems including:
**      Compuserve IBMSW
**      Cul-De-Sac (Holliston, MA.)
**      HAL-PC BBS (Houston, TX.)
**   and software library houses including:
**      Public (Software) Library (Houston, TX.)
**
**   or by registration:
**      $10 for Docs, Small Model Library
**      $25 for Docs, S, D, L, P libraries, and complete library source
**              in C and Assembler
**     Steven E. Margison
**     124 Sixth Street
**     Downers Grove, IL, 60515
**
*/

#include <stdio.h>
#include <smdefs.h>

char paths[256],
     fullname[100],
     dirname[100],
     batname[14],
     comname[14],
     exename[14],
     fname[14];
int current;

main(argc, argv)
int argc;
char *argv[];
{
   int i, j, k;
   char c;
   if(argc != 2) usage();
   strcpy(fname, argv[1]);   /* copy argument to fname */
   newext(fname, batname, "BAT");   /* make fname.bat */
   newext(fname, comname, "COM");   /* make fname.com */
   newext(fname, exename, "EXE");   /* make fname.exe */
   dirname[0] = NULL;
   current = YES;
   findpgm();
   current = NO;
   if((j = getpath(paths)) == 0) notfound();
   k = 0;
   while(j > 0) {
      i = 0;
      while(paths[k] != NULL) {
      c = paths[k];      /* save for slash check */
         dirname[i++] = paths[k++];
         --j;
         }
      if (c != '\\') dirname[i++] = '\\';
      dirname[i] = NULL;
      ++k;      /* position to next charcter in paths[] */
      --j;
      findpgm();
      }
   notfound();
   }

findpgm() {
   FILE *fd;
   strcpy(fullname, dirname);   /* test for .com file */
   strcat(fullname, comname);
   if((fd = fopen(fullname, "r")) != NULL) goto found;
   strcpy(fullname, dirname);
   strcat(fullname, exename);   /* test for .exe file */
   if((fd = fopen(fullname, "r")) != NULL) goto found;
   strcpy(fullname, dirname);
   strcat(fullname, batname);   /* test for .bat file */
   if((fd = fopen(fullname, "r")) != NULL) goto found;
   return(NO);         /* file not found */

   found:
   if(current) puts("File found in current directory");
   fputs(fullname, stdout);
   fputc('\n', stdout);
   fclose(fd);
   exit(0);
   }

notfound() {
   error("File not found in PATH");
   }

usage() {
   fputs("INPATH Version 1.03\n", stderr);
   fputs("Copyright 1986, Steven E. Margison\n", stderr);
   error("usage:  inpath <filename>");
   }

