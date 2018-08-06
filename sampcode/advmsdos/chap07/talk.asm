	title	TALK --- Simple terminal emulator
        page    55,132
        .lfcond             ;list false conditionals too

;
; TALK.ASM  --- Simple IBM PC terminal emulator
; 
; Copyright (c) 1988 Ray Duncan
;
; Build:  MASM TALK;
;         LINK TALK;
;
; Usage:  TALK
;

stdin   equ     0               ; standard input handle
stdout  equ     1               ; standard output handle
stderr  equ     2               ; standard error handle

cr      equ     0dh             ; ASCII carriage return
lf      equ     0ah             ; ASCII line feed
bsp     equ     08h             ; ASCII backspace
escape  equ     1bh             ; ASCII escape code

dattr   equ     07h             ; display attribute to use
                                ; while in emulation mode

bufsiz  equ     4096            ; size of serial port buffer

echo    equ     0               ; 0=full-duplex, -1=half-duplex

true    equ     -1
false   equ     0

com1    equ     true            ; Use COM1 if nonzero
com2    equ     not com1        ; Use COM2 if nonzero

pic_mask  equ   21h             ; 8259 int. mask port
pic_eoi   equ   20h             ; 8259 EOI port
        
        if      com1
com_data equ    03f8h           ; port assignments for COM1 
com_ier  equ    03f9h
com_mcr  equ    03fch
com_sts  equ    03fdh
com_int  equ    0ch             ; COM1 interrupt number
int_mask equ    10h             ; IRQ4 mask for 8259
        endif

        if      com2
com_data equ    02f8h           ; port assignments for COM2
com_ier  equ    02f9h
com_mcr  equ    02fch
com_sts  equ    02fdh
com_int  equ    0bh             ; COM2 interrupt number 
int_mask equ    08h             ; IRQ3 mask for 8259
        endif

_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:_DATA,es:_DATA,ss:STACK

talk    proc    far             ; entry point from MS-DOS

        mov     ax,_DATA        ; make data segment addressable 
        mov     ds,ax
        mov     es,ax
                                ; initialize display for 
                                ; terminal emulator mode...

        mov     ah,15           ; get display width and 
        int     10h             ; current display mode
        dec     ah              ; save display width for use
        mov     columns,ah      ; by the screen clear routine

        cmp     al,7            ; enforce text display mode
        je      talk2           ; mode 7 ok, proceed

        cmp     al,3
        jbe     talk2           ; modes 0-3 ok,proceed

        mov     dx,offset msg1
        mov     cx,msg1_len
        jmp     talk6           ; print error message and exit

talk2:  mov     bh,dattr        ; clear screen and home cursor
        call    cls

        call    asc_enb         ; capture serial port interrupt
                                ; vector and enable interrupts

        mov     dx,offset msg2  ; display message
        mov     cx,msg2_len     ; 'Terminal emulator running'
        mov     bx,stdout       ; BX = standard output handle
        mov     ah,40h          ; Fxn 40H = write file or device
        int     21h             ; transfer to MS-DOS

talk3:  call    pc_stat         ; keyboard character waiting?
        jz      talk4           ; nothing waiting, jump

        call    pc_in           ; read keyboard character

        cmp     al,0            ; is it a function key?
        jne     talk32          ; not function key, jump

        call    pc_in           ; function key, discard 2nd
                                ; character of sequence
        jmp     talk5           ; then terminate program

talk32:                         ; keyboard character received
        if      echo
        push    ax              ; if half-duplex, echo
        call    pc_out          ; character to PC display
        pop     ax              
        endif

        call    com_out         ; write char. to serial port

talk4:  call    com_stat        ; serial port character waiting?
        jz      talk3           ; nothing waiting, jump

        call    com_in          ; read serial port character

        cmp     al,20h          ; is it control code?
        jae     talk45          ; jump if not

        call    ctrl_code       ; control code, process it

        jmp     talk3           ; check keyboard again

talk45:                         ; non-control char. received,
        call    pc_out          ; write it to PC display

        jmp     talk4           ; see if any more waiting

talk5:                          ; function key detected, 
                                ; prepare to terminate...

        mov     bh,07h          ; clear screen and home cursor
        call    cls

        mov     dx,offset msg3  ; display farewell message
        mov     cx,msg3_len

talk6:  push    dx              ; save message address
        push    cx              ; and message length

        call    asc_dsb         ; disable serial port interrupts
                                ; and release interrupt vector

        pop     cx              ; restore message length
        pop     dx              ; and address

        mov     bx,stdout       ; handle for standard output
        mov     ah,40h          ; Fxn 40H = write device
        int     21h             ; transfer to MS-DOS    

        mov     ax,4c00h        ; terminate program with
        int     21h             ; return code = 0

talk    endp


com_stat proc   near            ; Check asynch status, returns  
                                ; Z=false if character ready
                                ; Z=true if nothing waiting
        push    ax
        mov     ax,asc_in       ; compare ring buffer pointers
        cmp     ax,asc_out
        pop     ax
        ret                     ; return to caller      

com_stat endp


com_in  proc    near            ; get character from serial
                                ; port buffer, returns
                                ; new character in AL

        push    bx              ; save register BX

com_in1:                        ; if no char waiting, wait
        mov     bx,asc_out      ; until one is received
        cmp     bx,asc_in
        je      com_in1         ; jump, nothing waiting

        mov     al,[bx+asc_buf] ; character is ready,
                                ; extract it from buffer

        inc     bx              ; update buffer pointer 
        cmp     bx,bufsiz
        jne     com_in2
        xor     bx,bx           ; reset pointer if wrapped
com_in2:                        
        mov     asc_out,bx      ; store updated pointer
        pop     bx              ; restore register BX
        ret                     ; and return to caller

com_in  endp


com_out proc    near            ; write character in AL 
                                ; to serial port   

        push    dx              ; save register DX
        push    ax              ; save character to send
        mov     dx,com_sts      ; DX = status port address

com_out1:                       ; check if transmit buffer
        in      al,dx           ; is empty (TBE bit = set)
        and     al,20h
        jz      com_out1        ; no, must wait

        pop     ax              ; get character to send
        mov     dx,com_data     ; DX = data port address
        out     dx,al           ; transmit the character
        pop     dx              ; restore register DX
        ret                     ; and return to caller

com_out endp


pc_stat proc    near            ; read keyboard status, returns
                                ; Z=false if character ready
                                ; Z=true if nothing waiting.
                                ; register DX destroyed.

        mov     al,in_flag      ; if character already    
        or      al,al           ; waiting, return status   
        jnz     pc_stat1

        mov     ah,6            ; otherwise call MS-DOS to
        mov     dl,0ffh         ; determine keyboard status
        int     21h

        jz      pc_stat1        ; jump if no key ready

        mov     in_char,al      ; got key, save it for
        mov     in_flag,0ffh    ; "pc_in" routine

pc_stat1:                       ; return to caller with
        ret                     ; Z flag set appropriately

pc_stat endp


pc_in   proc    near            ; read keyboard character,
                                ; return it in AL
                                ; DX may be destroyed.

        mov     al,in_flag      ; key already waiting?  
        or      al,al
        jnz     pc_in1          ; yes,return it to caller

        call    pc_stat         ; try and read a character
        jmp     pc_in

pc_in1: mov     in_flag,0       ; clear char. waiting flag
        mov     al,in_char      ; and return AL = character
        ret 

pc_in   endp


pc_out  proc    near            ; write character in AL
                                ; to the PC's display.

        mov     ah,0eh          ; ROM BIOS Fxn 0EH = 
                                ; "teletype output"
        push    bx              ; save register BX
        xor     bx,bx           ; assume page 0
        int     10h             ; transfer to ROM BIOS
        pop     bx              ; restore register BX
        ret                     ; and return to caller

pc_out  endp


cls     proc    near            ; clear display using 
                                ; char. attribute in BH
                                ; registers AX, CX, 
                                ; and DX destroyed.

        mov     dl,columns      ; set DL,DH = X,Y of
        mov     dh,24           ; lower right corner
        mov     cx,0            ; set CL,CH=X,Y of
                                ; upper left corner
        mov     ax,600h         ; ROM BIOS Fxn 06H =
                                ; "scroll or initialize
                                ; window" 
        int     10h             ; transfer to ROM BIOS
        call    home            ; set cursor at (0,0)
        ret                     ; and return to caller

cls     endp


clreol  proc    near            ; clear from cursor to end
                                ; of line using attribute
                                ; in BH.  Register AX, CX,
                                ; and DX destroyed.

        call    getxy           ; get current cursor pos'n
        mov     cx,dx           ; current position = "upper 
                                ; left corner" of window
        mov     dl,columns      ; "lower right corner" X is 
                                ; max columns, Y is same
                                ; as upper left corner
        mov     ax,600h         ; ROM BIOS Fxn 06H = 
                                ; "scroll or initialize
                                ; window"
        int     10h             ; transfer to ROM BIOS
        ret                     ; return to caller

clreol  endp


home    proc    near            ; put cursor at home position

        mov     dx,0            ; set (X,Y) = (0,0)
        call    gotoxy          ; position the cursor
        ret                     ; return to caller

home    endp


gotoxy  proc    near            ; position the cursor
                                ; call with DL=X, DH=Y

        push    bx              ; save registers
        push    ax

        mov     bh,0            ; assume page 0
        mov     ah,2            ; ROM BIOS Fxn 02H = 
                                ; set cursor position
        int     10h             ; transfer to ROM BIOS

        pop     ax              ; restore registers
        pop     bx
        ret                     ; and return to caller

gotoxy  endp


getxy   proc    near            ; get cursor position,
                                ; Returns DL=X, DH=Y

        push    ax              ; save registers
        push    bx
        push    cx

        mov     ah,3            ; ROM BIOS Fxn 03H =
                                ; get cursor position
        mov     bh,0            ; assume page 0
        int     10h             ; transfer to ROM BIOS

        pop     cx              ; restore registers
        pop     bx
        pop     ax
        ret                     ; and return to caller

getxy   endp


ctrl_code proc  near            ; process control code
                                ; call with AL=char

        cmp     al,cr           ; if carriage return
        je      ctrl8           ; just send it  

        cmp     al,lf           ; if line feed
        je      ctrl8           ; just send it

        cmp     al,bsp          ; if backspace
        je      ctrl8           ; just send it

        cmp     al,26           ; is it cls control code?
        jne     ctrl7           ; no, jump

        mov     bh,dattr        ; cls control code, clear
        call    cls             ; screen and home cursor

        jmp     ctrl9

ctrl7:
        cmp     al,escape       ; is it Escape character?
        jne     ctrl9           ; no, throw it away

        call    esc_seq         ; yes, emulate CRT terminal
        jmp     ctrl9

ctrl8:  call    pc_out          ; send CR, LF, or backspace
                                ; to the display

ctrl9:  ret                     ; return to caller

ctrl_code endp


esc_seq proc    near            ; decode Televideo 950 escape
                                ; sequence for screen control

        call    com_in          ; get next character
        cmp     al,84           ; is it clear to end of line?
        jne     esc_seq1        ; no, jump

        mov     bh,dattr        ; yes, clear to end of line
        call    clreol
        jmp     esc_seq2        ; then exit     

esc_seq1:
        cmp     al,61           ; is it cursor positioning?
        jne     esc_seq2        ; no jump

        call    com_in          ; yes, get Y parameter
        sub     al,33           ; and remove offset
        mov     dh,al

        call    com_in          ; get X parameter
        sub     al,33           ; and remove offset
        mov     dl,al
        call    gotoxy          ; position the cursor 

esc_seq2:                       ; return to caller
        ret

esc_seq endp


asc_enb proc    near            ; capture serial port interrupt
                                ; vector and enable interrupt

                                ; save address of previous
                                ; interrupt handler...
        mov     ax,3500h+com_int ; Fxn 35H = get vector
        int     21h             ; transfer to MS-DOS
        mov     word ptr oldvec+2,es
        mov     word ptr oldvec,bx

                                ; now install our handler...
        push    ds              ; save our data segment
        mov     ax,cs           ; set DS:DX = address
        mov     ds,ax           ; of our interrupt handler
        mov     dx,offset asc_int
        mov     ax,2500h+com_int ; Fxn 25H = set vector
        int     21h             ; transfer to MS-DOS
        pop     ds              ; restore data segment
        
        mov     dx,com_mcr      ; set modem control register
        mov     al,0bh          ; DTR and OUT2 bits
        out     dx,al

        mov     dx,com_ier      ; set interrupt enable 
        mov     al,1            ; register on serial
        out     dx,al           ; port controller

        in      al,pic_mask     ; read current 8259 mask
        and     al,not int_mask ; set mask for COM port
        out     pic_mask,al     ; write new 8259 mask

        ret                     ; back to caller

asc_enb endp
         

asc_dsb proc    near            ; disable interrupt and 
                                ; release interrupt vector
        
        in      al,pic_mask     ; read current 8259 mask
        or      al,int_mask     ; reset mask for COM port
        out     pic_mask,al     ; write new 8259 mask

        push    ds              ; save our data segment
        lds     dx,oldvec       ; load address of
                                ; previous interrupt handler    
        mov     ax,2500h+com_int ; Fxn 25H = set vector
        int     21h             ; transfer to MS-DOS    
        pop     ds              ; restore data segment

        ret                     ; back to caller

asc_dsb endp


asc_int proc    far             ; interrupt service routine
                                ; for serial port

        sti                     ; turn interrupts back on

        push    ax              ; save registers
        push    bx
        push    dx
        push    ds

        mov     ax,_DATA        ; make our data segment 
        mov     ds,ax           ; addressable

        cli                     ; clear interrupts for
                                ; pointer manipulation

        mov     dx,com_data     ; DX = data port address
        in      al,dx           ; read this character

        mov     bx,asc_in       ; get buffer pointer
        mov     [asc_buf+bx],al ; store this character
        inc     bx              ; bump pointer
        cmp     bx,bufsiz       ; time for wrap?
        jne     asc_int1        ; no, jump
        xor     bx,bx           ; yes,reset pointer

asc_int1:                       ; store updated pointer
        mov     asc_in,bx

        sti                     ; turn interrupts back on

        mov     al,20h          ; send EOI to 8259
        out     pic_eoi,al

        pop     ds              ; restore all registers
        pop     dx
        pop     bx
        pop     ax

        iret                    ; return from interrupt

asc_int endp

_TEXT   ends


_DATA   segment word public 'DATA'

in_char db      0               ; PC keyboard input char.
in_flag db      0               ; <>0 if char waiting

columns db      0               ; highest numbered column in
                                ; current display mode (39 or 79)

msg1    db      cr,lf
        db      'Display must be text mode.'
        db      cr,lf
msg1_len equ $-msg1

msg2    db      'Terminal emulator running...'
        db      cr,lf
msg2_len equ $-msg2

msg3    db      'Exit from terminal emulator.'
        db      cr,lf
msg3_len equ $-msg3

oldvec  dd      0               ; original contents of serial 
                                ; port interrupt vector

asc_in  dw      0               ; input pointer to ring buffer
asc_out dw      0               ; output pointer to ring buffer

asc_buf db      bufsiz dup (?)  ; communications buffer

_DATA   ends


STACK   segment para stack 'STACK'

        db      128 dup (?)

STACK   ends

        end     talk            ;  defines entry point  

