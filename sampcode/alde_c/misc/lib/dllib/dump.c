/*
**                HEX FILE DUMP UTILITY
**   Displays any file in hex and character representation
**   in color or monochrome, depending upon type of video
**   card installed.  Displays 20 lines per screen and waits
**   for a keypress.
**
**   This program was originally distributed by Datalight with
**   the Datalight C Developer's Kit.  The program has been
**   extensively modified by Steven E. Margison, Downers Grove, IL.
**   It is placed into the public domain by S.E. Margison, with the
**   understanding of all users that portions may be copyrighted
**   by Datalight and/or others.  No charge may be made for this
**   program under any circumstances.  Exception: a copying charge
**   may be made if this program is included on a disk with other
**   PD or shareware programs.
**
**   Rev 1.10   3-16-87  S.E. Margison
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
** Program requires at least one parameter, the name of the file to dump.
** A second parameter is taken to be an offset value to start dumping,
** default is the start of the file.  If a third value is present it is
** taken to be a hex byte value to highlite whenever encountered in the file.
*/

#include <stdio.h>
#include <ctype.h>
#include <smdefs.h>
#include <screen.h>

int adrclr,    /* address field color */
    hexclr,    /* hex data color */
    highclr,   /* highlite color */
    prompt,    /* prompt line color */
    ascclr;    /* ascii color */

main(argc,argv)
int argc;
char *argv[];
{
   FILE *f;
   int i, buffer[16], lines, view, vflag;
   unsigned long offset = 0;

   vflag = NO;      /* default highlite off */

   if ((argc < 2) or (argc > 4))
      error("Use: dump file [starting offset in hex] [highlite byte]");

   if((f = fopen(argv[1], "rb")) is NULL) cant(argv[1]);

   if (argc > 2) {  /* get an offset if so indicated */
      sscanf(argv[2],"%lx",&offset);
      fseek(f,offset,0);
      }

   if (argc is 4) {  /* get the value to highlite */
      sscanf(argv[3], "%02x", &view);
      vflag = YES;
      }

   i = stuff(0);   /* check video type */
   if(i is MONO) {
      prompt = BLINKING;
      adrclr = WHITE;
      hexclr = HIWHITE;
      ascclr = WHITE;
      highclr = HIGHBLINK;
      }
   else {         /* for CGA cards */
      prompt = BROWN;
      adrclr = LTGREEN;
      hexclr = YELLOW;
      ascclr = LTCYAN;
      highclr = HIWHITE;
      }

   disp_open();    /* use Datalight direct video access routines */
   disp_cls();
   disp_move(0,0);
   disp_flush();
   lines = 20;
   while (TRUE) {
      for (i = 0; i < 16; i++) buffer[i] = fgetc(f);
      if (buffer[0] == -1) break;   /* done */
      disp_setattr(adrclr);   /* set address color */
      disp_printf("%04lx: ",offset);  /* display address */
      disp_setattr(hexclr);   /* set hex value color */
      for (i = 0; i < 16; i++) {
         if (buffer[i] != -1) {
            if(vflag and (buffer[i] is view)) {  /* highlite this byte? */
               disp_setattr(highclr);
               disp_printf("%02x ", buffer[i]);
               disp_setattr(hexclr);
               }
            else disp_printf("%02x ", buffer[i]);
            }
         else disp_puts("   ");
         }
      disp_setattr(ascclr);  /* display ascii data color */
      disp_puts("   ");
      for (i = 0; i < 16; i++) {
         if (buffer[i] != -1) {
            if (!isprint(buffer[i])) buffer[i] = '.';
            disp_putc(buffer[i]);
            }
         else disp_putc(' ');
         }
      disp_putc('\n');
      if(lines-- is 0) {
         disp_setattr(prompt);
         disp_say(22, 10, "Press any key to continue...");
         getkey();
         disp_cls();
         disp_flush();
         lines = 20;
         }
      offset += 16;
      }  /* end of while */
   disp_setattr(prompt);
   disp_say(22, 10, "End of File Encountered");
   disp_flush();
}


disp_puts(buf)
char *buf;
{
   while(*buf isnot NULL) disp_putc(*buf++);
}

