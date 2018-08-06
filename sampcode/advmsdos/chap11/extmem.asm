         title     EXTMEM --- get/put extended memory
         page      55,132

;
; EXTMEM.ASM --- Demonstration of access to
;                extended memory using ROM BIOS
;                Int 15H Function 87H
;
; Copyright (C) June 1988 Ray Duncan
;
; Build:   MASM EXTMEM;
;	   LINK EXTMEM;
;
; Usage:   EXTMEM
; 

stdin	equ	0		; standard input device
stdout	equ	1		; standard output device	
stderr	equ	2		; standard error device

cr	equ	0dh		; ASCII carriage return
lf	equ	0ah		; ASCII line feed


DGROUP	group	_DATA,STACK	; 'automatic data group'


_DATA	segment	word public 'DATA'

bmdt	db	30h dup (0)	; block move descriptor table	

buff1	db	80h dup ('?')	; source buffer
buff2	db	80h dup (0)	; destination buffer

_DATA	ends


STACK	segment	para stack 'STACK'

	dw	64 dup (?)

STACK	ends


_TEXT	segment	word public 'CODE'

	assume	cs:_TEXT,ds:DGROUP,ss:STACK

main	proc	far       	; entry point from MS-DOS

	mov	ax,DGROUP	; set DS = our data segment
	mov	ds,ax
	mov	es,ax

				; copy 'buff1' to extended
				; memory address 100000H
	mov	dx,10h		; DX:AX = destination
	mov	ax,0		; extended mem. address
	mov	bx,seg buff1	; DS:BX = source conv.
	mov	ds,bx		; memory address
	mov	bx,offset buff1
	mov	cx,80h		; CX = bytes to move
	mov	si,seg bmdt	; ES:SI = block move 
	mov	es,si		; descriptor table
	mov	si,offset bmdt
	call	putblk		; request transfer


				; fill buff2 from extended
				; memory address 100000H
	mov	dx,10h		; DX:AX = source extended 
	mov	ax,0		; memory address
	mov	bx,seg buff2	; DS:BX = destination 
	mov	ds,bx		; conventional mem. address
	mov	bx,offset buff2
	mov	cx,80h		; CX = bytes to move
	mov	si,seg bmdt	; ES:SI = block move
	mov	es,si		; descriptor table
	mov	si,offset bmdt
	call	getblk		; request transfer

	mov	ax,4c00h	; no error, terminate program 
	int	21h		; with return code = 0

main2:	mov	ax,4c01h	; error, terminate program
	int	21h		; with return code = 1 

main	endp			; end of main procedure




getblk	proc	near		; Transfer block from extended
				;   memory to real memory
				; Call with 
				; DX:AX = linear 32-bit 
				;         extended memory address
				; DS:BX = segment and offset
				;         destination address
				; CX    = length in bytes
				; ES:SI = block move descriptor table
				; Returns
				; AH    = 0 if transfer OK

	mov	es:[si+10h],cx	; store length into descriptors
	mov	es:[si+18h],cx 

				; store access rights bytes
	mov	byte ptr es:[si+15h],93h
	mov	byte ptr es:[si+1dh],93h

	mov	es:[si+12h],ax	; source extended memory address
	mov	es:[si+14h],dl

				; convert destination segment
				; and offset to linear address
	mov	ax,ds		; segment * 16
	mov	dx,16
	mul	dx
	add	ax,bx		; + offset -> linear address
	adc	dx,0

	mov	es:[si+1ah],ax	; store destination address
	mov	es:[si+1ch],dl

	shr	cx,1		; convert length to words
	mov	ah,87h		; Int 15H Fxn 87h = block move
	int	15h		; transfer to ROM BIOS 

	ret			; back to caller

getblk	endp


putblk	proc	near		; Transfer block from real 
				;   memory to extended memory
				; Call with 
				; DX:AX = linear 32-bit
				;         extended memory address
				; DS:BX = segment and offset
				;         source address
				; CX    = length in bytes
				; ES:SI = block move descriptor table
				; Returns
				; AH    = 0 if transfer OK

	mov	es:[si+10h],cx	; store length into descriptors
	mov	es:[si+18h],cx 

				; store access rights bytes
	mov	byte ptr es:[si+15h],93h
	mov	byte ptr es:[si+1dh],93h

	mov	es:[si+1ah],ax	; store destination extended 
	mov	es:[si+1ch],dl	; memory address

				; convert source segment and
				; offset to linear address
	mov	ax,ds		; segment * 16
	mov	dx,16
	mul	dx
	add	ax,bx		; + offset -> linear address
	adc	dx,0

	mov	es:[si+12h],ax	; store source address
	mov	es:[si+14h],dl

	shr	cx,1		; convert length to words
	mov	ah,87h		; Int 15H Fxn 87h = block move
	int	15h		; transfer to ROM BIOS 

	ret			; back to caller

putblk	endp

_TEXT	ends

	end	main		; defines program entry point

