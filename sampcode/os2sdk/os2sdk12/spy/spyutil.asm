;  SPY Assembly language utility functions.

?WIN=1	    ; Use Windows prolog/epilog
?PLM=1	    ; Use PLM calling convention
DOS5=1
.xlist
include cmacros.inc
.list



EXTRN	DOSCHGFILEPTR:FAR
EXTRN	DOSREAD:FAR


sBegin	CODE
assumes cs,CODE
assumes ds,DATA
.286p

; FValidPointer
;
; FValidPointer(VOID FAR *pVoid, SHORT cbStruct)
;
cProc	FValidPointer,<NEAR,PUBLIC>
ParmD  pVoid
ParmW	cbStruct
cBegin
	xor	ax,ax			; Assume Bad address
	mov	bx,SEG_pVoid		; Get the selector of the address
	lar	cx,bx			; make sure we have access
	jnz	NoAccess		; no access

	lsl	cx,bx			; get the segment limit
	mov	bx,OFF_pVoid		; check range
	add	bx,cbStruct		; Add number of bytes on
	jc	NoAccess		; overflowed
	cmp	cx,bx
	jc	NoAccess		; Would overflow, again bad pointer
	inc	ax			; Return No zero for TRUE

NoAccess:
cEnd



; FGuessValidPointer
;
; FGuessValidPointer(VOID FAR *pVoid, SHORT cbStruct)
; Since a pointer may have come from a different process, we can not
; actually validate the pointer.  The Best we can do is simply look
; at the selector, and see if it looks reasonable.  For now simply check
; that the selector is for ring 3.  Ie the low order two bits are set.
;
cProc	FGuessValidPointer,<NEAR,PUBLIC>
ParmD  pVoid
ParmW	cbStruct
cBegin
	xor	ax,ax			; Assume Bad address
	mov	bx,SEG_pVoid		; Get the selector of the address
	and	bx,03h			; only look at two low order bits
	cmp	bx,03h			; Are they both set
	jnz	NoGVAccess		; not ring 3 assume no access
	inc	ax			; Return No zero for TRUE

NoGVAccess:
cEnd



cProc	DebugFileSeek,<PUBLIC,NEAR,PASCAL>
    parmW   fh
    parmD   amt
    parmW   typ
cBegin
	push	ax
	push	ax
	mov	bx,sp
	push	fh
	mov	dx,SEG_amt
	mov	ax,OFF_amt
	mov	cx,typ
	cmp	cx,2
	jle	noshift
shiftamt:
	shl	ax,1
	rcl	dx,1
	loop	shiftamt
noshift:
	push	dx
	push	ax
	push	cx
	push	ss
	push	bx
	call	DOSCHGFILEPTR
	or	ax,ax
	pop	ax
	pop	dx
	jz	seek_done
	xor	ax,ax
	xor	dx,dx
seek_done:
cEnd



cProc	DebugFileRead,<PUBLIC,NEAR,PASCAL>
    parmW   fh
    parmD   lpBuf
    parmW   nBytes
cBegin
	push	ax
	mov	ax,sp
	push	fh
	push	SEG_lpBuf
	push	OFF_lpBuf
	push	nBytes
	push	ss
	push	ax
	call	DOSREAD
	or	ax,ax
	pop	ax
	jz	read_done
	xor	ax,ax
read_done:
cEnd

cProc lstrcat,<PUBLIC>,<SI,DI,DS>
    parmD  szDest
    parmD  szSource

cBegin	lstrcat

; first lets get the length of the source string and setup pointer to
; source string

	les	di,szSource
	mov	cx,-1		; count the bytes negatively
	xor	ax,ax		; Look for a null

	repne scasb		; find null byte & get source length
	inc	cx		; cx=-count of bytes in source string
				;    (including null)
	neg	cx		; cx=+count
	mov	bx,cx		; save the count

	lds	si,szSource	; now setup Source pointer

; now find the end of destination string.
	les	di,szDest
	xor	ax,ax
	mov	cx,0ffffH
	repne scasb
	dec	di		; di points to dest null terminator

	mov	cx,bx		; restore count of bytes to copy

	rep movsb		; concatenate the strings

cEnd	lstrcat

sEnd	CODE
end
