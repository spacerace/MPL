
COMM.C.1



/* NOTE: this code is indented using tabs set every 4 columns, not 8. */

/*
 * Rudimentary communication program; written in C with I/O
 * routines in assembler, using "#asm" construct in DeSmet C.
 */

#define TRUE 1
#define FALSE 0
#define NULL 0

#define comm_stat head!=tail

int head=0,
	tail=0;

char comm_buffer[8192];

main()
{ 
	char commget();

	int c,
		echo=FALSE,
		verbose=FALSE;

	initcomm();

	goto inside;
	while((c=my_ci())<256) /* alt's and function keys are >=256 */
	{
		commput(c);
		if(echo)
			my_co(c);
inside:	while(my_stat()==0)
			if(comm_stat)
				if(is_nice(c=commget()))
					my_co(c);
				else
					if(verbose)
					{
						my_co('^');
						my_co(127&(c+'@'));
					} 
	}
	switch(c>>8) /* process function key */
	{
		case '\060': /* b	send break	*/
			make_break();
			my_puts("** break **\n");
			break;
		case '\022': /* e	echo toggle	*/
			echo = !echo;
			if(echo)
				my_puts("** echo on **\n");
			else
				my_puts("** echo off **\n");
			break;
		case '\020': /* q	quit		*/
			my_puts("** returning to DOS **\n");
			goto byebye;
		case '\057': /* v	verbose		*/
			verbose = !verbose;
			if(verbose)
				my_puts("** verbose on **\n");
			else
				my_puts("** verbose off **\n");
			break; 
	}
	goto inside;

byebye:
	killcomm();
}

initcomm()
{
#asm

; initialize communication port,
; and install interrupt handler

; save dseg for interrupt handler
	mov		ax,ds
	mov		cs:dssave,ax

; set int vector to my handler
	push	ds
	xor		ax,ax
	mov		ds,ax
	mov		[30h],offset handler
	mov		ax,cs 
	mov		[32h],ax
	pop		ds

; assert DTR, RTS, and OUT2
	mov		dx,3fch
	mov		al,11
	out		dx,al

; enable interrupts for data ready
	mov		dx,3f9h
	mov		al,1
	out		dx,al

; clear any outstanding int
	mov		dx,3f8h
	in		al,dx

; enable interrupts
	sti

; enable IRQ4 on 8259A
	in		al,21h
	and		al,0efh
	out		21h,al 

; and return
	jmp		init_finis

dssave:	dw 0

handler:
	push	ds
	push	ax
	push	dx
	push	bx

; restore appropriate dseg
	mov		ax,cs:dssave
	mov		ds,ax

; grab the byte from comm port
	mov		dx,3f8h
	in		al,dx

; put it in the buffer
	mov		bx,tail_
	mov		comm_buffer_[bx],al
 
; tail=tail+1 (mod 8192)
	inc		bx
	cmp		bx,8192
	jl		handler_around
	xor		bx,bx

handler_around:
	mov		tail_,bx

; tell the 8259A EOI
	mov		dx,20h
	mov		al,32
	out		dx,al

	pop		bx
	pop		dx
	pop		ax
	pop		ds

; reenable interrupts
	sti

	iret
 
init_finis:
#
}

commput(c)
int c;
{
#asm
; put the character out the port
	mov		dx,3f8h
	mov		ax,[bp+4]
	out		dx,al
#
}

char commget()
{
	char temp;

		temp=comm_buffer[head++];
		head%=8192;
		return(temp);
}
 
killcomm()
{
#asm
; disconnect the interrupt handler
	in		al,21h
	or		al,10h
	out		21h,al
#
}

make_break()
{
#asm
; set bit on LCR
	mov		dx,3fbh
	in		al,dx
	push	ax
	or		al,40h
	out		dx,al

; wait a wee bit (~200 ms)
	mov		bx,8
outer:
	mov		cx,7000 
tight:
	loop	tight
	dec		bx
	jnz		outer

; and turn break back off
	pop		ax
	out		dx,al
#
}

is_nice(c) /* true for those characters that "should" be received */
char c;
{
	c&=127;
	if(c>=' ' && c<='~')	/* printable */
		return(TRUE);
	if(c>6 && c<11)			/* BEL, BS, HT, LF */
		return(TRUE);
	if(c==13 || c==27)		/* CR, ESC */
		return(TRUE);
	return(FALSE);
}
 
my_puts(s) /* puts, using my_co() */
char *s;
{
	while(*s)
	{
		if(*s=='\n')
			my_co('\r');
		my_co(*(s++));
	}
}

my_co(c) /* character output, smaller and faster than DeSmet's,
			using DOS function call 2.00 */
char c;
{
#asm
	mov		ah,2
	mov		dx,[bp+4]
	int		21h
#
}

my_stat() /* true if character is ready from keyboard */
{ 
#asm
	mov		ah,1
	int		16h
	jz		stat_no_char
	mov		ax,1
	jmp		stat_finis

stat_no_char:
	xor		ax,ax

stat_finis:
#
}

my_ci() /* get character from keyboard, using BIOS function call */
{
#asm
	xor		ah,ah
	int		16h
	or		al,al
	jz		my_ci_finis
	xor		ah,ah

my_ci_finis: 
#
}
------------------------------------------------------------------------------
The termcap for DOS 2.00 with ansi device driver follows:
------------------------------------------------------------------------------
pm|pcmon|IBM-PC using DOS 2.00 ansi device driver (monochrome)|\
	:am:bc=^H:bw:ce=\E[K:cl=\E[2J:cm=\E[%i%d;%dH:co#80:cr=^M:\
	:do=^J:ho=\E[H:kb=^H:li#25:ll=\E[25H:nd=\E[C:\
	:pt:se=\E[0m:so=\E[1m:ta=^I:up=\E[A:xt:
pc|pccol|IBM-PC using DOS 2.00 ansi device driver (color)|\
	:am:bc=^H:bw:ce=\E[K:cl=\E[2J:cm=\E[%i%d;%dH:co#80:cr=^M:\
	:do=^J:ho=\E[H:kb=^H:li#25:ll=\E[25H:nd=\E[C:\
	:pt:se=\E[0;32m:so=\E[1m:ta=^I:up=\E[A:xt:
------------------------------------------------------------------------------
and that's it! Enjoy. I place this in the public domain, and would appreciate
feedback. Improvements and bug reports especially desired. If the code is too
cryptic or uncommented, feel free to send questions to:
					Bennett Todd
					...{decvax,ihnp4,akgua}!mcnc!ecsvax!bet
