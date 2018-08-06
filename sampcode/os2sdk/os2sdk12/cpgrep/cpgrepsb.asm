; Created by Microsoft Corp. 1986
name cpgrepsub

retlen	equ	2			; Size of return address on stack

dgroup	group	_data

extrn	_casesen:	word		; Case-sensitivity flag
extrn	_stringlist:	word		; Table of string lists
extrn	_target:	byte		; Target string
extrn	_targetlen:	word		; Length of target string
extrn	_transtab:	byte		; Translation table for _findlist

;	This segment is puposely word-aligned.  See note
;	in _findlist below.

_text	segment word public 'code'
	assume	cs:_text, ds:dgroup, es:nothing, ss:dgroup

; char			*findone(buffer,bufend)
; char			*buffer		/* Buffer in which to search */
; char			*bufend;	/* End of buffer */
;
; NOTE: targetlen MUST BE greater than zero

buffer	equ	word ptr [bp+retlen+2]
bufend	equ	word ptr [bp+retlen+4]

	EVEN

	public	_findone
_findone proc	near
	push	bp
	mov	bp,sp
	push	di
	push	si
	push	es
	push	ds			; ES = DS
	pop	es
	mov	cx,bufend		; CX = end of buffer
	mov	di,buffer		; ES:DI = buffer
	sub	cx,di			; CX = length of buffer
	jbe	sfnomatch		;  length less than or equal to zero
	mov	dx,_targetlen		; DX = length of target
	dec	dx			; Decrement it
	sub	cx,dx			; target must fit in buffer
	jbe	sfnomatch		;  (no match if buffer too short)

;	CX = buffer length
;	DX = target length (minus first character)
;	ES:DI = buffer pointer

sf0:	jcxz	sfnomatch		; No match if count zero
	mov	si,offset dgroup:_target ; DS:SI = target
	lodsb				; AL = first byte of target
	repne scasb			; Look for first character
	jne	sfnomatch		;  jump if not found
	mov	bx,cx			; BX = buffer length
	mov	ax,di			; AX = buffer pointer
	mov	cx,dx			; Get count for cmpsb
	or	cx,cx			; Zero? (JCXZ doesn't set flag)
	je	sf1			;  yes, skip compare
	repe cmpsb			; Do string compare
sf1:	mov	di,ax			; DI = buffer pointer
	mov	cx,bx			; CX = buffer length
	jne	sf0			; Loop if no match
	dec	ax			; AX = offset of start of match
	jmp	short sf4

sfnomatch:
	xor	ax,ax			; No match
sf4:	pop	es
	pop	si
	pop	di
	pop	bp
	ret
_findone endp


; int			revfind(s,c,slen)
; char			*s;		/* String to search */
; int			c;		/* Char to search for */
; int			slen;		/* Length of s */

s	equ	[bp+retlen+2]
c	equ	[bp+retlen+4]
slen	equ	[bp+retlen+6]

	EVEN

	public	_revfind
_revfind proc	near
	push	bp
	mov	bp,sp
	push	di
	push	es
	push	ds
	pop	es
	mov	di,s
	mov	ax,c
	mov	cx,slen
	jcxz	rf1
	std
	repne scasb
	cld
	mov	cx,s
	jne	rf0
	inc	di
rf0:	sub	cx,di
rf1:	mov	ax,cx
	pop	es
	pop	di
	pop	bp
	ret
_revfind endp


; int			countlines(start,finish)
; char			*start;
; char			*finish;

start	equ	[bp+retlen+2]
finish	equ	[bp+retlen+4]

	EVEN

	public	_countlines
_countlines proc near
	push	bp
	mov	bp,sp
	push	di
	push	es
	push	ds
	pop	es
	xor	dx,dx			; Accumulate count in DX
	mov	di,start		; ES:DI points to start
	mov	cx,finish		; Put length in CX
	sub	cx,di
	jbe	cl1			;  branch if no bytes
	mov	al,0Ah			; Search for linefeeds
cl0:	jcxz	cl1			; Exit loop if count zero
	repne scasb			; Do search
	jne	cl1			;  branch if none found
	inc	dx			; Increment count
	jmp	short cl0		; Loop
cl1:	mov	ax,dx			; Return line count in AX
	pop	es
	pop	di
	pop	bp
	ret
_countlines endp


; char			*findlist(buffer,bufend)
; char			*buffer;	/* Buffer to search */
; char			*bufend;	/* End of buffer */

savesi	equ	word ptr [bp-2]
endbyte	equ	byte ptr [bp-4]

stringnode struc
	s_alt	dw	?		; List of alternate portions
	s_suf	dw	?		; Pointer to suffix string list
	s_must	dw	?		; Length of portion that must match
stringnode ends

	EVEN

flworker dw	findsubi, findsub	; Worker dispatch table

	public	_findlist
_findlist proc	near
	ASSUME	DS:DGROUP, ES:NOTHING, SS:DGROUP

	push	bp
	mov	bp,sp
	sub	sp,4			; Make room for local vars
	push	di
	push	si
	push	ds
	pop	es
    ASSUME	ES:DGROUP

;	We mark the end of our search buffer with 0FFh so that
;	any comparisons that might run past the end of the buffer
;	will fail on the 0FFh.  We choose 0FFh so that if the
;	comparison fails on it, it will always appear as though
;	the string in the buffer is greater that the string in
;	the search list.  This will prevent us from stopping
;	the search too soon.  Of course, we must restore the byte
;	when we're done.

	mov	bx,bufend		; BX = end of buffer
	mov	al,0FFh			; End marker
	xchg	byte ptr [bx],al	; AL = byte after end of buffer
	mov	endbyte,al		; Save the byte

	mov	cx,bx			; CX = end of buffer
	mov	si,buffer		; SI = buffer
	sub	cx,si			; CX = buffer length
	jbe	fl1			;  no match if empty buffer
	mov	bx,offset dgroup:_transtab ; BX = translation table address

	mov	di,_casesen		; Get flag
	shl	di,1			; Scale to word index
	call	cs:flworker[di]		; Call helper
	jc	fl1			;  branch if no match

;	We have a match
;
;	SI = offset of first character past end of matched string
;	savesi = offset of first character past start of matched string

	mov	ax,savesi		; AX = 1st char past start
	dec	ax			; AX = start of matched string
	jmp	short fl2

;	We did not find a match

fl1:
	xor	ax,ax			; Return NULL

;	Restore end byte before leaving

fl2:
	mov	bx,bufend		; BX = end of buffer
	mov	dl,endbyte		; DL = end byte
	mov	[bx],dl			; Restore byte

	pop	si
	pop	di
	mov	sp,bp
	pop	bp
	ret

_findlist endp


;***	findsub - case-sensitive worker for _findlist
;
;	This function does most of the work for
;	case-sensitive multi-string searches.
;
;	ENTRY	BX = address of translation table
;		CX = number of bytes left in buffer
;		DS:SI = buffer pointer
;		SS:BP = pointer to stack frame for _findlist
;	EXIT	Carry set
;		    No match
;		Carry clear
;		    DS:SI = pointer to first character after match
;	USES	AX, CX, DX, DI, SI, Flags

	EVEN

	public	findsub, fs0, fs1, fs2, fs3, fs4, fs5, fs6
findsub	proc	near
	ASSUME	DS:DGROUP, ES:DGROUP, SS:DGROUP

fs0:
	xor	ax,ax			; AH = 0

;	AH = 0
;	BX = address of translation table
;	CX = number of bytes left in buffer
;	SI = buffer pointer
;	DS = ES = SS = DGROUP

fs1:
	lodsb				; Character in AL
	xlat byte ptr [bx]		; Translate character to index
	or	al,al			; Zero means invalid 1st byte
	loopz	fs1			;  if so, try next character

;	Either the zero bit is set, meaning the buffer is empty,
;	or the zero bit is clear, meaning we have a valid first
;	character.  Either way, CX has been decremented.

	jz	fs6			;  branch if buffer empty
	mov	savesi,si		; Save buffer pointer
	shl	ax,1			; Scale to word index
	mov	di,ax
	mov	di,_stringlist[di]	; DI points to string record
	or	di,di			; One byte match? (OR clears carry)
	jz	fs3			;  yes, skip ahead

;	Loop to search for match.
;	BX = address of translation table
;	DI = pointer to string record
;	SI = pointer into buffer

fs2:
	mov	cx,[di].s_must		; CX = length of string
	sub	di,cx			; DI = pointer to string
	mov	dx,si			; Save pointer to start of suffix
	repe cmpsb			; Strings match?
	ja	fs4			;  no, try alternate if follows
	jb	fs5			;  no, cannot be in this list
	add	di,cx			; DI = pointer to string record
	mov	di,[di].s_suf		; Get pointer to suffix string list
	or	di,di			; Is there one? (OR clears carry)
	jnz	fs2			;  yes, keep looking

;	Match found

fs3:
	ret				;  no, we have a match

;	Try alternate suffix

fs4:
	add	di,cx			; DI = pointer to string record
	mov	di,[di].s_alt		; Get pointer to alternate
	mov	si,dx			; Restore SI to start of suffix
	or	di,di			; Is there one?
	jnz	fs2			;  yes, loop

;	Try new first character

fs5:
	mov	cx,bufend		; CX = end of buffer
	mov	si,savesi		; Restore SI to saved value
	sub	cx,si			; CX = length of buffer
	ja	short fs0		; Try next character in buffer

;	No match

fs6:
	stc				; No match
	ret

findsub	endp


;***	findsubi - case-insensitive worker for _findlist
;
;	This function does most of the work for
;	case-insensitive multi-string searches.
;
;	ENTRY	BX = address of translation table
;		CX = number of bytes left in buffer
;		DS:SI = buffer pointer
;		SS:BP = pointer to stack frame for _findlist
;	EXIT	Carry set
;		    No match
;		Carry clear
;		    DS:SI = pointer to first character after match
;	USES	AX, CX, DX, DI, SI, Flags

	EVEN

	public	findsubi
findsubi proc	near
	ASSUME	DS:DGROUP, ES:DGROUP, SS:DGROUP

fsi0:
	xor	ax,ax			; AH = 0

;	AH = 0
;	BX = address of translation table
;	CX = number of bytes left in buffer
;	SI = buffer pointer
;	DS = ES = SS = DGROUP

fsi1:
	lodsb				; Character in AL
	xlat byte ptr [bx]		; Translate character to index
	or	al,al			; Zero means invalid 1st byte
	loopz	fsi1			;  if so, try next character

;	Either the zero bit is set, meaning the buffer is empty,
;	or the zero bit is clear, meaning we have a valid first
;	character.  Either way, CX has been decremented.

	jz	fsi7			;  branch if buffer empty
	mov	savesi,si		; Save buffer pointer
	shl	ax,1			; Scale to word index
	mov	di,ax
	mov	di,_stringlist[di]	; DI points to string record
	or	di,di			; One byte match? (OR clears carry)
	jz	fsi4			;  yes, skip ahead

;	Loop to search for match.
;	BX = address of translation table
;	DI = pointer to string record
;	SI = pointer into buffer

fsi2:
	mov	cx,[di].s_must		; CX = length of string
	sub	di,cx			; DI = pointer to string
	mov	dx,si			; Save pointer to start of suffix
fsi3:	lodsb				; Byte in AL, SI = SI + 1
	mov	ah,[di]			; Byte in AH, DI = DI + 1
	inc	di
	or	ax,2020h		; Fold bytes onto lower case
	cmp	al,ah			; Compare bytes
	loope	fsi3			; Loop while same
	ja	fsi5			;  no, try alternate if follows
	jb	fsi6			;  no, cannot be in this list
	add	di,cx			; DI = pointer to string record
	mov	di,[di].s_suf		; Get pointer to suffix string list
	or	di,di			; Is there one? (OR clears carry)
	jnz	fsi2			;  yes, keep looking

;	Match found

fsi4:
	ret				;  no, we have a match

;	Try alternate suffix

fsi5:
	add	di,cx			; DI = pointer to string record
	mov	di,[di].s_alt		; Get pointer to alternate
	mov	si,dx			; Restore SI to start of suffix
	or	di,di			; Is there one?
	jnz	fsi2			;  yes, loop

;	Try new first character

fsi6:
	mov	cx,bufend		; CX = end of buffer
	mov	si,savesi		; Restore SI to saved value
	sub	cx,si			; CX = length of buffer
	ja	short fsi0		; Try next character in buffer

;	No match

fsi7:
	stc				; No match
	ret

findsubi endp


; int			strnspn(s,t,n)
; char			*s;		/* String to search */
; char			*t;		/* Target list */
; int			n;		/* Length of s */

s	equ	word ptr [bp+retlen+2]
t	equ	word ptr [bp+retlen+4]
n	equ	word ptr [bp+retlen+6]

	EVEN

	public	_strnspn
_strnspn proc	near
	push	bp
	mov	bp,sp
	push	di
	push	si
	push	ds
	pop	es
	cld
	mov	bx,t			; BX = t
	mov	di,bx			; DI = t
	xor	al,al			; Search for 0 byte
	mov	cx,0FFFFh
	repne scasb
	dec	di			; Back up to 0
	sub	di,bx			; DI = length of t
	jz	spn1			; Done if length of t is 0
	mov	dx,di			; DX = length of t
	mov	si,s			; SI = s
	mov	cx,n			; CX = length of s
	jcxz	spn1			; Check for null string
	push	bp
spn0:	lodsb				; AL = next char in s
	mov	bp,cx			; BP = length of s
	mov	cx,dx			; CX = length of t
	mov	di,bx			; DI = t
	repne scasb			; Scan until match found
	mov	cx,bp			; CX = length of s
	loope	spn0			; Loop if match found
	pop	bp
	je	spn1			; Skip ahead if end of s reached
	dec	si			; Back up one char
spn1:	sub	si,s			; SI = length of prefix
	mov	ax,si			; AX = length of prefix
	pop	si
	pop	di
	pop	bp
	ret
_strnspn endp


; int			strncspn(s,t,n)
; char			*s;		/* String to search */
; char			*t;		/* Target list */
; int			n;		/* Length of s */

	EVEN

	public	_strncspn
_strncspn proc	near
	push	bp
	mov	bp,sp
	push	di
	push	si
	push	ds
	pop	es
	cld
	mov	bx,t			; BX = t
	mov	di,bx			; DI = t
	xor	al,al			; Search for 0 byte
	mov	cx,0FFFFh
	repne scasb
	dec	di			; Back up to 0
	sub	di,bx			; DI = length of t
	mov	ax,n			; Assume length of t is 0
	jz	cspn2			; Done if length of t is 0
	mov	dx,di			; DX = length of t
	mov	si,s			; SI = s
	mov	cx,ax			; CX = length of s
	jcxz	cspn1			; Check for null string
	push	bp
cspn0:	lodsb				; AL = next char in s
	mov	bp,cx			; BP = length of s
	mov	cx,dx			; CX = length of t
	mov	di,bx			; DI = t
	repne scasb			; Scan until match found
	mov	cx,bp			; CX = length of s
	loopne	cspn0			; Loop if match not found
	pop	bp
	jne	cspn1			; Skip ahead if end of s reached
	dec	si			; Back up one char
cspn1:	sub	si,s			; SI = length of prefix
	mov	ax,si			; AX = length of prefix
cspn2:	pop	si
	pop	di
	pop	bp
	ret
_strncspn endp


;	cmpsen - case-sensitive comparison
;
;	ENTRY	DS:SI = buffer
;		ES:DI = string
;		CX = length of string
;	EXIT	CX = length of string unused
;		DI = unused portion of string
;		Z set
;		    match found
;		Z clear
;		    no match
;	USES	CX, DI, SI, Flags

	EVEN

cmpsen	proc	near
	repe cmpsb
	ret
cmpsen	endp


;	cmpinsen - case-insensitive comparison
;
;	ENTRY	DS:SI = buffer
;		ES:DI = string
;		CX = length of string
;	EXIT	CX = length of string unused
;		DI = unused portion of string
;		Z set
;		    match found
;		Z clear
;		    no match
;	USES	AX, CX, DI, SI, Flags

	EVEN

cmpinsen proc	near
cmpi0:	lodsb				; Byte in AL, SI = SI + 1
	mov	ah,[di]			; Byte in AH, DI = DI + 1
	inc	di
	or	ax,2020h		; Fold bytes onto lower case
	cmp	al,ah			; Compare bytes
	loope	cmpi0			; Loop while same
	ret
cmpinsen endp


; void			matchstrings(s1,s2,len,nmatched,leg)
; char			*s1;		/* First string */
; char			*s2;		/* Second string */
; int			len;		/* Length */
; int			*nmatched;	/* Number of bytes matched */
; int			*leg;		/* Less than, equal, greater than */

cm_s1		equ	word ptr [bp+retlen+2]
cm_s2		equ	word ptr [bp+retlen+4]
cm_len		equ	word ptr [bp+retlen+6]
cm_nmatched	equ	word ptr [bp+retlen+8]
cm_leg		equ	word ptr [bp+retlen+10]

	EVEN

	public	_matchstrings
_matchstrings proc near
	ASSUME	DS:DGROUP, ES:NOTHING, SS:DGROUP

	push	bp
	mov	bp,sp
	push	di
	push	si
	push	ds
	pop	es
    ASSUME	ES:DGROUP
	mov	di,cm_s2
	mov	si,cm_s1
	mov	cx,cm_len
	cmp	_casesen,0
	je	cm0
	call	cmpsen
	jmp	short cm1
cm0:	call	cmpinsen
cm1:	mov	bx,cm_leg
	mov	word ptr [bx],0		; Assume equal
	jz	cm2			;  yes, skip ahead
	mov	word ptr [bx],1		; Assume greater than
	jg	cm1a			;  yes, skip ahead
	mov	word ptr [bx],-1	; Less than
cm1a:	dec	si
cm2:	sub	si,cm_s1
	mov	bx,cm_nmatched
	mov	[bx],si
	pop	si
	pop	di
	pop	bp
	ret

_matchstrings endp


; int			strcmp(s1,s2)
; char			*s1;		/* First string */
; char			*s2;		/* Second string */

	public	_strcmp
_strcmp	proc	near
	push	bp
	mov	bp,sp
	push	di
	push	si
	push	ds
	pop	es
	mov	si,[bp+4]		; DS:SI = s1
	mov	di,[bp+6]		; ES:DI = s2
sc0:	lodsb				; AL = *s1++
	scasb				; AL - *s2++
	jne	sc1			;  branch if no match
	or	al,al			; End of s1?
	jne	sc0			;  no, loop
	cbw				; AX = 0
	jmp	short sc2		; Exit
sc1:	mov	ax,1			; Assume s1 > s2
	jg	sc2			;  yes, branch
	neg	ax			; s1 < s2
sc2:	pop	si
	pop	di
	pop	bp
	ret
_strcmp	endp


	public	_bpt
_bpt	proc	near
	int	3
	ret
_bpt	endp

_text	ends

_data	segment word public 'data'
_data	ends

end
