title	EXEC - Load & Execute Programs from Lattice 'C'
page	64,132
name	EXEC
comment |	EXEC by Ted Reuss.  For free distribution only.   10/83

Corrected: SS is no set to PSP segment before call to EXEC.  4/9/84

   This code is not very elegant, but it seems to do what it was designed
to do, so I am putting it the public domain.  When DOS loads a program
either .com or .exe all available memory is assigned to it.  On the first
call to EXEC memflg must be non-zero so that unused memory can be returned
to DOS.  EXEC assumes that the stack segment is loaded higher in memory
than all other segments.  This assumption holds true for many compilers.
   Note that if you load & execute a program which in turn loads and executes
another program via EXEC that memflg must be non-zero on the first call to
EXEC by each child process.
  A simple way to build the command line is:

     struct _line { char cnt; char cmd[250] } cmdline;
     strcpy( cmdline.cmd, source );		/* move command string */
     cmdline.cnt = strlen( cmdline.cmd );	/* set cnt */
     cmdline.cmd[cmdline.cnt--] = '\r';         /* replace NULL with '\r'
						   and decrement count */

Note that the NULL terminator is replaced by a Carriage Return and that
the character count does not include either the CR or the count byte.

When using COMMAND.COM *file can be passed as NULL and the command line
should begin with /C as per DOS 2.0 manual page F-1.

Be sure to pass a complete file specification with both drive and path name.

The File Control Blocks are not used in this version and I don't think DOS
uses them anyway.

I added the parameter memflg today (4/3/84) for the public domain version.
My runtime version of C.OBJ is modified to call the DOS function SETBLOCK (4AH)
so that I do not need to adjust memory.  I tested the new version with DEBUG
and everything seems to work ok.

Note: To open separate files as stdin or stdout for the child process:
      1) close( N );  /* where N = 0 for stdin or 1 for stdout */
      2) open( f,.... )  /* DOS always uses the lowest handle available.
			    therefore if you closed stdin then next file
			    opened will be assigned to stdin */

		Ted Reuss c/o South Texas Software, Inc.
			      4544 Post Oak Place, Suite 222
			      Houston, TX  77027
			      713/877-8205
	|
PGROUP	GROUP	PROG
PROG	SEGMENT BYTE PUBLIC 'PROG'
	ASSUME	CS:PGROUP
;
; name		EXEC -- loads and executes a program
;
; synopsis	sta = exec( file, cmdlin, memflg );
;		short sta;	Return code
;				 = 0 if successful, else
;				 = DOS error number
;		char *file;	[d:][path]filename.ext
;				 if = NULL, COMMAND.COM is used.
;				      Drive/Path is hard coded.
;		char *cmdlin;	Ptr to command line of:
;				 1 byte -  number of characters in command
;				 bytes xx - command passed to program
;				 1 byte - carriage return
;		short memflg;	If != 0, then adjust memory allocation
;
	PUBLIC	EXEC
EXEC	PROC	NEAR
	push	bp
	mov	bp,sp
	mov	cs:ss_save,ss		;save ss
	mov	cs:sp_save,sp		;save sp
	mov	bx,[bp+6]		;ptr to command line
	mov	word ptr cs:p_cmd,bx
	mov	word ptr cs:p_cmd+2,ds
	mov	si,[bp+4]		;ptr to filename
	test	si,si
	jnz	exe10			;if filename was passed
	mov	si,offset cs:cmdcom	;else use command.com
exe10:
	mov	ah,51h			;get PSP segment
	int	21h
	mov	dx,bx			; dx <- PSP segment
	cmp	word ptr [bp+8],0	;check memflg
	jz	exe20
	mov	es,bx			; es <- PSP segment
	mov	bx,ss
	add	bx,1000h		;add stack segment size
	sub	bx,dx			;subtract PSP
	mov	ah,4ah			;DOS setblock
	int	21h
	jc	exe30			;if error
exe20:
	mov	bx,offset cs:parmblk
	push	cs
	pop	es			; ES:BX -> parmblk
	mov	dx,si			; DS:DX -> file
	mov	ax,4B00h		;DOS exec function
	int	21h
exe30:
	cli				;interrupts off
	mov	bx,cs:ss_save		;restore ss
	mov	ss,bx
	mov	sp,cs:sp_save		;restore sp
	sti				;interrupts back on
	mov	ds,bx			;restore ds
	mov	es,bx			;restore es
	pop	bp			;restore bp
	jc	execx			;if error, ax has error #
	xor	ax,ax			;clear return value
execx:
	ret				;back to caller

ss_save dw	0			;register save area
sp_save dw	0
;
parmblk equ	$			;Parameter Block
envir	dw	0			;seg adr of environment
p_cmd	dd	0			;ptr to command line @ PSP+80h
p_fcb1	dd	fcb			;ptr to default FCB  @ PSP+5Ch
p_fcb2	dd	fcb			;ptr to default FCB  @ PSP+6Ch

fcb	db	0,11 dup(' '),4 dup(0)  ;default FCB
cmdcom	db	'A:\COMMAND.COM',0

EXEC	ENDP
PROG	ENDS
	END
                            0,11 dup(' '),4 dup(0)  ;default FCB
cmdcom	db	'A:\COMMAND.COM',0

EXEC	ENDP
PROG	ENDS
	END
                            