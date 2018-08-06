		TITLE	'Listing 6-8'
		NAME	Line11
		PAGE	55,132

;
; Name:		Line11
;
; Function:	Draw a line in 640x480 2-color mode (MCGA, VGA) 
;
; Caller:	Microsoft C:
;
;			void Line11(x1,y1,x2,y2,n);
;
;			int x1,y1,x2,y2;	/* pixel coordinates */
;
;			int n;			/* pixel value */
;

ARGx1		EQU	word ptr [bp+4]	; stack frame addressing
ARGy1		EQU	word ptr [bp+6]
ARGx2		EQU	word ptr [bp+8]
ARGy2		EQU	word ptr [bp+10]
ARGn		EQU	byte ptr [bp+12]
VARincr1	EQU	word ptr [bp-2]
VARincr2	EQU	word ptr [bp-4]
VARroutine	EQU	word ptr [bp-6]

BytesPerLine	EQU	80		; bytes in one row of pixels
ByteOffsetShift	EQU	3		; used to convert pixels to byte offset

DGROUP		GROUP	_DATA

_TEXT		SEGMENT	byte public 'CODE'
		ASSUME	cs:_TEXT,ds:DGROUP

		EXTRN	PixelAddr10:near

		PUBLIC	_Line11
_Line11		PROC	near

		push	bp		; preserve caller registers
		mov	bp,sp
		sub	sp,6		; stack space for local variables
		push	si
		push	di

; check for vertical line

		mov	si,BytesPerLine	; SI := initial y-increment

		mov	cx,ARGx2
		sub	cx,ARGx1	; CX := x2 - x1
		jz	VertLine11	; jump if vertical line

; force x1 < x2

		jns	L01		; jump if x2 > x1

		neg	cx		; CX := x1 - x2
		
		mov	bx,ARGx2	; exchange x1 and x2
		xchg	bx,ARGx1
		mov	ARGx2,bx

		mov	bx,ARGy2	; exchange y1 and y2
		xchg	bx,ARGy1
		mov	ARGy2,bx

; calculate dy = ABS(y2-y1)

L01:		mov	bx,ARGy2
		sub	bx,ARGy1	; BX := y2 - y1
		jnz	L02

		jmp	HorizLine11	; jump if horizontal line

L02:		jns	L03

		neg	bx		; BX := y1 - y2
		neg	si		; negate y-increment

; select appropriate routine for slope of line

L03:		mov	VARroutine,offset LoSlopeLine11
		cmp	bx,cx
		jle	L04		; jump if dy <= dx (slope <= 1)
		mov	VARroutine,offset HiSlopeLine11
		xchg	bx,cx		; exchange dy and dx

; calculate initial decision variable and increments

L04:		shl	bx,1		; BX := 2 * dy
		mov	VARincr1,bx	; incr1 := 2 * dy
		sub	bx,cx
		mov	di,bx		; DI := d = 2 * dy - dx
		sub	bx,cx
		mov	VARincr2,bx	; incr2 := 2 * (dy - dx)

; calculate first pixel address

		push	cx		; preserve this register
		mov	ax,ARGy1	; AX := y
		mov	bx,ARGx1	; BX := x
		call	PixelAddr10	; AH := bit mask
					; ES:BX -> buffer
					; CL := # bits to shift left

		mov	al,ARGn		; AL := unshifted pixel value
		shl	ax,cl		; AH := bit mask in proper position
					; AL := pixel value in proper position

		mov	dx,ax		; DH := bit mask
					; DL := pixel value
		not	dh		; DH := inverse bit mask

		pop	cx		; restore this register
		inc	cx		; CX := # of pixels to draw

		jmp	VARroutine	; jump to appropriate routine for slope


; routine for vertical lines

VertLine11:	mov	ax,ARGy1	; AX := y1
		mov	bx,ARGy2	; BX := y2
		mov	cx,bx
		sub	cx,ax		; CX := dy
		jge	L31		; jump if dy >= 0

		neg	cx		; force dy >= 0
		mov	ax,bx		; AX := y2

L31:		inc	cx		; CX := # of pixels to draw
		mov	bx,ARGx1	; BX := x
		push	cx		; preserve this register
		call	PixelAddr10	; AH := bit mask
					; ES:BX -> video buffer
					; CL := # bits to shift left
		mov	al,ARGn		; AL := pixel value
		shl	ax,cl		; AH := bit mask in proper position
					; AL := pixel value in proper position
		not	ah		; AH := inverse bit mask
		pop	cx		; restore this register

; draw the line

		test	al,al
		jz	L33		; jump if pixel value = 0

L32:		or	es:[bx],al	; set pixel values in buffer
		add	bx,si
		loop	L32
		jmp	short L34

L33:		and	es:[bx],ah	; reset pixel values in buffer
		add	bx,si
		loop	L33

L34:		jmp	Lexit


; routine for horizontal lines (slope = 0)

HorizLine11:	mov	ax,ARGy1
		mov	bx,ARGx1
		call	PixelAddr10	; AH := bit mask
					; ES:BX -> video buffer
					; CL := # bits to shift left
		mov	di,bx		; ES:DI -> buffer

		mov	dh,ah
		not	dh		; DH := unshifted bit mask for leftmost
					;        byte
		mov	dl,0FFh		; DL := unshifted bit mask for
					;	 rightmost byte

		shl	dh,cl		; DH := reverse bit mask for first byte
		not	dh		; DH := bit mask for first byte

		mov	cx,ARGx2
		and	cl,7
		xor	cl,7		; CL := number of bits to shift left
		shl	dl,cl		; DL := bit mask for last byte

; determine byte offset of first and last pixel in the line

		mov	ax,ARGx2	; AX := x2
		mov	bx,ARGx1	; BX := x1

		mov	cl,ByteOffsetShift	; number of bits to shift to
						;  convert pixels to bytes

		shr	ax,cl		; AX := byte offset of x2
		shr	bx,cl		; BX := byte offset of x1
		mov	cx,ax
		sub	cx,bx		; CX := (# bytes in line) - 1

; propagate pixel value throughout one byte

		mov	bx,offset DGROUP:PropagatedPixel
		mov	al,ARGn		; AL := pixel value
		xlat

; set pixels in leftmost byte of the line

		or	dh,dh
		js	L43		; jump if byte-aligned (x1 is leftmost
					;  pixel in byte)
		or	cx,cx
		jnz	L42		; jump if more than one byte in the line

		and	dl,dh		; bit mask for the line
		jmp	short L44

L42:		mov	ah,al
		and	ah,dh		; AH := masked pixel bits
		not	dh		; DH := reverse bit mask for 1st byte
		and	es:[di],dh	; zero masked pixels in buffer
		or	es:[di],ah	; update masked pixels in buffer
		inc	di
		dec	cx

; use a fast 8086 machine instruction to draw the remainder of the line

L43:		rep	stosb		; update all pixels in the line

; set pixels in the rightmost byte of the line

L44:		and	al,dl		; AL := masked pixels for last byte
		not	dl
		and	es:[di],dl	; zero masked pixels in buffer
		or	es:[di],al	; update masked pixels in buffer

		jmp	Lexit


; routine for dy <= dx (slope <= 1)	; ES:BX -> video buffer
					; CX = #pixels to draw
					; DH = inverse bit mask
					; DL = pixel value in proper position
					; SI = bytes per pixel row
					; DI = decision variable
LoSlopeLine11:

L10:		mov	ah,es:[bx]	; AH := byte from video buffer

L11:		and	ah,dh		; zero pixel value at current bit offset
		or	ah,dl		; set pixel value in byte

		ror	dl,1		; rotate pixel value
		ror	dh,1		; rotate bit mask
		jnc	L14		; jump if bit mask rotated to
					;  leftmost pixel position

; bit mask not shifted out

		or	di,di		; test sign of d
		jns	L12		; jump if d >= 0

		add	di,VARincr1	; d := d + incr1
		loop	L11

		mov	es:[bx],ah	; store remaining pixels in buffer
		jmp	short Lexit

L12:		add	di,VARincr2	; d := d + incr2
		mov	es:[bx],ah	; update buffer

		add	bx,si		; increment y
		loop	L10
		jmp	short Lexit

; bit mask shifted out

L14:		mov	es:[bx],ah	; update buffer
		inc	bx		; BX := offset of next byte

		or	di,di		; test sign of d
		jns	L15		; jump if non-negative

		add	di,VARincr1	; d := d + incr1
		loop	L10
		jmp	short Lexit

L15:		add	di,VARincr2	; d := d + incr2

		add	bx,si		; increment y
		loop	L10
		jmp	short Lexit


; routine for dy > dx (slope > 1)	; ES:BX -> video buffer
					; CX = #pixels to draw
					; DH = inverse bit mask
					; DL = pixel value in proper position
					; SI = bytes per pixel row
					; DI = decision variable
HiSlopeLine11:

L21:		and	es:[bx],dh	; zero pixel value in video buffer
		or	es:[bx],dl	; set pixel value in byte

		add	bx,si		; increment y

L22:		or	di,di		; test sign of d
		jns	L23		; jump if d >= 0

		add	di,VARincr1	; d := d + incr1
		loop	L21

		jmp	short Lexit


L23:		add	di,VARincr2	; d := d + incr2

		ror	dl,1		; rotate pixel value
		ror	dh,1		; rotate bit mask
		cmc			; cf set if bit mask not rotated to
					;  leftmost pixel position

		adc	bx,0		; BX := offset of next byte

		loop	L21


Lexit:		pop	di
		pop	si
		mov	sp,bp
		pop	bp
		ret

_Line11		ENDP

_TEXT		ENDS


_DATA		SEGMENT	word public 'DATA'

PropagatedPixel DB      00000000b       ; 0
                DB      11111111b       ; 1

_DATA		ENDS

		END