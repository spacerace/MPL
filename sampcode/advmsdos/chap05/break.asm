        title   Ctrl-C & Ctrl-Break Handlers
        page    55,132

;
; Ctrl-C and Ctrl-Break handler for Microsoft C 
; programs running on IBM PC compatibles
; 
; by Ray Duncan
;
; Assemble with:  C>MASM /Mx BREAK;
;
; This module allows C programs to retain control
; when the user enters a Ctrl-Break or Ctrl-C.  
; It uses Microsoft C parameter passing conventions
; and assumes the C small memory model.
;
; The procedure _capture is called to install
; a new handler for the Ctrl-C and Ctrl-Break
; interrupts (1BH and 23H).  _capture is passed
; the address of a static variable, which will be
; set to True by the handler whenever a Ctrl-C 
; or Ctrl-Break is detected.  The C syntax is:
; 
;               static int flag;
;               capture(&flag);
;
; The procedure _release is called by the C program
; to restore the original Ctrl-Break and Ctrl-C
; handler.  The C syntax is:
;
;               release();
;
; The procedure ctrlbrk is the actual interrupt 
; handler.  It receives control when a software 
; Int 1BH is executed by the ROM BIOS or Int 23H 
; is executed by MS-DOS.  It simply sets the C 
; program's variable to True (1) and returns.
;

args    equ     4               ; stack offset of arguments, 
                                ; C small memory model

cr      equ     0dh             ; ASCII carriage return
lf      equ     0ah             ; ASCII line feed


_TEXT   segment word public 'CODE'

        assume cs:_TEXT


        public  _capture
_capture proc   near            ; take over Control-Break 
                                ; and Ctrl-C interrupt vectors

        push    bp              ; set up stack frame
        mov     bp,sp

        push    ds              ; save affected registers
        push    di
        push    si

                                ; save address of 
                                ; calling program's "flag"
        mov     ax,word ptr [bp+args]
        mov     word ptr cs:flag,ax
        mov     word ptr cs:flag+2,ds

                                ; save address of original
        mov     ax,3523h        ; Int 23H handler
        int     21h
        mov     word ptr cs:int23,bx
        mov     word ptr cs:int23+2,es

        mov     ax,351bh        ; save address of original
        int     21h             ; Int 1BH handler
        mov     word ptr cs:int1b,bx
        mov     word ptr cs:int1b+2,es

        push    cs              ; set DS:DX = address
        pop     ds              ; of new handler
        mov     dx,offset _TEXT:ctrlbrk

        mov     ax,02523H       ; set Int 23H vector
        int     21h

        mov     ax,0251bH       ; set Int 1BH vector
        int     21h

        pop     si              ; restore registers     
        pop     di
        pop     ds

        pop     bp              ; discard stack frame
        ret                     ; and return to caller

_capture endp


        public  _release
_release proc   near            ; restore original Ctrl-C
                                ; and Ctrl-Break handlers

        push    bp              ; save registers
        push    ds
        push    di
        push    si

        lds     dx,cs:int1b     ; get address of previous
                                ; Int 1BH handler

        mov     ax,251bh        ; set Int 1BH vector
        int     21h

        lds     dx,cs:int23     ; get address of previous
                                ; Int 23H handler

        mov     ax,2523h        ; set Int 23H vector
        int     21h

        pop     si              ; restore registers     
        pop     di              ; and return to caller
        pop     ds
        pop     bp
        ret

_release endp


ctrlbrk proc    far             ; Ctrl-C and Ctrl-Break
                                ; interrupt handler

        push    bx              ; save registers
        push    ds

                                
        lds     bx,cs:flag      ; get address of C program's
                                ; "flag variable"

                                ; and set the flag "true"
        mov     word ptr ds:[bx],1
        
        pop     ds              ; restore registers
        pop     bx

        iret                    ; return from handler

ctrlbrk endp


flag    dd      0               ; far pointer to caller's
                                ; Ctrl-Break or Ctrl-C flag

int23   dd      0               ; address of original 
                                ; Ctrl-C handler
        
int1b   dd      0               ; address of original
                                ; Ctrl-Break handler

_TEXT   ends

        end

