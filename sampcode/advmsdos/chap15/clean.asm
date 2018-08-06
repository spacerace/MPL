        title   CLEAN - Text file filter 
        page    55,132
;
; CLEAN.ASM     Filter to turn document files into
;               normal text files.
;
; Copyright 1988 Ray Duncan
;
; Build:        C>MASM CLEAN;
;               C>LINK CLEAN;
;                       
; Usage:        C>CLEAN  <infile  >outfile
;
; All text characters are passed through with high 
; bit stripped off.  Form feeds, carriage returns, 
; and line feeds are passed through.  Tabs are expanded 
; to spaces.  All other control codes are discarded.
;

tab     equ     09h             ; ASCII tab code
lf      equ     0ah             ; ASCII line feed
ff      equ     0ch             ; ASCII form feed
cr      equ     0dh             ; ASCII carriage return
blank   equ     020h            ; ASCII space code
eof     equ     01ah            ; Ctrl-Z End-of-file 

tabsiz  equ     8               ; width of tab stop

bufsiz  equ     128             ; size of input and
                                ; output buffers

stdin   equ     0000            ; standard input handle
stdout  equ     0001            ; standard output handle
stderr  equ     0002            ; standard error handle


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:_DATA,es:_DATA,ss:STACK 

clean   proc    far             ; entry point from MS-DOS.

        push    ds              ; save DS:0000 for final
        xor     ax,ax           ; return to MS-DOS, in case
        push    ax              ; Function 4CH can't be used.   

        mov     ax,_DATA        ; make data segment addressable
        mov     ds,ax
        mov     es,ax

        mov     ah,30h          ; check version of MS-DOS
        int     21h
        cmp     al,2            ; MS-DOS 2.0 or later?
        jae     clean1          ; jump if version OK

                                ; MS-DOS 1.x, display error
                                ; message and exit...
        mov     dx,offset msg1  ; DS:DX = message address
        mov     ah,9            ; Fxn 9 = display string
        int     21h             ; transfer to MS-DOS
        ret                     ; then exit the old way

clean1: call    init            ; initialize input buffer

clean2: call    getc            ; get character from input
        jc      clean9          ; exit if end of stream

        and     al,07fh         ; strip off high bit

        cmp     al,blank        ; is it a control char?
        jae     clean4          ; no, write it

        cmp     al,eof          ; is it end of file?
        je      clean8          ; yes, write EOF and exit

        cmp     al,tab          ; is it a tab?
        je      clean6          ; yes, expand it to spaces

        cmp     al,cr           ; is it a carriage return?
        je      clean3          ; yes, go process it.

        cmp     al,lf           ; is it a line feed?
        je      clean3          ; yes, go process it.

        cmp     al,ff           ; is it a form feed?
        jne     clean2          ; no, discard it  

clean3: mov     column,0        ; if CR, LF, or FF
        jmp     clean5          ; reset column to zero

clean4: inc     column          ; if non-control char.
                                ; increment column counter
                                
clean5: call    putc            ; write char. to stdout
        jnc     clean2          ; if disk not full,
                                ; get another character

                                ; write failed...
        mov     dx,offset msg2  ; DS:DX = error message
        mov     cx,msg2_len     ; CX = message length
        mov     bx,stderr       ; BX = standard error handle
        mov     ah,40h          ; Fxn 40H = write
        int     21h             ; transfer to MS-DOS

        mov     ax,4c01h        ; Fxn 4CH = terminate
                                ; return code = 1
        int     21h             ; transfer to MS-DOS

clean6: mov     ax,column       ; tab code detected
        cwd                     ; tabsiz - (column MOD tabsiz)
        mov     cx,tabsiz       ; is number of spaces needed
        idiv    cx              ; to move to next tab stop
        sub     cx,dx

        add     column,cx       ; also update column counter

clean7: push    cx              ; save spaces counter

        mov     al,blank        ; write an ASCII space
        call    putc

        pop     cx              ; restore spaces counter
        loop    clean7          ; loop until tab stop 

        jmp     clean2          ; get another character
                                
clean8: call    putc            ; write EOF mark

clean9: call    flush           ; write last output buffer
        mov     ax,4c00h        ; Fxn 4CH = terminate
                                ; return code =0
        int     21h             ; transfer to MS-DOS

clean   endp


getc    proc    near            ; get char. from stdin
                                ; returns Carry = 1 if
                                ; end of input, else
                                ; AL = char, Carry = 0

        mov     bx,iptr         ; get input buffer pointer
        cmp     bx,ilen         ; end of buffer reached?
        jne     getc1           ; not yet, jump

                                ; more data is needed...
        mov     bx,stdin        ; BX = standard input handle
        mov     cx,bufsiz       ; CX = length to read
        mov     dx,offset ibuff ; DS:DX = buffer address
        mov     ah,3fh          ; Fxn 3FH = read
        int     21h             ; transfer to MS-DOS
        jc      getc2           ; jump if read failed
        
        or      ax,ax           ; was anything read?
        jz      getc2           ; jump if end of input

        mov     ilen,ax         ; save length of data
        xor     bx,bx           ; reset buffer pointer

getc1:  mov     al,[ibuff+bx]   ; get char. from buffer
        inc     bx              ; bump buffer pointer

        mov     iptr,bx         ; save updated pointer
        clc                     ; return char in AL
        ret                     ; and Carry = 0 (clear)

getc2:  stc                     ; end of input stream
        ret                     ; return Carry = 1 (set)

getc    endp


putc    proc    near            ; send char. to stdout
                                ; returns Carry = 1 if
                                ; error, else Carry = 0

        mov     bx,optr         ; store character into
        mov     [obuff+bx],al   ; output buffer

        inc     bx              ; bump buffer pointer
        cmp     bx,bufsiz       ; buffer full?
        jne     putc1           ; no, jump

        mov     bx,stdout       ; BX = standard output handle
        mov     cx,bufsiz       ; CX = length to write
        mov     dx,offset obuff ; DS:DX = buffer address
        mov     ah,40h          ; Fxn 40H = write
        int     21h             ; transfer to MS-DOS
        jc      putc2           ; jump if write failed

        cmp     ax,cx           ; was write complete?
        jne     putc2           ; jump if disk full

        xor     bx,bx           ; reset buffer pointer

putc1:  mov     optr,bx         ; save buffer pointer
        clc                     ; write successful,
        ret                     ; return Carry = 0 (clear)

putc2:  stc                     ; write failed or disk full,
        ret                     ; return Carry = 1 (set)

putc    endp


init    proc    near            ; initialize input buffer

        mov     bx,stdin        ; BX = standard input handle
        mov     cx,bufsiz       ; CX = length to read
        mov     dx,offset ibuff ; DS:DX = buffer address
        mov     ah,3fh          ; Fxn 3FH = read
        int     21h             ; transfer to MS-DOS
        jc      init1           ; jump if read failed
        mov     ilen,ax         ; save actual bytes read
init1:  ret

init    endp


flush   proc    near            ; flush output buffer

        mov     cx,optr         ; CX = bytes to write
        jcxz    flush1          ; exit if buffer empty
        mov     dx,offset obuff ; DS:DX = buffer address
        mov     bx,stdout       ; BX = standard output handle
        mov     ah,40h          ; Fxn 40H = write
        int     21h             ; transfer to MS-DOS
flush1: ret

flush   endp

_TEXT   ends


_DATA   segment word public 'DATA'

ibuff   db      bufsiz dup (0)  ; input buffer
obuff   db      bufsiz dup (0)  ; output buffer

iptr    dw      0               ; ibuff pointer
ilen    dw      0               ; bytes in ibuff
optr    dw      0               ; obuff pointer 

column  dw      0               ; current column counter

msg1    db      cr,lf
        db      'clean: need MS-DOS version 2 or greater.'
        db      cr,lf,'$'

msg2    db      cr,lf
        db      'clean: disk is full.'
        db      cr,lf
msg2_len equ    $-msg2

_DATA   ends


STACK   segment para stack 'STACK'

        dw      64 dup (?)

STACK   ends

        end     clean
