;	carsound	Control PC Speaker
;       by Mike Elkins  Mike's "C" Board  619-722-8724
;       for Microsoft C 3.0 
;
;	sound(len,freq)
;	int freq	frequency of sound
;	int len		length of sound
;
_text 	segment	byte public 'code'
	assume	cs:_text

	public	_sound

arg	struc
oldbp	dw	?
retn	dw	?
len	dw	?
freq	dw	?
arg	ends

CARSOUND:
_sound	proc    near

	push	bp
	mov	bp,sp
	push	ds
	xor	ax,ax		;set up for low memory stuff
	mov	ds,ax

	mov	di,[bp].freq	;put frequency in di
	mov   	al,0B6h   	;set up timer for sound
	out	43h,al
	mov	dx,14h		;divide frequency by 1331000
	div	di
	out	42h,al		;send frequency to timer
	mov	al,ah
	out	42h,al
	in	al,61h		;get current timer port value
	push	ax		;save it
	or	al,3		;turn on speaker
 	out	61h,al
	mov	bx,[bp].len	;wait loop
loop:	mov	cx,280		;wait 1 ms
deloop:	loop	deloop
	dec	bx
	jnz	loop
	jmp	off

off: 	pop	ax		;recover speaker port value
	out	61h,al		;turn speaker off

	pop	ds		;put things back and return
	pop	bp
        ret


_sound	endp
_text    	ends
     	end

