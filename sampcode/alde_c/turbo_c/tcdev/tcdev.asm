;History:1,1
;07-14-87 00:18:30 add a user-settable device attribute word
;07-13-87 23:30:27 use dgroup:offset, NOT offset
;07-13-87 23:28:57 change the definition of _BSS.
;07-13-87 23:26:10 move mystack to _BSS
;07-10-87 23:22:52 make a programmable-settable device name.

	INCLUDE c:\turboc\include\RULES.ASI

Header@

ExtProc@	device_driver, __CDECL__

DSeg@

ptrsav	dw	?,?			; request packet address

savess	dw	?			; old stack save
savesp	dw	?

DSegEnd@

_BSS	SEGMENT WORD PUBLIC 'BSS'
	dw	256 dup (?)
mystack	label	byte
_BSS	ends

_BSSEND SEGMENT
PubSym@		enddata, <LABEL BYTE>, __Cdecl__
_BSSEND ENDS

CSeg@

device_header	struc
		dw	-1,-1
device_attr	dw	?
device_strategy	dw	?
device_intr	dw	?
device_name	db	'        '
device_header	ends

	public	_dev_name
_dev_name	equ	$.device_name
	public	_dev_attr
_dev_attr	equ	$.device_attr
	device_header<,, strat, intr>
	db	0			;leave room for a trailing null.

strat proc	far
	mov	cs:ptrsav,bx
	mov	cs:ptrsav+2,es		; save es:bx as dword
	ret
strat endp

intr	proc	far

;	save everything

	push	ax

	mov	cs:savess,ss
	mov	cs:savesp,sp

	mov	ax,cs
	mov	ss,ax
	mov	sp,offset dgroup:mystack

	push	bx
	push	cx
	push	dx
	push	si
	push	di
	push	bp
	push	ds
	push	es

	mov	ax,cs
	mov	ds,ax

	push	ds:ptrsav+2
	push	ds:ptrsav
	call	device_driver@
	add	sp,4

	pop	es
	pop	ds
	pop	bp
	pop	di
	pop	si
	pop	dx
	pop	cx
	pop	bx

	mov	ss,cs:savess
	mov	sp,cs:savesp

	pop	ax
	ret

intr	endp

CSegEnd@

	end				; end of program
