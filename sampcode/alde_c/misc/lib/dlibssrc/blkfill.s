*	char *blkfill(dest, data, len)
*	register char *dest;
*	register char data;
*	register int len;
*	/*
*	 *	Fill <dest> will <len> bytes of <data>.  A pointer to <dest>
*	 *	is returned.
*	 */
*	{
*		register char *p = dest;
*	
*		while(len--)
*			*dest++ = data;
*		return(p);
*	}

.text
.globl _blkfill
_blkfill:
	move.l	4(a7),a0	* destination
	move.b	8(a7),d0	* fill data
	move.w	10(a7),d1	* number of bytes
	bra	blkfill2
blkfill1:
	move.b	d0,(a0)+	* fill loop
blkfill2:
	dbra	d1,blkfill1
	move.l	4(a7),d0	* return destination pointer
	rts
