/*
**                WORD COUNT UTILITY
**
** syntax:
** wc [-cwlsv] file
** -c count only characters
** -w count only words
** -l count only lines
** -s gives only a hex checksum
** -q silences reporting of filenames
** file is input file (Do NOT use stdin redirection!)
** options must be first argument only
** wildcard filenames are allowed, as well as multiple filenames
**
** ========== COPYRIGHT 1986 BY STEVEN E. MARGISON ==============
** 9-18-86 A  Datalight C
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
**
*/

#include <stdio.h>
#include <smdefs.h>

int binflg, copt, wopt, lopt, sopt, allopt, verbose;
char path[64], fname[MAXFN], dest[64 + MAXFN + 1];
FILE *fd;

main(argc, argv)
int argc;
char *argv[];
{
   char *cksum, *nc, *nl, *nw;
   int index, c, inword, argci;

   if(argc < 2) usage();
   verbose = YES;
   sopt = lopt = wopt = copt = FALSE;
   allopt=TRUE;
   argci = 1;

   if(argv[argci][0] is '-') {
      index = 0;
      while(argv[argci][++index] isnot NULL) {
         switch(tolower(argv[argci][index])) {
            case 'q':
               verbose = NO;
               break;
            case 'c': 
               copt = TRUE;
               allopt = FALSE;
               break;
            case 'w':
               wopt = TRUE;
               allopt = FALSE;
               break;
            case 'l':
               lopt = TRUE;
               allopt = FALSE;
               break;
            case 's':
               sopt = TRUE;
               allopt = FALSE;
               break;
            default:
               usage();
         } /* end of switch */
      } /* end of while */
   argci++;    /* get around options */
   } /* end of option "if" */

   while(argci < argc) {
      cksum = nc = nl = nw = 0;
      inword = NO;
      if(argv[argci][0] is '-') usage();

      do_open(argv[argci++]);

      while((c = fgetc(fd)) isnot EOF) {
         cksum += c;
         if(binflg) continue;
         if(c is '\n') {
            ++nl;
            ++nc;
            ++nc;     /* because a newline is actually two characters */
            inword = NO;
         }
         else ++nc;
         if(isspace(c)) inword = NO;
         else if(inword is NO) {
            inword = YES;
            ++nw;
            }
         }  /* end of inner while */
      if(allopt and !binflg)
         printf("%6d characters  %d words  %d lines  %6x checksum\n",
            nc, nw, nl, cksum);
      if(binflg) printf("%x checksum\n", cksum);

      if(copt) printf("%d\n", nc);
      if(wopt) printf("%d\n", nw);
      if(lopt) printf("%d\n", nl);
      if(sopt) printf("%x\n", cksum);
      fclose(fd);
   } /* end of file loop */
}

usage() {
fputs("WC Version 1.42 9-18-86  Copyright 1986 S.E. Margison\n", stderr);
error("usage: wc [-cwlsq] <file1, file2, filen>");
}

do_open(string) char *string; {
   binflg = NO;
   if(exttyp(string, "OBJ") is YES) { binflg = YES; goto AA; }
   if(exttyp(string, "EXE") is YES) { binflg = YES; goto AA; }
   if(exttyp(string, "COM") is YES) { binflg = YES; goto AA; }
   if(exttyp(string, "ARC") is YES) { binflg = YES; goto AA; }
   if(exttyp(string, "LIB") is YES) binflg = YES;
   AA:
   if(!binflg) {
      if((fd = fopen(string, "r")) is NULL) cant(string);
      }
   else {
      if((fd = fopen(string, "rb")) is NULL) cant(string);
      }
   if(verbose) printf("File: %s\n", string);
   }

