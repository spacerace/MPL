;
;	PASCAL:
;		function do_spawn (method: byte; 
;				   swapfname, execfname, cmdtail: string; 
;				   envlen: word; var envp)
;	C:
;		int do_spawn (unsigned char method, 
;			      char *swapfname, char *execfname, char *cmdtail,
;		              unsigned envlen, char *envp)
;
;	Assemble with
;
;	tasm  /DPASCAL spawn	  	- Turbo Pascal (Tasm only), near
;	tasm  /DPASCAL /DFARCALL spawn	- Turbo Pascal (Tasm only), far
;	?asm  spawn;		  	- C, default model (small)
;	?asm  /DMODL=large spawn  	- C, large model
;	
;	NOTE:	For C, change the 'model' directive below according to your
;		memory model, or define MODL=xxx on the command line.
;
;	The 'method' parameter determines the swap/spawn/exec
;	function:
;		00 = Spawn, don't swap
;		01 = Spawn, swap
;		80 = Exec, don't swap
;
;	If swapping (01), the following flags can be ORed into 'method':
;
;		02 = Use EMS if possible
;		04 = Use 'create temporary file' call (swapfname is path only)
;
;
;	For 'cmdtail' and 'execfname', the first byte must contain 
;	the length of the string, even when calling from C.
;
;	'swapfname' and 'execfname' must be zero terminated, even when
;	calling from Pascal.
;
	IFDEF	PASCAL
	.model	tpascal
;
ptrsize	=	1
	ELSE
	IFNDEF	MODL
	.model	small,c
	ELSE
%	.model	MODL,c
	ENDIF
;
ptrsize	=	@datasize
	ENDIF
;
stacklen	=	160		; 80 word local stack
;
blocksize	=	16 * 1024	; Write block size
blocksize_paras	=	blocksize / 16
blockshift	=	10		; shift factor for paragraphs
blockmask	=	blocksize_paras - 1	; Write block mask
;
; Method flags
;
SWAPPING	=	01h
USE_EMS		=	02h
CREAT_TEMP	=	04h
TERMINATE	=	80h
;
EMM_INT		=	67h
;
exec_block	struc
envseg	dw	?
ppar	dw	?
pparseg	dw	?
fcb1	dw	?
fcb1seg	dw	?
fcb2	dw	?
fcb2seg	dw	?
exec_block	ends
;
mcb		struc
id		db	?
owner		dw	?
paras		dw	?
		db	3 dup(?)
mcb_reserved	db	?
mcb		ends
;
next_mcbs	struc
next_addr	dw	?
next_size	dw	?
next_blocks	dw	?
next_rest	dw	?
next_mcbs	ends
;
;----------------------------------------------------------------------
;
;	The memory structure for the shrunk-down code
;
parseg	struc
parbeg	db	5ch dup(?)		; start after PSP
;
page_frame	dw	?		; EMS page frame (0 if no EMS)
memsize		dw	?		; total paragraphs
rd_blocks	dw	?		; number of blocks in swapfile
rd_rest		dw	?		; number of bytes in last block
save_ss		dw	?		; saved global ss
save_sp		dw	?		; saved global sp
spx		dw	?		; saved local sp
next_mcb	db	TYPE next_mcbs dup(?)
expar		db	TYPE exec_block dup (?) ; exec-parameter-block
zero		dw	?		; Zero command tail length (dummy)
method		db	?		; method parameter
handle		dw	?		; swap file or EMS handle (0 for EXEC)
;
parseg	ends
;
	.data
	IFDEF	PASCAL
	extrn	prefixseg: word
	ELSE
	extrn	_psp: word
	ENDIF
;
;
	.code
;
;       This part of the program code will be moved to address
;	'codeloc' and executed there.
;
;	Registers on entry:
;		BX	= paragraphs to keep
;		CX 	= length of environment to copy or zero
;		DS:SI	= environment source
;		ES:DI	= environment destination
;		(ES = our low core code segment)
;
doexec:
	jcxz	noenvcpy
	rep movsb
noenvcpy:
	push	es			; DS = ES = low core
	pop	ds
	cmp	ds:handle,0
	je	no_shrink
        mov	ah,04ah
	int     21h                     ; free memory
	mov	bx,ds:next_mcb.next_addr
;
free_loop:
	or	bx,bx
	jz	no_shrink
	mov	es,bx
	mov	bx,es:mcb_reserved.next_addr
	mov	ax,es
	inc	ax
	mov	es,ax
	mov	ah,049h
	int	21h
	jmp	free_loop
;
no_shrink:
	push	ds
	pop	es
        mov     dx,offset filename      ; params for exec
        mov     bx,offset expar
        mov     ax,04b00h
        int     21h                     ; exec
;
	mov	bx,cs
	mov	ds,bx
	mov	es,bx
	mov	ss,bx
	mov	sp,ds:spx
	cld
	push	ax
	pushf
;
	test	ds:method,TERMINATE	; exec?
	jz	exec_ckswap
	jmp	exec_term		; terminate if yes
;
exec_ckswap:
	test	ds:method,SWAPPING	; swap?
	jnz	exec_noterm		; go swap back in if yes
	jmp	exec_retn		; return if spawn and no swap
;
exec_noterm:
	mov	ah,4ah			; expand memory
	mov	bx,ds:memsize
	int	21h
	jnc	exec_memok
	jmp	exec_term		; terminate if error
;
;	Swap memory back
;
exec_memok:
	cmp	ds:page_frame,0
	jne	exec_swems
	jmp	exec_swfile		; go swap from file if not EMS
;
;	Swap in memory from EMS
;
exec_swems:
	mov	ax,cs
	dec	ax
	push	ax			; push current MCB
	push	ds			; push data segment
	mov	ax,offset next_mcb
	push	ax			; and next MCB pointer
	mov	dx,ds:handle
	mov	cx,ds:rd_blocks
	push	ds:rd_rest		; push bytes in last block
	mov	ds,ds:page_frame
	xor	bx,bx
	mov	di,swapbeg
;
swap_in_ems:
;
	jcxz	swin_ems_rest
;
swin_ems:
	push	cx
	mov	ax,4400h
	int	EMM_INT
	or	ah,ah
	jnz	exec_term
	mov	cx,blocksize
	mov	si,0
	push	di
	rep movsb
	pop	di
	mov	ax,es
	add	ax,blocksize_paras
	mov	es,ax
	pop	cx
	inc	bx
	loop	swin_ems
;
swin_ems_rest:
	pop	cx
	jcxz	swin_ems_rdy
	mov	ax,4400h
	int	EMM_INT
	or	ah,ah
	jnz	exec_term
	mov	si,0
	rep movsb
	inc	bx
;
swin_ems_rdy:
	pop	si			; next offset
	pop	ds			; segment
	pop	es			; current MCB
	mov	ax,[si].next_addr
	or	ax,ax
	jz	swin_ems_complete
	push	ax			; next MCB
	push	ax			; also is data segment for next struc
	mov	cx,offset mcb_reserved
	push	cx			; next offset
	mov	ax,[si].next_rest
	push	ax			; bytes in last block
;
	push	bx
	push	dx
	call	get_mcb			; alloc MCB, ES = dest segment
	pop	dx
	pop	bx
	mov	cx,[si].next_blocks	; number of blocks
	mov	ds,cs:page_frame	; reload page frame addr
	mov	di,0
	jmp	swap_in_ems
;
swin_ems_complete:
	mov	ah,45h
	int	EMM_INT
	mov	ax,cs
	mov	es,ax
	mov	ds,ax
	jmp	short exec_retn
;
exec_term:
	cmp	cs:page_frame,0
	je	eterm_noems
	mov	dx,cs:handle
	mov	ah,45h
	int	EMM_INT
eterm_noems:
	mov	ax,4c00h                ; terminate process
        int     21h
;
;	Swap in memory from file
;
exec_swfile:
	mov	ax,cs
	dec	ax
	push	ax			; push current MCB
	push	ds			; push data segment
	mov	ax,offset next_mcb
	push	ax			; and next MCB pointer
	push	ds:rd_rest		; push bytes in last block
	mov	bx,ds:handle
	mov	cx,blocksize
	mov	dx,swapbeg
	mov	si,ds:rd_blocks
;
exec_rdblocks:
	mov	ah,3fh			; read file
	cmp	si,0
	je	exec_rdr
	dec	si
	int	21h
	jc	exec_term		; terminate if error reading
	mov	ax,ds
	add	ax,blocksize_paras
	mov	ds,ax
	jmp	exec_rdblocks
;
exec_rdr:
	pop	cx			; bytes in last block
	jcxz	exec_norest
	int	21h
	jc	exec_term		; terminate if error reading
;
exec_norest:
	pop	si			; next offset
	pop	ds			; segment
	pop	es			; current MCB
	mov	ax,[si].next_addr
	or	ax,ax
	jz	swin_file_complete
	push	ax			; next MCB
	push	ax			; also is data segment for next struc
	mov	cx,offset mcb_reserved
	push	cx			; next offset
	mov	ax,[si].next_rest
	push	ax			; bytes in last block
;
	push	bx
	call	get_mcb			; alloc MCB, ES = dest segment
	pop	bx
	mov	si,[si].next_blocks	; number of blocks
	mov	ax,es
	mov	ds,ax
	mov	cx,blocksize
	mov	dx,0
	jmp	exec_rdblocks
;
swin_file_complete:
	mov	ah,3eh
	int	21h			; close file
	mov	ax,cs
	mov	ds,ax
	mov	es,ax
;
exec_retn:
	popf
	pop	ax
	jc	exec_fault		; return EXEC error code if fault
	mov	ah,4dh			; else get program return code
	int	21h
;	ret	far			; can't use a RET here since
	db	0cbh			; we are using .model
;
exec_fault:
	mov	ah,3			; return error as 03xx
;	ret	far
	db	0cbh
;
;
;	get_mcb allocates a block of memory by modifying the MCB chain
;	directly.
;
;	On entry, DS:SI points to the next_mcbs descriptor for the block,
;		  ES:0  is the MCB for the current block.
;
;	On exit,  ES is the wanted MCB.
;
;	Uses 	AX, BX, CX
;
get_mcb	proc	near
; 	
	cmp	es:id,4dh		; 4d means normal MCB
	jnz	gmcb_abort		; halt if no next
	mov	ax,es			; current MCB
	add	ax,es:paras
	inc	ax
	mov	es,ax			; next MCB
	cmp	ax,[si].next_addr
	ja	gmcb_abort		; halt if next MCB > wanted
	je	mcb_found		; jump if same addr as wanted
	add	ax,es:paras		; last addr
	cmp	ax,[si].next_addr
	jb	get_mcb			; loop if last < wanted
	cmp	es:owner,0
	jne	gmcb_abort		; halt if not free
;
;	The wanted MCB starts within the current MCB. We now have to
;	create a new MCB at the wanted position, which is initially
;	free, and shorten the current MCB to reflect the reduced size.
;
	mov	bx,es			; current
	inc	bx			; + 1 (header doesn't count)
	mov	ax,[si].next_addr
	sub	ax,bx			; paragraphs between MCB and wanted
	mov	bx,es:paras		; paras in current MCB
	sub	bx,ax			; remaining paras
	dec	bx			; -1 for header
	mov	es:paras,ax		; set new size for current
	mov	cl,es:id		; old id
	mov	es:id,4dh		; set id: there is a next
	mov	ax,[si].next_addr	; now point to new MCB
	mov	es,ax
	mov	es:id,cl		; and init to free
	mov	es:owner,0
	mov	es:paras,bx
;
;	We have found an MCB at the right address. If it's not free,
;	abort. Else check the size. If the size is ok, we're done 
;	(more or less).
;
mcb_found:
	mov	es,ax
	cmp	es:owner,0
	je	mcb_check		; continue if free
;
gmcb_abort:
	jmp	exec_term
;
mcb_check:
	mov	ax,es:paras		; size
	cmp	ax,[si].next_size	; size needed
	jae	mcb_ok			; ok if enough space
;
;	If there's not enough room in this MCB, check if the next
;	MCB is free, too. If so, coalesce both MCB's and check again.
;
	cmp	es:id,4dh
	jnz	gmcb_abort		; halt if no next
	push	es			; save current
	mov	bx,es
	add	ax,bx
	inc	ax			; next MCB
	mov	es,ax
	cmp	es:owner,0		; next free ?
	jne	gmcb_abort		; halt if not
	mov	ax,es:paras		; else load size
	inc	ax			; + 1 for header
	mov	cl,es:id		; and load ID
	pop	es			; back to last MCB
	add	es:paras,ax		; increase size
	mov	es:id,cl		; and store ID
	jmp	mcb_check		; now try again
;
;	The MCB is free and large enough. If it's larger than the
;	wanted size, create another MCB after the wanted.
;
mcb_ok:
	mov	bx,es:paras
	sub	bx,[si].next_size
	jz	mcb_no_next		; ok, no next to create
	push	es
	dec	bx			; size of next block
	mov	ax,es
	add	ax,[si].next_size
	inc	ax			; next MCB addr
	mov	cl,es:id		; id of this block
	mov	es,ax			; address next
	mov	es:id,cl		; store id
	mov	es:paras,bx		; store size
	mov	es:owner,0		; and mark as free
	pop	es			; back to old MCB
	mov	es:id,4dh		; mark next block present
	mov	ax,[si].next_size	; and set size to wanted
	mov	es:paras,ax
;
mcb_no_next:
	mov	es:owner,cx		; set owner to current PSP
	ret				; all finished (whew!)
;
get_mcb	endp
;
ireti:
	iret
;
iretoff	=	offset ireti - offset doexec
;
execlen	=	$-doexec
;
;--------------------------------------------------------------------
;
parseg2	struc
		db	TYPE parseg dup(?)
;
codeloc		db	execlen dup(?)	; code
;	
div0_off	dw	?		; divide by zero vector save
div0_seg	dw	?
xfcb1		db	16 dup(?)	; default FCB
xfcb2		db	16 dup(?)	; default FCB
filename	db	66 dup(?)	; exec filename
progpars	db	128 dup(?)	; command tail
		db	stacklen dup(?)	; stack space
;
parseg2	ends
;
mystack		equ	progpars+128+stacklen
parend		equ	mystack
;
reslen	= (offset parend - offset parbeg)
;
keep_paras	= (reslen + 15) shr 4	; paragraphs to keep
swapbeg		= keep_paras shl 4		; start of swap space
savespace	= swapbeg - 5ch		; length of overwritten area
;
;	Space for saving the part of the memory image below the
;	swap area that is overwritten by our code.
;
	.data
save_dat	db	savespace dup(?)
;       
	.code
;
emm_name	db	'EMMXXXX0'
;
	IFDEF	PASCAL
	IFDEF	FARCALL
do_spawn	PROC	far pmethod: byte, swapfname: dword, execfname: dword, params: dword, envlen: word, envp: dword
	ELSE
do_spawn	PROC	near pmethod: byte, swapfname: dword, execfname: dword, params: dword, envlen: word, envp: dword
	ENDIF
	ELSE
do_spawn	PROC	uses si di,pmethod:byte,swapfname:ptr byte,execfname:ptr byte,params:ptr byte,envlen:word,envp:ptr byte
	ENDIF
;
	public	do_spawn
;
	push	ds
	mov	ax,ds
	mov	es,ax
	cld
;
	IFDEF	PASCAL
	mov	ax,prefixseg
	ELSE
	mov	ax,_psp
	ENDIF
	mov	ds,ax			; DS points to PSP
;
;	Save the memory below the swap space
;
	mov	si,5ch
	mov	di,offset save_dat
	mov	cx,savespace
	rep movsb
;
	mov	es,ax			; ES points to PSP
	dec	ax
	mov	ds,ax			; DS now points to MEM CTL
	mov	bx,word ptr ds:3	; allocated paragraphs
	mov	es:memsize,bx
;
;	Now walk the chain of memory blocks, chaining all blocks
;	belonging to this process. Four unused words in the MCB are
;	used to store the paragraph address and size of the next block.
;
	push	es
	mov	bx,ds:owner		; the current process
	mov	di,offset next_mcb
;
mcb_chain:
	cmp	ds:id,4dh		; normal block?
	jne	mcb_ready		; ready if end
	mov	cx,ds
	add	cx,ds:paras		; start + length
	inc	cx			; next MCB
	mov	ds,cx
	cmp	bx,ds:owner		; our process?
	jne	mcb_chain		; loop if not
	mov	es:[di].next_addr,ds
	mov	ax,ds:paras
	mov	es:[di].next_size,ax
	inc	ax
	push	ax
	mov	cl,blockshift
	shr	ax,cl			; number of blocks
	mov	es:[di].next_blocks,ax
	pop	ax
	and	ax,blockmask
	mov	es:[di].next_rest,ax
	mov	ax,ds
	mov	es,ax
	mov	di,offset mcb_reserved
	jmp	mcb_chain
;
mcb_ready:
	mov	es:[di].next_addr,0
	pop	es
;
	mov	es:page_frame,0
;
;	Swap out memory
;
	mov	al,pmethod
	mov	es:method,al
	test	al,SWAPPING
	jnz	do_swap
	jmp	no_swap
;
do_swap:
	mov	bx,es:memsize
	sub	bx,keep_paras		; minus resident paragraphs
	mov	ax,bx
	mov	cl,blockshift
	shr	ax,cl			; number of blocks in swapfile
	mov	es:rd_blocks,ax
	xchg	ax,bx
	and	ax,blockmask
	mov	cl,4
	shl	ax,cl			; number of bytes in last block
	mov	es:rd_rest,ax
;
;	Check for EMS swap
;
	test	pmethod,USE_EMS
	jnz	try_ems
	jmp	no_ems
;
try_ems:
	or	ax,ax
	jz	no_rest
	inc	bx			; number of EMS blocks needed
;
;	For EMS, we have to determine the total number of blocks
;	for all memory blocks.
;
no_rest:
	mov	si,offset next_mcb
	push	es
	pop	ds
;
tot_blocks:
	mov	cx,[si].next_addr
	or	cx,cx
	jz	tot_ready
	add	bx,[si].next_blocks
	mov	ax,[si].next_rest
	mov	ds,cx
	mov	si,offset mcb_reserved
	or	ax,ax
	jz	tot_blocks
	inc	bx
	jmp	tot_blocks
;
tot_ready:
	push	bx
	push	es
	mov	al,EMM_INT
	mov	ah,35h
	int	21h			; get EMM int vector
	mov	ax,cs
	mov	ds,ax
	mov	si,offset emm_name
	mov	di,10
	mov	cx,8
	repz cmpsb
	pop	es
	pop	bx
	jz	ems_ok_1
	jmp	no_ems
;
ems_ok_1:
	mov	ah,40h
	int	EMM_INT
	or	ah,ah
	jz	ems_ok_2
	jmp	no_ems
;
ems_ok_2:
	mov	ah,46h
	int	EMM_INT
	or	ah,ah
	jz	ems_ok_3
	jmp	no_ems
;
ems_ok_3:
	cmp	al,30h
	jae	ems_ok_4
	jmp	no_ems
;
ems_ok_4:
	push	bx
	mov	ah,41h
	int	EMM_INT
	mov	es:page_frame,bx
	pop	bx
	or	ah,ah
	jz	ems_ok_5
	jmp	no_ems
;
;	EMS present, try to allocate pages
;
ems_ok_5:
	mov	ah,43h
	int	EMM_INT
	or	ah,ah
	jz	ems_ok_6
	jmp	no_ems
;
;	EMS pages allocated, swap to EMS
;
ems_ok_6:
	mov	es:handle,dx
	push	es
	mov	ax,es
	mov	ds,ax
	push	ds
	mov	ax,offset next_mcb
	push	ax
	push	es:rd_rest
	mov	cx,es:rd_blocks
	mov	es,es:page_frame
	xor	bx,bx
	mov	si,swapbeg
;
swap_ems:
	jcxz	swapout_erest		; jump if no full blocks
;
swapout_ems:
	push	cx
	mov	ax,4400h			; map page, phys = 0
	int	EMM_INT
	or	ah,ah
	jnz	ems_error
	mov	cx,blocksize
	mov	di,0
	push	si
	rep movsb
	pop	si
	mov	ax,ds
	add	ax,blocksize_paras
	mov	ds,ax
	pop	cx
	inc	bx
	loop	swapout_ems
;
swapout_erest:
	pop	cx			; remaining bytes
	push	cx
	push	cx
	jcxz	swapout_erdy
	mov	ax,4400h		; map page, phys = 0
	int	EMM_INT
	or	ah,ah
	jnz	ems_error
	mov	di,0
	rep movsb
	inc	bx
;
swapout_erdy:
	add	sp,4
	pop	si			; next offset
	pop	ds			; segment
;
	mov	ax,[si].next_addr
	or	ax,ax
	jz	ems_complete
	push	ax
	mov	cx,offset mcb_reserved
	push	cx
	mov	cx,[si].next_blocks
	mov	si,[si].next_rest
	push	si
	mov	si,0
	mov	ds,ax
	jmp	swap_ems
;
ems_complete:
	pop	es
	jmp	no_swap
;
ems_error:
	add	sp,8
	pop	es
	mov	ah,45h			; release EMS pages on error
	int	EMM_INT
;
;	No or not enough EMS storage, swap to file
;
no_ems:
	mov	es:page_frame,0
	IF	ptrsize
	lds	dx,swapfname
	ELSE
	pop	ds
	push	ds
	mov	dx,swapfname
	ENDIF
	inc	dx
	mov	cx,2			; hidden
	mov	ah,3ch			; create file
	test	pmethod,CREAT_TEMP
	jz	no_temp
	mov	ah,5ah
;
no_temp:
	int	21h
	jc	spawn_error
	mov	es:handle,ax
	mov	bx,ax
;
	mov	ax,es
	mov	ds,ax
	push	ds
	mov	si,offset next_mcb
	mov	cx,es:rd_blocks
	mov	di,es:rd_rest
	mov	dx,swapbeg
;
swap_file:
	jcxz	swout_rest
;
swout_blocks:
	push	cx
	mov	cx,blocksize
	mov	ah,40h
	int	21h
	pop	cx
	jc	spawn_error
	mov	ax,ds
	add	ax,blocksize_paras
	mov	ds,ax
	loop	swout_blocks
;
swout_rest:
	mov	cx,di
	jcxz	swap_ready
	mov	ah,40h
	int	21h
	jnc	swap_ready
;
spawn_error:
	add	sp,2
	pop	ds
	mov	ax,100h
	ret
;
swap_ready:
	pop	ds
	mov	ax,[si].next_addr
	or	ax,ax
	jz	swap_complete
	mov	cx,[si].next_blocks
	mov	di,[si].next_rest
	mov	dx,0
	mov	si,offset mcb_reserved
	mov	ds,ax
	push	ds
	jmp	swap_file
;
;	Swapout complete
;
swap_complete:
	mov	ax,4200h
	xor	cx,cx
	mov	dx,cx
	int	21h			; rewind file
;
no_swap:
;
;	Prepare exec parameter block
;
	mov	ax,es
	mov	es:expar.fcb1seg,ax
	mov	es:expar.fcb2seg,ax
	mov	es:expar.pparseg,ax
	mov	es:expar.envseg,0
;
;	The 'zero' word is located at 80h in the PSP, the start of
;	the command line. So as not to confuse MCB walking programs,
;	a command line length of zero is inserted here.
;
	mov	es:zero,0d00h		; 00h,0dh = empty command line
;
;	Init default fcb's by parsing parameter string
;
	IF	ptrsize
	lds	si,params
	ELSE
	pop	ds
	push	ds
	mov	si,params
	ENDIF
	push	si
	mov	di,offset xfcb1
	mov	es:expar.fcb1,di
	push	di
	mov	cx,16
	xor	ax,ax
	rep stosw			; init both fcb's to 0
	pop	di
	mov	ax,2901h
	IFDEF	PASCAL
	inc	si
	ENDIF
	int	21h
	mov	di,offset xfcb2
	mov	es:expar.fcb2,di
	mov	ax,2901h
	int	21h
	pop	si
;
;	move command tail string into low core
;
	mov	cl,byte ptr [si]
	xor	ch,ch
	mov	di,offset progpars
	mov	es:expar.ppar,di
	inc	cx
	rep movsb
	mov	al,0dh
	stosb
;
;	move filename string into low core
;
	IF	ptrsize
	lds	si,execfname
	ELSE
	mov	si,execfname
	ENDIF
	lodsb
	mov	cl,al
	xor	ch,ch
	mov	di,offset filename
	rep movsb
	xor	al,al
	stosb
;
;	Setup environment copy
;
	mov	bx,keep_paras		; paras to keep
	mov	cx,envlen		; environment size
	jcxz	no_environ		; go jump if no environment
	mov	ax,cx			; convert envsize to paras
	add	ax,15
	shr	ax,1
	shr	ax,1
	shr	ax,1
	shr	ax,1
	add	bx,ax			; add envsize to paras to keep
	IF	ptrsize
	lds	si,envp
	ELSE
	mov	si,envp
	ENDIF
;
	mov	ax,es			; low core segment
	add	ax,keep_paras		; plus fixed paras
	mov	es:expar.envseg,ax	; = new environment segment
;
;	Save stack regs, switch to local stack
;
no_environ:
	mov	es:save_ss,ss
	mov	es:save_sp,sp
	mov	ax,es
	mov	ss,ax
	mov	sp,offset mystack
;
	push	cx
	push	si
	push	ds
;
;	Move code into low core
;
	mov	di,offset codeloc
	call	near ptr nextloc        ; where are we ?
nextloc:
	pop	si			; si = abs. addr of 'nextloc'
	push	si
        add     si,doexec-nextloc
        mov     cx,execlen
	push	cs
	pop	ds
        rep movsb                       ; move down code
;
;	save and patch INT0 (division by zero) vector
;
	xor	ax,ax
	mov	ds,ax
	mov	ax,word ptr ds:0
	mov	es:div0_off,ax
	mov	ax,word ptr ds:2
	mov	es:div0_seg,ax
	mov	word ptr ds:0,offset codeloc + iretoff
	mov	word ptr ds:2,es
;
;	Push return address on local stack
;
	pop	ax
	add	ax,exec_cont-nextloc
;
	pop	ds			; pop environment segment
	pop	si			; pop environment offset
	pop	cx			; pop environment length
	mov	di,swapbeg		; pop environment destination
;
	push	cs			; push return segment
	push	ax			; push return offset
	mov	es:spx,sp		; save stack pointer
;
;	Goto low core code
;
	push	es			; push entry segment
        mov	ax,offset codeloc
        push	ax			; push entry offset
;	ret	far			; can't use RET here because
	db	0cbh			; of .model
;
;----------------------------------------------------------------
;
;	low core code will return to this location
;
exec_cont:
	mov	ss,es:save_ss		; reload stack
	mov	sp,es:save_sp
;
;	restore INT0 (division by zero) vector
;
	xor	cx,cx
	mov	ds,cx
	mov	cx,es:div0_off
	mov	word ptr ds:0,cx
	mov	cx,es:div0_seg
	mov	word ptr ds:2,cx
;
	pop	ds
;
	mov	bx,es:page_frame	; save EMS status in BX
;
;	Restore overwritten part of program
;
	mov	si,offset save_dat
	mov	di,5ch
	mov	cx,savespace
	rep movsb
;
	or	bx,bx			; EMS swap?
	jnz	exec_finish		; ready if yes
;
	push	ds			; else delete swapfile
	push	ax
	IF	ptrsize
	lds	dx,swapfname
	ELSE
	mov	dx,swapfname
	ENDIF
	inc	dx
;
	mov	ah,41h
	int	21h			; delete file
	pop	ax
	pop	ds
;
exec_finish:
	ret
;	
do_spawn	ENDP
;
        END


