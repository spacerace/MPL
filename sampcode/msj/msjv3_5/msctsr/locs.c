/* Figure 3. locs.c, a program that shows how to determine the size of a
/* program. */

   #include <dos.h>
   extern unsigned int _psp, end;
   extern unsigned char _osmajor, _osminor;

   main()
   {
         char huge *startofitall;
         char huge *endofitall;
         unsigned blength;         /* byte length: psp + text + data */
         unsigned plength;         /* paragraph length */

         printf("Dos version %d.%d\n", _osmajor,_osminor);

         FP_SEG(startofitall) = _psp;
         FP_OFF(startofitall) = 0;
         endofitall = (char huge *)&end;
         blength = endofitall - startofitall;      /* bytes */
         plength = blength;
         if (plength & 0xf)      /* round up to next 16 byte para */
               plength += 0x10;
         plength >>= 4;          /* convert to paragraphs */

         printf("start %Fp, end %Fp, size %u (bytes) %u (paragraphs)\n",
               startofitall, endofitall, blength, plength);
   }
