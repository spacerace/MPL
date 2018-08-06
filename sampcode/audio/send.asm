; SEND.ASM
;
; Simple asm routine to take packaged request from C, point to it
; with ES:BX, and send it to the device driver.

?PLM = 0	; Use C calling conventions
?WIN = 0	; We're not windows
include cmacros.inc

sysdev	struc
sdevnext	dd	?	;Pointer to next device header
sdevatt 	dw	?	;Attributes of the device
sdevstrat	dw	?	;Strategy entry point
sdevint 	dw	?	;Interrupt entry point
sdevname	db	8 dup (?) ;Name of device (only first byte used for block)
sysdev	ends

sBegin data
dev_strat	dd	?
dev_int		dd	?
sEnd   data

sBegin code
	assumes cs, code
	assumes ds, data

cProc	send_req, PUBLIC, <si, di>
parmW	req
parmW	reqseg
parmW	drv
parmW	drvseg
cBegin	send_req
	mov	es,drvseg
	mov	word ptr dev_strat+2,es
	mov	word ptr dev_int+2,es
	mov	bx,drv
	mov	ax,es:[bx].sdevstrat
	mov	word ptr dev_strat,ax
	mov	ax,es:[bx].sdevint
	mov	word ptr dev_int,ax

	mov	bx,req
	mov	es,reqseg
	call	dword ptr [dev_strat]
	call	dword ptr [dev_int]
cEnd	send_req
sEnd	code
end
