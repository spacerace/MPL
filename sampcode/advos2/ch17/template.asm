	title	TEMPLATE -- Sample Device Driver
        page    55,132
        .286

;
; TEMPLATE.ASM
;
; A sample OS/2 character device driver.  The driver command code
; routines are stubs only and have no effect but to return a
; nonerror "done" status.
;
; Assemble with:  C> masm template.asm;
; Link with:  C> link template,template.sys,,os2,template
;
; To install the driver, add "DEVICE=TEMPLATE.SYS" to CONFIG.SYS
; and reboot.
;
; Copyright (C) 1988 Ray Duncan
;

maxcmd  equ     26              ; maximum allowed command code

stdin   equ     0               ; standard device handles
stdout  equ     1
stderr  equ     2

cr      equ     0dh             ; ASCII carriage return
lf	equ	0ah		; ASCII linefeed

        extrn   DosWrite:far


DGROUP  group   _DATA

_DATA   segment word public 'DATA'

                                ; device driver header...
header  dd      -1              ; link to next device driver
        dw      8880h           ; device attribute word
        dw      Strat           ; Strategy entry point
	dw	0		; IDC entry point
        db      'TEMPLATE'      ; logical device name
	db	8 dup (0)	; reserved

devhlp  dd      ?               ; DevHlp entry point

wlen    dw      ?               ; receives DosWrite length 

                                ; Strategy routine dispatch table
				; for request packet command code...
dispch  dw      Init            ; 0  = initialize driver
        dw      MediaChk        ; 1  = media check
        dw      BuildBPB        ; 2  = build BIOS parameter block
        dw      Error           ; 3  = not used
        dw      Read            ; 4  = read from device
	dw	NdRead		; 5  = nondestructive read
        dw      InpStat         ; 6  = return input status
        dw      InpFlush        ; 7  = flush device input buffers
        dw      Write           ; 8  = write to device
        dw      WriteVfy        ; 9  = write with verify
        dw      OutStat         ; 10 = return output status
        dw      OutFlush        ; 11 = flush output buffers
        dw      Error           ; 12 = not used
        dw      DevOpen         ; 13 = device open
        dw      DevClose        ; 14 = device close
	dw	RemMedia	; 15 = removable media
        dw      GenIOCTL        ; 16 = generic IOCTL
        dw      ResetMed        ; 17 = reset media
        dw      GetLogDrv       ; 18 = get logical drive
        dw      SetLogDrv       ; 19 = set logical drive
        dw      DeInstall       ; 20 = de-install
        dw      Error           ; 21 = not used
        dw      PartFD          ; 22 = partitionable fixed disks
        dw      FDMap           ; 23 = get fixed disk unit map
        dw      Error           ; 24 = not used
        dw      Error           ; 25 = not used
        dw      Error           ; 26 = not used

ident   db      cr,lf,lf
	db	'TEMPLATE Sample OS/2 Device Driver'
        db      cr,lf
ident_len equ $-ident

_DATA   ends


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP,es:NOTHING

Strat   proc    far             ; Strategy entry point
                                ; ES:BX = request packet address

        mov     di,es:[bx+2]    ; get command code from packet
        and     di,0ffh
        cmp     di,maxcmd       ; supported by this driver?
        jle     Strat1          ; jump if command code OK

        call    Error           ; bad command code
        jmp     Strat2

Strat1: add     di,di           ; branch to command code routine
        call    word ptr [di+dispch]

Strat2: mov     es:[bx+3],ax    ; status into request packet
        ret                     ; back to OS/2 kernel

Strat   endp


Intr    proc  far               ; driver Interrupt handler

        clc                     ; signal we owned interrupt
        ret                     ; return from interrupt

Intr    endp


; Command code routines are called by the Strategy routine
; via the Dispatch table with ES:BX pointing to the request
; header.  Each routine should return ES:BX unchanged
; and AX = status to be placed in request packet:
; 0100H if 'done' and no error
; 0000H if thread should block pending interrupt
; 81xxH if 'done' and error detected (xx=error code)

MediaChk proc   near            ; function 1 = media check
        
        mov     ax,0100h        ; return 'done' status
        ret

MediaChk endp


BuildBPB proc   near            ; function 2 = build BPB

        mov     ax,0100h        ; return 'done' status
        ret

BuildBPB endp


Read    proc    near            ; function 4 = read

        mov     ax,0100h        ; return 'done' status
        ret

Read    endp

 
NdRead	proc	near		; function 5 = nondestructive read

        mov     ax,0100h        ; return 'done' status
        ret

NdRead  endp


InpStat proc    near            ; function 6 = input status

        mov     ax,0100h        ; return 'done' status
        ret

InpStat endp


InpFlush proc   near            ; function 7 = flush input buffers

        mov     ax,0100h        ; return 'done' status
        ret

InpFlush endp


Write   proc    near            ; function 8 = write

        mov     ax,0100h        ; return 'done' status
        ret

Write   endp


WriteVfy proc   near            ; function 9 = write with verify

        mov     ax,0100h        ; return 'done' status
        ret
        
WriteVfy endp


OutStat proc    near            ; function 10 = output status

        mov     ax,0100h        ; return 'done' status
        ret

OutStat endp


OutFlush proc   near            ; function 11 = flush output buffers

        mov     ax,0100h        ; return 'done' status
        ret

OutFlush endp


DevOpen proc    near            ; function 13 = device open

        mov     ax,0100h        ; return 'done' status
        ret

DevOpen endp
        

DevClose proc   near            ; function 14 = device close

        mov     ax,0100h        ; return 'done' status
        ret

DevClose endp


RemMedia proc   near            ; function 15 = removable media

        mov     ax,0100h        ; return 'done' status
        ret

RemMedia endp


GenIOCTL proc   near            ; function 16 = generic IOCTL

        mov     ax,0100h        ; return 'done' status
        ret

GenIOCTL endp


ResetMed proc   near            ; function 17 = reset media

        mov     ax,0100h        ; return 'done' status
        ret

ResetMed endp


GetLogDrv proc  near            ; function 18 = get logical drive

        mov     ax,0100h        ; return 'done' status
        ret

GetLogDrv endp


SetLogDrv proc  near            ; function 19 = set logical drive

        mov     ax,0100h        ; return 'done' status
        ret

SetLogDrv endp


DeInstall proc	near		; function 20 = deinstall driver

        mov     ax,0100h        ; return 'done' status
        ret

DeInstall endp


PartFD  proc    near            ; function 22 = partitionable 
                                ;               fixed disk 
        mov     ax,0100h        ; return 'done' status
        ret

PartFD  endp


FDMap   proc    near            ; function 23 = get fixed disk
                                ;               logical unit map
        mov     ax,0100h        ; return 'done' status
        ret

FDMap   endp


Error   proc    near            ; bad command code

        mov     ax,8103h        ; error bit + 'done' status
                                ; + "Unknown Command" code
        ret

Error   endp


Init    proc    near            ; function 0 = initialize

        mov     ax,es:[bx+14]   ; get DevHlp entry point
        mov     word ptr devhlp,ax
        mov     ax,es:[bx+16]
        mov     word ptr devhlp+2,ax

                                ; set offsets to end of code
                                ; and data segments
        mov     word ptr es:[bx+14],offset _TEXT:Init
        mov     word ptr es:[bx+16],offset DGROUP:ident

                                ; display sign-on message...
        push    stdout          ; standard output handle
        push    ds              ; address of message
        push    offset DGROUP:ident
        push    ident_len       ; length of message
        push    ds              ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite        ; transfer to OS/2
                                      
        mov     ax,0100h        ; return 'done' status
        ret

Init    endp

_TEXT   ends

        end
