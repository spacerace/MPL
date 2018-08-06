; The following example interrogates the mouse driver and displays the
; results.

gotmsg	db	"mouse driver found", 0dh, 0ah, 24h
nopmsg	db	"mouse drive not found", 0dh, 0ah, 24h
revmsg	db	"revision $"
crlf	db	0dh, 0ah, 24h

ten	db	10

	mov	bx, 0			; must be 0 for this call
	mov	ah, 0fah		; fa = interrogate driver
	int	10h			; interrogate!
	or	bx, bx			; bx = 0 ?
	jnz	found			; branch if driver present
	mov	dx, offset nopmsg	; assume nopmsg in data segment
	mov	ah, 09h			; 9 = print string
	int	21h			; output not found message
	jmp	continue		; that's all for now

found:	mov	dx, offset gotmsg	; assume gotmsg in data segment
	mov	ah, 09h			; 9 = print string
	int	21h			; output found message
	mov	dx, offset revmsg	; assume revmsg in data segment
	mov	ah, 09h			; 9 = print string
	int	21h			; output "revision"
	mov	dl, es:[bx]		; dl = major release number
	add	dl, "0"			; convert to ascii
	mov	ah, 2			; 2 = display character
	int	21h			; output major release number
	mov	dl, "."			; dl = "."
	mov	ah, 2			; 2 = display character
	int	21h			; output a period
	mov	al, es:[bx+1]		; al = minor release number
	xor	ah, ah			; ah = 0
	idiv	ten			; al = 10ths, ah = 100ths
	mov	bx, ax			; save ax in bx
	mov	dl, al			; dl = 10ths
	add	dl, "0"			; convert to ascii
	mov	ah, 2			; 2 = display character
	int	21h			; output minor release 10ths
	mov	dl, bh			; dl = 100ths
	add	dl, "0"			; convert to ascii
	mov	ah, 2			; 2 = display character
	int	21h			; output minor release 100ths
	mov	dx, offset crlf		; assume crlf in data segment
	mov	ah, 09h			; 9 = print string
	int	21h			; output end of line

continue:				; the end
