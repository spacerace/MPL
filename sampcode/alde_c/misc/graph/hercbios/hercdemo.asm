        page    ,132
	title	hercdemo - quick and dirty herc demo
;
; Hercdemo - (c) 1986 by Reid Brown, P.O. Box 1023, Issaquah, WA.  98027
;
;	This is a quick and dirty program to show off the Hercules
;	board.  I place if freely into the public domain so others
;	can diddle their Herc boards as well, so long as it is not
;	used for any commercial purposes.
;

message	macro	row,col,string
	mov	cx,row
	mov	bx,col
	mov	si,offset string
	call	print
        endm

hdraw	macro	row,col,len
	mov	cx,row
	mov	bx,col
	mov	dx,len
	call	hline
        endm

vdraw	macro	row,col,len
	mov	cx,row
	mov	bx,col
	mov	dx,len
	call	vline
        endm

code    segment
	assume	cs:code, ds:code, es:code, ss:code
        org     100h
start:	jmp	begin

IBMROM	equ	0F000h
CHARTAB	equ	0FA6Eh
HERCSEG	equ	0B800h

MODEPORT equ	3B8h

STRING0 db      'HERCULES DEMO',0
STRING1	db	'A funny thing happened on the way to the forum.',0
STRING2	db	'You, too, can have a rewarding career in computers...',0
STRING3 db      'Strike any key to proceed...',0


CON8	db	8
CON180	db	180

begin	proc	near
	mov	dx, MODEPORT		; select herc mode port
	mov	al, 08Ah
	out	dx, al			; graphics mode, video on, page 1

	call	graphics		; set 6845 for graphics
        call    clear			; clear screen RAM

        vdraw   10,5,20                 ; draw a vertical
        vdraw   10,65,20                ; - hopefully, a box!
        hdraw   10,6,60                 ; draw a line
        hdraw   30,6,60                 ; and another

        message 12,29,STRING0           ; 1st message
        message 17,10,STRING1           ; 2nd message
        message 23,10,STRING2           ; 3nd message
        message 29,20,STRING3           ; ...
        
;
; done - clean up and reset
;
        mov	ax, 0C07h		; flush buffer, read char
	int	21h			; wait for any input

        call    text                    ; reset 6845

	mov	dx, MODEPORT		; select herc mode port
	mov	al, 00001000B		; re-enable mono
	out	dx, al
        
	mov	ax, 4C00h
	int	21h			; exit
begin	endp

;
; print - print text in graphics mode
;
;	cx = row
;	bx = column
;	si = address of string (null terminated) to print
;
print	proc	near

loop:	lodsb				; get next char
	or	al, al			; end of display?
	je	pdone
	call	display
	inc	bx			; bump to next column
	jmp	loop
pdone:	ret

print	endp

;
; display - output an 8x8 character from the IBM ROM to the Herc board
;
; AX = char, BX = column (0-89), CX = row(0-42)  ** all preserved **
;
display	proc near
	push	ds			; save the lot
	push	es
	push	ax
	push	bx
	push	cx
	push	dx
	push	si
	push	di

; setup ds -> IBM ROM, and si -> index into IBM ROM character table located
;	at 0fa6eh in the ROM

	and	ax, 07fh
	mul	CS:CON8			; mult by 8 bytes of table per char
	mov	si, ax
	mov	ax, IBMROM
	mov	ds, ax
	assume	ds:nothing
	add	si, CHARTAB		; add offset of character table

; compute index into Hercules screen memory for scan line 0.  The remaining
;	seven scan lines are all at fixed offsets from the first.
;
;	Since graphics mode treats the screen as sets of 16x4 "characters",
;	we need to map an 8x8 real character onto the front or back of
;	a pair of graphics "characters".  The first four scan lines of our
;	8x8 character will map to the top graphics "character", and the second
;	four scan lines map to the graphics character on the "line" (4 scan
;	lines high) below it.
;
;	For some exotic hardware reason (probably speed), all scan line 0
;	bits (i.e. every fourth scan line) are stored in memory locations
;	0-2000h in the screen buffer.  All scan line 1 bits are stored
;	2000h-4000h.  Within these banks, they are stored by rows.  The first
;	scan line on the screen (scan line 0 of graphics character row 0)
;	is the first 45 words of memory in the screen buffer.  The next 45
;	words are the first scan line graphics row 1, and since graphics
;	"characters" are 4 bits high, this second scan line is physically
;	the fifth scan line displayed on the screen.
;
;	SO, to display an 8x8 character, the 1st and 5th rows of dots are
;	both scan line 0 of the graphics "character", the 2nd and 6th are
;	scan line 1, and so on.
;
;	The column (0-89) tells which byte in a scan line we need to load.
;	Since it takes two rows of graphics characters to hold one row of
;	our characters, column+90 is a index to scan line 4 rows of pixels
;	higher (n+4).  Thus 180 bytes of screen memory in any bank (0h, 2000h,
;	4000h, 6000h) represent a row of 8x8 characters.
;	
;	The starting location in screen memory for the first scan line of
;	a character to be displayed will be:  	(row*180)+column
;	The 5th scan line will be at:		(row*180)+column+90
;
;	The second and 6th scan lines will be at the above offsets plus
;	the bank offset of 2000h.  The third and 7th, add 4000h and finally
;	the 4th and 8th, add 6000h.
;
	mov	ax, HERCSEG
	mov	es, ax			; es = hercules page 0
	mov	ax, cx			; get row
	mul	CS:CON180		; mult by 180(10)
	mov	di, ax			; di = index reg
	cld				; insure right direction

;output 8 segments of character to video ram

	lodsb				; line 0
	mov	es:[di+bx], al
	lodsb
	mov	es:[di+bx+2000h], al	; line 1
	lodsb
	mov	es:[di+bx+4000h], al	; line 2
	lodsb
	mov	es:[di+bx+6000h], al	; line 3
	lodsb
	mov	es:[di+bx+90], al	; line 4
	lodsb
	mov	es:[di+bx+2000h+90], al	; line 5
	lodsb
	mov	es:[di+bx+4000h+90], al	; line 6
	lodsb
	mov	es:[di+bx+6000h+90], al	; line 7

	pop	di
	pop	si
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	pop	es
	pop	ds
	ret
display	endp

;
; clear - clear page 1 of the screen buffer to zero (effectively, blank
;	the screen)
;
clear   proc
        push    es
        push    ax
        push    cx
        push    di

	mov	ax, HERCSEG
	mov	es, ax
	xor	di, di
	mov	cx, 4000h
	xor	ax, ax
	cld

	rep stosw			; zero out screen page
	
	pop	di
	pop	cx
	pop	ax
	pop	es
	ret
clear	endp

;
; hline - draw a horizontal line at scan line 0 of a specified 8x8 character
;	cell sized row, for a specified no. of characters
;
;	cx = row
;	bx = col
;	dx = len
;
hline    proc near
        push    es
        push    ax
        push    cx
        push    di

	mov	ax,cx			; copy to accum for multiply
	mul	cs:CON180		; mult by 180 to get offset
	mov	di,ax			; copy to dest ptr
	add	di,bx			; add column offset
	mov	cx,dx			; put byte count in count reg
	mov	ax, HERCSEG
	mov	es, ax
        mov     ax, 0ffh		; pattern = all bits ON
	cld

	rep stosb			; put bits on screen
	
	pop	di
	pop	cx
	pop	ax
	pop	es
	ret

hline    endp

;
; vline - draw a vertical line in the last bit position of a given char
;	cell, extending downward for a specified no. of chars
;
;	cx = row
;	bx = col
;	dx = len
;
vline    proc near
        push    es
        push    ax
        push    cx
        push    di

	mov	ax,cx			; copy to accum for multiply
	mul	cs:CON180		; mult by 180 to get offset
	mov	di,ax			; copy to dest ptr
	mov	cx,dx			; put byte count in count reg
	mov	ax, HERCSEG
	mov	es, ax
        mov     ax, 1			; pattern = last bit on

vloop:	mov	es:[di+bx], al
	mov	es:[di+bx+2000h], al	; line 1
	mov	es:[di+bx+4000h], al	; line 2
	mov	es:[di+bx+6000h], al	; line 3
	mov	es:[di+bx+90], al	; line 4
	mov	es:[di+bx+2000h+90], al	; line 5
	mov	es:[di+bx+4000h+90], al	; line 6
	mov	es:[di+bx+6000h+90], al	; line 7
	add	di,180		        ; advance to next line
	loop	vloop			; continue for all rows
	
	pop	di
	pop	cx
	pop	ax
	pop	es
	ret

vline    endp

graphics proc near
	mov	si, offset graph_parms
g_cont:	mov	dx, 3b4h		; point to index reg
	mov	cx, 16
	xor	ax, ax
g_loop:	mov	al, ah
	out	dx, al
	inc	dx
	mov	al, [si]
	out	dx, al
	dec	dx
	inc	ah
	inc	si
	loop	g_loop
	ret
graph_parms db	35h,2dh,2eh,7,5bh,2,57h,57h,2,3,0,0,0,0,0,0
graphics endp

text	proc	near
	mov	si, offset text_parms
	jmp	g_cont
text_parms db	61h,50h,52h,0fh,19h,6,19h,19h,2,0dh,0bh,0ch,00,00,00,00
text	endp


code    ends

	end	start
