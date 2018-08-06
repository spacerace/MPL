/**************************************************************/
/*							      */
/*  NAME						      */
/*							      */
/*	 cls  - clear video screen			      */
/*							      */
/*  SYNOPSIS						      */
/*							      */
/*	 cls(); 					      */
/*							      */
/*  DESCRIPTION 					      */
/*							      */
/*	 This function clears the terminal.  It requires      */
/*	 no parameters.  "cls" uses the "csysint" C-to-       */
/*	 assembler interface.				      */
/*							      */
/*  RETURNS						      */
/*							      */
/*	 No return codes are provided.			      */
/*							      */
/**************************************************************/
cls()
{
     struct regset {
	  char al;
	  char ah;
	  char bl;
	  char bh;
	  char cl;
	  char ch;
	  char dl;
	  char dh;
     };
     unsigned csysint();
     struct regset sreg;     /* regs to send to csysint */
     struct regset rreg;     /* regs returned from csysint */
     unsigned flags;	     /* machine status register */
     int interrupt;	     /* interrupt code requested */

     interrupt = 0x10;
     sreg.ah = 0;
     sreg.al = 0x03;

     sreg.bh = 0;
     sreg.bl = 0;
     sreg.ch = 0;
     sreg.cl = 0;
     sreg.dh = 0;
     sreg.dl = 0;

     flags = csysint(interrupt,&sreg,&rreg);
}
/*   ---- test program for above routine commented out..
main()
{
     cprintf("Test pattern to fill screen.\n");
     cprintf("Test pattern to fill screen.\n");
     cprintf("Test pattern to fill screen.\n");
     cprintf("Test pattern to fill screen.\n");
     cprintf("Test pattern to fill screen.\n");
     cprintf("Test pattern to fill screen.\n");
     cprintf("Test pattern to fill screen.\n");
     cprintf("Test pattern to fill screen.\n");

     cls();
}  end of commented out code  for test routine */
