;***	life2.asm - assembler routines for life.c
;*
;*	Contains:	_draw_grid - draws 79x45 grid on screen
;*			_dostep    - advances the internal and screen grids
;*					 one generation
;*			scrfill    - private screen routine for _dostep
;*			scrremove  - private screen routine for _dostep
;*


;***	segment definitions for the link with C
;
_TEXT	SEGMENT  BYTE PUBLIC 'CODE'
_TEXT	ENDS
_DATA	SEGMENT  WORD PUBLIC 'DATA'
_DATA	ENDS
CONST	SEGMENT  WORD PUBLIC 'CONST'
CONST	ENDS
_BSS	SEGMENT  WORD PUBLIC 'BSS'
_BSS	ENDS
DGROUP	GROUP	CONST, _BSS, _DATA
	ASSUME	CS: _TEXT, DS: DGROUP, SS: DGROUP, ES: DGROUP

;***	the public routines
;
	PUBLIC	_draw_grid, _dostep


;***	Global variables from LIFE.C
;
	EXTRN	_ScrSeg:WORD
	EXTRN	_ScrCol:WORD
	EXTRN	_ScrRow:WORD

;***	OS/2 functions
;
	EXTRN	VIOSCRUNLOCK:FAR
	EXTRN	VIOSCRLOCK:FAR


_TEXT	   SEGMENT
;***	_draw_grid - clears screen and draws 79x45 grid
;*
;*	Entry:	ds:_ScrSeg - screen buffer segment, from life.c
;*
;*	Exit:	None
;*
_draw_grid	PROC NEAR
	push	bp			; startup
	mov	bp,sp
	sub	sp,2			; one word variable for VIOSCRLOCK
	push	di			; save register used by C compiler

;	get screen access
	mov	ax,1
	push	ax			; wait until screen available
	lea	ax,byte ptr [bp-2]
	push	ss
	push	ax			; far address of return code
	sub	ax,ax
	push	ax			; reserved word
	call	VIOSCRLOCK		; call DOS for access

;	get parameter and set up
	mov	es,ds:_ScrSeg		; (es) = screen buffer segment
	xor	di,di			; (di) = offset in screen (start at 0)

;	loop to draw 45 rows of the tops and middles of grid squares
	mov	dx,45			; (dx) = rows of grid squares, counter
dg1:	mov	ax,5555h		; (ax) = alternating bit pattern
	mov	cx,39			; (cx) = number of full words on a line
	rep	stosw			; fill line with 5555h
	mov	es:[di],al		; finish off last odd byte
	inc	di			; (di) = start of next row
	inc	di
	mov	ax,08080h		; (ax) = left bit on
	mov	cx,40			; (cx) = number of words on a line
	rep	stosw			; fill next line with f0f0h
	dec	dx			; at screen bottom?
	jnz	dg1			; if no, loop

;	draw the last line
	mov	ax,5555h		; (ax) = alternating bits
	mov	cx,39			; (cx) = full words on last line
	rep	stosw			; do fill
	mov	es:[di],al		; finish off line with odd byte

;	clear bottom on screen on even bit plane
	inc	di			; (di) = start of last few lines
	sub	ax,ax			; (ax) = blank
	mov	cx,400			; (cx) = words left at bottom
	rep	stosw

;	draw the grid lines on the odd bit plane
	mov	di,2000h		; (di) = top of odd bit plane
	mov	ax,08080h		; (ax) = left bit on
	mov	cx,40*2*45		; (cx) = words to fill
	rep	stosw			; fill remainder with f0f0h

;	clear bottom on screen on odd bit plane
	sub	ax,ax			; (ax) = blank
	mov	cx,400			; (cx) = words left at bottom
	rep	stosw

;	return the screen
	sub	ax,ax
	push	ax			; reserved word
	call	VIOSCRUNLOCK		; call DOS to return screen

;	return
	pop	di
	mov	sp,bp
	pop	bp
	ret
_draw_grid	ENDP


;***	_dostep - advances InGrid one generation.
;*
;*	Entry:	char far *INGRID - InGrid, main internal bit map of cells
;*		char far *INGRID2 - InGrid2, working area of same size
;*		unsigned INROW - InRow, bits per row of above to areas
;*		unsigned INCOL - InCol, rows of above to areas
;*		ds:_ScrSeg	 - from life.c
;*		ds:_ScrRow	 - from life.c
;*		ds:_ScrCol	 - from life.c
;*
;*	Exit:	InGrid is updated on generation in accordance with
;*		the rules of life.
;*		Returns in AX the return code from its VIOSCRLOCK call
;*
INGRID EQU [bp+4]
INGRID2 EQU [bp+8]
INROW EQU [bp+12]
INCOL EQU [bp+14]

_dostep PROC	NEAR
	push	bp
	mov	bp,sp
	sub	sp,2			; make room for local var
	push	di
	push	si
	push	es
	push	ds

	; try to lock screen and put return code in [BP-2]
	xor	ax,ax
	push	ax			; don't wait until screen available
	lea	ax,byte ptr [bp-2]
	push	ss
	push	ax			; far address of return code
	sub	ax,ax
	push	ax			; reserved word
	call	VIOSCRLOCK		; call DOS for access

	; get pointers to the two internal grids
	les	di,INGRID2		 ; (es:di) = @InGrid2
	lds	si,INGRID		; (ds:si) = @InGrid

	; copy InGrid into InGrid2
	mov	ax,INCOL	      ; (ax) = cells per row
	mul	word ptr INROW	      ; (ax) = total cells
	mov	cl,4
	shr	ax,cl			; (ax) = cell/16 = total words
	mov	cx,ax			; (cx) = total words
	rep	movsw			; move InGrid into InGrid2


	; main step loop
	;   I use a rather odd algorithm.  "neighbors" is defined
	;   here as all cells in a 3x3 grid summed.  If neighbors < 3, the
	;   the cell in the center is turned off.  If neighbors = 3, the cell
	;   is turned on.  If neighbors = 4, the cell is left in its current
	;   state.  If neighbors > 4, the cell is turned off.
	push	ds
	pop	es			; (es) = InGrid's segment
	lds	bx,INGRID2		 ; (ds) = InGrid2's segment
					; (bx) = pointer to offset in both
	mov	di,INROW	      ; (di) = row count
	mov	si,INCOL	      ; (si) = column count
	shr	si,1
	shr	si,1
	shr	si,1			; (si) = bytes per row in grids

	; loop within the grid through all its rows
	;    here we come when at the start of a row
ds1:	mov	cx,INCOL	      ; (cx) = column count
	mov	al,80h			; (al) = bit mask for current bit
	xor	dx,dx			; (dh) = #of neighbors 1 col back
					; (dl) = #of neighbors at cur col

	; get number of neighbors on current row
ds2:	xor	ah,ah			; (ah) = count of neighbors on cur row
	test	ds:[bx],al		; is current bit on?
	jz	ds3			; if not go on
	inc	ah			; if yes, inc neighbor count
ds3:	cmp	bx,si			; are we on top row
	jb	ds4			; if yes, don't check above top
	push	bx			; save bx
	sub	bx,si			; backup bx to previous row
	test	ds:[bx],al		; is up a row bit on?
	pop	bx			; restore bx
	jz	ds4			; if not go on
	inc	ah			; if yes, inc neighbor count
ds4:	cmp	di,1			; are we on last row?
	jz	ds5			; if yes, don't read off edge
	test	ds:[bx+si],al		; is bit down a row on?
	jz	ds5			; if not, go on to final neighbor addin
	inc	ah			; if yes, inc neighbor count

	; add up the total number of neighbors and decide what to do
ds5:	cmp	cx,INCOL		; are we at far left?
	jz	ds12			; if yes, don't affect bits off edge
	add	dh,dl			; (dh) = previous two rows of neighbors
	add	dh,ah			; (dh) = total neighbor count
	cmp	dh,3
	jb	ds6			; if neighbors <3, turn off the cell
	je	ds9			; if neighbors = 3, turn on the cell
	cmp	dh,4			;
	je	ds12			; if neighbors = 4, do nothing

	; turn off the center bit
ds6:	rol	al,1			; backup to center bit
	jnc	ds7			; if no overflow, go on
	dec	bx			; else, backup to last byte
ds7:	test	es:[bx],al		; is it already off
	jz	ds8			; if yes, don't do it again
	not	al			; turn on all bits but one to blank
	and	es:[bx],al		; force off bit in InGrid
	not	al			; restore al

	pop	ds			; (ds) =  C's data ptr
	call	scrremove		; turn off on screen
	push	ds			; resave C's data ptr
	mov	ds,[bp+8+2]		; (ds) = seg of InGrid2

ds8:	ror	al,1			; put back al to foreward bit
	jnc	ds12			; if no overflow, go to next bit
	inc	bx			; if overflow, restore bx
	jmp	ds12			; go on to next bitl

	; turn on the center bit
ds9:	rol	al,1			; backup to center bit
	jnc	ds10			; if no overflow, go on
	dec	bx			; else, backup to last byte
ds10:	test	es:[bx],al		; is it already on?
	jnz	ds11			; if yes, don't do it again
	or	es:[bx],al		; force on bit in InGrid
	pop	ds			; (ds) =  C's data ptr
	call	scrfill 		; turn on on screen
	push	ds			; resave C's data ptr
	mov	ds,[bp+8+2]		; (ds) = seg of InGrid2
ds11:	ror	al,1			; put back al to foreward bit
	jnc	ds12			; if no overflow, go to next bit
	inc	bx			; if overflow, restore bx

	; advance to the next bit
ds12:	ror	al,1			; (al) mask advances to next bit
	mov	dh,dl			; move done neighbor count
	mov	dl,ah			; move the new neighbor row into bl
	jnc	ds13			; if still in same byte, jmp
	inc	bx			; if at byte end, advance to next byte
ds13:	loop	ds2			; loop through rest of bits on line


	; at the end of the row
	dec	di			; decrement row count
	jz	dsx			; if at end of grid, goto exit routine
	jmp	ds1			; if not at bottom, go to next row

	; at the end of the grid
	; return the screen
dsx:	cmp	byte ptr [bp-2],0	; is the screen locked?
	jnz	dsx1			; if not, don't unlock

	sub	ax,ax
	push	ax			; reserved word
	call	VIOSCRUNLOCK		; call DOS to return screen

dsx1:	xor	ah,ah
	mov	al,[bp-2]		; return code from SCRLOCK

	pop	ds
	pop	es
	pop	si
	pop	di
	mov	sp,bp
	pop	bp
	ret
_dostep ENDP


;***	scrfill - fills in a cell on the screen in 79x45 grid
;*
;*	Entry:	(di) = row number counted from bottom up (InRow-di=row to fill)
;*		(cx) = col number from right-1 (InCol-1-cx = col to fill)
;*		(ds) = data segment of life.c
;*
;*	Exit:	None, saves all registers
;*
scrfill proc   near
	cmp	[bp-2],byte ptr 0	; is the screen available?
	jnz	sfxx			; if not, exit

	push	es
	push	ax
	push	bx
	push	dx

	; get screen pointer
	mov	es,ds:_ScrSeg		; (es) = screen buf segment
	mov	ax,INROW
	sub	ax,di			; (ax) = row
	cmp	ax,ds:_ScrRow		; is it on the screen?
	jae	sfx			; if off, exit
	mul	WORD PTR INCOL		;	 * InCol
	shl	ax,1			;	 * 2
	mov	dx,INCOL
	sub	dx,cx
	dec	dx			;		(dx) = col to fill
	cmp	dx,ds:_ScrCol		; is it on the screen?
	jae	sfx			; if off, exit
	add	ax,dx			;	 + col = scrn offset of cell

	; fill in screen cell
	mov	bx,ax			; (es:bx) = far ptr to screen cell
	mov	BYTE PTR es:[bx+2000h],7fh	; first line of cell
	mov	BYTE PTR es:[bx+80],7fh 	; second
	mov	BYTE PTR es:[bx+2000h+80],7fh	; third line of cell

sfx:	pop	dx
	pop	bx
	pop	ax
	pop	es
sfxx:	ret
scrfill endp


;***	scrremove - removes a cell on the screen in 79x45 grid
;*
;*	Entry:	(di) = row number counted from bottom up (InRow-di=row to fill)
;*		(cx) = col number from right-1 (InCol-1-cx = col to fill)
;*		(ds) = data segment of life.c
;*
;*	Exit:	None, saves all registers
;*
scrremove proc	 near
	cmp	[bp-2],byte ptr 0	; is the screen available?
	jnz	srxx			; if not, exit

	push	es
	push	ax
	push	bx
	push	dx

	; get screen pointer
	mov	es,ds:_ScrSeg		; (es) = screen buf segment
	mov	ax,INROW
	sub	ax,di			; (ax) = row
	cmp	ax,ds:_ScrRow		; is it on the screen?
	jae	srx			; if off, exit
	mul	WORD PTR INCOL		;	 * InCol
	shl	ax,1			;	 * 2
	mov	dx,INCOL
	sub	dx,cx
	dec	dx			;		(dx) = col
	cmp	dx,ds:_ScrCol		; is it on the screen?
	jae	srx			; if off, exit
	add	ax,dx			;	 + col = scrn offset of cell

	; remove screen cell
	mov	bx,ax			; (es:bx) = far ptr to screen cell
	mov	BYTE PTR es:[bx+2000h],80h	; first line of cell
	mov	BYTE PTR es:[bx+80],80h 	; second
	mov	BYTE PTR es:[bx+2000h+80],80h	; third line of cell

srx:	pop	dx
	pop	bx
	pop	ax
	pop	es
srxx:	ret
scrremove endp

_TEXT	ENDS
END
