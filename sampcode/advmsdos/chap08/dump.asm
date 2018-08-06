        title   DUMP --- display file contents
        page    55,132

; 
;  DUMP --- Display contents of file in hex and ASCII
;
;  Build:   C>MASM DUMP;
;           C>LINK DUMP;
; 
;  Usage:   C>DUMP unit:\path\filename.exe [ >device ]
; 
;  Copyright 1988 Ray Duncan
; 

cr      equ     0dh             ; ASCII carriage return
lf      equ     0ah             ; ASCII line feed
tab	equ	09h		; ASCII tab code
blank	equ	20h		; ASCII space code

cmd 	equ     80h             ; buffer for command tail

blksize equ     16		; input file record size

stdin	equ	0		; standard input handle
stdout 	equ 	1             	; standard output handle
stderr 	equ 	2		; standard error handle


_TEXT	segment	word public 'CODE'

        assume  cs:_TEXT,ds:_DATA,es:_DATA,ss:STACK


dump    proc    far     	; entry point from MS-DOS

        push    ds              ; save DS:0000 for final
        xor     ax,ax           ; return to MS-DOS, in case
        push    ax		; Function 4CH can't be used.	

        mov     ax,_DATA        ; make our data segment
        mov     ds,ax           ; addressable via DS register.

				; check MS-DOS version
        mov     ax,3000h	; Fxn 30H = get version
        int     21h		; transfer to MS-DOS
        cmp     al,2		; major version 2 or later?
        jae     dump1           ; yes, proceed

				; if MS-DOS 1.x, display
				; error message and exit
        mov     dx,offset msg3  ; DS:DX = message address
	mov	ah,9		; Fxn 9 = print string
	int	21h		; transfer to MS-DOS
	ret			; then exit the old way

dump1:  			; check if filename present
	mov	bx,offset cmd	; ES:BX = command tail
	call	argc		; count command arguments
	cmp	ax,2		; are there 2 arguments?
	je	dump2		; yes, proceed

				; missing filename, display
				; error message and exit
        mov     dx,offset msg2  ; DS:DX = message address
	mov	cx,msg2_len	; CX = message length
        jmp     dump9           ; go display it

dump2:				; get address of filename
	mov	ax,1		; AX = argument number
				; ES:BX still = command tail
	call	argv		; returns ES:BX = address,
				; and AX = length

	mov	di,offset fname	; copy filename to buffer
	mov	cx,ax		; CX = length

dump3:	mov	al,es:[bx]	; copy one byte
	mov	[di],al
	inc	bx    		; bump string pointers
	inc	di
	loop	dump3		; loop until string done
	mov	byte ptr [di],0	; add terminal null byte

	mov	ax,ds		; make our data segment
	mov	es,ax		; addressable by ES too

				; now open the file
	mov	ax,3d00h	; Fxn 3DH = open file
				; mode 0 = read only
	mov	dx,offset fname	; DS:DX = filename
	int	21h		; transfer to MS-DOS
	jnc	dump4		; jump, open successful

				; open failed, display
				; error message and exit
	mov	dx,offset msg1	; DS:DX = message address
	mov	cx,msg1_len	; CX = message length
	jmp	dump9		; go display it

dump4:	mov	fhandle,ax	; save file handle

dump5:  			; read block of file data
	mov	bx,fhandle	; BX = file handle
	mov	cx,blksize	; CX = record length
	mov	dx,offset fbuff	; DS:DX = buffer
	mov	ah,3fh		; Fxn 3FH = read
	int	21h		; transfer to MS-DOS

	mov	flen,ax		; save actual length
	cmp	ax,0		; end of file reached?
	jne	dump6		; no, proceed

	cmp	word ptr fptr,0	; was this the first read?
	jne	dump8		; no, exit normally

				; display empty file
				; message and exit
	mov	dx,offset msg4	; DS:DX = message address
	mov	cx,msg4_len	; CX = length
	jmp	dump9		; go display it

dump6:				; display heading at 
				; each 128 byte boundary
	test	fptr,07fh	; time for a heading?
	jnz	dump7		; no, proceed

				; display a heading	
	mov	dx,offset hdg	; DS:DX = heading address
	mov	cx,hdg_len	; CX = heading length
	mov	bx,stdout	; BX = standard output
	mov	ah,40h		; Fxn 40H = write
	int	21h		; transfer to MS-DOS

dump7:	call	conv		; convert binary record
				; to formatted ASCII
	
				; display formatted output
	mov	dx,offset fout	; DX:DX = output address
	mov	cx,fout_len	; CX = output length
	mov	bx,stdout	; BX = standard output
	mov	ah,40h		; Fxn 40H = write
	int	21h		; transfer to MS-DOS

	jmp	dump5		; go get another record

dump8:				; close input file
	mov	bx,fhandle	; BX = file handle
	mov	ah,3eh		; Fxn 3EH = close
	int	21h		; transfer to MS-DOS

	mov	ax,4c00h	; Fxn 4CH = terminate,
				; return code = 0
	int	21h		; transfer to MS-DOS

dump9:                          ; display message on
				; standard error device
				; DS:DX = message address
				; CX = message length
	mov	bx,stderr	; standard error handle
	mov	ah,40h		; Fxn 40H = write
	int	21h		; transfer to MS-DOS

	mov	ax,4c01h	; Fxn 4CH = terminate,
				; return code = 1
	int	21h		; transfer to MS-DOS
	
dump    endp


conv 	proc	near		; convert block of data
				; from input file

	mov	di,offset fout 	; clear output format 
	mov	cx,fout_len-2	; area to blanks
	mov	al,blank
	rep stosb

	mov	di,offset fout 	; convert file offset
	mov	ax,fptr		; to ASCII for output
	call	w2a

	mov	bx,0		; init. buffer pointer

conv1:	mov	al,[fbuff+bx]	; fetch byte from buffer
	mov	di,offset foutb	; point to output area
			
				; format ASCII part...
				; store '.' as default
	mov	byte ptr [di+bx],'.'

	cmp	al,blank	; in range 20H - 7EH?
	jb	conv2		; jump, not alphanumeric.

	cmp	al,7eh		; in range 20H - 7EH?
	ja	conv2		; jump, not alphanumeric.

	mov	[di+bx],al	; store ASCII character.

conv2:				; format hex part...
	mov	di,offset fouta ; point to output area
	add	di,bx		; base addr + (offset*3)
	add	di,bx
	add	di,bx
	call	b2a		; convert byte to hex 

	inc	bx		; advance through record
	cmp	bx,flen		; entire record converted?
	jne	conv1		; no, get another byte

				; update file pointer
	add	word ptr fptr,blksize

	ret

conv	endp


w2a	proc	near		; convert word to hex ASCII
				; call with AX = value
				;           DI = addr for string
				; returns AX, DI, CX destroyed

	push	ax		; save copy of value		
	mov	al,ah
	call	b2a		; convert upper byte	

	pop	ax		; get back copy
	call	b2a		; convert lower byte
	ret

w2a	endp


b2a	proc	near          	; convert byte to hex ASCII
                                ; call with AL=binary value
				;           DI=addr for string
                                ; returns   AX, DI, CX modified 

	sub	ah,ah		; clear upper byte
	mov	cl,16
	div	cl		; divide byte by 16
	call	ascii		; quotient becomes the first
	stosb			; ASCII character
	mov	al,ah
	call	ascii		; remainder becomes the
	stosb			; second ASCII character
	ret

b2a	endp


ascii   proc 	near		; convert value 0-0FH in AL 
				; into "hex ASCII" character
		
        add     al,'0'		; offset to range 0-9
        cmp     al,'9'		; is it > 9?
        jle     ascii2		; no, jump
        add     al,'A'-'9'-1	; offset to range A-F,

ascii2:	ret			; return AL = ASCII char.

ascii	endp


argc	proc	near		; count command line arguments
				; call with ES:BX = command line
				; returns   AX = argument count

	push	bx		; save original BX and CX 
	push	cx		;  for later
	mov	ax,1		; force count >= 1

argc1:	mov	cx,-1		; set flag = outside argument

argc2:	inc	bx		; point to next character 
	cmp	byte ptr es:[bx],cr
	je	argc3		; exit if carriage return
	cmp	byte ptr es:[bx],blank
	je	argc1		; outside argument if ASCII blank
	cmp	byte ptr es:[bx],tab	
	je	argc1		; outside argument if ASCII tab

				; otherwise not blank or tab,
	jcxz	argc2		; jump if already inside argument

	inc	ax		; else found argument, count it
	not	cx		; set flag = inside argument
	jmp	argc2		; and look at next character

argc3:	pop	cx		; restore original BX and CX
	pop	bx
	ret			; return AX = argument count

argc	endp


argv	proc	near		; get address & length of
				; command line argument
				; call with ES:BX = command line
				;           AX    = argument no.
				; returns   ES:BX = address
				;           AX    = length

	push	cx		; save original CX and DI 
	push	di

	or	ax,ax		; is it argument 0?
	jz	argv8		; yes, jump to get program name

	xor	ah,ah		; initialize argument counter

argv1:	mov	cx,-1		; set flag = outside argument

argv2:	inc	bx		; point to next character 
	cmp	byte ptr es:[bx],cr
	je	argv7		; exit if carriage return
	cmp	byte ptr es:[bx],blank
	je	argv1		; outside argument if ASCII blank
	cmp	byte ptr es:[bx],tab	
	je	argv1		; outside argument if ASCII tab

				; if not blank or tab...
	jcxz	argv2		; jump if already inside argument

	inc	ah		; else count arguments found
	cmp	ah,al		; is this the one we're looking for?
	je	argv4		; yes, go find its length
	not	cx		; no, set flag = inside argument
	jmp	argv2		; and look at next character

argv4:				; found desired argument, now
				; determine its length...
	mov	ax,bx		; save param. starting address 

argv5:	inc	bx		; point to next character
	cmp	byte ptr es:[bx],cr
	je	argv6		; found end if carriage return
	cmp	byte ptr es:[bx],blank
	je	argv6		; found end if ASCII blank
	cmp	byte ptr es:[bx],tab	
	jne	argv5		; found end if ASCII tab

argv6:	xchg	bx,ax		; set ES:BX = argument address
	sub	ax,bx		; and AX = argument length
	jmp	argvx		; return to caller

argv7:	xor	ax,ax		; set AX = 0, argument not found
	jmp	argvx		; return to caller

argv8:				; special handling for argv=0
	mov	ax,3000h	; check if DOS 3.0 or later
	int	21h		; (force AL=0 in case DOS 1)
	cmp	al,3
	jb	argv7		; DOS 1 or 2, return null param.
	mov	es,es:[2ch]	; get environment segment from PSP
	xor	di,di		; find the program name by
	xor	al,al		; first skipping over all the
	mov	cx,-1		; environment variables...
	cld
argv9:	repne scasb		; scan for double null (can't use
	scasb			; (SCASW since might be odd addr.)
	jne	argv9		; loop if it was a single null
	add	di,2		; skip count word in environment
	mov	bx,di		; save program name address
	mov	cx,-1		; now find its length... 
	repne scasb		; scan for another null byte
	not	cx		; convert CX to length 
	dec	cx
	mov	ax,cx		; return length in AX

argvx:				; common exit point
	pop	di		; restore original CX and DI
	pop	cx
	ret			; return to caller

argv	endp

_TEXT    ends


_DATA   segment word public 'DATA'

fname 	db	64 dup (0)      ; buffer for input filespec

fhandle	dw	0               ; token from PCDOS for input file.

flen	dw	0		; actual length read

fptr	dw	0		; relative address in file 

fbuff	db	blksize dup (?)	; data from input file

fout	db	'nnnn'		; formatted output area
	db	blank,blank
fouta	db	16 dup ('nn',blank)
	db	blank
foutb	db	16 dup (blank),cr,lf
fout_len equ	$-fout

hdg	db	cr,lf		; heading for each 128 bytes
	db	7 dup (blank)	; of formatted output
	db	'0  1  2  3  4  5  6  7  '
	db	'8  9  A  B  C  D  E  F',cr,lf
hdg_len equ	$-hdg


msg1    db	cr,lf
        db      'dump: file not found'
	db	cr,lf
msg1_len equ	$-msg1

msg2	db      cr,lf
	db	'dump: missing file name'
	db	cr,lf
msg2_len equ	$-msg2

msg3	db      cr,lf
	db      'dump: wrong MS-DOS version'
	db      cr,lf,'$'

msg4	db	cr,lf
	db	'dump: empty file'
	db	cr,lf
msg4_len equ	$-msg4

_DATA 	ends    


STACK	segment para stack 'STACK'

        db      64 dup (?)

STACK	ends

        end     dump

