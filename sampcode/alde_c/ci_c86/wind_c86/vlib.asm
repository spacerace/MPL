
;	module:		vlib
;	programmer:	Ray L. McVay
;	started:	10 Oct 82
;	version:	2.01, 3 Aug 84
;
;	A library of c bios video functions originally written to
;	replace the int10() function of small-c:PC.

;	history:
;	1.0	small-c:PC version
;	2.0	ported to DeSmet c

;		Calling convention for DeSmet is to push parameters
;		rightmost first then do an intrasegment call to the
;		function.

;		Char's, int's and unsigned's are returned in AX.
;		Long's are returned in DX:AX.

;		CS, DS, SS, SP and BP should be preserved.

;     Dec 84    ported to CI C86 by Mike Elkins

VIDEO	EQU	16	;BIOS video interrupt
        include model.h
        include prologue.h

;	get video mode
; int get_mode_();

	PUBLIC	GET_MODE_
GET_MODE_  proc near
	mov	ah,15
	int	VIDEO
	cbw
	ret
GET_MODE_   endp

;	set video mode
; set_mode_(mode)
; int	mode;

	PUBLIC	SET_MODE_
SET_MODE_  proc near
	push	bp
	mov	bp,sp
	mov	al,[bp+4]
	mov	ah,0
	INT	VIDEO
	pop	bp
	RET
SET_MODE_  endp

;	set cursor type		
; set_curtyp_(start,end)
; int	start,end;
; Note: if start > end then cursor is turned off.

	PUBLIC	SET_CURTYP_
SET_CURTYP_  proc near
	push	bp
	mov	bp,sp
	mov	ch,[bp+4]	; top line
	mov	cl,[bp+6]	; bottom line
	mov	ah,1
	INT	VIDEO
	pop	bp
	RET
SET_CURTYP_  endp

;	gotoxy - set cursor position	
; gotoxy_(x, y, page)
; int	x,y,page;

	PUBLIC	GOTOXY_
GOTOXY_  proc near
	push	bp
	mov	bp,sp
	mov	dl,[bp+4]	; DL = x = column
	mov	dh,[bp+6]	; DH = y = row
	mov	bh,[bp+8]	; BH = page
	mov	ah,2
	INT	VIDEO
	pop	bp
	RET
GOTOXY_  endp

;	read cursor position
; getxy_(page)
; int	page;
; xpos = getxy(page) & 255;
; ypos = getxy(page) >> 8;

	PUBLIC	GETXY_
GETXY_  proc near
	push	bp
	mov	bp,sp
	mov	bh,[bp+4]	;BH = page
	MOV	AH,3
	INT	VIDEO
	MOV	AX,DX		;return(256*row+column)
	pop	bp
	RET
GETXY_  endp

;	get light pen position
; int gltpen_(buff)
; int	buff[4];
; Note: Returns 0 if pen was not triggered, 1 if it was.
;	Stores XY values in integer array at buff.

	PUBLIC	GLTPEN_
GLTPEN_  proc near
	push	bp
	mov	bp,sp
	MOV	AH,4
	INT 	VIDEO
	OR	AH,AH		;check status
	JZ	DOSTAT		;it was bad
	mov	si,[bp+4]	;get addres of buffer
	MOV	[SI],BX		;buff[0]=X for graphics
	MOV	[SI]+2,CH	;buff[1]=Y
	MOV	[SI]+4,DL	;buff[2]=X for text
	MOV	[SI]+6,DH	;buff[3]=Y for text
	MOV	AH,1		;return OK status
DOSTAT  LABEL NEAR
	MOV	AL,AH
	CBW
	pop	bp
	RET
GLTPEN_  endp

;	select active page	
; setpage_(page)
; int	page;

	PUBLIC	SETPAGE_
SETPAGE_  proc near
	push	bp
	mov	bp,sp
	mov	al,[bp+4]	;page
	MOV	AH,5
	INT	VIDEO
	pop	bp
	RET
SETPAGE_  endp

;	scroll a window up	
; scrlup_(window,count,attrib)
; int window[4],count,attrib;
; Note: Window defines the upper-left and lower-right
;	character positions of an area of the text screen.
;	A count of 0 clears the window.
;	The attribute is used on the line(s) blanked.

	PUBLIC	SCRLUP_
SCRLUP_  proc near
	push	bp
	CALL	SCRL1
	MOV	AH,6
	INT	VIDEO
	pop	bp
	RET

SCRL1 	LABEL NEAR
	mov	bp,sp
	mov	bx,[bp+6]	;get base of window array
	MOV	CL,[bx]		;window[0] = U.L. X
	MOV	CH,[bx]+2	;window[1] = U.L. Y
	MOV	DL,[bx]+4	;window[2] = L.R. X
	MOV	DH,[bx]+6	;window[3] = L.R. Y
	mov	al,[bp+8]	;AL = count
	mov	bh,[bp+10]	;BH = attribute
	RET
SCRLUP_  endp

;	scroll a window down	
; scrldn_(window,count,attrib)
; int window[4],count,attrib;

	PUBLIC	SCRLDN_
SCRLDN_  proc near
	push	bp
	CALL	SCRL1
	MOV	AH,7
	INT	VIDEO
	pop	bp
	RET
SCRLDN_  endp

;	read character & attribute under the cursor
; int vgetc_(page)
; int	page;

	PUBLIC	VGETC_
VGETC_  proc near
	push	bp
	mov	bp,sp
	MOV	BH,[bp+4]
	MOV	AH,8
	INT	VIDEO
	pop	bp
	RET
VGETC_  endp

;	write character & attribute at cursor
; vputca_(chr,page,count)
; int	chr,page,count;
; Note: Chr contains attribute in hi byte.
;	Count is the number of times to write the character.
;	(Good for tops and bottoms of windows or boxes.)

	PUBLIC	VPUTCA_
VPUTCA_  proc near
	push	bp
	CALL	VPUT1
	MOV	AH,9
	INT	VIDEO
	pop	bp
	RET

VPUT1 	LABEL NEAR
	mov	bp,sp
	mov	AX,[bp+6]	;attrib/char
	mov	bh,[bp+8]	;page
	mov	CX,[bp+10]	;count
	MOV	BL,AH		;attrib to BL
	RET
VPUTCA_  endp

;	vputc - write character only at cursor	
; vputc_(chr,page,count)
; int	chr,page,count;
; Note:	Same as vputca() except uses existing attributes.

	PUBLIC	VPUTC_
VPUTC_  proc near
	push	bp
	CALL	VPUT1
	MOV	AH,10
	INT	VIDEO
	pop	bp
	RET
VPUTC_  endp

;	set background or pallet or alpha border color		
; pcolor_(id,val)
; int	id,val;
; Note:	If id == 0 then val is background color.
;	If id == 1 and mode is graphics then val is pallet.
;	If id == 1 and mode is text then val is border.

	PUBLIC	PCOLOR_
PCOLOR_  proc near
	push	bp
	mov	bp,sp
	mov	bh,[bp+4]	;id
	mov	bl,[bp+6]	;val
	MOV	AH,11
	INT	VIDEO
	pop	bp
	RET
PCOLOR_  endp

;	write a graphics dot	
; plot_(color,horz,vert)
; int	color,horz,vert;

	PUBLIC	PLOT_
PLOT_  proc near
	push	bp
	CALL	SETDOT
	MOV	AH,12
	INT	VIDEO
	pop	bp
	RET

SETDOT  LABEL NEAR
	mov	bp,sp
	mov	AX,[bp+6]	;color
	mov	CX,[bp+8]	;horiz
	mov	DX,[bp+10]	;vert
	RET
PLOT_  endp

;	read a graphic dot color
; int get_dot_(dummy,horz,vert)	
; int	dummy,horz,vert;
; Note: the dummy parameter is used so SETDOT can be shared.

	PUBLIC	GET_DOT_
GET_DOT_  proc near
	push	bp
	CALL	SETDOT		; set up the registers
	MOV	AH,13
	INT	VIDEO
	cbw
	pop	bp
	RET			; AX = dot color
GET_DOT_  endp

        include epilogue.h
	END
