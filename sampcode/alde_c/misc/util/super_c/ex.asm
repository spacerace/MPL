;               Example Assembly Program
;
; This program simply prints "Hello, world!" It demonstrates all the 
; elements of an assembly language program.

;
; Stack segment:
;
stakseg segment stack           ; Define the stack segment

        db      20 dup ('stack   ') ; Fill it with "stack"

stakseg ends                    ; End of stack segment

;
; Code segment:
;
codeseg segment                 ; Define code segment

        assume  cs:codeseg, ds:codeseg ; Assume CS points to code segment

msg:    db      'Hello, world!',0DH,0AH,0

main    proc far                ; Main routine

        push    ds              ; Set up for return to DS:0, which will, in
        xor     ax,ax           ;   turn, return us to DOS
        push    ax

        mov     ax,codeseg      ; DS = codeseg
        mov     ds,ax
        mov     bx,offset msg   ; BX = &msg

loop:   mov     al,[bx]         ; AL = *BX
        inc     bx              ; BX++
        or      al,al           ; AL == 0?
        jz      done            ; If yes, exit the loop
        push    bx              ; Save BX
        mov     ah,14           ; Tell ROM BIOS we want function 14, TTY write
        mov     bl,3            ; Using an attribute of 3 (white on black)
        int     10H             ; Call ROM BIOS video function TTY write
        pop     bx              ; Restore BX
        jmp     loop            ; Go do the next character in the string

done:   ret                     ; Return to DOS

main    endp                    ; End of main routine

codeseg ends                    ; End of code segment

        end     main            ; End of assembly code; start at main

