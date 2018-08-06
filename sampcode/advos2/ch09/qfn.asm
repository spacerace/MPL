        title   QFN.ASM --- qualify file name
        page    55,132
        .286

; QFN.ASM -- Qualify File Name (OS/2 Version)
; Copyright (C) 1988 Ray Duncan
;
; Call with:    DS:SI = filename address
;               AX    = length
;
; Returns:      Carry = clear if filename OK
;               DS:SI = qualified filename
;               AX    = length
;               or
;               Carry = set if bad filename
; 
;               Registers other than DS:SI
;               and AX are preserved.

        extrn   DosChDir:far    ; OS/2 API functions
        extrn   DosQCurDisk:far
        extrn   DosQCurDir:far
        extrn   DosSelectDisk:far

DGROUP  group   _DATA

_DATA   segment word public 'DATA'

cdrive  dw      0               ; current drive
cpath   db      '\',64 dup (0)  ; current directory
cpsiz   dw      $-cpath-1
drvmap  dd      ?               ; valid drive bitmap
ndrive  dw      0               ; new drive

tbuff   db      64 dup (0)      ; target directory

qbuff   db      'X:\'           ; qualified pathname
        db      64 dup (0)
qbsiz   dw      $-qbuff-3

fname   dw      ?               ; filename address
flen    dw      ?               ; filename length

_DATA   ends


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP

        public  qfn             ; make visible to Linker

qfn     proc    near            ; qualify file name

        push    bx              ; save registers        
        push    cx
        push    dx
        push    di
        push    es

        mov     flen,ax         ; save length and 
        mov     fname,si        ; address of filename

        mov     ax,ds           ; make DGROUP addressable
        mov     es,ax           ; with ES register

                                ; save current drive...
        push    ds              ; receives drive ID
        push    offset DGROUP:cdrive
        push    ds              ; receives valid drive bitmap
        push    offset DGROUP:drvmap
        call    DosQCurDisk     ; transfer to OS/2      

                                ; save current directory...
        push    0               ; drive ID = current
        push    ds              ; receives directory path
        push    offset DGROUP:cpath+1
        push    ds              ; contains buffer length
        push    offset DGROUP:cpsiz
        call    DosQCurDir      ; transfer to OS/2

                                ; did caller specify drive?
        mov     di,fname        ; get address of name
        mov     cx,flen         ; get length of name

        cmp     cx,2            ; if drive, length must 
                                ; be >= 2 chars.
        jl      qfn2            ; too short, no drive

                                ; check for drive delimiter
        cmp     byte ptr [di+1],':'
        jne     qfn2            ; no delimiter, jump

        mov     al,[di]         ; get ASCII drive code
        or      al,20h          ; fold to lower case
        sub     al,'a'-1        ; convert it to binary
        xor     ah,ah

                                ; now select new drive
        push    ax              ; binary drive ID
        call    DosSelectDisk   ; transfer to OS/2
        or      ax,ax           ; was drive valid?
        jz      qfn1            ; jump, drive was OK
        jmp     qfn8            ; exit, drive not valid

qfn1:   add     di,2            ; bump pointer past drive
        sub     cx,2            ; and decrement length

qfn2:                           ; get current directory 
                                ; again for new drive...
        push    0               ; drive ID = current
        push    ds              ; receives directory path
        push    offset DGROUP:cpath+1
        push    ds              ; contains buffer length
        push    offset DGROUP:cpsiz
        call    DosQCurDir      ; transfer to OS/2

                                ; scan off path if any
        push    di              ; save start of path
        mov     al,'\'          ; path delimiter

qfn3:   mov     fname,di        ; save path pointer
        mov     flen,cx         ; save path length
        jcxz    qfn4            ; jump if none left

        repne scasb             ; any '\' left in path?
        je      qfn3            ; loop if '\' found

qfn4:   pop     si              ; recover starting address
                                ; of path portion

                                ; copy path to local buffer
                                ; and make it ASCIIZ...
        mov     di,offset DGROUP:tbuff
        mov     cx,fname        ; calculate path length
        sub     cx,si
        jz      qfn6            ; jump, no path at all
        cmp     cx,1            ; root directory?
        je      qfn5            ; jump if root
        dec     cx              ; else discard last '\'

qfn5:   rep movsb               ; transfer path and
        xor     al,al           ; append null byte
        stosb
        
                                ; select target directory...
        push    ds              ; directory path address
        push    offset DGROUP:tbuff
        push    0               ; DWORD reserved
        push    0
        call    DosChDir        ; transfer to OS/2
        or      ax,ax           ; directory valid?
        jnz     qfn8            ; jump, no such directory

qfn6:                           ; build up full pathname...
                                ; get current drive
        push    ds              ; receives drive ID
        push    offset DGROUP:ndrive
        push    ds              ; receives valid drive bitmap
        push    offset DGROUP:drvmap
        call    DosQCurDisk     ; transfer to OS/2      
        mov     ax,ndrive       ; convert binary drive code
        add     ax,'a'-1        ; to ASCII drive identifier
        mov     qbuff,al

                                ; get current directory
        push    0               ; drive ID = current
        push    ds              ; receives directory path
        push    offset DGROUP:qbuff+3
        push    ds              ; contains buffer length
        push    offset DGROUP:qbsiz
        call    DosQCurDir      ; transfer to OS/2
        or      ax,ax           ; operation successful?
        jnz     qfn8            ; no, return error

                                ; point to path component
        mov     di,offset DGROUP:qbuff+3
        cmp     byte ptr [di],0 ; is current directory
                                ; the root directory?
        je      qfn7            ; yes, jump

        xor     al,al           ; scan for null byte at
        mov     cx,-1           ; end of path name
        repne scasb             ; and append backslash
        mov     byte ptr [di-1],'\'

qfn7:                           ; now append filename
                                ; to drive and path...
        mov     si,fname        ; filename address
        cmp     byte ptr [si],'.'
        je      qfn8            ; exit if directory alias
        mov     cx,flen         ; filename length
        rep movsb               ; copy it
                                        
                                ; set DS:SI = address 
        mov     si,offset DGROUP:qbuff
        mov     ax,di           ; and AX = length of
        sub     ax,si           ; fully qualified filename

        call    makelc          ; fold filename to lower
                                ; case to make it pretty

        clc                     ; set Carry = false to 
                                ; indicate success and
        jmp     qfn9            ; go to common exit point

qfn8:                           ; come here if any 
                                ; error detected...
        stc                     ; set Carry = true to 
                                ; indicate error

qfn9:   pushf                   ; save Carry flag
        push    ax              ; save final length

                                ; restore original directory
        push    ds              ; address of directory path
        push    offset DGROUP:cpath
        push    0               ; DWORD reserved
        push    0
        call    DosChDir        ; transfer to OS/2

                                ; restore original drive
        push    cdrive          ; binary drive ID
        call    DosSelectDisk

        pop     ax              ; restore length
        popf                    ; and Carry flag

        pop     es              ; restore other affected
        pop     di              ; registers
        pop     dx
        pop     cx
        pop     bx
        ret                     ; back to caller

qfn     endp


makelc  proc    near            ; string -> lower case
                                ; DS:SI = address
                                ; AX = length

        push    bx              ; save BX contents
        xor     bx,bx           ; BX will be pointer

mlc1:                           ; change A-Z to a-z
        cmp     byte ptr [bx+si],'A'
        jb      mlc2
        cmp     byte ptr [bx+si],'Z'
        ja      mlc2
        or      byte ptr [bx+si],20h

mlc2:   inc     bx              ; advance through string
        cmp     bx,ax           ; done with string yet?
        jne     mlc1            ; no, check next char.

        pop     bx              ; restore BX and
        ret                     ; return to caller

makelc  endp

_TEXT   ends

        end

