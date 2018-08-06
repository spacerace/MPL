; A test of interfacing a C callable masm function.
; This function simply adds 20 to int parm passed.
; and returns it to C.

	name	cadd

public	_cadd, _cons

DGROUP	group	_DATA
	assume	ds:DGROUP

_DATA	segment	word	public	'DATA'
_cons	dw	20
_DATA	ends

_TEXT	segment	byte	public	'CODE'
	assume	cs:_TEXT

_cadd   proc    near
        push    bp
        mov     bp, sp
        push    di
        push    si
        mov     ax, [bp+4]
        add     ax, _cons
        pop     si
        pop     di
        mov     sp, bp
        pop     bp
        ret
_cadd   endp

_TEXT   ends
        end
