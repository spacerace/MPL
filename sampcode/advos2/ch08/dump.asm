	title	DUMP -- Display File Contents
        page    55,132
        .286

;
; DUMP.ASM
;
; Displays the binary contents of a file in hex and ASCII on the
; standard output device.
;
; Assemble with:  C> masm dump.asm;
; Link with:  C> link dump,,,os2,dump
;
; Usage is:  C> dump pathname.ext  [>destination]
;
; Copyright (C) 1988 Ray Duncan
;

cr      equ     0dh                     ; ASCII carriage return
lf      equ     0ah                     ; ASCII line feed
blank   equ     20h                     ; ASCII space code

blksize equ     16                      ; size of input file records

stdout  equ     1                       ; standard output handle
stderr  equ     2                       ; standard error handle

        extrn   DosOpen:far
        extrn   DosRead:far
        extrn   DosWrite:far
        extrn   DosClose:far
        extrn   DosExit:far

        extrn   argc:near               ; returns argument count
        extrn   argv:near               ; returns argument pointer

DGROUP  group   _DATA


_DATA   segment word public 'DATA'

fname   db      64 dup (0)              ; name of input file

fhandle dw      0                       ; input file handle

faction dw      0                       ; action from DosOpen

fptr    dw      0                       ; relative file address

rlen    dw      0                       ; actual number of bytes 
                                        ; read by DosRead

wlen    dw      0                       ; actual number of bytes
                                        ; written by DosWrite

output  db      'nnnn',blank,blank      ; output format area
outputa db      16 dup ('nn',blank)
        db      blank
outputb db      16 dup (blank),cr,lf
output_len equ $-output

hdg     db      cr,lf
        db      7 dup (blank)
        db      '0  1  2  3  4  5  6  7  '
        db      '8  9  A  B  C  D  E  F',cr,lf
hdg_len equ $-hdg

fbuff   db      blksize dup (?)         ; data from file

msg1    db      cr,lf
        db      'dump: file not found'
        db      cr,lf
msg1_len equ $-msg1

msg2    db      cr,lf
	db	'dump: missing filename'
        db      cr,lf
msg2_len equ $-msg2

msg3    db      cr,lf
        db      'dump: empty file'
        db      cr,lf
msg3_len equ $-msg3

_DATA   ends    


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP

dump    proc    far                     ; entry point from OS/2

        push    ds                      ; make DGROUP addressable
        pop     es                      ; via ES

        call    argc                    ; is filename present
        cmp     ax,2                    ; in command tail?
        je      dump1                   ; yes, proceed 

        mov     dx,offset msg2          ; missing or illegal filespec,
        mov     cx,msg2_len
        jmp     dump9                   ; display error message and exit

dump1:                                  ; copy filename from command
                                        ; tail to local buffer

        mov     ax,1                    ; get pointer to command tail
        call    argv                    ; argument in ES:BX 
        mov     cx,ax                   ; CX = filename length 
        mov     di,offset fname         ; DS:DI = local buffer

dump2:  mov     al,es:[bx]              ; copy filename byte by byte
        mov     [di],al
        inc     bx
        inc     di
        loop    dump2   

        push    ds                      ; restore ES = DGROUP
        pop     es

dump4:                                  ; try to open file...
        push    ds                      ; address of filename
        push    offset fname
        push    ds                      ; receives file handle
        push    offset fhandle
        push    ds
        push    offset faction          ; receives DosOpen action
        push    0                       ; file size (ignored)
        push    0
        push    0                       ; file attribute (ignored)
	push	1			; OpenFlag:
					; fail if file doesn't exist
        push    40h                     ; OpenMode: deny-none,
					; deny-none, access = read-only
        push    0                       ; reserved DWORD 0
        push    0
        call    DosOpen                 ; transfer to OS/2
        or      ax,ax                   ; was open successful?
        jz      dump5                   ; yes, proceed
        
        mov     dx,offset msg1          ; open failed, display
        mov     cx,msg1_len             ; error message and exit
        jmp     dump9                   

dump5:  call    rdblk                   ; initialize file buffer
        cmp     rlen,0                  ; anything read?
        jne     dump6                   ; jump, got some data 
	cmp	fptr,0			; no data, was this first read?
        jne     dump8                   ; no, end of file reached

	mov	dx,offset msg3		; empty file, print error
        mov     cx,msg3_len             ; message and exit
        jmp     dump9

dump6:  test    fptr,07fh               ; time for a heading?
        jnz     dump7                   ; no, jump

                                        ; write heading...
        push    stdout                  ; standard output handle
        push    ds                      ; address of heading
        push    offset hdg              
        push    hdg_len                 ; length of heading
        push    ds                      ; receives bytes written
        push    offset wlen
        call    DosWrite

dump7:  call    cnvblk                  ; convert one block of 
                                        ; binary data to ASCII

                                        ; write formatted output...
        push    stdout                  ; standard output handle
        push    ds                      ; address of output
        push    offset output           
        push    output_len              ; length of output
        push    ds                      ; receives bytes written
        push    offset wlen
        call    DosWrite

        jmp     dump5                   ; get more data

dump8:                                  ; end of file reached...
        push    fhandle                 ; close input file 
        call    DosClose                ; transfer to OS/2

                                        ; final exit to OS/2...
        push    1                       ; terminate all threads
	push	0			; return code = 0 (success)
        call    DosExit                 ; transfer to OS/2

dump9:                                  ; print error message on
                                        ; standard error device 
        push    stderr
        push    ds                      ; address of message
        push    dx
        push    cx                      ; length of message
        push    ds                      ; receives bytes written
        push    offset wlen
        call    DosWrite                ; transfer to OS/2

                                        ; final exit to OS/2...
        push    1                       ; terminate all threads
	push	1			; return code = 1 (error)
        call    DosExit                 ; transfer to OS/2
        
dump    endp


; RDBLK:	Read block of data from input file
;
; Call with:    nothing
; Returns:	AX = error code (0 = no error)
; Uses:         nothing

rdblk   proc    near

        push    fhandle                 ; input file handle
        push    ds                      ; buffer address
        push    offset fbuff
        push    blksize                 ; buffer length
        push    ds                      ; receives bytes read
        push    offset rlen
        call    DosRead                 ; transfer to OS/2
        ret                             ; back to caller

rdblk   endp


; CNVBLK:	Format one binary record for output
;
; Call with:    nothing
; Returns:      nothing 
; Uses:         AX, BX, CX, DX, DI

cnvblk  proc    near

        mov     di,offset output        ; clear output format 
        mov     cx,output_len-2         ; area to blanks
        mov     al,blank
        rep stosb

        mov     di,offset output        ; convert current file 
        mov     ax,fptr                 ; offset to ASCII
        call    wtoa

        xor     bx,bx                   ; point to start of data

cb1:    mov     al,[fbuff+bx]           ; get next byte of data
                                        ; from input file

	lea	di,[bx+outputb] 	; calculate output address
                                        ; for ASCII equivalent
        mov     byte ptr [di],'.'       ; if control character,
        cmp     al,blank                ; substitute a period   
	jb	cb2			; jump, not alphanumeric
        cmp     al,7eh          
	ja	cb2			; jump, not alphanumeric
	mov	[di],al 		; store ASCII character

cb2:                                    ; now convert byte to hex
        mov     di,bx                   ; calculate output address
	imul	di,di,3 		; (position*3) + base address
        add     di,offset outputa       
        call    btoa                    ; convert data byte to hex 

        inc     bx                      ; advance through record
        cmp     bx,rlen                 ; entire buffer converted?
        jne     cb1                     ; no, get another byte

        add     fptr,blksize            ; update file offset

        ret                             ; back to caller

cnvblk  endp


; WTOA:         Convert word to hex ASCII
; 
; Call with:    AX    = data to convert
;               ES:DI = storage address
; Returns:      nothing
; Uses:         AX, CL, DI

wtoa    proc    near

        push    ax                      ; save original value
        mov     al,ah
        call    btoa                    ; convert upper byte    

        pop     ax                      ; restore original value
        call    btoa                    ; convert lower byte

        ret                             ; back to caller

wtoa    endp


; BTOA:         Convert byte to hex ASCII
; 
; Call with:    AL    = data to convert
;               ES:DI = storage address
; Returns:      nothing
; Uses:         AX, CL, DI

btoa    proc    near

        sub     ah,ah                   ; clear upper byte

        mov     cl,16                   ; divide by 16
        div     cl

        call    ascii                   ; convert quotient
        stosb                           ; store ASCII character

        mov     al,ah
        call    ascii                   ; convert remainder 
        stosb                           ; store ASCII character

        ret                             ; back to caller

btoa    endp


; ASCII:        Convert nibble to hex ASCII
; 
; Call with:    AL    = data to convert in low 4 bits
; Returns:      AL    = ASCII character
; Uses:         nothing

ascii   proc    near

        add     al,'0'                  ; add base ASCII value  
        cmp     al,'9'                  ; is it in range 0-9?
        jle     ascii2                  ; jump if it is

        add     al,'A'-'9'-1            ; no, adjust for range A-F

ascii2: ret				; return ASCII character in AL

ascii   endp

_TEXT   ends

        end     dump
