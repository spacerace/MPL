

;----------------------- this one needs work ---------------------------
;
; name		crt_cls2 - clear screen or graphics buffer
;
; synopsis	crt_cls2(segment,offset)
;			int segment;	
;			int offset;
;
;
; description	this routine uses the REP MOVSW instruction for fast
;		clearing of the screen or a graphics buffer. Use segment
;		0xb000 and offset 0 for mono, segment 0xb800 and offset
;		0 for color.  To clear a graphics buffer, use the appro-
;		priate segment and offset values.  Large model pointers
;		must be separated into segment and offset values which
;		must be passed separately.
;
; notes		on some machines, this routine is significantly faster
;		than a screen clear using the BIOS screen scroll up
;		routine.  
;
;		DI register destroyed.  ES register used and
;		then restored.
;
;		If used in text modes, it clears ALL text pages.
;-----------------------------------------------------------------------



	include	dos.mac


	IF	LPROG
X	EQU	6		;OFFSET OF ARGUMENTS
	ELSE
X	EQU	4		;OFFSET OF ARGUMENTS
	ENDIF


SEGM	equ	[bp+x+2]
OFFS	equ	[bp+x+4]

	PSEG


	PUBLIC	crt_cls2

	IF	LPROG
crt_cls2	PROC	FAR
	ELSE
crt_cls2	PROC	NEAR
	ENDIF


	push	bp
	push	es
	mov	bp,sp
	mov	ax,SEGM
	mov	es,ax			; set es to color video segment

	mov	di,OFFS			; offset to graphics ram
	mov	cx,4000			; # words to fill
	mov	ax,0
	cld
	rep	stosw

	mov	ax,SEGM
	add	ax,2000h
	mov	di,ax		; offset to graphics ram
	mov	cx,4000		; # words to fill
	mov	ax,0
	cld
	rep	stosw

	pop	es
	pop	bp		; restore registers
	ret

crt_cls2	endp

	ENDPS	
	END


