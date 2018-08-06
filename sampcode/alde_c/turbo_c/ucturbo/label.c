main()
{
   /*                     label.c
    *
    *  This program will print a set of disk labels on standard mailing
    *  label forms using the IBM graphics printers.  The labels are the
    *  same ones issued with the distribution disks.  If you pass on a
    *  copy of the library please do it right and use proper labels.
    *  Thanks - Dave Miller
    *
    *  Copyright (c) 1987 by David A. Miller but placed in the public
    *  domain for the sole purpose of reprinting Unicorn Library Labels
    *  by library users.
    *
    */

   lprintf("\n");
   prtwidon();
   prtdubon();
   prtempon();
   lprintf(" Unicorn Library \n");
   prtempof();
   prtwidof();
   prtcomon();
   lprintf(" A graphics and general purpose library for TURBO C using\n");
   lprintf(" IBM and Tandy 1000 computers.  Disk 1,  Version 4.0\n");
   lprintf("                   Unicorn Software\n");
   lprintf("           Copyright (c) 1987 David A. Miller\n");
   prtcomof();


   lprintf("\n");
   prtwidon();
   prtdubon();
   prtempon();
   lprintf(" Unicorn Library \n");
   prtempof();
   prtwidof();
   prtcomon();
   lprintf(" A graphics and general purpose library for TURBO C using\n");
   lprintf(" IBM and Tandy 1000 computers.  Disk 2,  Version 4.0\n");
   lprintf("                   Unicorn Software\n");
   lprintf("           Copyright (c) 1987 David A. Miller\n");
   prtcomof();



}
