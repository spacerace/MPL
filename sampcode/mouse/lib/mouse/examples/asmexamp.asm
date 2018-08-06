;	    Mouse Function Calls and Microsoft Assembler
;
;	    To Run :	MASM ASMEXAMP;
;			LINK ASMEXAMP;
;
;			MAKE ASMEXAMP.MAK
;
;


stack  segment stack   'stack'
		db	256 dup(?)
stack	ends
;
data		segment public	'data'
		msg1	db   "Microsoft Mouse NOT found","$"
		msg2	db   "Graphics cursor limit at center of the screen",0dh,0ah
			db   "Press the left mouse button to EXIT","$"
data		ends
;
code		segment public	'code'
		assume cs:code, ds:data, es:data; ss:stack
start:
		push	bp
		mov	bp,sp
		mov	ax,seg data		;Set DS to the
		mov	ds,ax			; data segment

		push	es			;Save PSP segment address
		mov	ax, 03533h		;Get int 33h vector
		int	21h			; by calling int 21
		mov	ax, es			;Check segment and
		or	ax, bx			;offset of int 33
		jnz	begin			;vector.  If 0 or pointing to
		mov	bl, es:[bx]		;an IRET driver not install
		cmp	bl, 0cfh
		jne	begin			;Exit

		mov	dx, offset msg1 	;Get not found message offset
		mov	ah, 09h 		;Output message to screen
		int	21h
		pop	es
		jmp	short exit		;Exit

begin:
		mov	ax,0			;Initialize mouse
		int	33h
		cmp	ax,0			;Is mouse installed?
		jz	exit			;No, exit
		mov	ax,0006h		;Set up for 640x200 resolution
		int	10h			;graphics mode (CGA-mode 6)
		mov	ax, 4			;Function 4 (set cursor position)
		mov	cx, 200 		;M3 = 200
		mov	dx, 100 		;M4 = 100
		int	33h
		mov	ax, 7			;Function 7
		mov	cx, 150 		;M3 = 150
		mov	dx, 450 		;M4 = 450
		int	33h
		mov	ax, 8			;Function 8
		mov	cx, 50			;M3 = 50
		mov	dx, 150 		;M4 = 150
		int	33h
		mov	ax,1			;Shoe the mouse cursor
		int	33h

		mov	dx, offset msg2 	;Get exit message
		mov	ah, 09h 		;Output message to screen
		int	21h
		xor	ax, ax

around:
		mov	ax,3			;Get mouse status
		int	33h
		cmp	bx,0001h		;Left button pressed ?
		jne	around			;Branch if left button NOT pressed

		mov	ax,0
		int	33h			;Reset mouse
		mov	ax,0003h		;Set up 80x25 character text mode
		int	10h
exit:
		mov	sp,bp
		pop	bp
		mov	ax,04c00h		;Terminate
		int	21h
;
code		ends
end		start
