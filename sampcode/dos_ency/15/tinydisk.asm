        name    tinydisk
        title   TINYDISK example block-device driver

; TINYDISK.ASM --- 64 KB RAMdisk
;
; Ray Duncan, July 1987
; Example of a simple installable block-device driver.

_TEXT   segment public 'CODE'

        assume  cs:_TEXT,ds:_TEXT,es:_TEXT

        org     0

MaxCmd  equ     12              ; max driver command code
                                ; (no MS-DOS 3.x functions)

cr      equ     0dh             ; ASCII carriage return
lf      equ     0ah             ; ASCII linefeed
blank   equ     020h            ; ASCII space code
eom     equ     '$'             ; end-of-message signal

Secsize equ     512             ; bytes/sector, IBM-compatible media

                                ; device-driver header
Header  dd      -1              ; link to next driver in chain
        dw      0               ; device attribute word
        dw      Strat           ; "Strategy" routine entry point
        dw      Intr            ; "Interrupt" routine entry point
        db      1               ; number of units, this device
        db      7 dup (0)       ; reserved area (block-device drivers)

RHPtr   dd      ?               ; segment:offset of request header

Secseg  dw      ?               ; segment base of sector storage

Xfrsec  dw      0               ; current sector for transfer
Xfrcnt  dw      0               ; sectors successfully transferred
Xfrreq  dw      0               ; number of sectors requested
Xfraddr dd      0               ; working address for transfer

Array   dw      BPB             ; array of pointers to BPB
                                ; for each supported unit


Bootrec equ     $

        jmp     $               ; phony JMP at start of
        nop                     ; boot sector; this field
                                ; must be 3 bytes

        db      'MS   2.0'      ; OEM identity field

                                ; BIOS Parameter Block (BPB)
BPB     dw      Secsize         ; 00H - bytes per sector
        db      1               ; 02H - sectors per cluster
        dw      1               ; 03H - reserved sectors
        db      1               ; 05H - number of FATs
        dw      32              ; 06H - root directory entries
        dw      128             ; 08H - sectors = 64 KB/secsize
        db      0f8h            ; 0AH - media descriptor
        dw      1               ; 0BH - sectors per FAT

Bootrec_len equ $-Bootrec


Strat   proc    far             ; RAMdisk strategy routine

                                ; save address of request header
        mov     word ptr cs:RHPtr,bx
        mov     word ptr cs:[RHPtr+2],es
        ret                     ; back to MS-DOS kernel

Strat   endp


Intr    proc    far             ; RAMdisk interrupt routine

        push    ax              ; save general registers
        push    bx
        push    cx
        push    dx
        push    ds
        push    es
        push    di
        push    si
        push    bp

        mov     ax,cs           ; make local data addressable
        mov     ds,ax

        les     di,[RHPtr]      ; ES:DI = request header

        mov     bl,es:[di+2]    ; get command code
        xor     bh,bh
        cmp     bx,MaxCmd       ; make sure it's valid
        jle     Intr1           ; jump, function code is ok
        mov     ax,8003h        ; set Error bit and
        jmp     Intr3           ; "Unknown Command" error code

Intr1:  shl     bx,1            ; form index to dispatch table and
                                ; branch to command-code routine
        call    word ptr [bx+Dispatch]
                                ; should return AX = status

        les     di,[RHPtr]      ; restore ES:DI = request header

Intr3:  or      ax,0100h        ; merge Done bit into status and store
        mov     es:[di+3],ax    ; status into request header

Intr4:  pop     bp              ; restore general registers
        pop     si
        pop     di
        pop     es
        pop     ds
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        ret                     ; return to MS-DOS kernel

Intr    endp


Dispatch:                       ; command-code dispatch table
                                ; all command-code routines are
                                ; entered with ES:DI pointing
                                ; to request header and return
                                ; the operation status in AX
        dw      Init            ;  0 = initialize driver
        dw      MediaChk        ;  1 = media check on block device
        dw      BuildBPB        ;  2 = build BIOS parameter block
        dw      Dummy           ;  3 = I/O control read
        dw      Read            ;  4 = read (input) from device
        dw      Dummy           ;  5 = nondestructive read
        dw      Dummy           ;  6 = return current input status
        dw      Dummy           ;  7 = flush device input buffers
        dw      Write           ;  8 = write (output) to device
        dw      Write           ;  9 = write with verify
        dw      Dummy           ; 10 = return current output status
        dw      Dummy           ; 11 = flush output buffers
        dw      Dummy           ; 12 = I/O control write


MediaChk proc  near             ; command code 1 = Media Check

                                ; return "not changed" code
        mov     byte ptr es:[di+0eh],1
        xor     ax,ax           ; and success status
        ret

MediaChk endp


BuildBPB proc  near             ; command code 2 = Build BPB

                                ; put BPB address in request header
        mov     word ptr es:[di+12h],offset BPB
        mov     word ptr es:[di+14h],cs
        xor     ax,ax           ; return success status
        ret

BuildBPB endp


Read    proc    near            ; command code 4 = Read (Input)

        call    Setup           ; set up transfer variables

Read1:  mov     ax,Xfrcnt       ; done with all sectors yet?
        cmp     ax,Xfrreq
        je      Read2           ; jump if transfer completed
        mov     ax,Xfrsec       ; get next sector number
        call    Mapsec          ; and map it
        mov     ax,es
        mov     si,di
        les     di,Xfraddr      ; ES:DI = requester's buffer
        mov     ds,ax           ; DS:SI = RAMdisk address
        mov     cx,Secsize      ; transfer logical sector from
        cld                     ; RAMdisk to requestor
        rep movsb
        push    cs              ; restore local addressing
        pop     ds
        inc     Xfrsec          ; advance sector number
                                ; advance transfer address
        add     word ptr Xfraddr,Secsize
        inc     Xfrcnt          ; count sectors transferred
        jmp     Read1

Read2:                          ; all sectors transferred
        xor     ax,ax           ; return success status
        les     di,RHPtr        ; put actual transfer count
        mov     bx,Xfrcnt       ; into request header
        mov     es:[di+12h],bx
        ret

Read    endp


Write   proc    near            ; command code 8 = Write (Output)
                                ; command code 9 = Write with Verify

        call    Setup           ; set up transfer variables

Write1: mov     ax,Xfrcnt       ; done with all sectors yet?
        cmp     ax,Xfrreq
        je      Write2          ; jump if transfer completed

        mov     ax,Xfrsec       ; get next sector number
        call    Mapsec          ; and map it
        lds     si,Xfraddr
        mov     cx,Secsize      ; transfer logical sector from
        cld                     ; requester to RAMdisk
        rep movsb
        push    cs              ; restore local addressing
        pop     ds
        inc     Xfrsec          ; advance sector number
                                ; advance transfer address
        add     word ptr Xfraddr,Secsize
        inc     Xfrcnt          ; count sectors transferred
        jmp     Write1

Write2:                         ; all sectors transferred
        xor     ax,ax           ; return success status
        les     di,RHPtr        ; put actual transfer count
        mov     bx,Xfrcnt       ; into request header
        mov     es:[di+12h],bx
        ret

Write   endp


Dummy   proc    near            ; called for unsupported functions

        xor     ax,ax           ; return success flag for all
        ret

Dummy   endp


Mapsec  proc    near            ; map sector number to memory address
                                ; call with AX = logical sector no.
                                ; return ES:DI = memory address

        mov     di,Secsize/16   ; paragraphs per sector
        mul     di              ; * logical sector number
        add     ax,Secseg       ; + segment base of sector storage
        mov     es,ax
        xor     di,di           ; now ES:DI points to sector
        ret

Mapsec  endp


Setup   proc    near            ; set up for read or write
                                ; call ES:DI = request header
                                ; extracts address, start, count

        push    es              ; save request header address
        push    di
        mov     ax,es:[di+14h]  ; starting sector number
        mov     Xfrsec,ax
        mov     ax,es:[di+12h]  ; sectors requested
        mov     Xfrreq,ax
        les     di,es:[di+0eh]  ; requester's buffer address
        mov     word ptr Xfraddr,di
        mov     word ptr Xfraddr+2,es
        mov     Xfrcnt,0        ; initialize sectors transferred count
        pop     di              ; restore request header address
        pop     es
        ret

Setup   endp


Init    proc    near            ; command code 0 = Initialize driver
                                ; on entry ES:DI = request header

        mov     ax,cs           ; calculate segment base for sector
        add     ax,Driver_len   ; storage and save it
        mov     Secseg,ax
        add     ax,1000h        ; add 1000H paras (64 KB) and
        mov     es:[di+10h],ax  ; set address of free memory
        mov     word ptr es:[di+0eh],0

        call    Format          ; format the RAMdisk

        call    Signon          ; display driver identification

        les     di,cs:RHPtr     ; restore ES:DI = request header
                                ; set logical units = 1
        mov     byte ptr es:[di+0dh],1
                                ; set address of BPB array
        mov     word ptr es:[di+12h],offset Array
        mov     word ptr es:[di+14h],cs

        xor     ax,ax           ; return success status
        ret

Init    endp


Format  proc    near            ; format the RAMdisk area

        mov     es,Secseg       ; first zero out RAMdisk
        xor     di,di
        mov     cx,8000h        ; 32 K words = 64 KB
        xor     ax,ax
        cld
        rep stosw

        mov     ax,0            ; get address of logical
        call    Mapsec          ; sector zero
        mov     si,offset Bootrec
        mov     cx,Bootrec_len
        rep movsb               ; and copy boot record to it

        mov     ax,word ptr BPB+3
        call    Mapsec          ; get address of 1st FAT sector
        mov     al,byte ptr BPB+0ah
        mov     es:[di],al      ; put media ID byte into it
        mov     word ptr es:[di+1],-1

        mov     ax,word ptr BPB+3
        add     ax,word ptr BPB+0bh
        call    Mapsec          ; get address of 1st directory sector
        mov     si,offset Volname
        mov     cx,Volname_len
        rep movsb               ; copy volume label to it

        ret                     ; done with formatting

Format  endp


Signon  proc    near            ; driver identification message

        les     di,RHPtr        ; let ES:DI = request header
        mov     al,es:[di+22]   ; get drive code from header,
        add     al,'A'          ; convert it to ASCII, and
        mov     drive,al        ; store into sign-on message

        mov     ah,30h          ; get MS-DOS version
        int     21h
        cmp     al,2
        ja      Signon1         ; jump if version 3.0 or later
        mov     Ident1,eom      ; version 2.x, don't print drive

Signon1:                        ; print sign-on message
        mov     ah,09H          ; Function 09H = print string
        mov     dx,offset Ident ; DS:DX = address of message
        int     21h             ; transfer to MS-DOS

        ret                     ; back to caller

Signon  endp


Ident   db      cr,lf,lf        ; driver sign-on message
        db      'TINYDISK 64 KB RAMdisk'
        db      cr,lf
Ident1  db      'RAMdisk will be drive '
Drive   db      'X:'
        db      cr,lf,eom


Volname db      'DOSREF_DISK'   ; volume label for RAMdisk
        db      08h             ; attribute byte
        db      10 dup (0)      ; reserved area
        dw      0               ; time = 00:00
        dw      0f01h           ; date = August 1, 1987
        db      6 dup (0)       ; reserved area

Volname_len equ $-volname

Driver_len dw (($-header)/16)+1 ; driver size in paragraphs

_TEXT   ends

        end
