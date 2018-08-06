;********************************************************************
;*
;*	GRAPHIC CHARACTER HANDLING
;*
;*		Dave Tutelman - 8/86
;*
;*-------------------------------------------------------------------
;*
;*	Fn 6	Scroll up - not yet implemented
;*	Fn 7	Scroll down - not yet implemented
;*	Fn 9	Write character with attribute
;*	Fn 10	Write character normal
;*	Fn 14	Write character Teletypewriter style
;*
;*	Also includes subroutines to:
;*	get_address	convert page/row/col to display address
;*	do_char		write a character to an address on display
;*	do_attrib	change a display address to some attribute
;*	full_screen_scroll	used by Fn 14, when it needs to scroll
;*	flash		momentarily flash to reverse video and back
;*
;********************************************************************
;
INCLUDE	hercbios.h

;-----------------------------------------------
extrn	exit_herc_bios:near
public	writechar,scroll_up,scroll_down,tty
public	scr_start,scr_length,num_rows
;------------------------------------
cseg	segment	public
	assume	cs:cseg,ds:bios_data
;

;**************************************************************
;*
;*	FUNCTION 6 & 7 - SCROLL UP & DOWN
;*	(Placeholder only - not yet implemented)
;*
;*	AL = Number of rows to scroll (if 0, blank scroll area)
;*	BH = Fill attribute (we ignore, since writechar destroys old attrib)
;*	CX = Upper left corner      (CH=row, CL=col)
;*	DX = Lower right corner     (DH=row, DL=col)
;*
;****************************************************************
;
scroll_up:
scroll_down:
;
	jmp	exit_herc_bios
page
;********************************************************************
;*
;*	FUNCTION 9 & 10 - WRITE A CHARACTER AT CURRENT CURSOR
;*
;*	AL = character code to be written
;*	AH = 9 (write char with attribute) or 10 (write char normal)
;*	BL = new attribute (limited selection in graphics mode)
;*	BH = display page number
;*	CX = count of how many characters to write
;*
;********************************************************************
;
writechar:	
;			; Get the address corresponding to cursor[page]
	push	bx
	mov	bl,bh		; page to BX
	xor	bh,bh
	shl	bx,1		; *2 for word pointer
	mov	dx,curs_pos[bx]	; appropriate cursor position to DX
	pop	bx
	call	get_address	; get display address in DI
;
wrchar_loop:
;			; Write a character to that address
	call	do_char		; arguments set up already
;
;			; If function 9, modify the character's attributes
	cmp	ah,9		; Function 9?
	jne	no_attrib	; no, don't do attributes
	call	do_attrib	; yes, and arguments already set up
no_attrib:
;
	inc	di		; move to next position, without moving
				;    the official cursor
	loop	wrchar_loop	; continue until CX count exhausted
	jmp	exit_herc_bios
page
;*****************************************************************
;*
;*	FUNCTION 14 - TELETYPEWRITER-STYLE CHARACTER WRITE
;*
;*	AL = Character to be written
;*
;******************************************************************
;
tty:
	assume	ds:bios_data
	mov	bl,active_page	; active page to BX
	xor	bh,bh
	mov	dx,curs_pos[bx]	; get cursor for active page
	push	bx
	mov	bh,bl		; move page to BH
	call	get_address	; address of character to DI
	pop	bx
;
;			; process the character
;			; check if CR
	cmp	al,13		; carriage return?
	jne	not_cr
	mov	dl,0		; go to first column
	jmp	fix_curs
;			; check if LF
not_cr:	cmp	al,10		; line feed?
	jne	not_lf
	inc	dh		; next line
	jmp	fix_curs
;			; check if BS
not_lf:	cmp	al,8		; backspace?
	jne	not_bs
	cmp	dl,0		; already first column?
	je	fix_curs	; yup. do nothing
	dec	dl		; nope. move cursor left one
	dec	di		; also move address pointer left one
	mov	al,32		; set character to space
	call	do_char		; and write the space, but don't move
	jmp	fix_curs
;			; check if BEL
not_bs:	cmp	al,7		; bell character?
	jne	not_bl
	pop	es		; restore registers
	pop	di
	pop	si
	pop	dx
	pop	cx
	pop	bx
	pop	ds
	jmp	vid_bios	; ... and use the normal BIOS to ring the bell
;	call	flash		; can't do BEL standard. Blink display instead
;	jmp	fix_curs
;			; ordinary printing character, so display it
not_bl:	call	do_char		; write it to screen
	inc	dl		; cursor one to the right
;
;			; now look at the cursor, do what's necessary to
;			; fix it up, and save it away.
fix_curs:
	cmp	dl,89		; beyond last column?
	jle	chk_scroll	; not yet
	xor	dl,dl		; yes. do a CR
	inc	dh		; ... and a LF
chk_scroll:
	cmp	dh,cs:num_rows	; now see if we're beyond last row?
	jl	exit_tty	; not yet
	call	full_screen_scroll
				; yes. Scroll the screen up one
	dec	dh		; ... and scroll cursor, too.
	jmp	chk_scroll
;
exit_tty:
	mov	curs_pos[bx],dx	; save cursor position
	jmp	exit_herc_bios
page
;--------------------------------------------------------------------
;
;	GET_ADDRESS  SUBROUTINE
;
;	BH = display page
;	DX = cursor position (DH=row, DL=col)
;
;	returns:
;	DI = displacement of top row of pixels
;
;--------------------------------------------------------------------
;
get_address:
	push	cx		; save it

;			; compute display address from cursor_pos
	mov	cl,dh		; get row # in cx
	xor	ch,ch
	shl	cx,1		; begin fast multiply by 90 (1011010 B)
	mov	di,cx
	shl	cx,1
	shl	cx,1
	add	di,cx
	shl	cx,1
	add	di,cx
	shl	cx,1
	shl	cx,1
	add	di,cx		; end fast multiply by 90
	mov	cx,di		; copy answer back to cx
	shl	di,1		; *2 for ibm graphics mode
	cmp	video_mode,herc_mode
	jne	ibm_ad		; not herc mode
	add	di,cx		; *3 for herc mode
ibm_ad:	xor	ch,ch		; columns in CX
	mov	cl,dl
	add	di,cx		; add in col. address in DI
	cmp	bh,0		; if page 1, set high-order bit of address
	je	pg0
	or	di,8000H
pg0:
;			; address now in DI
;
	pop	cx		; restore it
	ret
page
;--------------------------------------------------------------------
;
;	DO_CHAR  SUBROUTINE
;
;	AL = character to write
;	DI = diplacement (address) of top row of pixels
;
;	Note: SI and ES are destroyed
;
;--------------------------------------------------------------------
;
do_char:
	push	ax
	push	bx
	push	cx
	push	di
	push	ds
;
;			; get scan pattern table pointer into BX
	cmp	video_mode,herc_mode
	je	herc_1
	mov	bx,offset ibm_pattern
				; IBM graphics mode - use appropriate table
	jmp	c_1
herc_1:	mov	bx,offset herc_pattern
				; herc graphics mode - use appropriate table
c_1:
;
;			; set up source address registers
	xor	ah,ah		; character to SI
	mov	si,ax
IFDEF iAPX286
	shl	si,3		; *8 for 8-byte table entry
ELSE
	shl	si,1		; *8 for 8-byte table entry
	shl	si,1
	shl	si,1
ENDIF
				; next, find beginning of table
	cmp	al,7Fh		; ROM or user table?
	jg	u_tbl
				; ROM table
	add	si,charbase	; character table base added to offset
	mov	ax,mpx_bios	; BIOS code segment to DS
	mov	ds,ax
	jmp	c_2
u_tbl:				; user table
	xor	ax,ax		; zero (interrupt vector) to DS
	mov	ds,ax
	mov	ax,si		; save table offset in AX
	assume	ds:intvec
	lds	si,user_table	; load DS:SI from interrupt vector
	add	si,ax		; add offset into table base
c_2:
;
;			; set up destination address registers
	mov	ax,pixbase	; get display segment in ES
	mov	es,ax
				; displacement already in DI
;
;
;			; transfer the character
	mov	ax,di		; save top-row displacement in AX
	mov	cx,8		; transfer 8 rows
	cld			; direction = up
c_loop:	mov	di,ax		; top-row displacement
	add	di,cs:[bx]	; add entry from scan-pattern table
	movsb			; actually transfer a byte and bump SI & DI
	add	bx,2		; next entry in scan-pattern table
	loop	c_loop
;
;			; if hercules mode, blank the extra rows
	pop	ds		; restore DS to Bios data
	assume	ds:bios_data
	cmp	video_mode,herc_mode
	jne	c_3
				; Hercules mode
	mov	si,ax		; don't need SI. Save top-row displacement
	xor	ax,ax		; zero AX
	mov	cx,4		; four rows to blank
	cld
c_blnk: mov	di,si		; top-row displacement
	add	di,cs:[bx]	; add entry from scan-pattern table
	stosb			; transfer a zero byte
	add	bx,2		; next entry in scan-pattern table
	loop	c_blnk
c_3:
;
;			; clean up and return
	pop	di
	pop	cx
	pop	bx
	pop	ax
	ret
page
;---------------------------------------------------------------------
;
;	DO_ATTRIB  SUBROUTINE
;
;	BL = attribute byte
;	DI = displacement (address) of top row of pixels
;	ES is destroyed
;
;	Because attributes don't "just happen" in graphics mode, here's
;	what we have to transform them to.
;
;	CODE	USUALLY MEANS		IBM MODE	HERC MODE
;	00	invisible		invisible	invisible
;	01	underline		[normal]	underline
;	07	normal			normal		normal
;	0F	hi-intens		[rev video]	[rev video]
;	70	rev video		rev video	rev video
;
;	Anything else displays as normal
;	Note that there's no way to make blinking happen.
;
;-----------------------------------------------------------------------
;
do_attrib:
	assume	ds:bios_data
	push	ax
	mov	ax,pixbase	; Display base to ES
	mov	es,ax
	pop	ax
;
;			; which attribute, if any?
	and	bl,7Fh		; mask off blink bit
	cmp	bl,0		; invisible?
	je	invisible
	cmp	bl,0Fh		; reverse video? (instead of bright)
	je	reverse
	cmp	bl,70H		; reverse video?
	je	reverse
	cmp	bl,01		; underline?
	je	underline
	ret			; none of the above. Display normal.
;
;			; underline the character
underline:
	cmp	video_mode,herc_mode
	je	ul_1
	ret			; don't do it for IBM mode
ul_1:	mov	byte ptr es:[di+40B4h],0FFh
				; move ones to 11th line of pixel array
				; 40B4h is the 11th entry in herc_pattern
	ret
;
;			; make it invisible
invisible:
	push	ax
	push	bx
	push	cx
	push	dx
	xor	ax,ax		; zero the AX
	cmp	video_mode,herc_mode
	je	herc_3
	mov	bx,offset ibm_pattern
				; point to scan pattern
	jmp	inv_1
herc_3:	mov	bx,offset herc_pattern
				; point to scan pattern
inv_1:	mov	dx,di		; save addr of top row of pixels in DX
	mov	cx,8		; 8 bytes to be moved
	cld			; direction = up
invis_loop:
	mov	di,dx		; top row address
	add	di,cs:[bx]	; add scan-table offset
	stosb			; move a zero byte
	add	bx,2		; bump the scan-table pointer
	loop	invis_loop
	pop	dx
	pop	cx
	pop	bx
	pop 	ax
	ret
;
;			; reverse video
reverse:
	push	bx
	push	cx
	push	dx
	cmp	video_mode,herc_mode
	je	herc_4
	mov	bx,offset ibm_pattern
				; point to scan pattern
	mov	cx,8		; 8 scan lines for IBM mode
	jmp	rev_1
herc_4:	mov	bx,offset herc_pattern
				; point to scan pattern
	mov	cx,12		; 12 scan lines for Hercules mode
rev_1:	mov	dx,di		; save addr of top row of pixels in DX
	cld			; direction = up
rev_loop:
	mov	di,dx		; top row address
	add	di,cs:[bx]	; add scan-table offset
	not	es:byte ptr[di]	; invert one scan line
	add	bx,2		; bump the scan-table pointer
	loop	rev_loop
	pop	dx
	pop	cx
	pop	bx
	ret
page
;--------------------------------------------------------------
;
;	SUBROUTINE  FULL_SCREEN_SCROLL
;
;	This scrolls the entire screen up one print line (8 or 12 pixels).
;	Actually, we'll protect one line on the bottom (e.g.-function keys).
;
;-----------------------------------------------------------------
;
;			; A few constants, initialized by set_mode
num_rows	db	?	; number of rows in display
				;   = 42 (IBM)  or 28 (Herc)
scr_start	dw	?	; 2*90 or 3*90 depending on mode
scr_length	dw	?	; number of words to transfer in a sweep
				;   = 41*start /2 = 3690	(IBM)
				;   = 27*start /2 = 3645	(Herc)
;
full_screen_scroll:
	assume	ds:bios_data
	push	ds
	push	ax
	push	cx
	mov	ax,pixbase	; start getting display segment
	cmp	active_page,0	; page 0?
	je	scr_pg0		
	add	ax,800H		; page 1. bump by half of 64K
scr_pg0:
	mov	ds,ax		; save display segment in DS
	mov	es,ax		; ... and in ES
;
	xor	ax,ax		; zero AX
	call	scr_shift
	mov	ax,2000H	; bump interlace counter
	call	scr_shift
	mov	ax,4000H
	call	scr_shift
	mov	ax,6000H
	call	scr_shift
;
	pop	cx
	pop	ax
	pop	ds
	ret
;
;
; scr_shift does the actual work of scrolling, one set of interlace
;   lines at a time.
;
scr_shift:
	cld			; block moves will be UP
	mov	di,ax		; interlace scan ID to DI
	mov	si,ax		; ... and to SI
	add	si,cs:scr_start
				; but bump by "start"
	mov	cx,cs:scr_length
				; set counter for transfer
	rep	movsw		; and scroll a set of lines
	xor	ax,ax		; set up a zero word
	mov	cx,cs:scr_start ; set counter for one more line
	shr	cx,1		; /2 for word transfers
	rep	stosw		; and blank the line
	ret
page
;-----------------------------------------------------------
;
;	SUBROUTINE FLASH
;
;	Flashes the screen inverse video and back to normal.
;	Used in place of an audible bell.
;
;-------------------------------------------------------------
;
flash:
	push	cx
	push	di
	push	es
;
	mov	di,pixbase	; get display area base
	cmp	active_page,0	; page 0?
	je	pg0_f
	add	di,800H		; no, page 1
pg0_f:	mov	es,di		; put resulting pointer to display in ES
;
;			; loop to invert screen
	xor	di,di		; point to beginning of display area
	mov	cx,4000H	; number of words to invert
flash_loop_1:
	not	word ptr es:[di]
				; invert one location
	add	di,2		; bump location pointer a word
	loop	flash_loop_1
;
;			; and invert it back to normal
	xor	di,di		; point to beginning of display area
	mov	cx,4000H	; number of words to invert
flash_loop_2:
	not	word ptr es:[di]
				; invert one location
	add	di,2		; bump location pointer a word
	loop	flash_loop_2
;
	pop	es
	pop	di
	pop	cx
	ret
page
;*****************************************************
;*	Data areas for character handling
;*****************************************************
;
pixels		db	12 dup(?)	; 12 bytes for pixel pattern
ibm_pattern	dw	0000h,2000h,4000h,6000h,005Ah,205Ah,405Ah,605Ah
herc_pattern	dw	4000h,6000h,005Ah,205Ah,405Ah,605Ah,00B4h,20B4h
blank_pattern	dw	0000h,2000h,40B4h,60B4h
;
;
cseg	ends
	end
