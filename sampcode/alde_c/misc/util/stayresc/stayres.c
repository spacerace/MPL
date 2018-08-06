
/****************************************************************************/
/*									    */
/*     STAYRES.C   Version 1.1	10/26/85	       Brian Irvine	    */
/*									    */
/****************************************************************************/
/*									    */
/* stayres.c - Code which can be used in a general way to create programs   */
/*	       in C which will terminate and stay resident.  This code	    */
/*	       will allow the use of DOS I/O without having the stack and   */
/*	       registers clobbered.  This code is written in DeSmet C and   */
/*	       uses library functions which may not be available in other   */
/*	       C compilers.  It also makes heavy use of the #asm compiler   */
/*	       directive to allow in-line assembly language within the C    */
/*	       code.  This code provides a general outline for a main()     */
/*	       function which can be modified to suit the users needs. All  */
/*	       the code necessary to terminate and stay resident is	    */
/*	       contained in this module; the user's program can be contain- */
/*	       ed entirely externally.	The program does not have to be a   */
/*	       COM file, and the amount of memory reserved is not limited   */
/*	       to 64K. The code has not been tested on program files with   */
/*	       greater than 64K of code.				    */
/*									    */
/*	       This code is based on a set of routines written in Turbo     */
/*	       Pascal by Lane H. Ferris which are available on the Borland  */
/*	       SIG on Compuserve.					    */
/*									    */
/*	       Brian Irvine						    */
/*	       3379 St Marys Place					    */
/*	       Santa Clara, CA 95051					    */
/*	       [71016,544]						    */
/*									    */
/****************************************************************************/


/****************************************************************************/
/*									    */
/* Modification Log:							    */
/*     Version 1.10 - 10/26/85						    */
/*     Corrected error in restoring stack which caused major problems.	    */
/*     Tested now with Personal Editor, Volkswriter Deluxe, Crosstalk.	    */
/*     Removed redundant code which restored registers twice, once from     */
/*     memory and again from the stack. 				    */
/*									    */
/****************************************************************************/

#include   "stdio.h"

/*----- Global variables ---------------------------------------------------*/

unsigned   dos_regs [10];	       /* save DOS registers here */
unsigned   dos_dseg;		       /* save the DS and SS regs */
unsigned   dos_sseg;		       /* for later convenience */
unsigned   dos_sp;		       /* storage for DOS stack pointer */
unsigned   c_sseg;		       /* save our stack segment here */
unsigned   c_sp;		       /* and our stack pointer here */
unsigned   stacksize;		       /* size of DOS/local stack */
int	   _rax, _rbx, _rcx, _rdx,     /* variables hold register values */
	   _rsi, _rdi, _rds, _res;

/*--------------------------------------------------------------------------*/
/* note: storage to save the DOS/C data segments must be allocated in the   */
/*	 code segment, so variable must be defined inside "#asm - #" area.  */
/*--------------------------------------------------------------------------*/

/*----- Constants ----------------------------------------------------------*/

#define    KB_INT      0x16    /* BIOS software int keyboard service routine */
#define    DOS_INT     0x21    /* DOS kitchen sink interrupt */
#define    USER_INT    0x67    /* INT 16H vector is moved to here */

#define    WAKEUP      0x71    /* scan code for Alt-F10 */
#define    PARAGRAPHS  0x1000  /* # of paragraphs of memory to keep */

/*----- Externals ----------------------------------------------------------*/

extern unsigned _PCB;	/* DeSmet C stores original sp value at _PCB + 2. */
extern void    program();	       /* user's program module entry point */

/*--------------------------------------------------------------------------*/
/*     Initial program startup code					    */
/*--------------------------------------------------------------------------*/

main ()
{

/* Save the C data segment and reserve storage in code segment for later use */

#asm

	   jmp	   get_ds

active_:   db	   0
c_ds_:	   dw	   0
dos_ds_:   dw	   0

get_ds:    push    ax
	   mov	   ax,ds
	   mov	   cs:c_ds_,ax
	   pop	   ax

#
   /* initialize program stack segment variable */

   c_sseg = _showds();

   /* get the current address of INT 16H and save it */

   _res = 0;		       /* just for DOS 3.0 */
   _rax = 0x3500 + USER_INT;
   _doint (DOS_INT);

   if ( _res != 0 )
       puts ("\nInterrupt 67H in use.  Can't install program.\n");
   else
       {

/*--------------------------------------------------------------------------*/
/*     Do your program initialization here because you won't get hold of    */
/*     it again until it is entered with the wakeup key sequence.	    */
/*--------------------------------------------------------------------------*/

       /* get address of current INT 16H routine */

       _rax = 0x3500 + KB_INT;
       _doint (DOS_INT);

       /* set the old INT 16H code up as INT 67H for later use */

       _rax = 0x2500 + USER_INT;
       _rds = _res;
       _rdx = _rbx;
       _doint (DOS_INT);

       /*  now set the INT 16H vector to point to our new service routine */

       _rax = 0x2500 + KB_INT;
       _rds = _showcs();

#asm
	   push    bx
	   lea	   bx, back_door_
	   mov	   word _rdx_,bx
	   pop	   bx
#
       _doint (DOS_INT);


       puts ("Resident program installed.\n");

       /* now terminate while staying resident */

       _rax = 0x3100;
       _rdx = PARAGRAPHS;
       _doint (DOS_INT);

       }

}


/*----- Interrupt 16H service routine --------------------------------------*/
/*									    */
/*     This function replaces the standard BIOS INT 16H service routine.    */
/*     It is called by DOS and our application program to get the next	    */
/*     character from the queue, check the shift status, or just see if     */
/*     there is a character in the queue.  All type 1 and 2 requests are    */
/*     passed through to the original service routine, through a long	    */
/*     jump to that address.  Type 0 requests (get the next character)	    */
/*     are handled through an INT 67H call to the original service rou-     */
/*     tine.  The character obtained is checked to see if it is the	    */
/*     designated wakeup key.  If not, it is passed on to DOS.	If it is,   */
/*     then part of the DOS stack is saved on the local stack, all the	    */
/*     processor registers are saved in memory and the program is	    */
/*     started up.  The C program environment and registers are estab-	    */
/*     lished and the DOS environment is saved for later restoration.	    */
/*     Upon return from the user's program, the DOS stack is restored       */
/*     from the data saved on the local stack, bringing DOS back to	    */
/*     where it was before the user program was turned on.		    */
/*     These operations allow programs to be written in C without having    */
/*     to worry about the resident routines destroying the DOS registers    */
/*     when file I/O is used.  It also allows the use of printf().	    */
/*									    */
/*--------------------------------------------------------------------------*/



void   back_door()
{
/*
   Current stack contents:
	       sp -> DOS ip
	   sp + 2 -> DOS cs
	   sp + 4 -> DOS flags

   C function prologue:
	   push    bp
	   mov	   bp,sp
*/

#asm
	   pop	   bp		       ;toss out the bp from the prologue
	   cmp	   cs:active_,1        ;if the program is currently active,
	   jne	   not_on	       ;go service the request
;
; long jump to F000:E82E
;
	   db	   0EAH
	   dw	   0E82EH
	   dw	   0F000H
;
not_on:    cmp	   ah,0 	       ;if this is a character request
	   jz	   chr_rqst	       ;check a little further
;
; long jump to F000:E82E
;
	   db	   0EAH
	   dw	   0E82EH
	   dw	   0F000H
;
chr_rqst:
	   int	   67H		       ;get the next character from the queue
	   cmp	   ah,71H	       ;if it's not the wakeup character,
	   jne	   skipall	       ;then take it back to the caller
	   mov	   cs:active_,1        ;else set the 'program active' flag
;
;  now we enter the program
;
	   cli			       ;no interrupts for now
	   mov	   cs:dos_ds_,ds       ;save the DOS data segment
	   mov	   ds,cs:c_ds_	       ;establish our data segment
	   push    bp		       ;save bp
	   mov	   bp,offset dos_regs_ ;point to dos_regs
	   mov	   ds:[bp+0], ax       ;copy the current registers into an
	   mov	   ds:[bp+2], bx       ;array in the local data segment
	   mov	   ds:[bp+4], cx
	   mov	   ds:[bp+6], dx
	   pop	   ds:[bp+8]	       ;get the bp and save it
	   mov	   ds:[bp+10], si
	   mov	   ds:[bp+12], di
	   push    ax
	   mov	   ax, cs:dos_ds_      ;save the DOS ds reg
	   mov	   ds:[bp+14], ax
	   pop	   ax
	   mov	   ds:[bp+16], es      ;save es
	   pushf		       ;save the flags too
	   pop	   ds:[bp+18]
	   mov	   word dos_sseg_,ss   ;save DOS stack segment
	   mov	   si,ss	       ;if DOS stack segment and C stack
	   mov	   es,si	       ;segment are the same, then use the
	   mov	   ss,word c_sseg_     ;current stack pointer for saving
	   cmp	   si,word c_sseg_     ;the registers. If not the same, then
	   mov	   si,sp	       ;we are entering the program so use
	   je	   keep_sp	       ;the starting stack pointer
	   mov	   si,ss:_PCB_ + 2
keep_sp:   xchg    sp,si	       ;save the registers on the stack
				       ;now using local stack (ds = ss)
	   push    [bp+0]	       ;save ax
	   push    [bp+2]	       ;save bx
	   push    [bp+4]	       ;save cx
	   push    [bp+6]	       ;save dx
				       ;don't save bp
	   push    [bp+10]	       ;save si
	   push    [bp+12]	       ;save di
	   push    [bp+14]	       ;save ds
	   push    [bp+16]	       ;save es
				       ;don't save flags
	   sub	   cx,cx	       ;now save 40 words or less from the
	   sub	   cx,si	       ;DOS stack onto the current stack
	   shr	   cx,1 	       ;If the stack size is less than 40
	   cmp	   cx,40	       ;save 'stacksize' words, else save 40
	   jle	   under40
	   mov	   cx,40
under40:   mov	   word stacksize_,cx
restack:   push    es:[si]
	   inc	   si
	   inc	   si
	   loop    restack
	   push    si		       ;save the count of words pushed on stack
	   mov	   word c_sp_,sp       ;save current stack pointer
	   sti
	   push    bp		       ;do the C function prologue here
	   mov	   bp,sp

#

/*----- Call your program from here ----------------------------------------*/

	   program();

/*----- Restore everything -------------------------------------------------*/

#asm
restore:
	   pop	   bp
	   cli			       ;no interrupts
	   mov	   sp,word c_sp_       ;get the stack pointer back
	   pop	   si		       ;get pointer to top of words to be moved
	   mov	   cx,word stacksize_  ;get count of words to move
	   mov	   es,word dos_sseg_   ;point es to caller's stack
unstack:   dec	   si		       ;back up one word
	   dec	   si
	   pop	   es:[si]	       ;restore the caller's stack from
	   loop    unstack
	   mov	   bp,si	       ;save pointer to top of caller's stack
	   pop	   es		       ;pop registers off the stack
	   pop	   di		       ;don't restore ds just yet
	   pop	   di
	   pop	   si
	   pop	   dx
	   pop	   cx
	   pop	   bx
	   pop	   ax
	   mov	   sp,bp	       ;restore the caller's stack pointer
	   mov	   ax,0 	       ;tell caller there was no character
	   mov	   ss,word dos_sseg_   ;switch back to the caller's stack
	   mov	   bp,offset dos_regs_
	   push    ds:[bp+18]	       ;restore the flags from memory
	   popf
	   mov	   bp,ds:[bp+8]        ;restore the caller's bp reg
	   mov	   cs:active_,0        ;reset the 'program active' flag
	   mov	   ds,cs:dos_ds_       ;finally restore caller's data segment
skipall:   iret 		       ;head on back

#

}


