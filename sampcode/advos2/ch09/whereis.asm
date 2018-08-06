	title	WHEREIS -- File Finder Utility
        page    55,132
        .286

; 
; WHEREIS.ASM
;
; A file finder utility that searches the current drive, starting
; with the root directory (\), for the specified pathname.
; Wildcard characters can be included.
;
; This program requires the modules ARGV.ASM and ARGC.ASM.
;
; Assemble with:  C> masm whereis.asm;
; Link with:  C> link whereis+argv+argc,,,os2,whereis
;
; Usage is:  C> whereis pathname
;
; Copyright (C) 1988 Ray Duncan
;

stdin   equ     0                       ; standard input handle
stdout  equ     1                       ; standard output handle
stderr  equ     2                       ; standard error handle

cr      equ     0dh                     ; ASCII carriage return
lf	equ	0ah			; ASCII linefeed

        extrn   DosChDir:far
        extrn   DosExit:far
        extrn   DosFindClose:far
        extrn   DosFindFirst:far
        extrn   DosFindNext:far
        extrn   DosQCurDir:far
        extrn   DosQCurDisk:far
        extrn   DosSelectDisk:far
        extrn   DosWrite:far

_srec	struc				; search result structure...
cdate   dw      ?                       ; date of creation
ctime   dw      ?                       ; time of creation
adate   dw      ?                       ; date of last access
atime   dw      ?                       ; time of last access
wdate   dw      ?                       ; date of last write
wtime   dw      ?                       ; time of last write
fsize   dd      ?                       ; file size
falloc  dd      ?                       ; file allocation
fattr   dw      ?                       ; file attribute
fcount  db      ?                       ; filename count byte
fname   db      13 dup (?)              ; ASCIIZ filename
_srec   ends


DGROUP  group   _DATA

_DATA   segment word public 'DATA'

root    db      '\',0                   ; name of root directory

parent  db      '..',0                  ; alias for parent directory

wild    db      '*.*',0                 ; matches all files

sname   db      64 dup (0)              ; filename for search

drvno   dw      0                       ; current drive
drvmap  dd      0                       ; logical drive bitmap

dname   db      'X:\'                   ; current drive ID
dbuf    db      80 dup (?)              ; current directory 
dbuf_len dw     ?                       ; length of buffer

sbuf    _srec   <>                      ; receives search results
sbuf_len equ $-sbuf

count   dw      0                       ; total files matched
wlen    dw      ?                       ; receives bytes written
shandle dw      -1                      ; directory search handle
scount	dw	1			; number of files to return

msg1    db      cr,lf
        db      'whereis: no files found'
        db      cr,lf
msg1_len equ $-msg1

msg2    db      cr,lf
msg2_len equ $-msg2

msg3    db      cr,lf
	db	'whereis: missing filename'
        db      cr,lf
msg3_len equ $-msg3

msg4    db      cr,lf
        db      'whereis: bad drive'
        db      cr,lf
msg4_len equ $-msg4

_DATA   ends


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP

        extrn   argv:near
        extrn   argc:near

whereis proc    far

        call    argc                    ; filename present in 
        cmp     ax,2                    ; command tail?
        jae     where1                  ; jump, filename present

                                        ; no filename, exit...
        mov     dx,offset DGROUP:msg3   ; error message address
        mov     cx,msg3_len             ; message length
        jmp     where6                  ; go terminate

where1:                                 ; get current drive... 
        push    ds                      ; receives drive code
        push    offset DGROUP:drvno
        push    ds                      ; receives drive bitmap
        push    offset DGROUP:drvmap
        call    DosQCurDisk             ; transfer to OS/2

        mov     ax,1                    ; get address and length
        call    argv                    ; of filename parameter
                                        ; returns ES:BX = address
                                        ;         AX    = length
        mov     cx,ax                   ; save length in CX     

        cmp     ax,2                    ; parameter length > 2?
        jle     where3                  ; no, jump
        cmp     byte ptr es:[bx+1],':'  ; drive delimiter present?
        jne     where3                  ; no, jump

        mov     al,es:[bx]              ; get ASCII drive code
	or	al,20h			; fold to lowercase
        xor     ah,ah
        sub     ax,'a'-1                ; convert drive code to
        mov     drvno,ax                ; binary and save it
	cmp	ax,1			; make sure drive valid
        jb      where2                  ; jump, bad drive
        cmp     ax,26
        ja      where2                  ; jump, bad drive

        add     bx,2                    ; advance command tail
        sub     cx,2                    ; pointer past drive code

                                        ; set drive for search...
        push    ax                      ; drive code
        call    DosSelectDisk           ; transfer to OS/2
        or      ax,ax                   ; drive OK?
        jz      where3                  ; jump, drive was valid

                                        ; bad drive, exit...
where2: mov     dx,offset DGROUP:msg4   ; error message address
        mov     cx,msg4_len             ; message length
        jmp     where6

where3: mov     di,offset DGROUP:sname  ; DS:DI = local buffer

where4: mov     al,es:[bx]              ; copy filename to local
        mov     [di],al                 ; buffer byte by byte...
        inc     di
        inc     bx
        loop    where4

        mov     byte ptr [di],0         ; append null byte

        push    ds                      ; make DGROUP addressable       
        pop     es                      ; with ES
        assume  es:DGROUP

        mov     dx,offset DGROUP:root   ; start searching with 
        call    schdir                  ; the root directory

        cmp     count,0                 ; any matching files found?
        jne     where5                  ; yes, exit silently

					; no, display 'no files'...
        push    stdout                  ; standard output handle
        push    ds                      ; message address
        push    offset DGROUP:msg1
        push    msg1_len                ; message length
        push    ds                      ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite                ; transfer to OS/2

where5:                                 ; final exit to OS/2...
        push    1                       ; terminate all threads
	push	0			; return code = 0 (success)
        call    DosExit                 ; transfer to OS/2

where6:                                 ; common error exit...
                                        ; DS:DX = msg, CX = length
        push    stderr                  ; standard output handle
        push    ds                      ; address of message
        push    dx
        push    cx                      ; length of message
        push    ds                      ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite                ; transfer to OS/2

                                        ; final exit to OS/2...
        push    1                       ; terminate all threads
        push    1                       ; exit code = 1 (error)
        call    DosExit                 ; transfer to OS/2

whereis endp


; SCHDIR:       search a directory for matching
;               files and any other directories
;
; Call with:    DS:DX = ASCIIZ directory name
; Returns:      nothing
; Uses:         all registers

schdir  proc    near

        push    shandle                 ; save old search handle
        mov     shandle,-1              ; initialize search handle

                                        ; set search directory...
        push    ds                      ; directory name address
        push    dx
        push    0                       ; reserved DWORD 0
        push    0
        call    DosChDir                ; transfer to OS/2

        call    schfile                 ; search current directory
                                        ; for matching files

                                        ; search for directories
        mov     scount,1                ; max matches to return
        push    ds                      ; target name address
        push    offset DGROUP:wild
        push    ds                      ; receives search handle
        push    offset DGROUP:shandle
	push	10h			; normal + dir attribute
        push    ds                      ; result buffer address
        push    offset DGROUP:sbuf
        push    sbuf_len                ; result buffer length
        push    ds                      ; receives match count
        push    offset DGROUP:scount                    
        push    0                       ; reserved DWORD 0
        push    0
        call    DosFindFirst            ; transfer to OS/2

        or      ax,ax                   ; find anything?
        jnz     schdir3                 ; no, jump

schdir1:                                ; found some match...
        test    sbuf.fattr,10h          ; is it a directory?
        jz      schdir2                 ; no, skip it

        cmp     sbuf.fname,'.'          ; is it . or .. entry?
        je      schdir2                 ; yes, skip it

                                        ; no, new directory found
        mov     dx,offset DGROUP:sbuf.fname
        call    schdir                  ; call self to search it

                                        ; restore old directory...
        push    ds                      ; address of '..' alias
        push    offset DGROUP:parent
        push    0                       ; reserved DWORD 0
        push    0
        call    DosChDir                ; transfer to OS/2

schdir2:                                ; found at least one match,
                                        ; look for next match...
        mov     scount,1                ; max matches to return
        push    shandle                 ; handle from DosFindFirst
        push    ds                      ; result buffer address
        push    offset DGROUP:sbuf
        push    sbuf_len                ; result buffer length
        push    ds                      ; receives match count
        push    offset DGROUP:scount                    
        call    DosFindNext             ; transfer to OS/2

        or      ax,ax                   ; any matches found?
        jz      schdir1                 ; yes, go process it

schdir3:                                ; end of search...
        push    shandle                 ; close search handle
        call    DosFindClose            ; transfer to OS/2

        pop     shandle                 ; restore previous handle

        ret                             ; back to caller

schdir  endp


; SCHFILE:      search current directory for
;               files matching string in 'sname'
;
; Call with:    nothing
; Returns:      nothing
; Uses:         all registers

schfile proc    near

        push    shandle                 ; save previous handle
        mov     shandle,-1              ; initialize search handle

        mov     scount,1                ; max matches to return
        push    ds                      ; name to match
        push    offset DGROUP:sname
        push    ds                      ; receives search handle
        push    offset DGROUP:shandle
        push    0h                      ; attribute=normal files
        push    ds                      ; result buffer address
        push    offset DGROUP:sbuf
        push    sbuf_len                ; result buffer length
        push    ds                      ; receives match count
        push    offset DGROUP:scount                    
        push    0                       ; reserved DWORD 0
        push    0
        call    DosFindFirst            ; transfer to OS/2

        or      ax,ax                   ; any matches found?
        jnz     schfile3                ; no, terminate search

schfile1:                               ; found matching file...
        call    pfile                   ; display its name

                                        ; look for next match...
        push    shandle                 ; handle from DosFindFirst
        push    ds                      ; result buffer address
        push    offset DGROUP:sbuf
        push    sbuf_len                ; result buffer length
        push    ds                      ; receives match count
        push    offset DGROUP:scount                    
        call    DosFindNext             ; transfer to OS/2

        or      ax,ax                   ; any more matches?
        jz      schfile1                ; yes, go display filename

schfile3:                               ; end of search...
        push    shandle                 ; close search handle
        call    DosFindClose            ; transfer to OS/2      

        pop     shandle                 ; restore previous handle
        ret                             ; return to caller

schfile endp


; PFILE:        display current drive and directory,
;               followed by filename from 'sbuf.fname'
;
; Call with:    nothing
; Returns:      nothing
; Uses:         all registers

pfile   proc    near

        inc     count                   ; count matched files

        call    pdir                    ; display drive:path

                                        ; fold name to lower case
        mov     bx,offset DGROUP:sbuf.fname
        call    makelc

                                        ; display filename...
        push    stdout                  ; standard output handle
        push    ds                      ; filename address
        push    offset DGROUP:sbuf.fname
        mov     al,sbuf.fcount          ; filename length
        xor     ah,ah
        push    ax
        push    ds                      ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite                ; transfer to OS/2

                                        ; send newline sequence...
        push    stdout                  ; standard output handle
        push    ds                      ; address of newline
        push    offset DGROUP:msg2
        push    msg2_len                ; length of newline
        push    ds                      ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite                ; transfer to OS/2

        ret                             ; return to caller

pfile   endp


; PDIR:         display current drive and directory
;
; Call with:    nothing
; Returns:      nothing
; Uses:         AX, BX, CX, DI, ES

pdir    proc    near

        mov     ax,drvno                ; convert binary drive 
        add     al,'A'-1                ; code to ASCII drive 
        mov     dname,al                ; and store it for output       

        mov     dbuf_len,dbuf_len-dbuf  ; initialize length of
                                        ; directory buffer

                                        ; get current directory...
        push    0                       ; drive 0 = default 
        push    ds                      ; receives ASCIIZ path
        push    offset DGROUP:dbuf
        push    ds                      ; contains buffer length
        push    offset DGROUP:dbuf_len
        call    DosQCurDir              ; transfer to OS/2
        
        mov     di,offset DGROUP:dbuf   ; address of path

        cmp     byte ptr [di],0         ; is path = root?       
        je      pdir1                   ; yes, jump

        mov     cx,dbuf_len-dbuf        ; no, scan for null 
        xor     al,al                   ; byte at end of path...
        repne scasb

        mov     byte ptr [di-1],'\'     ; append a backslash

pdir1:  mov     bx,offset DGROUP:dname  ; fold everything to
	call	makelc			; lowercase

                                        ; now display drive:path...
        push    stdout                  ; standard output handle
        push    ds                      ; address of pathname
        push    offset DGROUP:dname
	sub	di,offset DGROUP:dname	; length of drive and path
        push    di
        push    ds                      ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite                ; transfer to OS/2

        ret                             ; back to caller

pdir    endp


; MAKELC:	convert ASCIIZ string to lowercase
;
; Call with:    DS:BX = string address
; Returns:      nothing
; Uses:         BX

makelc  proc    near
        
make1:  cmp     byte ptr [bx],0         ; end of string?
        je      make3                   ; jump if end

        cmp     byte ptr [bx],'A'       ; check next character
	jb	make2			; jump, not uppercase
        cmp     byte ptr [bx],'Z'
	ja	make2			; jump, not uppercase

	or	byte ptr [bx],20h	; fold to lowercase

make2:  inc     bx                      ; advance through string
        jmp     make1

make3:  ret                             ; back to caller

makelc  endp

_TEXT   ends

        end     whereis
