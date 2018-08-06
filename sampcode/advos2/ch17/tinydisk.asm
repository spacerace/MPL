	title	TINYDISK -- RAM Disk Device Driver
        page    55,132
        .286

;
; TINYDISK.ASM
;
; A sample OS/2 block device driver that installs a 64 KB RAM disk.
;
; Assemble with:  C> masm tinydisk.asm;
; Link with:  C> link tinydisk,tinydisk.sys,,os2,tinydisk
;
; To install the driver, add "DEVICE=TINYDISK.SYS" to CONFIG.SYS
; and reboot.
;
; Copyright (C) 1988 Ray Duncan
;

maxcmd  equ     26              ; maximum allowed command code

secsize equ     512             ; bytes/sector, IBM compatible media

stdin   equ     0               ; standard device handles
stdout  equ     1
stderr  equ     2

cr      equ     0dh             ; ASCII carriage return
lf	equ	0ah		; ASCII linefeed

PhysToVirt   equ 15h            ; DevHlp services
UnPhysToVirt equ 32h
VerifyAccess equ 27h
AllocPhys    equ 18h

        extrn   DosWrite:far


DGROUP  group   _DATA

_DATA   segment word public 'DATA'

				; device driver header...
header  dd      -1              ; link to next device driver
        dw      0080h           ; device attribute word
                                ; bits 7-9 = driver level
        dw      Strat           ; Strategy entry point
	dw	0		; reserved
        db      0               ; units (set by OS/2)
	db	15 dup (0)	; reserved

devhlp  dd      ?               ; DevHlp entry point

wlen    dw      ?               ; receives DosWrite length 

dbase   dd      ?               ; 32-bit physical address,
				; base of RAM disk storage

xfrsec  dw      0               ; current sector for transfer
xfrcnt  dw      0               ; sectors successfully transferred
xfrreq  dw      0               ; number of sectors requested   
xfraddr dd      0               ; working address for transfer 

array   dw      bpb             ; array of pointers to BPB 
                                ; for each supported unit

bootrec equ     $               ; logical sector 0 boot record
        jmp     $               ; JMP at start of boot sector,
	nop			; this field must be 3 bytes
	db	'IBM 10.1'	; OEM identity field
                                ; ---BIOS Parameter Block-----
bpb     dw      secsize         ; 00H bytes per sector
        db      1               ; 02H sectors per cluster
        dw      1               ; 03H reserved sectors
        db      1               ; 05H number of FATs
        dw      64              ; 06H root directory entries
        dw      128             ; 08H total sectors 
        db      0f8h            ; 0AH media descriptor
        dw      1               ; 0BH sectors per FAT
        dw      1               ; 0DH sectors per track
        dw      1               ; 0FH number of heads
        dd      0               ; 11H hidden sectors
        dd      0               ; 15H large total sectors (if 
				;     word at offset 08H = 0)
        db      6 dup (0)       ; 19H reserved
                                ; ---End of BPB, 31 bytes-----
bootrec_len equ $-bootrec       ; length of boot sector data
                                ; additional words needed by
				; Generic IOCTL Cat 8 Function 63H
                                ; Get Device Parameters call
        dw      0               ; number of cylinders
        db      7               ; device type = unknown
        dw      1               ; device attribute word
gdprec_len equ $-bpb            ; length of Generic IOCTL buffer 

                                ; Strategy routine dispatch table
				; for request packet command code...
dispch  dw      Init            ; 0  = initialize driver
        dw      MediaChk        ; 1  = media check on block device
        dw      BuildBPB        ; 2  = build BIOS parameter block
        dw      Error           ; 3  = reserved
        dw      Read            ; 4  = read (input) from device
	dw	Error		; 5  = nondestructive read
        dw      Error           ; 6  = return input status
        dw      Error           ; 7  = flush device input buffers
        dw      Write           ; 8  = write (output) to device
        dw      Write           ; 9  = write with verify
        dw      Error           ; 10 = return output status
        dw      Error           ; 11 = flush output buffers
        dw      Error           ; 12 = reserved
        dw      Error           ; 13 = device open
        dw      Error           ; 14 = device close
	dw	Error		; 15 = removable media
        dw      GenIOCTL        ; 16 = generic IOCTL
        dw      Error           ; 17 = reset media
        dw      GSLogDrv        ; 18 = get logical drive
        dw      GSLogDrv        ; 19 = set logical drive
	dw	Error		; 20 = deinstall
        dw      Error           ; 21 = reserved
        dw      Error           ; 22 = partitionable fixed disks
        dw      Error           ; 23 = get fixed disk unit map
        dw      Error           ; 24 = reserved
        dw      Error           ; 25 = reserved
        dw      Error           ; 26 = reserved

                                ; start of data discarded
                                ; after initialization

ident	db	cr,lf,lf	; successful installation message...
	db	'TINYDISK 64 KB RAM disk for OS/2'
        db      cr,lf
	db	'RAM disk will be drive '
drive   db      'X:'
        db      cr,lf
ident_len equ $-ident

abort   db      cr,lf           ; aborted installation message
	db	'TINYDISK installation aborted'
        db      cr,lf
abort_len equ $-abort

volname db	'TINYDISK   '	; volume label for RAM disk
        db      08h             ; attribute byte
        db      10 dup (0)      ; reserved area
        dw      0               ; time = 00:00
        dw      1021h           ; date = January 1, 1988
        db      6 dup (0)       ; reserved area
volname_len equ $-volname

_DATA   ends


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP,es:NOTHING

Strat   proc    far             ; Strategy entry point
                                ; ES:BX = request packet

        mov     di,es:[bx+2]    ; get command code from packet
        and     di,0ffh
        cmp     di,maxcmd       ; supported by this driver?
        jle     Strat1          ; jump if command code OK

        call    Error           ; bad command code
        jmp     Strat2

Strat1: add     di,di           ; go to command code routine
        call    word ptr [di+dispch]

Strat2:                         ; return with AX = status
        mov     es:[bx+3],ax    ; put status in request packet
        ret                     ; return to OS/2 kernel

Strat   endp


; Command code routines are called by the Strategy routine
; via the Dispatch table with ES:BX pointing to the request
; header.  Each routine should return ES:BX unchanged,
; and AX = status to be placed in request packet:
; 0100H if 'done' and no error
; 0000H if thread should block pending interrupt
; 81xxH if 'done' and error detected (xx = error code)


MediaChk proc   near            ; function 1 = media check
        
                                ; return 'not changed' code
        mov     byte ptr es:[bx+0eh],1

        mov     ax,0100h        ; return 'done' status
        ret

MediaChk endp


BuildBPB proc   near            ; function 2 = build BPB

                                ; put BPB address into
                                ; request packet
        mov     word ptr es:[bx+12h],offset DGROUP:bpb
        mov     word ptr es:[bx+14h],ds

        mov     ax,0100h        ; return 'done' status
        ret

BuildBPB endp


Read    proc    near            ; function 4 = read

        push    es              ; save request packet address
        push    bx

        call    setup           ; set up transfer variables 

Read1:  mov     ax,xfrcnt       ; done with all sectors yet?
        cmp     ax,xfrreq
        je      read2           ; jump if transfer completed

        mov     ax,ds           ; set ES = DGROUP
        mov     es,ax

        mov     ax,xfrsec       ; get sector number
        call    MapDS           ; and map it to DS:SI
                                ; (may force mode switch)

        push    es              ; save DGROUP selector

                                ; convert destination physical
				; address to virtual address...
        mov     bx,word ptr es:xfraddr
        mov     ax,word ptr es:xfraddr+2
        mov     cx,secsize      ; segment length
        mov     dh,1            ; leave result in ES:DI
        mov     dl,PhysToVirt   ; function number
        call    es:devhlp       ; transfer to kernel

        mov     cx,secsize      ; transfer logical sector from
	cld			; RAM disk to requestor
        rep movsb

        pop     ds              ; restore DGROUP addressing

	sti			; PhysToVirt may mask interrupts

        inc     xfrsec          ; advance sector number

                                ; advance transfer address
        add     word ptr xfraddr,secsize
        adc     word ptr xfraddr+2,0

        inc     xfrcnt          ; count sectors transferred
        jmp     read1

Read2:                          ; all sectors transferred

        mov     dl,UnPhysToVirt ; function number
        call    devhlp          ; transfer to kernel

        pop     bx              ; restore request packet address
        pop     es

        mov     ax,xfrcnt       ; put actual transfer count
        mov     es:[bx+12h],ax  ; into request packet

        mov     ax,0100h        ; return 'done' status
        ret

Read    endp


Write   proc    near            ; functions 8,9 = write

        push    es              ; save request packet address
        push    bx

        call    setup           ; set up transfer variables 

Write1: mov     ax,xfrcnt       ; done with all sectors yet?
        cmp     ax,xfrreq
        je      write2          ; jump if transfer completed

        mov     ax,xfrsec       ; get sector number
        call    MapES           ; map it to ES:DI
                                ; (may force mode switch)

        push    ds              ; save DGROUP selector

                                ; convert source physical
				; address to virtual address...
        mov     bx,word ptr xfraddr
        mov     ax,word ptr xfraddr+2
        mov     cx,secsize      ; segment length
        mov     dh,0            ; leave result in DS:SI
        mov     dl,PhysToVirt   ; function number
        call    devhlp          ; transfer to kernel

        mov     cx,secsize      ; transfer logical sector from
	cld			; requestor to RAM disk
        rep movsb

        pop     ds              ; restore DGROUP addressing
        sti                     ; PhysToVirt might have masked
        inc     xfrsec          ; advance sector number

                                ; advance transfer address
        add     word ptr xfraddr,secsize
        adc     word ptr xfraddr+2,0

        inc     xfrcnt          ; count sectors transferred
        jmp     write1

Write2:                         ; all sectors transferred

        mov     dl,UnPhysToVirt ; function number
        call    devhlp          ; transfer to kernel

        pop     bx              ; restore request packet address
        pop     es

        mov     ax,xfrcnt       ; put actual transfer count
        mov     es:[bx+12h],ax  ; into request packet

        mov     ax,0100h        ; return 'done' status
        ret

Write   endp


GenIOCTL proc   near            ; function 16 = generic IOCTL

        push    es              ; save request packet address
        push    bx

        mov     ax,8103h        ; assume unknown command

                                ; Get Device Parameters call?
        cmp     byte ptr es:[bx+0dh],8
	jne	Gen9		; no, set 'done' + 'error'
        cmp     byte ptr es:[bx+0eh],63h
	jne	Gen9		; no, set 'done' + 'error'

                                ; verify user's access
        mov     ax,es:[bx+15h]  ; selector
        mov     di,es:[bx+13h]  ; offset 
        mov     cx,gdprec_len   ; length to be written
        mov     dh,1            ; need read/write access
        mov     dl,VerifyAccess ; function number
        call    devhlp          ; transfer to kernel

        mov     ax,810ch        ; if no access, exit with
	jc	Gen9		; 'done' + general failure

                                ; get destination address
        les     di,dword ptr es:[bx+13h]
                                ; copy device info to caller
        mov     si,offset DGROUP:bpb
        mov     cx,gdprec_len   ; length of parameter block
        cld
        rep movsb

        mov     ax,0100h        ; set 'done' status

Gen9:   pop     bx              ; restore request packet address
        pop     es
        ret                     ; and return with status

GenIOCTL endp


GSLogDrv proc   near            ; function 18, 19 = get
                                ; or set logical drive

                                ; return code indicating there
                                ; are no logical drive aliases
        mov     byte ptr es:[bx+1],0

        mov     ax,0100h        ; return 'done' status
        ret

GSLogDrv endp


Error   proc    near            ; bad command code 

        mov     ax,8103h        ; error bit + 'done' status
        ret                     ; + "Unknown Command" code

Error   endp


MapES   proc    near            ; map sector number to 
                                ; virtual memory address
                                ; call with AX = logical sector
                                ; return ES:DI = memory address
                                ;        and Carry clear
                                ; or     Carry set if error

        mov     di,secsize      ; bytes per sector
        mul     di              ; * logical sector number
        xchg    ax,dx           ; AX:BX := 32-bit physical
        mov     bx,dx           ; sector address
        add     bx,word ptr dbase
        adc     ax,word ptr dbase+2

        mov     cx,secsize
        mov     dh,1            ; result to ES:DI
        mov     dl,PhysToVirt   ; function number
        call    devhlp          ; transfer to kernel

        ret                     ; return ES:DI = sector address

MapES   endp


MapDS   proc    near            ; map sector number to 
                                ; virtual memory address 
                                ; call with AX = logical sector
                                ; return DS:SI = memory address
                                ;        and Carry clear
                                ; or     Carry set if error

        mov     si,secsize      ; bytes per sector
        mul     si              ; * logical sector number
        xchg    ax,dx           ; AX:BX := 32-bit physical
        mov     bx,dx           ; sector address
        add     bx,word ptr dbase
        adc     ax,word ptr dbase+2

        mov     cx,secsize
        mov     dh,0            ; result to DS:SI
        mov     dl,PhysToVirt   ; function number
        call    devhlp          ; transfer to kernel

        ret                     ; return DS:SI = sector address

MapDS   endp


Setup   proc    near            ; set up for read/write
                                ; ES:BX = request packet
                                ; extracts address, start, count

        mov     ax,es:[bx+14h]  ; starting sector number
        mov     xfrsec,ax

        mov     ax,es:[bx+12h]  ; sectors requested
        mov     xfrreq,ax

        mov     ax,es:[bx+0eh]  ; requestor's buffer address
        mov     word ptr xfraddr,ax
        mov     ax,es:[bx+10h]
        mov     word ptr xfraddr+2,ax

        mov     xfrcnt,0        ; initialize sectors 
                                ; transferred counter
        ret

Setup   endp

                                ; start of code discarded
                                ; after initialization

Init    proc    near            ; function 0 = initialize

        mov     ax,es:[bx+0eh]  ; get DevHlp entry point
        mov     word ptr devhlp,ax
        mov     ax,es:[bx+10h]
        mov     word ptr devhlp+2,ax

        mov     al,es:[bx+16h]  ; unit code for this drive
        add     al,'A'          ; convert to ASCII and place
	mov	drive,al	; in output string

                                ; display sign-on message...
        push    stdout          ; handle for standard output
        push    ds              ; address of message
        push    offset DGROUP:ident
        push    ident_len       ; length of message
        push    ds              ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite

                                ; set offsets to end of code
                                ; and data segments
        mov     word ptr es:[bx+0eh],offset _TEXT:Init
        mov     word ptr es:[bx+10h],offset DGROUP:ident

                                ; logical units supported
        mov     byte ptr es:[bx+0dh],1

                                ; pointer to BPB array
        mov     word ptr es:[bx+12h],offset DGROUP:array
        mov     word ptr es:[bx+14h],ds

        push    es              ; save request packet address
        push    bx

				; allocate RAM disk storage
        mov     bx,0            ; AX:BX = size of allocated
        mov     ax,1            ; block in bytes
        mov     dh,0            ; request block above 1 MB
        mov     dl,AllocPhys    ; function number
        call    devhlp          ; transfer to kernel
        jc      Init9           ; jump if allocation failed

                                ; save physical address
                                ; of allocated block
        mov     word ptr dbase,bx
        mov     word ptr dbase+2,ax

	call	format		; format the RAM disk
        jc      Init9           ; jump if format failed

        pop     bx              ; restore request packet address
        pop     es

        mov     ax,0100h        ; return 'done' status
        ret

Init9:                          ; abort driver installation

        pop     bx              ; restore request packet address
        pop     es

                                ; display installation aborted...
        push    stdout          ; handle for standard output
        push    ds              ; address of message
        push    offset DGROUP:abort
        push    abort_len       ; length of message
        push    ds              ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite        ; transfer to OS/2

                                ; set zero units
        mov     byte ptr es:[bx+0dh],0

                                ; set lengths of code and
                                ; data segments
        mov     word ptr es:[bx+0eh],0
        mov     word ptr es:[bx+10h],0

        mov     ax,810ch        ; return error + done +
                                ; general failure
        ret

Init    endp


Format	proc	near		; format the RAM disk area

                                ; calculate length of 
				; RAM disk control areas
        mov     al,byte ptr bpb+5
        cbw                     ; number of FATs
        mov     cx,bpb+0bh      ; * sectors per FAT
        mul     cx
	mov	cx,bpb+6	; entries in root directory
	shr	cx,4		; divided by 16 for sectors
        add     ax,cx           ; (FAT + dir sectors
        add     ax,bpb+3        ; + reserved sectors)
        mov     cx,secsize      ; * bytes/sector
        mul     cx              ; = total length
        mov     cx,ax

				; convert RAM disk base to
                                ; virtual address
        mov     bx,word ptr dbase
        mov     ax,word ptr dbase+2
        mov     dh,1            ; leave result in ES:DI
        mov     dl,PhysToVirt   ; function number
        call    devhlp          ; transfer to kernel
        jc      Format9         ; jump if error

        xor     ax,ax           ; now zero control areas
        cld                     ; (assume CX still = length)
        rep stosb

        mov     ax,0            ; get address of logical 
        call    MapES           ; sector zero
        jc      Format9         ; jump if error
        mov     si,offset DGROUP:bootrec
        mov     cx,bootrec_len  ; copy boot record
        rep movsb               ; to logical sector zero

        mov     ax,word ptr bpb+3
        call    MapES           ; get address of FAT sector
        jc      Format9         ; jump if error
        mov     al,byte ptr bpb+0ah     
        mov     es:[di],al      ; media ID byte into FAT
        mov     word ptr es:[di+1],-1

        mov     ax,word ptr bpb+3
        add     ax,word ptr bpb+0bh
        call    MapES           ; get address of directory
        jc      Format9         ; jump if error
        mov     si,offset DGROUP:volname
        mov     cx,volname_len
        rep movsb               ; copy volume label to it

        mov     dl,UnPhysToVirt ; function number
        call    devhlp          ; transfer to kernel

        clc                     ; signal format successful

Format9:                        ; return with Carry = 0
        ret                     ; if success, 1 if error

Format  endp

_TEXT   ends

        end
