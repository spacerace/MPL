******************************************************************************
*
* dstart.s -- startup code for the C runtime library
*	
*	IMPORTANT: SEE THE DESCRIPTION OF THE "STACK" VARIABLE, BELOW.
*
*	This is the startup code for any application running on the Atari ST
*	with the public domain standard library routines by Dale Schumacher.
*
*	This must be the first object file in a LINK command. When the
*	operating system gives it control, our process has ALL of memory
*	allocated to it, and our stack pointer is at the top.
*	This code (conditionally) gives some of that memory back
*	to the operating system, places its stack pointer at the top
*	of the memory it still owns, sets up some global variables.
*	It then calls __main, the run-time library startup routine
*	(which parses the command line into argc/argv, opens stdin and
*	stdout, etc., before calling the programmer's _main).
*
*	This object file also includes __exit, which is the procedure the
*	runtime library calls to exit back to the operating system.
*
******************************************************************************
*
*	CONFIGUING THIS STARTUP FILE
*
*	The __STKSIZ variable (which is _STKSIZ in C source files) tells
*	the startup routine how much space to set aside for stack/heap.
*	The malloc(), lalloc(), realloc(), etc. memory management functions
*	DO NOT allocate space from the heap (unlike the Alcyon C routines),
*	so you typically don't need a huge stack unless you're doing lots
*	of recursion.
*
*	If __STKSIZ is positive, it represents the number of bytes of
*	stack/heap that you want to reserve.  If __STKSIZ is negative,
*	it means "keep all BUT -(__STKSIZ) bytes" for stack/heap.  There
*	is a minumum stack/heap size assembled into this module called
*	MINSTACK.  Regardless of what __STKSIZ is, you will get at least
*	MINSTACK byte, or the program will exit with "no enough memory".
*	If the __STKSIZ variable is not defined by your program (the
*	linker gives us a NULL pointer in that case), MINSTACK will be
*	used instead.
*
*		An example using __STKSIZ this in C is:
*
*			/* outside all function blocks */
*			unsigned long _STKSIZ = 32767;	/* 32K stack+heap */
*		or
*			unsigned long _STKSIZ = -8192;	/* keep all but 8K */
*
*		Note that in C, all variables get an underscore stuck on
*		the front, so you just use one underscore in your program.
*		Note also that it has to be all upper-case.
*
*	Note that if you give back less than 512 bytes, you still shouldn't
*	use Pexec(), and if you give back less than (about) 4K, you shouldn't
*	use the AES or VDI.
*

MINSTACK=4096		* minimum stack+heap size. (also __STKSIZ if undefined)
FUDGE=512		* minimum space to leave ABOVE our stack

* BASEPAGE ADDRESSES:
p_lowtpa=$0		* Low TPA address (basepage address)
p_hitpa=$4		* High TPA address (and initial stack pointer)
p_tbase=$8		* ptr to Code segment start
p_tlen=$c		* Code segment length
p_dbase=$10		* ptr to Data segment start
p_dlen=$14		* Data segment length
p_bbase=$18		* ptr to Bss  segment start
p_blen=$1c		* Bss  segment length
p_dta=$20		* ptr to process's initial DTA
p_parent=$24		* ptr to process's parent's basepage
p_reserved=$28		* reserved pointer
p_env=$2c		* ptr to environment string for process
p_cmdlin=$80		* Command line image

* GEMDOS functions:
cconws=$09		* Cconws(string): write to console
mshrink=$4a		* Mshrink(newsize): shrink a block to a new size
pterm=$4c		* Pterm(code): exit, return code to parent

.globl	__STKSIZ	* global variable holding stack size
.globl	__main		* required external function... calls user's main()

.globl	__base
.globl	__break
.globl	__argc
.globl	__argv
.globl	__envp
* ?	.globl	___cpmrv

*
*  Must be first object file in link statement
*
	.text
.globl	__start
__start:
	move.l	sp,a1		* save our initial sp (used by ABORT)
	move.l	4(sp),a0	* a0 = basepage address
	move.l	a0,__base	* base = a0
	move.l	p_bbase(a0),d0	* d0 = bss seg start
	add.l	p_blen(a0),d0	* d0 += bss length  (d0 now = start of heap)
	move.l	d0,__break	* __break = first byte of heap

	move.l	#__STKSIZ,a1	* load address of __STKSIZ.
	move.l	a1,d1		* move it to data register for undefined check.
	beq	keepmin		* if it's zero(undefined), keep minimum stack.
	move.l	(a1),d1
	bmi	giveback	*	if (__STKSIZ < 0) goto giveback;
	add.l	d0,d1		*	d1 = __base+__STKSIZ; /* new sp */
	bra	gotd1

keepmin:			* __STKSIZ was undefined; keep minimum.
	move.l	#MINSTACK,d1
	add.l	d0,d1		*	d1 = __base + MINSTACK;
	bra	gotd1		*	goto gotd1;

giveback:
	add.l	p_hitpa(a0),d1	*	d1 += hitpa;

gotd1:	move.l	d1,sp		* gotd1: sp = d1;

*
* DOSHRINK: take SP as a requested stack pointer. Place it
* between (__break+MINSTACK) and (p_hitpa(a0)-FUDGE).  If we can't,
* abort. Otherwise, we return the remaining memory back to the o.s.
* The reason we always shrink by at least FUDGE bytes is to work around
* a bug in the XBIOS Malloc() routine: when there are fewer than 512
* bytes in the largest free block, attempting a Pexec() breaks the
* memory management system, thus, FUDGE must be at least 512.
*
* PSEUDOCODE:
* doshrink(sp)
* {
*	/* if too low, bump it up */
*	if (sp < (__break + MINSTACK))
*		sp = (__break + MINSTACK);
*
*	/* if too high, bump it down */
*	if (sp > (hitpa - FUDGE)) {
*		sp = (hitpa - FUDGE);
*
*		/* if now too low, there's not enough memory */
*		if (sp < (__break + MINSTACK))
*			goto abort;
*	}
*	Mshrink(0,__base,(sp - __base));
* }
*
	move.l	d0,d1		*   d1 = __break;
	add.l	#MINSTACK,d1	*   d1 += MINSTACK;
	cmp.l	d1,sp		*   if ((__break+MINSTACK) < sp)
	bhi	minok		* 	goto minok;
	move.l	d1,sp		*   else sp = (__break+MINSTACK)
minok:				* minok:
	move.l	p_hitpa(a0),d2	*   d2 = hitpa;
	sub.l	#FUDGE,d2	*   d2 -= FUDGE;
	cmp.l	d2,sp		*   if ((hitpa - FUDGE) > sp)
	bcs	maxok		*	goto maxok;
*				*   else {
	move.l	d2,sp		*	sp = (hitpa - FUDGE);
	cmp.l	d1,d2		* 	if ((__break+MINSTACK) > (hitpa-FUDGE))
	bcs	abort		*	    goto abort;	/* BAD NEWS */
*				*   }
maxok:

*
* STACK LOCATION HAS BEEN DETERMINED. Return unused memory to the o.s.
*
	move.l	sp,d1		*   d1 = sp;
	and.l	#-2,d1		*   /* ensure d1 is even */
	move.l	d1,sp		*   sp = d1;
	sub.l	a0,d1		*   d1 -= __base; /* d1 == size to keep */

	move.l	d1,-(sp)	* push the size to keep
	move.l	a0,-(sp)	* and start of this block (our basepage)
	clr.w	-(sp)		* and a junk word
	move	#mshrink,-(sp)	* and the function code
	trap	#1		* Mshrink(0,__base,(sp-base))
	add.l	#12,sp		* clean the stack after ourselves

*
* Finally, the stack is set up. Now call _main(cmdline, length)
*
	move.l	__base,a0	* set up _main(cmdline,length)
	lea.l	p_cmdlin(a0),a2	* a2 now points to command line
	move.b	(a2)+,d0	* d0 = length; a2++;
	ext.w	d0		* extend byte count into d0.w
	move.w	d0,-(a7)	* push length
	move.l	a2,-(a7)	* Push commnd
	clr.l	a6		* Clear frame pointer
	jsr	__main		* call _main routine (it shouldn't return)
	clr.w	-(a7)		* IF* it returns, we exit immediately
	trap	#1		* with a zero status code

*
* _exit(code)	Terminate process, return code to the parent.
*
.globl	__exit
__exit:
	tst.l	(a7)+		* drop return PC off the stack, leaving code
	move.w	#pterm,-(a7)	* push function number
	trap	#1		* and trap.

*
* abort: used if the stack setup above fails. Restores the initial sp,
* prints a message, and quits with the error ENSMEM.
*
abort:				* print an abortive message and quit
	move.l	a1,sp		* restore initial sp
	pea.l	abortmsg	* push string address
	move.w	#cconws,-(a7)	* and function code
	trap	#1		* and trap to print message
	addq.l	#6,a7		* clean off stack
	move.w	#-39,-(a7)	* push error number -39: ENSMEM
	jsr	__exit		* and exit with it.

*
* stack overflow error!
*
.globl	__sovf
__sovf:
	move.l	#ovf,-(sp)	* push message address
	move.w	#cconws,-(sp)	* push fn code
	trap	#1		* Issue message
	move.w	#-1,-(a7)	* push return code (-1)
	move.w	#pterm,-(a7)	* push function number
	trap	#1		* and trap.

*
*
.globl	_brk
_brk:		
	cmp.l	__break,sp	* compare current break with current stack
	bcs	__sovf		* actual stack overflow!
	movea.l	4(sp),a0	* get new break
	move.l	a0,d0		* compare with stack, including 256-byte
	adda.l	#$100,a0	* chicken factor
	cmpa.l	a0,sp		* if (sp < a0+256)
	bcs	badbrk		* 	bad break;
	move.l	d0,__break	* OK break: save the break
	clr.l	d0		* Set OK return
	rts			* return
badbrk:
	move.l	#-1,d0		* Load return reg
	rts			* Return

*
*
.globl	___BDOS
___BDOS:
	link	a6,#0		* link
	move.w	8(sp),d0	* Load func code
	move.l	10(sp),d1	* Load Paramter
	trap	#2		* Enter BDOS
	cmpa.l	__break,sp	* Check for stack ovf
	bcs	__sovf		* overflow! print msg and abort
	unlk	a6		* no error; return
	rts			* Back to caller

*
*	Hooks into the operating system through various traps
*
.globl	_gemdos
_gemdos:
	move.l	(sp)+,traddr	* pop return address
	trap	#1		* do gemdos trap
	move.l	traddr,-(sp)	* push return address
	rts			* do normal return

.globl	_bios
_bios:
	move.l	(sp)+,traddr	* pop return address
	trap	#13		* do gemdos trap
	move.l	traddr,-(sp)	* push return address
	rts			* do normal return

.globl	_xbios
_xbios:
	move.l	(sp)+,traddr	* pop return address
	trap	#14		* do gemdos trap
	move.l	traddr,-(sp)	* push return address
	rts			* do normal return


**********************************************************************
*
*	Data area
*
	.data
ovf:		.dc.b	'Stack Overflow',$d,$a,0	  * Overflow message
abortmsg:	.dc.b	'Cannot initialize stack',$d,$a,0 * Abort message
.globl	___pname
___pname:	.dc.b	'runtime',0,0	* Program name
.globl	___tname
___tname:	.dc.b	'CON:',0	* Terminal device name
.globl	___lname
___lname:	.dc.b	'LST:',0	* List device name
.globl	___xeof
___xeof:	.dc.b	$1a		* Control-Z


**********************************************************************
*
*	BSS AREA
*
	.bss
	.even
__base:		.ds.l	1	* -> Base Page
__break:	.ds.l	1	* Break location
__argc:		.ds.w	1	* number of command line args
__argv:		.ds.l	1	* -> parsed command line args
__envp:		.ds.l	1	* -> environment string
* ?	___cpmrv:	.ds.w	1	* Last CP/M return val
traddr:		.ds.l	1	* system trap hook return address
	.end
