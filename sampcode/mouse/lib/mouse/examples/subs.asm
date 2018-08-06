;
;
; This is the subroutine for mouse.lib test in FORTRAN
; and Pascal examples.	Procedure graf sets the screen
; video mode into hi-res graphics.
;
;
;
mdata		segment byte public 'data'

		msg	db   "Mouse Driver NOT installed","$"

mdata		ends




mcode		segment para public 'CODE'
		assume	cs:mcode
;
		public	graf
;
graf		proc	far
		push	bp
		mov	ax, 06h 		;change to graphics
		int	10h			;mode by calling
		pop	bp			;int 10 service
		ret
graf		endp
;
;
;
		public	chkdrv
;
chkdrv		proc	far
		push	bp
		push	es

		mov	ax, 03533h		;get int 33h
		int	21h			;by calling int 21
		mov	ax, es			;check segment and
		or	ax, bx			;offset of int 33
		jnz	back			;vector if 0 or IRET
		mov	bl, es:[bx]		;mouse driver not installed
		cmp	bl, 0cfh
		jne	back			;exit

		mov	ax,seg mdata		;set up DS to
		mov	ds,ax			;point to data seg
		mov	dx, offset msg		;get message
		mov	ah, 09h 		;out to screen
		int	21h
		pop	es
		pop	bp
		mov	ax,04c00h		;function code for
		int	21h			;end process

	back:
		pop	es
		pop	bp
		ret
chkdrv		endp
;
mcode		ends
		end
