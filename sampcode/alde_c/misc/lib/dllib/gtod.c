/*
**                GET TIME OF DAY UTILITY
**
** S.E. Margison  Copyright 1986  All rights reserved
** 9-18-86 A  V1.01  Datalight C
**
**  Options default to date, time, 12 hour format, no seconds, American
**          style reporting.  Either - or / used for options, or none.
**  -d   report (D)ate only
**  -t   report (T)ime only
**  -s   report (S)econds (only if -t option used)
**  -l   Report date in (L)ong format
**  -n   Include the (N)ame of the weekday, if -l format
**  -m   report time in (M)ilitary format (12 hour format if not used)
**  -e   report date in (E)uropean format
**  -r   for time and date reporting, (R)everse order to time first
**  If any options are used, no default values are assumed.
**  No options is the same as -dt
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
#include <time.h>
#include <smdefs.h>

int nf, tf, df, sf, mf, rf, ef, lf;   /* option flags */

char datestr[64],   /* place for formatted date string */
     timestr[16];   /* place for formatted time sring */

extern char *wkdayname();
extern char *monthis();

struct tm *ptm;
long clockval;

main(argc, argv)
int argc;
char *argv[];
{

    /* first thing to do is read the clock! */

   clockval = time(0);
   ptm = localtime(&clockval);

   doargs(argc, argv);  /* parse any options */

   if(!tf && !df) tf = df = YES;

   if(!tf) timestr[0] = NULL;   /* skip the time */
   else mktime();

   if(!df) datestr[0] = NULL;   /* skip the date */
   else mkdate();

   if(!rf) printf("%s  %s\n", datestr, timestr);
   else printf("%s  %s\n", timestr, datestr);
}

doargs(argc, argv)
int argc;
char *argv[];
{
   int i;
   nf = tf = df = sf = mf = rf = ef = lf = NO;
   while(--argc > 0) {
      i = 0;
      while(argv[argc][i] != NULL) {
         switch(tolower(argv[argc][i++])) {
            case '-': case '/':
               break;
            case 'n':
               nf = YES;
               break;
            case 'd':
               df = YES;
               break;
            case 't':
               tf = YES;
               break;
            case 's':
               sf = YES;
               break;
            case 'm':
               mf = YES;
               break;
            case 'r':
               rf = YES;
               break;
            case 'e':
               ef = YES;
               break;
            case 'l':
               lf = YES;
               break;
            case 'v':
               fputs("GTOD Version 1.02\n", stderr);
               fputs("GTOD ? for usage message\n", stderr);
               error("Copyright 1986 S.E. Margison");
            case '?':
               usage();
            default:
               fputc(argv[argc][--i], stderr);
               error(": invalid option");
            }  /* end of switch */
         } /* end of inner while */
      }  /* end of outer while */
   }

mkdate()
{
   int i, mpos, dpos;
   char *mstr, dstr[6];
   mpos = 0;
   dpos = 3;
   if(lf) {
      dstr[0] = (ptm->tm_mday / 10) | '0';
      if(dstr[0] == '0') dstr[0] = ' ';
      dstr[1] = (ptm->tm_mday % 10) | '0';
      dstr[2] = NULL;
      if(nf) {
         i = weekday(ptm->tm_mon+1, ptm->tm_mday, ptm->tm_year+1900);
         mstr = wkdayname(i);
         strcpy(datestr, mstr);
         strcat(datestr, ", ");
         }
      mstr = monthis(ptm->tm_mon);
      if(ef) {
         strcat(datestr, dstr);
         strcat(datestr, " ");
         strcat(datestr, mstr);
         }
      else {
         strcat(datestr, mstr);
         strcat(datestr, " ");
         strcat(datestr, dstr);
         }
      strcat(datestr, ", ");
      i_dstr(dstr, ptm->tm_year+1900);
      strcat(datestr, dstr);
      return;
      }

   if(ef) { mpos = 3; dpos = 0; }

   ptm->tm_mon++;      /* Datalight calls month 0 = January */
   datestr[mpos++] = (ptm->tm_mon / 10) | '0';
   datestr[mpos] = (ptm->tm_mon % 10) | '0';
   datestr[2] = datestr[5] = '/';
   datestr[dpos++] = (ptm->tm_mday / 10) | '0';
   datestr[dpos] = (ptm->tm_mday % 10) | '0';
   i = ptm->tm_year;
   if(i > 100) i -= 100;
   datestr[6] = (i / 10) | '0';
   datestr[7] = (i % 10) | '0';
   datestr[8] = NULL;
   }

mktime()
{
   int hrs, eos;
   timestr[2] = ':';
   timestr[3] = (ptm->tm_min / 10) | '0';
   timestr[4] = (ptm->tm_min % 10) | '0';
   if(sf) {
      timestr[5] = ':';
      timestr[6] = (ptm->tm_sec / 10) | '0';
      timestr[7] = (ptm->tm_sec % 10) | '0';
      eos = 8;
      }
   else eos = 5;
   if(mf) {
      timestr[0] = (ptm->tm_hour / 10) | '0';
      timestr[1] = (ptm->tm_hour % 10) | '0';
      timestr[eos] = NULL;
      }
   else {
      timestr[eos++] = ' ';
      timestr[eos] = 'A';

      if(ptm->tm_hour >= 12) timestr[eos] = 'P';  /* set PM */

      if(ptm->tm_hour > 12) hrs = ptm->tm_hour - 12;
      else if(ptm->tm_hour == 0) hrs = 12;   /* midnight = 12AM */
      else hrs = ptm->tm_hour;

      timestr[++eos] = 'M';
      timestr[++eos] = NULL;
      timestr[0] = (hrs / 10) | '0';
      timestr[1] = (hrs % 10) | '0';
      if(timestr[0] == '0') timestr[0] = ' ';
      }
   }

usage() {
puts("GTOD <options> or <-options> or </options>");
puts("redirect stdout as desired");
puts("d   report (D)ate only");
puts("t   report (T)ime only");
puts("s   report (S)econds (only if time is enabled)");
puts("l   Report date in (L)ong format  (September 10, 1986)");
puts("n   Include the (N)ame of the weekday, if -l format");
puts("m   report time in (M)ilitary format (else, 12 hour format with AM/PM)");
puts("e   report date in (E)uropean format  (DD/MM/YY)");
puts("r   for time and date reporting, (R)everse order to time first");
puts("    No options is the same as -dt:  MM/DD/YY hh:mm");
puts("v   Version, Serial No., Copyright message");
puts("?   You know this one already!");
aabort(BELL);
   }

