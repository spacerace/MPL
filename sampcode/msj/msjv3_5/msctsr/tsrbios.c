
/* Figure 5. Simple video BIOS routines. */

   /*
    * a couple of BIOS routines for tsrs
    */
   #include "tsrbios.h"
   #include <dos.h>

   #define BIOS_VIDEOINT 0x10
   #define BIOS_VIDEOINT_RDCH 0x8
   #define BIOS_VIDEOINT_WRCH 0x9
   #define BIOS_CURSOR_ARRAY (int far *)(0x450)
   #define BIOS_VIDEO_PAGE_NUM (char far *)(0x462)

   union REGS regs;

   scr_swapmsg(msg, savbuf, row, col)
   unsigned char *msg, *savbuf;
   {
         unsigned val;

         while(*msg) {
               *savbuf++ = val = scr_read_ch(row, col);
               val &= 0xff00;    /* preserve attribute */
               val |= *msg++;
               scr_write_ch(val, row, col++);
         }
         *savbuf = 0;
   }

   unsigned scr_read_ch(row, col)
   int row, col;
   {
         /* poke cursor val into bios loc */
         *scr_get_curs_addr() = row<<8 | col;

         /* use bios int to get char */
         regs.h.ah = BIOS_VIDEOINT_RDCH;
         regs.h.bh = *BIOS_VIDEO_PAGE_NUM;
         int86(BIOS_VIDEOINT, &regs, &regs);
         return regs.x.ax;
   }

   void scr_write_ch(ch, row, col)
   unsigned ch;
   int row, col;
   {
         /* poke cursor val into bios loc */
         *scr_get_curs_addr() = row<<8 | col;

         /* use bios int to get char */
         regs.h.ah = BIOS_VIDEOINT_WRCH;
         regs.h.al = ch;   /* char */
         regs.x.cx = 1;    /* one char */
         regs.h.bh = *BIOS_VIDEO_PAGE_NUM;
         regs.h.bl = ch>>8;      /* attr */
         int86(BIOS_VIDEOINT, &regs, &regs);
   }

   int far * scr_get_curs_addr()
   {
         return BIOS_CURSOR_ARRAY + (int)(*BIOS_VIDEO_PAGE_NUM);
   }
