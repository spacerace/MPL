/*
**                OKIDATA PRINTER SETUP UTILITY
** Copyright 1987, S.E. Margison
** 1-11-87 A
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
#include <keys.h>

#define NONE 0xff
#define PICA 0
#define COND 1
#define DRAFT 0
#define LQ 1
#define beep putchar(BELL)

short length, linespc, lqspc, pitch, style;
short printer;  /* 0 = PRN, 1 = LPT1, 2 = LPT2 */

char pica[] =  "NORMAL   ",
     cond[] =  "CONDENSED",
     draft[] = "DRAFT         ",
     lq[] =    "LETTER QUALITY",
     lpt1[] = "LPT1:",
     lpt2[] = "LPT2:",
     prn[] = "PRN: ";

char line1[] = "OKIDATA ML84 CONFIGURATOR V1.10",
     line2[] = "- by Steven E. Margison -",
     line3[] = "Copyright 1986 -- All Rights Reserved",
     line4[] = "F1   Pitch: ",
     line5[] = "F2   Style: ",
     line6[] = "F3   LQ spacing: ",
     line7[] = "F4   Form Length (lines): ",
     line8[] = "F5   Line Spacing: ",
     line9[] = "F6   ",
     line10[]= "F7   Output Channel - ",
     line11[]= "F8   Normal Defaults",
     line12[]= "F9   Send Control Codes",
     line13[]= "FK10  -- Return to DOS --",
     line14[]= "Waiting...",
     cline[]="                                                        ";

main() {           /* no command line arguments */
   int key;
   printer = 0;
   lqspc = linespc = pitch = style = length = NONE;
   domenu(NO);           /* initialize the screen */

   while(YES) {
      key = getkey();    /* get a function code */
      if(key < 0xff) {
         beep;
         continue;
         }
      key &= 0x7f;
      switch(key) {
         case FK1:   /* select pitch */
            dofk1();
            if((pitch is COND) and (style is LQ)) {
               style = DRAFT;
               putstyle();
               }
            prompt();
            break;
         case FK2:   /* select style */
            dofk2();
            if((style is LQ) and (pitch is COND)) {
               pitch = PICA;
               putpitch();
               }
            prompt();
            break;
         case FK3:   /* specify LQ character spacing */
            if((lqspc = get2digs(9, 32)) is NONE) { prompt(); break; }
            if(style isnot LQ) {
               style = LQ;
               putstyle();
               if(pitch is COND) {
                  pitch = PICA;
                  putpitch();
                  }
               }
            if(lqspc > 0x11) {
               lqspc = 0;
               warn(1);
               putlqspc();
               }
            prompt();
            break;
         case FK4:   /* specify Form length in lines */
            if((length = get2digs(10, 41)) is NONE) { prompt(); break; }
            if(length is 0) {
               length = 0x66;
               warn(2);
               putlength();
               }
            prompt();
            break;
         case FK5:  /* specify line spacing */
            dofk5();
            prompt();
            break;
         case FK6:
            beep;
            break;
         case FK7:  /* select output channel */
            dokey7();
            prompt();
            break;
         case FK8:   /* set normal options */
            linespc = 0;  /* 6 lpi */
            putlinespc();
            length = 0x66;
            putlength();
            style = DRAFT;
            putstyle();
            pitch = PICA;
            putpitch();
            lqspc = NONE;
            putlqspc();
            printer = 0;
            putprinter();
            prompt();
            break;
         case FK9:  /* write the codes out and exit */
            dofk9();
            break;
         case FK10:
            exit2dos();
            domenu(YES);
            break;
         default:
            beep;
         }   /* end of switch */
      }      /* end of main while */
   }         /* end of main() */


domenu(cmd) int cmd; {
   int i;
   /* if cmd is YES, then place current setting in menu for each item */
   cls();
   mkbox(0, 0, 80, 22, 0);   /* place the border on blank screen */
   i = cntr(line1);
   d_say(3, i, line1);
   i = cntr(line2);
   d_say(4, i, line2);
   i = cntr(line3);
   d_say(5, i, line3);
   d_say(7, 15, line4);
   if(cmd and (pitch isnot NONE)) putpitch();
   d_say(8, 15, line5);
   if(cmd and (style isnot NONE)) putstyle();
   d_say(9, 15, line6);
   if(cmd and (lqspc isnot NONE)) putlqspc();
   d_say(10, 15, line7);
   if(cmd and (length isnot NONE)) putlength();
   d_say(11, 15, line8);
   if(cmd and (linespc isnot NONE)) putlinespc();
   d_say(12, 15, line9);
   d_say(13, 15, line10);
   putprinter();
   d_say(14, 15, line11);
   d_say(15, 15, line12);
   d_say(16, 15, line13);
   prompt();
   }

cntr(str) char *str; {   /* returns column to start printing */
   int i;
   i = strlen(str);
   --i;     /* account for newline character */
   return(((80 - i) / 2) - 1);
   }

prompt() {
   d_say(19, 15, cline);
   d_say(19, 15, line14);
   }

warn(value) int value; {
   beep;
   switch(value) {
      case 1:
         d_say(19, 15, "Value too large.  Range is 0-11.");
         break;
      case 2:
         d_say(19, 15, "Length value of 0 meaningless.");
         break;
      default:
         prompt();
         return;
      }
   d_say(20, 15, "Strike any key to continue...");
   getkey();
   d_say(20, 15, cline);
   prompt();
   }


putpitch() {
   if(pitch is PICA) { d_say(7, 27, pica); return; }
   if(pitch is COND) { d_say(7, 27, cond); return; }
   }

putstyle() {
   if(style is DRAFT) { d_say(8, 27, draft); return; }
   if(style is LQ) { d_say(8, 27, lq); return; }
   }

putlqspc() {
   char i;
   d_pos(9, 32, 0);
   if(lqspc is NONE) { fputs("  ", stdout); return; }
   i = (char)((lqspc >>4) | '0');
   fputc(i, stdout);
   i = (char)((lqspc & 0x0f) | '0');
   fputc(i, stdout);
   }

putlength() {
   char i;
   d_pos(10, 41, 0);
   i = (char)((length >>4) | '0');
   fputc(i, stdout);
   i = (char)((length & 0x0f) | '0');
   fputc(i, stdout);
   }

putlinespc() {
   d_pos(11, 34, 0);
   if(linespc is 0) puts("6 lpi");
   else puts("8 lpi");
   }

putprinter() {
   d_pos(13, 37, 0);
   switch(printer) {
      case 0: default:
         puts(prn);
         return;
      case 1:
         puts(lpt1);
         return;
      case 2:
         puts(lpt2);
         return;
      }
   }

dofk1() {
   char chr;
   if(pitch is NONE) { pitch = PICA; putpitch(); }
   d_pos(7, 27, 0);
   while((chr = getkey()) isnot '\r') {
      if(chr > 255) { beep; continue; }
      if(chr isnot ' ') { beep; continue; }
      ++pitch;
      pitch &= 0x01;  /* toggle between 0 and 1 only */
      putpitch();
      d_pos(7, 27, 0);
      }
   }

dofk2() {
   char chr;
   if(style is NONE) { style = DRAFT; putstyle(); }
   d_pos(8, 27, 0);
   while((chr = getkey()) isnot '\r') {
      if(chr > 255) { beep; continue; }
      if(chr isnot ' ') { beep; continue; }
      ++style;
      style &= 0x01;  /* toggle between 0 and 1 only */
      putstyle();
      d_pos(8, 27, 0);
      }
   }

dofk5() {
   char chr;
   if(linespc is NONE) { linespc = 0; putlinespc(); }
   d_pos(11, 34, 0);
   while((chr = getkey()) isnot '\r') {
      if(chr > 255) { beep; continue; }
      if(chr isnot ' ') { beep; continue; }
      ++linespc;
      linespc &= 0x01;  /* toggle between 0 and 1 only */
      putlinespc();
      d_pos(11, 34, 0);
      }
   }

dokey7() {
   char chr;
   d_pos(13, 37, 0);
   while((chr = getkey()) isnot '\r') {
      if(chr > 255) { beep; continue; }
      if(chr isnot ' ') { beep; continue; }
      ++printer;
      if(printer > 2) printer = 0;  /* trap wrap around */
      putprinter();
      d_pos(13, 37, 0);
      }
   }

dofk9() {
   short i, j;
   short codes[32];
   FILE *fd;
   i = 0;
   fputs("  Sending codes...", stdout);
   if(pitch is PICA) codes[i++] = 0x12;
   if(pitch is COND) codes[i++] = 0x0f;
   if(style isnot NONE) {
      codes[i++] = ESC;
      codes[i++] = 0x58;
      switch(style) {
         default:
            --i; --i; break;
         case DRAFT:
            codes[i++] = 0;
            break;
         case LQ:
            codes[i++] = 1;
            break;
         }
      }
   if((style is LQ) and (lqspc isnot NONE)) {
      codes[i++] = ESC;
      codes[i++] = 0x56;
      codes[i++] = (((lqspc >> 4) * 10) + (lqspc & 0x0f));
      }
   if(length isnot NONE) {
      codes[i++] = ESC;
      codes[i++] = 0x43;
      codes[i++] = (((length >> 4) * 10) + (length & 0x0f));
      }
   if(linespc is 0) {
      codes[i++] = ESC;
      codes[i++] = 0x32;
      }
   if(linespc is 1) {
      codes[i++] = ESC;
      codes[i++] = 0x30;
      }
   /* i has count of characters to place */

   if(printer is 1) fd = fopen("LPT1", "wb");
   if(printer is 2) fd = fopen("LPT2", "wb");
   else fd = stdprn;

   if(fd is NULL) error("Cannot open printer channel");

   for(j = 0; j < i; ++j) fputc(codes[j], fd);
   cls();
   exit(0);
   }


get2digs(row, col) int row, col; {
   int i, j;
   char chr;
   i = j = 0;
   d_pos(row, col, 0);
   puts("  ");
   d_pos(row, col, 0);
   while((chr = getkey()) isnot '\r') {
      if(i > 2) { beep; continue; }
      if(chr > 255) { beep; continue; }
      if(chr is '\b') {
         j = i = 0;
         d_pos(row, col, 0);
         puts("  ");
         d_pos(row, col, 0);
         continue;
         }
      if(chr is ESC) {
         d_pos(row, col, 0);
         puts("  ");
         return(NONE);
         }
      if(!isdigit(chr)) { beep; continue; }
      putchar(chr);  /* echo it to screen */
      j = j<<4;
      j |= (chr & 0x0f);
      ++i;
      }
   if(i is 0) return(NONE);
   else return(j);
   }

