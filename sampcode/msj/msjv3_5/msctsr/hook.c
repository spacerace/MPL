
/* Figure 2. A simple program that hooks a vector and then chains to the
/* original handler. */

   #include <dos.h>

   #define DOS_INT 0x21

   void far interrupt newint21();            /* DOS */

/* Note: Original MSJ code had a typo in the following line of code -
         oldint21 is correct, not oldin21 as in the article figure */

   void (interrupt far * oldint21)();
   unsigned long syscalls = 0;

   void interrupt far newint21(es,ds,di,si,bp,sp,bx,dx,cx,ax,
                               ip,cs,flags)
   unsigned es, ds, di, si, bp, sp, bx, dx, cx, ax, ip, cs, flags;
   {
         syscalls++;
         _chain_intr(oldint21);
   }

   main()
   {
         /* save old int21 vector */
         oldint21 = _dos_getvect(DOS_INT);
         _dos_setvect(DOS_INT, newint21);

/* Note : the printf in the following line is not in the original
          MSJ code - will return 2 syscalls, rather than 1 as per 
	  the article */

	 printf("Waiting for you ... please hit a key\n\n"); 
         getch();
         printf("%lu syscalls\n", syscalls);
         /* restore old int21 vector */
         _dos_setvect(DOS_INT, oldint21);
	 exit(0);
   }
