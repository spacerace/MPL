;*******************************************************
;*	
;*	SET OF GRAPHICS ROUTINES FOR HERCULES BIOS
;*
;*		Dave Tutelman - 8/86
;*
;*-------------------------------------------------------
;*
;*	do_pixel:	draws, erases, or reads a pixel, given an
;*			offset and mask.
;*
;**********************************************************
;
INCLUDE	hercbios.h

;------------------------------------------
public	wr_pixel,end_herc
extrn	exit_herc_bios:near
;-----------------------------------------
;
cseg	segment	public
	assume	cs:cseg,ds:bios_data
;
;
;**********************************************
;*
;*	read or write a pixel:
;*		DX = row # ( y )
;*		CX = col # ( x )
;*		AH = 12 for write, 13 for read
;*		AL = value (0,1, if bit 7=1 EXOR the value)
;*
;**********************************************
;
wr_pixel:
	mov	bh,video_mode	; check for valid mode
	cmp	bh,herc_mode
	je	do_pixel
	cmp	bh,ibm_mode
	je	do_pixel
	jmp	exit_herc_bios		; invalid mode. don't do it.
;
do_pixel:
	push	ax		; save function and pixel value
;
;			; first compute the address of byte to be modified
;			; = 90*[row/4] + [col/8] + 2^D*[row/4] + 2^F*page
	mov	bh,cl		; col (low order) in BH
	mov	bl,dl		; row (low order) in BL
	and	bx,0703H	; mask the col & row remainders
IFDEF iAPX286
	shr	cx,3		; col / 8
	shr	dx,2		; row / 4
	mov	al,90
	mul	dx		; AX = 90*[ row/4 ]
	add	ax,cx		;  ... + col/8
	shl	bl,5		; align row remainder
ELSE			; same as above, obscure but fast for 8086
	shr	cx,1		; divide col by 8
	shr	cx,1
	shr	cx,1
	shr	dx,1		; divide row by 4
	shr	dx,1
	shl	dx,1		; begin fast multiply by 90 (1011010 B)
	mov	ax,dx
	shl	dx,1
	shl	dx,1
	add	ax,dx
	shl	dx,1
	add	ax,dx
	shl	dx,1
	shl	dx,1
	add	ax,dx		; end fast multiply by 90
	add	ax,cx		; add on the col/8
	shl	bl,1		; align row remainder
	shl	bl,1
	shl	bl,1
	shl	bl,1
	shl	bl,1
ENDIF
	add	ah,bl		; use aligned row remainder
	cmp	active_page,0	; page 0 active?
	je	end_adr_calc	; yup
	or	ah,80H		; page 1 active. Set MSB of address
end_adr_calc:		; address of byte is now in AX
;
	mov	dx,pixbase	; base of pixel display to DX
	mov	es,dx		; ...and thence to segment reg
	mov	si,ax		; address of byte w/ pixel to index reg
	mov	cl,bh		; bit addr in byte
	mov	al,80H		; '1000 0000' in AL 
	shr	al,cl		; shift mask to line up with bit to read/write
	mov	bl,al	
;
	pop	bx		; now retrieve original AX into BX
				;      function=BH, pixel value=BL
	cmp	bh,13		; what to do with the pixel?
	je	read_pix	; read the pixel.
	cmp	bl,0		; write the pixel. But how?
	je	clr_pix		; clear the pixel
	jl	exor_pix	; exclusive-or the pixel
;
set_pix:		; set the pixel
	or	es:[si],al	; or the mask with the right byte
	jmp	exit_herc_bios
;
clr_pix:		;clear the pixel
	not	al		; invert the mask, so zero on bit to be cleared
	and	es:[si],al	; and the mask with the right byte
	jmp	exit_herc_bios
;
exor_pix:		; exclusive-or the pixel
	mov	ah,07fH		; mask to rid 7th bit
	and	ah,bl		; pixval w/o XOR flag
	jnz	do_exor		; ExOr with a 1?
	jmp	exit_herc_bios	; no! XOR (0,x) = x. just return.
do_exor:
	xor	es:[si],al	; EXOR the pixel with the mask.
	jmp	exit_herc_bios
;
read_pix:		; read the pixel
	and	al,es:[si]	; read the bit into AL
	jz	rd_zro
	mov	al,1		; if it ain't zero, it's one
rd_zro:	jmp	exit_herc_bios
;
;
end_herc:			; label for end of package. Used by hercbios
				;    to install it.
;
cseg	ends
	end
