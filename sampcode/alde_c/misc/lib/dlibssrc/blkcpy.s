*	char *blkcpy(dest, source, len)
*	register char *dest;
*	register char *source;
*	register int len;
*	/*
*	 *	Copies the <source> block to the <dest>.  <len> bytes are
*	 *	always copied.  No terminator is added to <dest>.  A pointer
*	 *	to <dest> is returned.
*	 */
*	{
*		register char *p = dest;
*	
*		if(source < dest) {
*			dest += len;
*			source += len;
*			while(len--)
*				*--dest = *--source;
*		}
*		else {
*			while(len--)
*				*dest++ = *source++;
*		}
*		return(p);
*	}

.text
.globl _lblkcpy
_lblkcpy:
	move.l	12(a7),d0	; number of bytes
	bra	blkcpy0
.globl _blkcpy
_blkcpy:
	move.w	12(a7),d0	; number of bytes
blkcpy0:
	move.l	4(a7),a1	; destination
	move.l	8(a7),a0	; source
	cmp.l	a0,a1		; check copy direction
	ble	blkcpy4
	add.l	d0,a0		; move pointers to end
	add.l	d0,a1
	bra	blkcpy2
blkcpy1:
	move.b	-(a0),-(a1)	; (s < d) copy loop
blkcpy2:
	dbra	d0,blkcpy1
	bra	blkcpy5
blkcpy3:
	move.b	(a0)+,(a1)+	; (s >= d) copy loop
blkcpy4:
	dbra	d0,blkcpy3
blkcpy5:
	move.l	4(a7),d0	; return destination pointer
	rts
