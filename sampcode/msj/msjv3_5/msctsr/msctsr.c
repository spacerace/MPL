/* Figure 4. MSCTSR.EXE - A TSR to pop up the time. */

   /*
    * tsr to pop up the time
    */

   /* Additional Notes:
    * Note that MSCTSR.EXE can be loaded more than once and will not
    * check to see if it is already loaded. A hot key combination
    * will be intercepted by each instance of MSCTSR that is loaded.
    * What results is that, for example, if three instances of MSCTSR
    * are loaded, when CTRL-W is hit, each instance of MSCTSR will
    * pop-up the time and restore the screen before control is returned.
    */

   #include "tsrbios.h"
   #include <dos.h>
   #include <bios.h>
   extern int _psp, end;
   extern unsigned _osmajor;

   extern union REGS regs;

   #define HOTKEY_CODE     0x11      /* W key */
   #define HOTKEY_STAT     0x4       /* ctrl */
   #define KEYPORT         0x60      /* keyboard i/o port */

   #define DOS_INT         0x21
   #define KEYSVC_INT      0x9
   #define DOS_PRINTSTRING 0x9

   #define StartOfTime 6   /* 6 characters into msg string */
   char msg[] = "****  HH:MM:SS  ****";
   char buf[] = "                    ";

   /* variables for time calculations */
   unsigned long ticks;
   int h,m,s;
   char *sptr;

   void (interrupt far * oldint9)();

   /*
    * look for the hotkey, and call popup when you see it
    */

   void interrupt far newint9(es,ds,di,si,bp,sp,bx,dx,cx,ax,
                              ip,cs,flags)
   unsigned es, ds, di, si, bp, sp, bx, dx, cx, ax, ip, cs, flags;
   {
         static unsigned char keycode;
         static char active = 0;

         if (active)
               _chain_intr(oldint9);

         keycode = inp(KEYPORT);
         if (keycode != HOTKEY_CODE)
               _chain_intr(oldint9);
         if (_bios_keybrd(_KEYBRD_SHIFTSTATUS) & HOTKEY_STAT) {
               /* got it */
               active = 1;
               (*oldint9)(); /* let normal routine process hotkey */
               popup();
               active =  0;
         } else
               _chain_intr(oldint9);
   }

   main()
   {
         char huge *startofitall;
         char huge *endofitall;
         unsigned length;

         /* check dos ver */
         if (_osmajor < 2) {
               dos_puts("Not loaded, requires dos v2 or over.\r\n$");
               exit();
         }

         /* save old int9 */
         oldint9 = _dos_getvect(KEYSVC_INT);
         _dos_setvect(KEYSVC_INT, (void (interrupt far *)())newint9);

         /* some location work */
         FP_SEG(startofitall) = _psp;
         FP_OFF(startofitall) = 0;
         endofitall = (char huge *)&end;
         length = endofitall - startofitall; /* bytes */
         if (length & 0xf) /* round up to next 16 byte para */
               length += 0x10;
         length >>= 4;     /* convert to paragraphs */

         dos_puts("Loaded ok\r\n$");
         _dos_keep(0, length);   /* discards stack, keeps all else */
   }

   /*
    * print a $ terminated string
    */
   dos_puts(s)
   char *s;
   {
         regs.x.dx = (int)s;
         regs.h.ah = DOS_PRINTSTRING;
         intdos(&regs,&regs);
   }

   popup()
   {
         int oldcursor;

         /* swallow hotkey keystroke */
         (void)_bios_keybrd(_KEYBRD_READ);

         /* save cursor */
         oldcursor = *scr_get_curs_addr();

         /* put time into message */
         bigben();

         /* popup message */
         scr_swapmsg(msg, buf, 12, 30);

         /* wait for keystroke */
         (void)_bios_keybrd(_KEYBRD_READ);

         /* erase message */
         scr_swapmsg(buf, msg, 12, 30);

         /* restore cursor */
         *scr_get_curs_addr() = oldcursor;
   }

   /*
    * Read ticks, convert to cumulative seconds, and then fill msg.
    *  cum_secs = ticks/18.2065
    *  (but I don't want to use floating point)
    *  or, cum_secs = (ticks * 10,000) / 182065
    *  (but that would overflow in a long)
    *  or, cum_secs = (ticks * 2,000) / 36413
    */

   bigben()
   {
         _bios_timeofday(_TIME_GETCLOCK, &ticks);
         ticks *= 2000L;         /* convert ... */
         ticks /= 36413L;  /* ... to seconds */
         h = ticks / 3600;
         m = ticks / 60;
         m = m % 60;
         s = ticks % 60;
         sptr = msg + StartOfTime;
         *sptr++ = h/10 + '0';
         *sptr++ = h%10 + '0';
         *sptr++ = ':';
         *sptr++ = m/10 + '0';
         *sptr++ = m%10 + '0';
         *sptr++ = ':';
         *sptr++ = s/10 + '0';
         *sptr++ = s%10 + '0';
   }
