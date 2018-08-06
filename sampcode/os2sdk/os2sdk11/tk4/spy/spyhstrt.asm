;  SPYHOOK library initialization routine
;  Created by Microsoft Corporation, 1989

?WIN=1	    ; Use Windows prolog/epilog
?PLM=1	    ; Use PLM calling convention
DOS5=1
.xlist
include cmacros.inc
.list

; Define some constants to help build stack trace
savedCS = 4
savedIP = 2
savedBP = 0
savedDS = -2

sBegin	DATA
assumes DS,DATA
public	__acrtused
__acrtused = 1
sEnd	DATA

sBegin	CODE
assumes cs,CODE
assumes ds,DATA

EXTRN	Init:NEAR
.286p

;
; Registers set up by DosLoadModule...
;
;   SI = heap size
;   DI = module ID
;   DS = library's automatic data segment
;
cProc	LoadProc,<FAR,PUBLIC>
cBegin	LoadProc

	push di
	call Init	      ; Init( hmod );

cEnd	LoadProc



; CopyStruct
;
; CopyStruct(pbSrc, pbDst, cb)
;
cProc	CopyStruct,<NEAR,PUBLIC>,<DS,SI,DI>
ParmD  pbSrc
ParmD  pbDest
ParmW  cb
cBegin

	mov	cx,cb
	jcxz	lcopydone	    ; all done if cb == 0

	mov	bx,seg_pbDest
	lar	ax,bx		    ; make sure we have access
	jnz	lcopyDone	    ; no access
	les	di,pbDest

	lsl	ax,bx		    ; get the segment limit
	mov	bx,di		    ; check range
	add	bx,cx		    ; ending byte to copy
	jc	lcopyDone	    ; overflowed
	cmp	ax,bx
	jc	lcopyDone	    ; no room at destination

	mov	bx,seg_pbSrc
	lar	ax,bx		    ; make sure we have access
	jnz	lcopyDone	    ; no access

	lds	si,pbSrc
	lsl	ax,bx		    ; get the segment limit
	mov	bx,si		    ; check range
	add	bx,cx		    ; ending byte to copy
	jc	lcopyDone	    ; overflowed
	cmp	ax,bx
	jc	lcopyDone	    ; Source is not big enough

	cmp	si,di
	jae	lcopyok
	mov	ax,cx
	dec	ax
	add	si,ax
	add	di,ax
	std
	rep	movsb
	cld
	jmp	short lcopydone
lcopyok:
	cld
	rep	movsb
lcopydone:
cEnd




cProc	BuildStackTrace,<NEAR,PUBLIC>,<SI,DI>
ParmD	pStackSave
ParmW	cCallsIgnore
ParmW	cCallsSave

cBegin
	les	di,pStackSave		; where to save away stack info
	mov	bx,bp
	mov	dx,ss
	lsl	dx,dx
;
; First we need to ignore the defined number of entries
	mov	cx,cCallsIgnore
	jcxz	SaveLoop		; Dont ignore any?
NextBPToIgnore:
	and	bx,0FFFEh		; Flush any INC BP bit
	jz	SaveLoop		; End of chain if zero
	cmp	bx,dx			; BP outside of stack segment?
	jae	SaveLoop		; Yes, end of chain
	mov	si,bx			; Save current BP
	mov	bx,ss:[bx].savedBP	; SS:BX -> next frame in BP chain
	and	bl,0FEh 		; Flush any INC BP bit
	cmp	bx,si			; savedBP valid?
	jbe	SaveLoop		; No, end of chain
	dec	cx			; decrement count of calls to ignore
	jnz	NextBPToIgnore		; Process the next one

;
; This loop will save away the specified number of calls into the
; passed in save area

SaveLoop:
	mov	cx,cCallsSave		; get count of calls to save

NextBPToSave:
	and	bx,0FFFEh		; Flush any INC BP bit
	jz	EndBPChain		    ; End of chain if zero
	cmp	bx,dx			; BP outside of stack segment?
	jae	EndBPChain		    ; Yes, end of chain
; see if short or long call and setup selLast
	mov	si,ss:[bx].savedCS	; Get the code segment
	lar	ax,si			; get the access rights
	jnz	CSNotValid		; No access assume not selector
	and	ah,018h 		; see if code segment
	cmp	ah,018h 		;
	jz	CSValid 		; code segment, so  save it

CSNotValid:
	xor	si,si			; Not valid, set selector to NULL
CSValid:
	mov	ax,ss:[bx].savedIP	; save away IP of caller
	stosw
	mov	ax,si			; and save away the code selector
	stosw
	dec	cx			; decrement count of how many to save

	mov	si,bx			; Save current BP
	mov	bx,ss:[bx].savedBP	; SS:BX -> next frame in BP chain
	and	bl,0FEh 		; Flush any INC BP bit
	cmp	bx,si			; savedBP valid?
	jbe	EndBPChain		; No
	jcxz	StackOverflowsSave	; Stack is deeper than save area

	jmp	NextBPToSave
;
; We reached the end of the BP chain before we saved the specified number
; of stack items.  zero out the next item to signal caller how many items
; were filled in

EndBPChain:
	jcxz	FillInSelectors
	xor	ax,ax			; Ended BP Chain Before count
	stosw				; Put A 0:0 to signal end
	stosw
	sub	di,4			; realign DI to last saved call
	mov	ax,es:[di-2]		; ax = Selector of last saved call
	jmp	FillInSelectors 	;

;
; We saved the specified number of items, before we reached the end of the
; BP chain.  If the last item we saved was a local call, we should continue
; to go through the stack until we find a valid far return address.

StackOverflowsSave:
	or	ax,ax			; Was last call local?
	jnz	FillInSelectors 	;

; loop to locate a FAR return address
NextBPToCheck:
	and	bx,0FFFEh		; Flush any INC BP bit
	jz	FarReturnNotFound	; End of chain if zero
	cmp	bx,dx			; BP outside of stack segment?
	jae	FarReturnNotFound	; Yes, end of chain
; see if short or long call and setup selLast
	mov	ax,ss:[bx].savedCS	; Get the code segment
	lar	cx,ax			; get the access rights
	jnz	CSNotValid2		; No access assume not selector
	and	ch,018h 		; see if code segment
	cmp	ch,018h 		;
	jz	FillInSelectors 	; ax has valid code selector, use it

CSNotValid2:
	mov	si,bx			; Save current BP
	mov	bx,ss:[bx].savedBP	; SS:BX -> next frame in BP chain
	and	bl,0FEh 		; Flush any INC BP bit
	cmp	bx,si			; savedBP valid?
	jg	NextBPToCheck
FarReturnNotFound:
	mov	ax,0ffffh		; use special value to show

;
; Loop through and convert all Short return addresses into long return
; addresses
;
FillInSelectors:
	mov	cx,off_pStackSave
FillInLoop:
	cmp	di,cx			; have we back tracked all the way?
	jz	FillInEnd		; Yes
	sub	di,4			; point back to previous item
	cmp	WORD PTR es:[di+2],0	; is the selector zero?
	jz	LocalReturn		; Yes, local return
	mov	ax,es:[di+2]		; No FAR return, save CS selector
	jmp	FillInLoop
LocalReturn:
	mov	es:[di+2],ax		; Make long return using saved CS
	jmp	FillInLoop

FillInEnd:
cEnd

sEnd	CODE

end	LoadProc
